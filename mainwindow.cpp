#include "mainwindow.h"
#include "ui_mainwindow.h"


QString root_dir;//应用根目录
QString updateReady_dir;//更新文件的根目录
QString updateBack_dir;//更新文件备份的根目录
QString start_dir;//项目根目录
int start_time;//文件更新起始时间
QFileInfoList GetFileList(QString path);//遍历文件夹
bool ftp_get(QString filename,QFile *file);
bool ftp_put(QString filename,QFile *file);
bool ftp_cd(QString filepath);
QString ftp_url;//ftp地址
QString ftp_name;//ftp用户名
QString ftp_pwd;//ftp密码
QFtp *ftp;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->dateTimeEdit->setCalendarPopup(true);
    root_dir = QCoreApplication::applicationDirPath();
    //初始化更新目录
    updateReady_dir = root_dir+"/updateReady";
    QDir updateReady(updateReady_dir);
    if(!updateReady.exists()) updateReady.mkdir(updateReady_dir);
    //初始化更新备份目录
    updateBack_dir = root_dir+"/updateBack";
    QDir updateBack(updateBack_dir);
    if(!updateBack.exists()) updateBack.mkdir(updateBack_dir);

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
    ui->edit_out->append("根目录文件数: "+QString::number(filelist.count()));
    for(int i=0,j=0;i<filelist.count();i++){
        QFileInfo tmp_fileinfo = filelist.at(i);
        QString tmp_path = tmp_fileinfo.path();
        int tmp_time = tmp_fileinfo.lastModified().toTime_t();
        QString tmp_url = tmp_fileinfo.absoluteFilePath();
        QString tmp_newurl = tmp_url;

        if(tmp_time >= start_time){
            j++;
            //判断路径是否存在，否则创建
            tmp_path.replace(start_dir,updateReady_dir);
            QDir dir(tmp_path);
            if(dir.exists() == false){
                dir.mkpath(tmp_path);
            }
            QFile::copy(tmp_url,tmp_newurl.replace(start_dir,updateReady_dir));
            ui->edit_out->append("第"+QString::number(j)+"项，正在复制"+tmp_fileinfo.fileName());
        }
    }
    ui->edit_out->append("任务完成");

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


void MainWindow::on_pushButton_2_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->edit_dir_2->setText(dir);
}

void MainWindow::on_btn_getNew_2_clicked()
{
    QFileInfoList filelist = GetFileList(updateReady_dir);
    ui->edit_out->append("任务数: "+QString::number(filelist.count()));
    for(int i=0;i<filelist.count();i++){
        QFileInfo tmp_fileinfo = filelist.at(i);
        QString path = tmp_fileinfo.absoluteFilePath();
        QString path2 = tmp_fileinfo.absolutePath();
        QFile need_file(path.replace(updateReady_dir,ui->edit_dir_2->text()));
        //备份被更新文件
        //判断路径是否存在，否则创建
        path2.replace(ui->edit_dir_2->text(),updateBack_dir);
        QDir dir(path2);
        if(dir.exists() == false){
            dir.mkpath(path2);
        }
        QString tmp_path = path;
        QFile::copy(path,tmp_path.replace(ui->edit_dir_2->text(),updateBack_dir));
        if(need_file.exists()){
            //文件存在则先备份
            QFile::rename(path,path+QDateTime::currentDateTime().toString("yyyyMMdd"));
            QFile::copy(tmp_fileinfo.absoluteFilePath(),path);
            ui->edit_out->append("第"+QString::number(i+1)+"项,备份后复制"+tmp_fileinfo.fileName());

        }else{
            //不存在直接复制
            QFile::copy(tmp_fileinfo.absoluteFilePath(),path);
            ui->edit_out->append("第"+QString::number(i+1)+"项,正在复制"+tmp_fileinfo.fileName());
        }
    }
    ui->edit_out->append("任务完成");
} 

void MainWindow::on_btn_getNew_3_clicked()
{
    //1.下载要替换的文件，不存在则忽略
    //2.上传更名后的被替换文件，不存在则忽略
    //3.上传新文件，存在则替换，检查错误信息，发生错误可能是目录不存在，创建目录后，再次上传
    /*
    QNetworkAccessManager *m = new QNetworkAccessManager(this);
    QUrl url("ftp://120.25.124.213/test/test.txt");
    url.setUserName("kang");
    url.setPassword("kang");
    QString dd = "123";
    m->put(QNetworkRequest(url),dd.toUtf8());
    */

    //ftp登陆
    ftp = new QFtp(this);
    ftp->connectToHost("120.25.124.213",21);
    ftp->login("kang","kang");

    QFileInfoList filelist = GetFileList(updateReady_dir);
    for(int i=0;i<filelist.count();i++){
        QFileInfo tmp_fileinfo = filelist.at(i);
        QString relat_path = tmp_fileinfo.absolutePath();
        QString relat_path2 = tmp_fileinfo.absolutePath();
        QString abs_path = tmp_fileinfo.absoluteFilePath();

        //切换到操作目录
        if(ftp_cd(relat_path.replace(updateReady_dir,""))){
            //下载被替换的文件
            QFile *file = new QFile(abs_path.replace(updateReady_dir,updateBack_dir));
            file->open(QFile::ReadWrite);
            if(ftp_get(tmp_fileinfo.fileName(),file)){
                //如果文件存在并且下载成功，自动上传更名后的备份文件
                ftp_put(tmp_fileinfo.fileName()+QDateTime::currentDateTime().toString("yyyyMMdd"),file);
            }
            //上传更新文件
            QFile *newfile = new QFile(tmp_fileinfo.absoluteFilePath());
            newfile->open(QIODevice::ReadWrite);
            ftp_put(tmp_fileinfo.fileName(),newfile);
        }
    }



}

void MainWindow::on_pushButton_3_clicked()
{
    QDir updateReady(updateReady_dir);
    updateReady.removeRecursively();
    updateReady.mkdir(updateReady_dir);
    QDir updateBack(updateBack_dir);
    updateBack.removeRecursively();
    updateBack.mkdir(updateBack_dir);
    ui->edit_out->setText("初始化完成");
}

/**
 * @brief ftp_get
 * @param filepath
 * @param filename
 * @param file
 * @return
 */
bool ftp_get(QString filename,QFile *file){
    ftp->get(filename,file);
    QEventLoop loop;
       QObject::connect(ftp,SIGNAL(done(bool)),&loop,SLOT(quit()));
    loop.exec();
    file->close();
    if(ftp->error() == 0){
        return true;
    }else{
        return false;
    }
}

bool ftp_put(QString filename,QFile *file){
    ftp->put(file,filename);

    QEventLoop loop;
       QObject::connect(ftp,SIGNAL(done(bool)),&loop,SLOT(quit()));
    loop.exec();
    file->close();
    if(ftp->error() == 0){
        return true;
    }else{
        return false;
    }
}

bool ftp_cd(QString filepath){
    ftp->cd(filepath);
    QEventLoop loop;
       QObject::connect(ftp,SIGNAL(done(bool)),&loop,SLOT(quit()));
    loop.exec();
    if(ftp->error() == 0){
        return true;
    }else{
        QStringList list;
        QString tmp;
        list = filepath.split("/");
        for(int i=1;i<list.count();i++){
            ftp->mkdir(list.at(i));
            loop.exec();
            tmp = tmp+"/"+list.at(i);
            ftp->cd(tmp);

        }
        if(ftp->error() == 0){
            return true;
        }else{
            return false;
        }
    }
}

