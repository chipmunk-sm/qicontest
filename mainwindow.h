#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_pushButton_openImage_clicked();

private:
    Ui::MainWindow *ui;
    static void SetIcon(QString path, QLabel *pLabelPath, QLabel *pLabelIcon, QLabel *pLabelSizes, QWindow *mainwindow);
};

#endif // MAINWINDOW_H
