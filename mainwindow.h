#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "ui_mainwindow.h"
#include "gamepad.h"

class MainWindow : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Gamepad *gamepad;
    QTimer *timer;
private slots:
    void on_pbA_clicked();
    void on_pbB_clicked();
    void on_pbX_clicked();
    void on_pbY_clicked();
    void on_pbTL_clicked();
    void on_pbTR_clicked();
    void on_pbStart_clicked();
    void on_pbSelect_clicked();

    void on_pbUp_pressed();
    void on_pbUp_released();
    void on_pbDown_pressed();
    void on_pbDown_released();
    void on_pbLeft_pressed();
    void on_pbLeft_released();
    void on_pbRight_pressed();
    void on_pbRight_released();

    void onTimer();
};
#endif // MAINWINDOW_H
