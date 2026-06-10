#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>

class Gamepad {
public:
    static Gamepad *getInstance();

    bool send(__u16 code);
    bool sendPress(__u16 code);
    bool sendRelease(__u16 code);
private:
    int fd;
    bool ok;

    Gamepad();
};

#endif // GAMEPAD_H
