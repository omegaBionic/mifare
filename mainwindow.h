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

    void on_Saisie_clicked();

    void on_Quitter_clicked();

    void on_Incrementer_clicked();

    void on_Decrementer_clicked();

    void on_Ecrire_clicked();

    void on_Lire_clicked();

    void on_Detect_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
