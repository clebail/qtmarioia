#include <QtDebug>
#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    gamepad = Gamepad::getInstance();
    timer = new QTimer(this);
    timer->setInterval(300);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start();
}

MainWindow::~MainWindow() {
    delete timer;
}

void MainWindow::on_pbA_clicked() {
    gamepad->send(BTN_A);
}

void MainWindow::on_pbB_clicked() {
    gamepad->send(BTN_B);
}

void MainWindow::on_pbX_clicked() {
    gamepad->send(BTN_X);
}

void MainWindow::on_pbY_clicked() {
    gamepad->send(BTN_Y);
}

void MainWindow::on_pbTL_clicked() {
    gamepad->send(BTN_TL);
}

void MainWindow::on_pbTR_clicked() {
    gamepad->send(BTN_TR);
}

void MainWindow::on_pbStart_clicked() {
    gamepad->send(BTN_START);
}

void MainWindow::on_pbSelect_clicked() {
    gamepad->send(BTN_SELECT);
}

void MainWindow::on_pbUp_pressed()    { gamepad->sendPress(BTN_DPAD_UP); }
void MainWindow::on_pbUp_released()   { gamepad->sendRelease(BTN_DPAD_UP); }

void MainWindow::on_pbDown_pressed()  { gamepad->sendPress(BTN_DPAD_DOWN); }
void MainWindow::on_pbDown_released() { gamepad->sendRelease(BTN_DPAD_DOWN); }

void MainWindow::on_pbLeft_pressed()  { gamepad->sendPress(BTN_DPAD_LEFT); }
void MainWindow::on_pbLeft_released() { gamepad->sendRelease(BTN_DPAD_LEFT); }

void MainWindow::on_pbRight_pressed()  { gamepad->sendPress(BTN_DPAD_RIGHT); }
void MainWindow::on_pbRight_released() { gamepad->sendRelease(BTN_DPAD_RIGHT); }

void MainWindow::onTimer() {
    if(cbA->checkState() == Qt::CheckState::Checked) {
        gamepad->send(BTN_A);
    }

    if(cbB->checkState() == Qt::CheckState::Checked) {
        gamepad->send(BTN_B);
    }
}
