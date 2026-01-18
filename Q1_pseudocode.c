INIT      = 0000
IDLE      = 0001
TESTING   = 0010
READY     = 0011
LAUNCH    = 0100
MAX_H     = 0101
DESCENT   = 0110
PARA_DEP  = 0111
LANDED    = 1000
RECOVERY  = 1001
SHUTDOWN  = 1010

int outputs[10]; 
state = INIT;

while (state != SHUTDOWN) {

    switch (state) {

        case INIT:
            outputs = {1, 1, 1, 0, 0, 0, 0, 0, 0, 0};
            if (input == OK) state = IDLE;
            break;

        case IDLE:
            outputs = {1, 0, 1, 0, 0, 0, 0, 0, 0, 0};
            if (input == CMD_TEST) state = TESTING;
            else if (input == ACCL) state = LAUNCH;
            break;

        case TESTING:
            outputs = {1, 1, 1, 0, 0, 1, 0, 0, 1, 0};
            if (TESTS == 1) state = READY;
            else if (TESTS == 0) state = INIT; 
            break;

        case READY:
            outputs = {1, 0, 1, 0, 0, 0, 0, 0, 0, 1};
            if (input == REBOOT && TESTS == 1) state = INIT;
            break;

        case LAUNCH:
            outputs = {1, 0, 1, 1, 0, 0, 0, 0, 0, 0};
            if (input == MAX_ALT) state = MAX_H;
            break;

        case MAX_H:
            outputs = {1, 1, 1, 1, 0, 0, 0, 0, 0, 0};
            if (input == MAX_ALT && input == ACCL) state = DESCENT;
            break;

        case DESCENT:
            outputs = {1, 1, 1, 1, 0, 1, 1, 0, 0, 0};
            if (input == ALTITUDE) state = PARA_DEP;
            break;

        case PARA_DEP:
            outputs = {1, 1, 1, 1, 1, 1, 1, 1, 0, 0};
            if (input == PRESSURE) state = LANDED;
            break;

        case LANDED:
            outputs = {1, 0, 0, 1, 1, 0, 0, 0, 0, 0};
            if (input == RECOVERED) state = RECOVERY;
            break;

        case RECOVERY:
            outputs = {1, 0, 0, 0, 1, 0, 0, 0, 0, 0};
            if (input == POWEROFF) state = SHUTDOWN;
            break;
            
        case SHUTDOWN:
            outputs = {0, 0, 0, 0, 1, 0, 0, 0, 0, 0};
            exit(); 
    }

    update_pins(outputs);
    delay(clock_cycle);
}
