# qicontest
QIcon load SVG  
test for Qt 5.7.1  
Kubuntu 17.04  

Bug affected to Dolphin, Applicaton Menu. Show icon for .desktop file (Icon section)

# Dolphin code
## dolphin-16.12.3/src/kitemviews/kstandarditemlistwidget.cpp
```
QPixmap KStandardItemListWidget::pixmapForIcon(const QString& name, const QStringList& overlays, int size, QIcon::Mode mode)
...
const QIcon icon = QIcon::fromTheme(name, fallbackIcon); <---- the SVG icon is not displayed for .desktop file (with full path for 'Icon')
```
### Qt bug "icon.availableSizes().isEmpty()"
## qtbase-opensource-src-5.7.1+dfsg/src/gui/image/qicon.cpp:1209
```
QIcon QIcon::fromTheme(const QString &name, const QIcon &fallback)
{
    QIcon icon = fromTheme(name);

    if (icon.isNull() || icon.availableSizes().isEmpty())
        return fallback;

    return icon;
}
```
## Qt 5.7.1, quote from src/gui/image/qiconengine.cpp:143

\value AvailableSizesHook Allows to query the sizes of the
contained pixmaps for pixmap-based engines. The \a data argument
of the virtual_hook() function is a AvailableSizesArgument pointer
that should be filled with icon sizes. Engines that work in terms
of a scalable, vectorial format normally return an empty list.

# Debug qtbase-opensource-src-5.7.1+dfsg (debug build) with qicontest
## Description:
### 1. use path + filename + extension

QIcon::addFile, d->engine == QSvgIconEngine

QSvgIconEngine has no ::virtual_hook(int id, void *data)

QIcon::availableSizes() <-- empty

availableSizes().isEmpty() == true

Result: the SVG icon is not displayed


### 2. use path + filename

QIcon::addFile, d->engine == QPixmapIconEngine

QPixmapIconEngine has QPixmapIconEngine::virtual_hook(int id, void *data)

QIcon::availableSizes() <-- not empty

availableSizes().isEmpty() == false

so, size load from SVG  
<svg... width="48" height="48"

Result: the SVG icon is Ok!


## breakpoints for qtbase-opensource-src-5.7.1+dfsg

## breakpoint at line 1041  src/gui/image/qicon.cpp  
d->engine = engine ? engine : new QPixmapIconEngine;
```
void QIcon::addFile(const QString &fileName, const QSize &size, Mode mode, State state)
{
    if (fileName.isEmpty())
        return;
    detach();
    if (!d) {

        QFileInfo info(fileName);
        QIconEngine *engine = iconEngineFromSuffix(fileName, info.suffix());
#ifndef QT_NO_MIMETYPE
        if (!engine)
            engine = iconEngineFromSuffix(fileName, QMimeDatabase().mimeTypeForFile(info).preferredSuffix());
#endif // !QT_NO_MIMETYPE
        d = new QIconPrivate;
        d->engine = engine ? engine : new QPixmapIconEngine;
    }

    d->engine->addFile(fileName, size, mode, state);

    // Check if a "@Nx" file exists and add it.
    QString atNxFileName = qt_findAtNxFile(fileName, qApp->devicePixelRatio());
    if (atNxFileName != fileName)
        d->engine->addFile(atNxFileName, size, mode, state);
}
```
## breakpoint at line 514 src/gui/image/qicon.cpp  
switch (id) {
```
void QPixmapIconEngine::virtual_hook(int id, void *data)
{
    switch (id) {
    case QIconEngine::AvailableSizesHook: {
        QIconEngine::AvailableSizesArgument &arg =
            *reinterpret_cast<QIconEngine::AvailableSizesArgument*>(data);
        arg.sizes.clear();
        for (int i = 0; i < pixmaps.size(); ++i) {
            QPixmapIconEngineEntry &pe = pixmaps[i];
            if (pe.size == QSize() && pe.pixmap.isNull()) {
                pe.pixmap = QPixmap(pe.fileName);
                pe.size = pe.pixmap.size();
            }
            if (pe.mode == arg.mode && pe.state == arg.state && !pe.size.isEmpty())
                arg.sizes.push_back(pe.size);
        }
        break;
    }
    default:
        QIconEngine::virtual_hook(id, data);
    }
}
```
## breakpoint at line 277 src/gui/image/qiconengine.cpp  
const_cast<QIconEngine *>(this)->virtual_hook....
```
QList<QSize> QIconEngine::availableSizes(QIcon::Mode mode, QIcon::State state) const
{
    AvailableSizesArgument arg;
    arg.mode = mode;
    arg.state = state;
    const_cast<QIconEngine *>(this)->virtual_hook(QIconEngine::AvailableSizesHook, reinterpret_cast<void*>(&arg));
    return arg.sizes;
}
```





