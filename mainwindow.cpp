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
uint8_t key_A[6] = {0xA0 ,0xA1 ,0xA2 ,0xA3 ,0xA4 ,0xA5};
uint8_t key_B[6] = {0xB0 ,0xB1 ,0xB2 ,0xB3 ,0xB4 ,0xB5};


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
    uint8_t block = 5; //14
    uint8_t trans_block = 6; //13
    bool auth_key = FALSE;
    uint8_t key_index = 2; //3
    uint32_t valeur = 0; //0
    uint8_t data;
    uint8_t data_trans;
    Mf_Classic_Read_Block(&MonLecteur, auth, block, &data, auth_key, key_index);
    Mf_Classic_Read_Block(&MonLecteur, auth, block, &data_trans, auth_key, key_index);
    qDebug() << "value in block 1 before incrementing: " << data  << "\n" ;
    qDebug() << "value in trans_block before incrementing: " << data  << "\n" ;

    bool status = Mf_Classic_Increment_Value(&MonLecteur, auth, block, valeur, trans_block, auth_key, key_index);

    if (status){
        qDebug() << "status : TRUE\n";
        Mf_Classic_Read_Block(&MonLecteur, auth, block, &data, auth_key, key_index);
        Mf_Classic_Read_Block(&MonLecteur, auth, trans_block, &data_trans, auth_key, key_index);
        qDebug() << "Value in block after incrementing : " << data << "\n";
        qDebug() << "Value in trans_block after incrementing : " << data_trans << "\n";

        LEDBuzzer(&MonLecteur, LED_ON);
    }
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
