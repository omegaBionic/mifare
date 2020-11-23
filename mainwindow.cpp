#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ODALID.h"
#include <QDebug>
#include <QtGui>
#define PSZHOST "192.168.1.4"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

ReaderName MonLecteur;
char pszHost[] = PSZHOST;


void MainWindow::on_Connect_clicked()
{
    uint16_t status = 0;
    //MonLecteur.Type = ReaderTCP;
    //strcpy(MonLecteur.IPReader, pszHost);
    MonLecteur.Type = ReaderCDC;
    MonLecteur.device = 0;
    status = OpenCOM(&MonLecteur);
    qDebug() << "OpenCOM: '" << status << "'";

     char version[30];
     uint8_t serial[4];
     char stackReader[20];

     status = Version(&MonLecteur, version, serial, stackReader);
     ui->Display->setText(version);
     ui->Display->update();
}
