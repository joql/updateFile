#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QMessageBox"
#include "QString"
#include "QFile"
#include "QIODevice"
#include <QFileDialog>
#include "QTextStream"
#include "QUrl"
#include "QtNetwork"
#include "QFtp"
#include "QDebug"


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
    void on_pushButton_clicked();

    void on_btn_getNew_clicked();

    void on_pushButton_2_clicked();

    void on_btn_getNew_2_clicked();

    void on_btn_getNew_3_clicked();

    void on_pushButton_3_clicked();


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
