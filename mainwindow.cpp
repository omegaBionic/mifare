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

    //background image
    QPixmap bkgnd("../mifare/ressources/background_blue.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

    //set title
    this -> setWindowTitle("Mifare Card");
}

MainWindow::~MainWindow()
{
    delete ui;
}

ReaderName MonLecteur;
char pszHost[] = PSZHOST;
uint8_t key_A[6] = {0xA0 ,0xA1 ,0xA2 ,0xA3 ,0xA4 ,0xA5};
uint8_t key_B[6] = {0xB0 ,0xB1 ,0xB2 ,0xB3 ,0xB4 ,0xB5};

uint8_t key_A_compteur[6] = {0xC0 ,0xC1 ,0xC2 ,0xC3 ,0xC4 ,0xC5};
uint8_t key_B_compteur[6] = {0xD0 ,0xD1 ,0xD2 ,0xD3 ,0xD4 ,0xD5};

uint32_t valeur = 25; //compteur

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
     if(status == 0){
        qDebug() << "Version : " << version << "\n";
        ui->Display->setText(version);
        ui->Display->update();
        status = LEDBuzzer(&MonLecteur, LED_YELLOW_ON);

     }
     else{
         qDebug() << "FAIL: CONNECT";
         ui->Display->setText("FAIL");
         ui->Display->update();
         LEDBuzzer(&MonLecteur, LED_RED_ON);
     }

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

    //charger la clef dans le lecteur
    BYTE key_index = 2;
    BYTE key_index_compteur = 3;
    status = Mf_Classic_LoadKey(&MonLecteur, Auth_KeyA, key_A, key_index);
    if(status != 0)
        qDebug() << "[FAILED] Loading Key with key_A";
    else
        qDebug() << "[SUCCESS] Loading Key with key_A";

    status = Mf_Classic_LoadKey(&MonLecteur, Auth_KeyB, key_B, key_index);
    if(status != 0)
        qDebug() << "[FAILED] Loading Key with key_B";
    else
        qDebug() << "[SUCCESS] Loading Key with key_B";

    status = Mf_Classic_LoadKey(&MonLecteur, Auth_KeyA, key_A_compteur, key_index_compteur);
    status = Mf_Classic_LoadKey(&MonLecteur, Auth_KeyB, key_B_compteur, key_index_compteur);

    //la prise de contact de la carte selon la norme ISO14443A avec un Request
    status = ISO14443_3_A_PollCard(&MonLecteur, atq, sak, uid, &uid_len );
    if(status != 0){
        qDebug() << "Prise de contact échouée";
        ui->Display_detected->setText("NOT DETECTED");
        LEDBuzzer(&MonLecteur, LED_RED_OFF);
    }
    else{
        for (int i = 0; i < uid_len; i++)
            qDebug("%02X", uid[i]);
        ui->Display_detected->setText("DETECTED");
        LEDBuzzer(&MonLecteur, LED_GREEN_OFF);
    }

}


void MainWindow::on_Quitter_clicked()
{
    uint16_t status = 0;
    RF_Power_Control(&MonLecteur, FALSE, 0);
    ISO14443_3_A_Halt(&MonLecteur) ;
    status = LEDBuzzer(&MonLecteur, LED_OFF);
    status = CloseCOM(&MonLecteur);
    qApp->quit();
}


void MainWindow::on_Lire_clicked()
{
    BYTE key_index = 2;
    uint16_t status = 0;
    //nom, prenom infos
    unsigned char nom[16];
    unsigned char prenom[16];

    //Lecture des blocs
    status = Mf_Classic_Read_Block(&MonLecteur, TRUE, block_nom, nom, Auth_KeyA, key_index);
    if(status != 0)
        qDebug() << "[FAILED] Reading 'nom'";
    else{
        qDebug() << "[SUCCESS] Reading 'nom'";
        qDebug("Nom: %s", nom);
    }

    status = Mf_Classic_Read_Block(&MonLecteur, TRUE, block_prenom, prenom, Auth_KeyA, key_index);
    if(status != 0)
        qDebug() << "[FAILED] Reading 'prenom'";
    else{
        qDebug() << "[SUCCESS] Reading 'prenom'";
        qDebug("Prénom: %s", prenom);
    }

    uint32_t valeur_compteur = 0;
    status = Mf_Classic_Read_Value(&MonLecteur,TRUE, block_compteur, &valeur_compteur, Auth_KeyA, 3);
    if(status != 0)
        qDebug() << "[FAILED] Reading 'compteur'";
    else{
        qDebug() << "[SUCCESS] Reading 'compteur'";
        qDebug() << "Compteur: " << valeur_compteur;
    }

    //print
    ui->nom_lu->setText((char*)nom);
    ui->prenom_lu->setText((char*)prenom);
    ui->compteur_lu->setText(QString::number(valeur_compteur));

    LEDBuzzer(&MonLecteur, LED_YELLOW_OFF);

}


void MainWindow::on_ecrire_nom_clicked()
{
    uint16_t status = 0;
    unsigned char nom[16];

    strncpy((char*)nom, ui->fenetreSaisieNom-> toPlainText().toUtf8().data(),16);

    status = Mf_Classic_Write_Block(&MonLecteur,TRUE, block_nom, nom,  Auth_KeyB, 2);
    if(status == 0){
        qDebug() << "[SUCCESS] 'nom' written";
        LEDBuzzer(&MonLecteur, LED_ON);
    }
    else{
        qDebug() << "[FAIL] 'nom' not written";
            LEDBuzzer(&MonLecteur, LED_RED_OFF);
    }

}

void MainWindow::on_ecrire_prenom_clicked()
{
    uint16_t status = 0;
    unsigned char prenom[16];

    strncpy((char*)prenom, ui->fenetreSaisiePrenom-> toPlainText().toUtf8().data(),16);

    status = Mf_Classic_Write_Block(&MonLecteur,TRUE, block_prenom, prenom,  Auth_KeyB, 2);
    if(status == 0){
        qDebug() << "[SUCCESS] 'prenom' written";
        LEDBuzzer(&MonLecteur, LED_ON);
    }
    else{
        qDebug() << "[FAIL] 'prenom' not written";
        LEDBuzzer(&MonLecteur, LED_RED_OFF);
    }
}

void MainWindow::on_ecrire_compteur_clicked()
{
    uint16_t status = 0;
    uint32_t valeur_compteur = 0;
    char valeur_compteur_char_ptr[16];
    BYTE key_index = 3;

    strncpy((char*)valeur_compteur_char_ptr, ui->fenetreSaisieCompteur-> toPlainText().toUtf8().data(),16);
    valeur_compteur = atoi(valeur_compteur_char_ptr);
    uint16_t status_restore = Mf_Classic_Restore_Value(&MonLecteur, TRUE, block_compteur, block_backup, Auth_KeyB, key_index );
    status = Mf_Classic_Write_Value(&MonLecteur,TRUE, block_compteur, valeur_compteur, Auth_KeyB, key_index);
    if(status == 0){
        qDebug() << "[SUCCESS] 'compteur' written";
        LEDBuzzer(&MonLecteur, LED_ON);
    }
    else{
        qDebug() << "[FAIL] 'compteur' not written";
        LEDBuzzer(&MonLecteur,  LED_RED_OFF);
    }
}

void MainWindow::on_plus_compteur_clicked()
{
    bool auth = TRUE;
    uint8_t key_index = 3; //3
    uint16_t status = 0;

    valeur = ui->valeur_compteur->value();
    uint16_t status_restore = Mf_Classic_Restore_Value(&MonLecteur, auth, block_compteur, block_backup, Auth_KeyB, key_index );
    status = Mf_Classic_Increment_Value(&MonLecteur, auth, block_compteur, valeur, block_compteur, Auth_KeyB, key_index);
    if(status == 0){
        qDebug() << "[Success] Incrementation";
        LEDBuzzer(&MonLecteur, LED_GREEN_OFF);
    }
    else{
        qDebug() << "[FAIL] Incrementation";
        LEDBuzzer(&MonLecteur, LED_RED_OFF);
    }
}

void MainWindow::on_minus_compteur_clicked()
{
    bool auth = TRUE;
    uint8_t key_index = 3; //3

    uint16_t status = 0;

    valeur = ui->valeur_compteur->value();
    uint16_t status_restore = Mf_Classic_Restore_Value(&MonLecteur, auth, block_compteur, block_backup, Auth_KeyB, key_index );
    status = Mf_Classic_Decrement_Value(&MonLecteur, auth, block_compteur, valeur, block_compteur, Auth_KeyB, key_index);
    if(status == 0){
        qDebug() << "[Success] Decrementation";
        LEDBuzzer(&MonLecteur, LED_YELLOW_OFF);
    }
    else
        qDebug() << "[FAIL] Decrementation";
        LEDBuzzer(&MonLecteur, LED_RED_OFF);
}
