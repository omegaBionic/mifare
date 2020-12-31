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

uint32_t valeur = 1; //compteur

const uint8_t secteur_id = 2;
const uint8_t secteur_compteur = 3;
const uint8_t block_prenom = secteur_id*4 + 1;
const uint8_t block_nom = secteur_id*4 + 2;
const uint8_t block_backup = secteur_compteur*4 + 1; //13
const uint8_t block_compteur = secteur_compteur*4 + 2; //14


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


void MainWindow::on_Detect_clicked()
{
    //Champs RF activé
    RF_Power_Control(&MonLecteur, TRUE, 0);

    //variables pour la prise de contact de la carte selon la norme ISO14443A avec un Request
    uint8_t atq[2];
    uint8_t sak[1];
    uint8_t uid[12];
    uint16_t uid_len = 12;
    uint16_t status = 0;

    //nom, prenom infos
    unsigned char nom[16];
    unsigned char prenom[16];

    //charger la clef dans le lecteur
    BYTE key_index = 2;
    //la prise de contact de la carte selon la norme ISO14443A avec un Request
    status = ISO14443_3_A_PollCard(&MonLecteur, atq, sak, uid, &uid_len );
    if(status != 0)
        qDebug() << "Prise de contact échouée";
    else
        qDebug() << "Prise de contact faite";
/*
    status = Mf_Classic_LoadKey(&MonLecteur, Auth_KeyA, key_A, key_index);
    status ? qDebug() << "Key loading failed" : qDebug() << "Key loading successful";
    if (!status)
        qDebug() << "";
        */
    status = Mf_Classic_LoadKey(&MonLecteur, Auth_KeyA, key_A, key_index);
    if(status != 0)
        qDebug() << "[FAILED]Loading Key with key_A";
    else
        qDebug() << "[SUCCESS]Loading Key with key_A";

}

void MainWindow::on_Saisie_clicked()
{
    //QString Text = ui->fenetreSaisie->toPlainText();
    //qDebug() << "Text : " << Text;
    uint16_t status = 0;
    unsigned char nom[16];
    unsigned char prenom[16];

    strncpy((char*)nom, ui->fenetreSaisieNom-> toPlainText().toUtf8().data(),16);
    strncpy((char*)prenom, ui->fenetreSaisiePrenom-> toPlainText().toUtf8().data(),16);

    status = Mf_Classic_Write_Block(&MonLecteur,TRUE, (block_nom), nom,  Auth_KeyB, 2);
    status = Mf_Classic_Write_Block(&MonLecteur,TRUE, (block_prenom), prenom,  Auth_KeyB, 2);



}

void MainWindow::on_Quitter_clicked()
{
    uint16_t status = 0;
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
    uint8_t key_index = 3; //3
    uint32_t data;
    uint32_t data_trans;
    int8_t test_lecture1 = Mf_Classic_Read_Value(&MonLecteur, auth, block_compteur, &data, Auth_KeyA, key_index);
    int8_t test_lecture2 = Mf_Classic_Read_Value(&MonLecteur, auth, block_backup, &data_trans, Auth_KeyA, key_index);
    qDebug() << "value in block " << block_compteur << " before incrementing: " << data ;
    qDebug() << "value in trans_block before incrementing: " << data_trans ;
    if(test_lecture1)
        qDebug() << "Success : lecture block_compteur";
    else
        qDebug() << "ERROR : LECTURE BLOCK_COMPTEUR";
    if(test_lecture2)
        qDebug() << "Success : lecteur block_backup";
    else
        qDebug() << "ERROR : LECTURE BLOCK_BACKUP";

    bool status = Mf_Classic_Increment_Value(&MonLecteur, auth, block_compteur, valeur, block_backup, Auth_KeyB, key_index);

    if (status){
        qDebug() << "status : TRUE\n";
        test_lecture1 = Mf_Classic_Read_Value(&MonLecteur, auth, block_compteur, &data, Auth_KeyA, key_index);
        test_lecture2 = Mf_Classic_Read_Value(&MonLecteur, auth, block_backup, &data_trans, Auth_KeyA, key_index);
        qDebug() << "Value in block after incrementing : " << data;
        qDebug() << "Value in block_backup after incrementing : " << data_trans;

        if(test_lecture1)
            qDebug() << "Success : lecture block_compteur";
        else
            qDebug() << "ERROR : LECTURE BLOCK_COMPTEUR";
        if(test_lecture2)
            qDebug() << "Success : lecteur block_backup";
        else
            qDebug() << "ERROR : LECTURE BLOCK_BACKUP";

        LEDBuzzer(&MonLecteur, LED_ON);
    }
}

void MainWindow::on_Decrementer_clicked()
{

    bool auth = TRUE;
    uint8_t key_index = 3; //3
    uint32_t data;
    uint32_t data_trans;
    bool status = Mf_Classic_Decrement_Value(&MonLecteur, auth, block_compteur, valeur, block_backup, Auth_KeyB, key_index);
    if(status )
        qDebug() << "status : TRUE\n";
        Mf_Classic_Read_Value(&MonLecteur, auth, block_compteur, &data, Auth_KeyA, key_index);
        Mf_Classic_Read_Value(&MonLecteur, auth, block_backup, &data_trans, Auth_KeyA, key_index);
        qDebug() << "Value in block after decrementing : " << data << "\n";
        qDebug() << "Value in trans_block after decrementing : " << data_trans << "\n";

    LEDBuzzer(&MonLecteur, LED_ON);
}

void MainWindow::on_Ecrire_clicked()
{

}

void MainWindow::on_Lire_clicked()
{

}


