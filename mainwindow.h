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
    QTimer *timer, *captureTimer;
    WId snes9xWid;
private slots:
    void on_pbA_pressed();
    void on_pbA_released();
    void on_pbB_pressed();
    void on_pbB_released();
    void on_pbX_pressed();
    void on_pbX_released();
    void on_pbY_pressed();
    void on_pbY_released();
    void on_pbTL_pressed();
    void on_pbTL_released();
    void on_pbTR_pressed();
    void on_pbTR_released();
    void on_pbStart_pressed();
    void on_pbStart_released();
    void on_pbSelect_pressed();
    void on_pbSelect_released();

    void on_pbUp_pressed();
    void on_pbUp_released();
    void on_pbDown_pressed();
    void on_pbDown_released();
    void on_pbLeft_pressed();
    void on_pbLeft_released();
    void on_pbRight_pressed();
    void on_pbRight_released();

    void on_pbSnes9x_clicked();

    void onTimer();
    void capture();
};
#endif // MAINWINDOW_H
