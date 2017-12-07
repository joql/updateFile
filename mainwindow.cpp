#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMessageBox"
#include "QString"
#include "QFile"
#include "QIODevice"
#include <QFileDialog>
#include "QTextStream"

int start_time;//文件更新起始时间
QString root_dir;//应用根目录
QString updateReady_dir;//更新文件的根目录
QString start_dir;//文件更新根目录
QFileInfoList GetFileList(QString path);//遍历文件夹

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->dateTimeEdit->setCalendarPopup(true);
    root_dir = QCoreApplication::applicationDirPath();
    updateReady_dir = root_dir+"/updateReady";
    QDir updateReady(updateReady_dir);
    updateReady.mkdir(updateReady_dir);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->edit_dir->setText(dir);
    start_dir = dir;
}

void MainWindow::on_btn_getNew_clicked()
{
    QDateTime tmp_time = ui->dateTimeEdit->dateTime();
    start_time = tmp_time.toTime_t();
    QFileInfoList filelist = GetFileList(start_dir);
    for(int i=0;i<filelist.count();i++){
        QFileInfo tmp_fileinfo = filelist.at(i);
        QString name= tmp_fileinfo.baseName();
        QString tmp_path = tmp_fileinfo.path();
        int tmp_time = tmp_fileinfo.lastModified().toTime_t();
        QString tmp_url = tmp_fileinfo.absoluteFilePath();
        QString tmp_url2 = tmp_url;

        tmp_path.replace(start_dir,updateReady_dir);
        QDir dir(tmp_path);
        if(dir.exists() == false){
            dir.mkpath(tmp_path);
        }
        if(tmp_time >= start_time){
            QFile::copy(tmp_url,tmp_url2.replace(start_dir,updateReady_dir));
            ui->edit_out->append(name+"|"+QString::number(tmp_time)+"|"+tmp_url+"|"+tmp_url2.replace(start_dir,updateReady_dir));
        }
    }

}


/**
 * 遍历文件夹
 * @brief GetFileList
 * @param path
 * @return
 */
QFileInfoList GetFileList(QString path)
{
    QDir dir(path);
    QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for(int i = 0; i != folder_list.size(); i++)
    {
         QString name = folder_list.at(i).absoluteFilePath();
         QFileInfoList child_file_list = GetFileList(name);
         file_list.append(child_file_list);
    }

    return file_list;
}

