#include <QtDebug>
#include <QScreen>
#include "mainwindow.h"
#include <X11/Xlib.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/Xutil.h>

static QString getWindowName(Display *display, Window w) {
    Atom netWmName = XInternAtom(display, "_NET_WM_NAME", False);
    Atom utf8String = XInternAtom(display, "UTF8_STRING", False);
    Atom type;
    int format;
    unsigned long nitems, bytes_after;
    unsigned char *data = nullptr;

    if (XGetWindowProperty(display, w, netWmName, 0, 1024, False,
                           utf8String, &type, &format, &nitems,
                           &bytes_after, &data) == Success && data) {
        QString name = QString::fromUtf8((char *)data);
        XFree(data);
        return name;
    }

    // fallback sur WM_NAME
    char *name = nullptr;
    if (XFetchName(display, w, &name) && name) {
        QString result = QString::fromLatin1(name);
        XFree(name);
        return result;
    }
    return {};
}

static WId findWindow(Display *display, Window root, const QString &title) {
    Window parent, *children;
    unsigned int n;
    WId result = 0;

    if (!XQueryTree(display, root, &root, &parent, &children, &n)) {
        fprintf(stderr, "XQueryTree 0");
        return 0;
    }

    for (unsigned int i = 0; i < n && !result; i++) {
        QString name = getWindowName(display, children[i]);
        if (!name.isEmpty()) {
            fprintf(stdout,"name : %s\n", name.toLatin1().constData());
            if (QString(name).contains(title, Qt::CaseInsensitive))
                result = children[i];
        }
        if (!result)
            result = findWindow(display, children[i], title);
    }

    XFree(children);
    return result;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(this);

    gamepad = Gamepad::getInstance();

    timer = new QTimer(this);
    timer->setInterval(300);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    timer->start();

    captureTimer = new QTimer(this);
    captureTimer->setInterval(16);
    connect(captureTimer, SIGNAL(timeout()), this, SLOT(capture()));
    captureTimer->start();
}

MainWindow::~MainWindow() {
    delete timer;
}

void MainWindow::on_pbA_pressed()      { gamepad->sendPress(BTN_A); }
void MainWindow::on_pbA_released()     { gamepad->sendRelease(BTN_A); }

void MainWindow::on_pbB_pressed()      { gamepad->sendPress(BTN_B); }
void MainWindow::on_pbB_released()     { gamepad->sendRelease(BTN_B); }

void MainWindow::on_pbX_pressed()      { gamepad->sendPress(BTN_X); }
void MainWindow::on_pbX_released()     { gamepad->sendRelease(BTN_X); }

void MainWindow::on_pbY_pressed()      { gamepad->sendPress(BTN_Y); }
void MainWindow::on_pbY_released()     { gamepad->sendRelease(BTN_Y); }

void MainWindow::on_pbTL_pressed()     { gamepad->sendPress(BTN_TL); }
void MainWindow::on_pbTL_released()    { gamepad->sendRelease(BTN_TL); }

void MainWindow::on_pbTR_pressed()     { gamepad->sendPress(BTN_TR); }
void MainWindow::on_pbTR_released()    { gamepad->sendRelease(BTN_TR); }

void MainWindow::on_pbStart_pressed()  { gamepad->sendPress(BTN_START); }
void MainWindow::on_pbStart_released() { gamepad->sendRelease(BTN_START); }

void MainWindow::on_pbSelect_pressed() { gamepad->sendPress(BTN_SELECT); }
void MainWindow::on_pbSelect_released(){ gamepad->sendRelease(BTN_SELECT); }

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
        gamepad->sendPress(BTN_A);
        gamepad->sendRelease(BTN_A);
    }

    if(cbB->checkState() == Qt::CheckState::Checked) {
        gamepad->sendPress(BTN_B);
        gamepad->sendRelease(BTN_B);
    }
}

void MainWindow::on_pbSnes9x_clicked() {
    Display *dpy = XOpenDisplay(nullptr);
    snes9xWid = findWindow(dpy, DefaultRootWindow(dpy), "Super Mario All-Stars (Europe) - Snes9x");
    XCloseDisplay(dpy);
}

void MainWindow::capture() {
    if(snes9xWid) {
        Display *dpy = XOpenDisplay(nullptr);
        XWindowAttributes attrs;
        XGetWindowAttributes(dpy, snes9xWid, &attrs);
        XCompositeRedirectWindow(dpy, snes9xWid, CompositeRedirectAutomatic);
        Pixmap pix = XCompositeNameWindowPixmap(dpy, snes9xWid);
        XImage *img = XGetImage(dpy, pix, 0, 0, attrs.width, attrs.height, AllPlanes, ZPixmap);
        if (img) {
            QImage frame((uchar *)img->data, attrs.width, attrs.height,
                         img->bytes_per_line, QImage::Format_RGB32);
            lblCapture->setPixmap(QPixmap::fromImage(
                frame.scaled(lblCapture->size(), Qt::KeepAspectRatio, Qt::FastTransformation)
            ));
            XDestroyImage(img);
        }
        XFreePixmap(dpy, pix);
        XCloseDisplay(dpy);
    }
}
