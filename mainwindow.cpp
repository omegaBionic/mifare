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
     qDebug() << "Version : " << version << "\n";
     ui->Display->setText(version);
     ui->Display->update();
}

void MainWindow::on_Saisie_clicked()
{
    QString Text = ui->fenetreSaisie->toPlainText();
    qDebug() << "Text : " << Text;
}

void MainWindow::on_Quitter_clicked()
{
    int16_t status = 0;
    RF_Power_Control(&MonLecteur, FALSE, 0);
    status = LEDBuzzer(&MonLecteur, LED_OFF);
    status = CloseCOM(&MonLecteur);
    qApp->quit();
}

void MainWindow::on_Enroller_clicked()
{

}

void MainWindow::on_Formater_clicked()
{

}

void MainWindow::on_Incrementer_clicked()
{
    //need to specify those values, specially blocks
    bool auth = TRUE;
    uint8_t block = 1;
    uint8_t trans_block = 2;
    bool auth_key = TRUE;
    uint8_t key_index = 1;
    uint32_t valeur = 2;
    uint8_t data;
    Mf_Classic_Read_Block(&MonLecteur, auth, block, &data, auth_key, key_index);
    qDebug() << "value in block : " << data  << "\n" ;
    bool status = Mf_Classic_Increment_Value(&MonLecteur, auth, block, valeur, trans_block, auth_key, key_index);

    LEDBuzzer(&MonLecteur, LED_ON);
}

void MainWindow::on_Decrementer_clicked()
{

}

void MainWindow::on_Ecrire_clicked()
{

}

void MainWindow::on_Lire_clicked()
{

}
