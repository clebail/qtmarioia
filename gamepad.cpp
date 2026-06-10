#include <QtDebug>
#include "gamepad.h"
#include "suinput.h"

#define NB_KEY      12
#define NB_DIR      4

static Gamepad *instance = nullptr;

Gamepad *Gamepad::getInstance() {
    if(instance == nullptr) {
        instance = new Gamepad();
    }

    return instance;
}

Gamepad::Gamepad() {
    int i;
    int keys[NB_KEY] = {BTN_A, BTN_B, BTN_X, BTN_Y, BTN_TL, BTN_TR, BTN_START, BTN_SELECT,
                        BTN_DPAD_UP, BTN_DPAD_DOWN, BTN_DPAD_LEFT, BTN_DPAD_RIGHT};
    int dirs[NB_DIR] = {ABS_X, ABS_Y, ABS_RX, ABS_RY};
    struct uinput_user_dev user_dev;

    memset(&user_dev, 0, sizeof(struct uinput_user_dev));
    strcpy(user_dev.name, "My gamepad");

    user_dev.absmax[ABS_X] = 512;
    user_dev.absmin[ABS_X] = -512;
    user_dev.absfuzz[ABS_X] = 0;
    user_dev.absflat[ABS_X] = 15;

    user_dev.absmax[ABS_Y] = 512;
    user_dev.absmin[ABS_Y] = -512;
    user_dev.absfuzz[ABS_Y] = 0;
    user_dev.absflat[ABS_Y] = 15;

    user_dev.absmax[ABS_RX] = 512;
    user_dev.absmin[ABS_RX] = -512;
    user_dev.absfuzz[ABS_RX] = 0;
    user_dev.absflat[ABS_RX] = 16;

    user_dev.absmax[ABS_RY] = 512;
    user_dev.absmin[ABS_RY] = -512;
    user_dev.absfuzz[ABS_RY] = 0;
    user_dev.absflat[ABS_RY] = 16;

    fd = suinput_open();

    if (fd != -1) {
        for (i = 0; i < NB_KEY; ++i) {
            suinput_enable_event(fd, EV_KEY, keys[i]);
        }

        for (i = 0; i < NB_DIR; ++i) {
            suinput_enable_event(fd, EV_ABS, dirs[i]);
        }

        suinput_create(fd, &user_dev);
    }

    qDebug() << fd;
}

bool Gamepad::sendPress(__u16 code) {
    if(fd != -1) {
        suinput_emit(fd, EV_KEY, code, 1);
        suinput_syn(fd);
        return true;
    }
    return false;
}

bool Gamepad::sendRelease(__u16 code) {
    if(fd != -1) {
        suinput_emit(fd, EV_KEY, code, 0);
        suinput_syn(fd);
        return true;
    }
    return false;
}

bool Gamepad::send(__u16 code) {
    if(fd != -1) {
        suinput_emit(fd, EV_KEY, code, 1);
        suinput_syn(fd);
        usleep(16000); // hold for ~1 snes9x frame so SDL sees the press
        suinput_emit(fd, EV_KEY, code, 0);
        suinput_syn(fd);

        qDebug() << "send" << code;

        return true;
    }

    return false;
}
