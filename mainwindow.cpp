#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"
#include "QString"
#include "QFile"
#include "QIODevice"
#include <QFileDialog>
#include "QTextStream"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->dateTimeEdit->setCalendarPopup(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    /*
       getOpenFileName函数说明
       函数原形： QStringList QFileDialog::getOpenFileNames(
       QWidget * parent = 0,
       const QString & caption = QString(),    //  打开文件对话框的标题
       const QString & dir = QString(),            //  查找目录
       const QString & filter = QString(),     //  设置需要过滤的文件格式
       QString * selectedFilter = 0,
       Options options = 0) [static]
       */
       //---获取文件名
       QString fileName = QFileDialog :: getOpenFileName(this, NULL, NULL, "*.h *.cpp *.txt");

       //---打开文件并读取文件内容
       QFile file(fileName);

       //--打开文件成功
       if (file.open(QIODevice ::ReadOnly | QIODevice ::Text))
       {
           QTextStream textStream(&file);
           while (!textStream.atEnd())
           {
               //---QtextEdit按行显示文件内容
               //showText->append(textStream.readLine());
           }
       }
       else    //---打开文件失败
       {
           /*
           information函数参数说明：
           函数原型
           QMessageBox::information(
           QWidget * parent,
           const QString & title,                  //--- 标题
           const QString & text,                   //---显示内容
           StandardButtons buttons = Ok,   //---OK按钮
           StandardButton defaultButton = NoButton)
           [static]
           */
           QMessageBox ::information(NULL, NULL, "open file error");
       }
}
