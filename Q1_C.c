#include <stdio.h>

#define INIT      0b0000
#define IDLE      0b0001
#define TESTING   0b0010
#define READY     0b0011
#define LAUNCH    0b0100
#define MAX_H     0b0101
#define DESCENT   0b0110
#define PARA      0b0111
#define LANDED    0b1000
#define RECOVERY  0b1001
#define SHUTDOWN  0b1010

int ok = 0;
int cmd_test = 0;
int tests = 0;
int reboot = 0;
int accl = 0;
int max_alt = 0;
int altitude = 0;
int pressure = 0;
int recovered = 0;
int poweroff = 0;

void outputs(int pwr, int gun, int tel, int bcn, int loc, int data, int vid, int para, int test, int rdy) {
    printf("PWR:%d GUN:%d TEL:%d BCN:%d LOC:%d DAT:%d VID:%d PAR:%d TST:%d RDY:%d\n", 
           pwr, gun, tel, bcn, loc, data, vid, para, test, rdy);
}

int main() {
    int state = INIT;

    printf("--- OBC SYSTEM STARTED ---\n");

    while (state != SHUTDOWN) {

        switch (state) {

            case INIT:
                outputs(1, 1, 1, 0, 0, 0, 0, 0, 0, 0);

                if (ok) {
                    state = IDLE;
                }
                break;

            case IDLE:
                outputs(1, 0, 1, 0, 0, 0, 0, 0, 0, 0);

                if (cmd_test) {
                    state = TESTING;
                } else if (accl) {
                    state = LAUNCH;
                }
                break;

            case TESTING:
                outputs(1, 1, 1, 0, 0, 1, 0, 0, 1, 0);

                if (tests) {
                    state = READY;
                } else {
                    state = INIT;
                }
                break;

            case READY:
                outputs(1, 0, 1, 0, 0, 0, 0, 0, 0, 1);

                if (reboot && tests) {
                    state = INIT;
                }
                break;

            case LAUNCH:
                outputs(1, 0, 1, 1, 0, 0, 0, 0, 0, 0);

                if (max_alt) {
                    state = MAX_H;
                }
                break;

            case MAX_H:
                outputs(1, 1, 1, 1, 0, 0, 0, 0, 0, 0);

                if (max_alt && accl) {
                    state = DESCENT;
                }
                break;

            case DESCENT:
                outputs(1, 1, 1, 1, 0, 1, 1, 0, 0, 0);

                if (altitude) {
                    state = PARA;
                }
                break;

            case PARA:
                outputs(1, 1, 1, 1, 1, 1, 1, 1, 0, 0);

                if (pressure) {
                    state = LANDED;
                }
                break;

            case LANDED:
                outputs(1, 0, 0, 1, 1, 0, 0, 0, 0, 0);

                if (recovered) {
                    state = RECOVERY;
                }
                break;

            case RECOVERY:
                outputs(1, 0, 0, 0, 1, 0, 0, 0, 0, 0);

                if (poweroff) {
                    state = SHUTDOWN;
                }
                break;

            default:
                state = SHUTDOWN;
                break;
        }

        break;
    }

    if (state == SHUTDOWN) {
        outputs(0, 0, 0, 0, 1, 0, 0, 0, 0, 0);
    }

    return 0;
}
