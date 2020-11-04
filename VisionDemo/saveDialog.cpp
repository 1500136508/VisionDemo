#include "saveDialog.h"
#include "ui_savedialog.h"
#include <QTextCodec>
#include <QFileInfo>
#include <QFileDialog>
#include <QIntValidator>
#include <QDir>
#include <QDebug>

saveDialog::saveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::saveDialog)
{
    ui->setupUi(this);
    ui->FrameLineEdit->setValidator(new QIntValidator(0,999,this));
    connect(ui->dirBtn, SIGNAL(clicked()), this, SLOT(savePathBtnSlot()));
    connect(ui->startBtn, SIGNAL(clicked()), this, SLOT(saveBtnSlot()));
    connect(ui->stopBtn, SIGNAL(clicked()), this, SLOT(stopBtnSlot()));
    connect(ui->checkBox3pic,SIGNAL(stateChanged(int)),this, SLOT(chooseModeThree()));
    connect(ui->checkBox4pic,SIGNAL(stateChanged(int)),this, SLOT(chooseModeFour()));
    connect(ui->pathLineEdit,SIGNAL(textEdited(QString)),this, SLOT(savePathCheckSlot()));
}

saveDialog::~saveDialog()
{
    delete ui;
}

void saveDialog::savePathBtnSlot()
{
    QTextCodec *code = QTextCodec::codecForName("GB2312");
    std::string workPath = code->fromUnicode(QFileDialog::getExistingDirectory(this, tr("Open Directory"), " ", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks)).data();
    qDebug() << QString::fromLocal8Bit(workPath.c_str());
    ui->pathLineEdit->setText(QString::fromLocal8Bit(workPath.c_str()));
    savePathCheckSlot();
}

void saveDialog::savePathCheckSlot()
{
    readyToSave = false;
    QFileInfo qfi(ui->pathLineEdit->text());
    if(qfi.isDir()){
        ui->info->setText("Save Path:"+ui->pathLineEdit->text());
        emit sendSavePath(ui->pathLineEdit->text());
        readyToSave = true;
    }
    else{
        ui->info->setText("Save Path Error");
        emit sendError();
    }
}

void saveDialog::saveBtnSlot()
{
    if (!readyToSave) return;

    QDir qd;
    std::string savePath = ui->pathLineEdit->text().toStdString();

//    qd.rmpath(QString::fromStdString(savePath+"//rgb"));
//    qd.rmpath(QString::fromStdString(savePath+"//ir"));
//    qd.rmpath(QString::fromStdString(savePath+"//depth"));
//    qd.rmpath(QString::fromStdString(savePath+"//rgb-alone"));
//    qd.rmpath(QString::fromStdString(savePath+"//pointcloud"));

    QDir dir;
    dir.setPath(QString::fromStdString(savePath+"//rgb"));
    dir.removeRecursively();
    dir.setPath(QString::fromStdString(savePath+"//ir"));
    dir.removeRecursively();
    dir.setPath(QString::fromStdString(savePath+"//depth"));
    dir.removeRecursively();
    dir.setPath(QString::fromStdString(savePath+"//rgb-alone"));
    dir.removeRecursively();
    dir.setPath(QString::fromStdString(savePath+"//pointcloud"));
    dir.removeRecursively();


    if (ui->checkBox3pic->isChecked()){
        qd.mkdir(QString::fromStdString(savePath));
        qd.mkdir(QString::fromStdString(savePath+"//rgb"));
        qd.mkdir(QString::fromStdString(savePath+"//ir"));
        qd.mkdir(QString::fromStdString(savePath+"//depth"));
        qd.mkdir(QString::fromStdString(savePath+"//rgb-alone"));
    }
    else if (ui->checkBox4pic->isChecked()){
        qd.mkdir(QString::fromStdString(savePath));
        qd.mkdir(QString::fromStdString(savePath+"//rgb"));
        qd.mkdir(QString::fromStdString(savePath+"//ir"));
        qd.mkdir(QString::fromStdString(savePath+"//depth"));
        qd.mkdir(QString::fromStdString(savePath+"//pointcloud"));
    }

    emit sendSaveStart(ui->FrameLineEdit->text().toInt());
}

void saveDialog::stopBtnSlot()
{
    emit sendSaveStop();
}

void saveDialog::showInfo(QString info)
{
    ui->info->setText(info);
}

void saveDialog::chooseModeThree()
{
    if (ui->checkBox3pic->isChecked() && ui->checkBox4pic->isChecked()) {
        ui->checkBox4pic->setCheckState(Qt::CheckState::Unchecked);
    }
    emit sendSaveMode(3);
}

void saveDialog::chooseModeFour()
{
    if (ui->checkBox3pic->isChecked() && ui->checkBox4pic->isChecked()) {
        ui->checkBox3pic->setCheckState(Qt::CheckState::Unchecked);
    }
    emit sendSaveMode(4);
}



