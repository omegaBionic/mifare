#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_Connect_clicked();

    //void on_Saisie_clicked();

    void on_Quitter_clicked();

    //void on_Incrementer_clicked();

    //void on_Decrementer_clicked();

    //void on_Ecrire_clicked();

    void on_Lire_clicked();

    void on_Detect_clicked();

    //void on_lire_nom_clicked();

    //void on_lire_prenom_clicked();

    //void on_lire_compteur_clicked();

    void on_ecrire_nom_clicked();

    void on_ecrire_prenom_clicked();

    void on_ecrire_compteur_clicked();

    void on_plus_compteur_clicked();

    void on_minus_compteur_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
