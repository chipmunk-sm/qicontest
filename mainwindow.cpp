#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>
#include <sys/utsname.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label_qt_ver->setText("Qt version: " QT_VERSION_STR);
    struct utsname kerninfo;
    uname(&kerninfo);
    ui->label_kernel_ver->setText(QString("%1 %2 %3").arg(kerninfo.sysname, kerninfo.release, kerninfo.version));
    on_pushButton_openImage_clicked();
}

void MainWindow::SetIcon(QString path, QLabel *pLabelPath, QLabel *pLabelIcon, QLabel *pLabelSizes, QWindow *mainwindow)
{

    pLabelPath->setText(path);

    QIcon tmpicon = QIcon::fromTheme(path);

    auto sizes = tmpicon.availableSizes();

    if(sizes.isEmpty())
    {
        pLabelSizes->setText("sizes.length() is empty");
        return;
    }

    const QPixmap pixmap = tmpicon.pixmap(mainwindow, pLabelPath->size(), QIcon::Mode::Normal, QIcon::State::On);

    pLabelIcon->setPixmap(pixmap);

    QString sSizes;

    for(auto i=0; i < sizes.size(); i++)
    {
        sSizes += QString::number(sizes[i].height()) + ":" + QString::number(sizes[i].width());
        if( i < sizes.size() -1 )
            sSizes += "," ;
    }

    pLabelSizes->setText(sSizes);


}

void MainWindow::on_pushButton_openImage_clicked()
{

#if 1

    QString filename = QFileDialog::getOpenFileName(this, "Open", QDir::currentPath(), "PNG SVG (*.png *.svg) ;; All files (*.*)");
    if(filename.isNull())
    {
        QMessageBox::critical(this, windowTitle(), QObject::tr("No file selected."), QMessageBox::Ok);
        return;
    }

    auto path1 = filename;
    auto path2 = QFileInfo(filename).absolutePath() + "/" + QFileInfo(filename).completeBaseName();

#else

    auto path1 = "/usr/share/tcpview/tcpviewb.svg";
    auto path2 = "/usr/share/tcpview/tcpviewb";


#endif

    SetIcon(path1, ui->label_path_1, ui->label_icon_1, ui->label_size_1, this->windowHandle());
    SetIcon(path2, ui->label_path_2, ui->label_icon_2, ui->label_size_2, this->windowHandle());

}

MainWindow::~MainWindow()
{
    delete ui;
}
