
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"


#if (defined (_WIN32) || defined (_WIN64))
#include <windows.h>

#define COLOR(string, val) {\
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);\
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;\
    WORD saved_attributes;\
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);\
    saved_attributes = consoleInfo.wAttributes;\
    SetConsoleTextAttribute(hConsole, val);\
    printf(string);\
    SetConsoleTextAttribute(hConsole, saved_attributes);\
    }

#define RED(string) { COLOR(string, FOREGROUND_RED) }
#define BLUE(string) { COLOR(string, FOREGROUND_BLUE) }
#define GREEN(string) { COLOR(string, 0x2) }
#define INTENSE(string) { COLOR(string, FOREGROUND_INTENSITY) }

#else

#define RED(a)         printf("\x1b[31m%s\x1b[0m", a);
#define GREEN(a)         printf("\x1b[32m%s\x1b[0m", a);
#define BLUE(a)         printf("\x1b[34m%s\x1b[0m", a);
#define INTENSE(a)         printf("\x1b[21m%s\x1b[0m", a);

#endif

/* Assumes that dest has enough space, use with care */
static void datacat(char *dest, char *src, size_t dsize) {
    for (int i = 0; i < dsize; i++) {
        dest[i] = src[i];
    }
}

extern void pointer_dump(char *data, size_t dsize) {
    int i = 0;
    while (i < dsize) {
        printf("%02x %07llx | ", 
            (unsigned char) *(data+i), (unsigned long long) (data+i));
        if (i % 8 == 7 || i+1 == dsize) {
            char tmp[9];

            if (i+1 == dsize) {
                int k = i;
                while (k % 8 < 7) {

                    printf("00 ------- | ");
                    k += 1;
                }
            }

            datacat(tmp, data+i-(i % 8), (i % 8)+1);
            tmp[8] = '\0';
            for (int m = 0; m < 8; m++) {
                if (m < i % 8 + 1) {
                    tmp[m] =  tmp[m] < 0x20 || tmp[m] == 0x7f? 0x2e: tmp[m];
                } else {
                    tmp[m] = 0x20;
                }
            }
            
            printf("|  %s\n", tmp);
        }
        i++;
    }
}

static void reverse(char *data, size_t dsize) {
    for (int i =0; i < dsize/2; i++) {
        char tmp = data[i];
        data[i] = data[dsize-i-1];
        data[dsize-i-1] = tmp;
    }
}

extern void r_pointer_dump(char *data, size_t dsize) {
    int i = dsize-1;
    while (i >= 0) {
        if (dsize == i+1) {
            int k = i;
            while (k % 8 < 7) {
                printf("00 ------- | ");
                k += 1;
            }
        }
        printf("%02x %07llx | ", 
            (unsigned char) *(data+i), (unsigned long long) (data+i));
        if (i % 8 == 0) {
            char tmp[9];
        
            if (8 > dsize - i ) {
                datacat(tmp, data+i, (dsize - i) % 8 );
                for (int m = 0; m < 8; m++) {
                    if (m < (dsize - i) % 8) {
                        tmp[m] =  tmp[m] < 0x20 || tmp[m] == 0x7f? 0x2e: tmp[m];
                    } else {
                        tmp[m] = 0x20;
                    }
                }
            } else {
                datacat(tmp, data+i, 8);
                for (int m = 0; m < 8; m++) {
                    tmp[m] =  tmp[m] < 0x20 || tmp[m] == 0x7f? 0x2e: tmp[m];
                }
            }
            reverse(tmp, 8);
            tmp[8] = '\0';
            
            printf("|  %s\n", tmp);
        }
        i--;
    }   
}

extern void hex_dump(char *data, size_t dsize) {
    int i = 0;
    while (i < dsize) {
        printf("%02x    ", (unsigned char) *(data + i));
        if (i % 8 == 7 || i+1 == dsize) {
            char tmp[9];

            if (i+1 == dsize) {
                int k = i;
                while (k % 8 < 7) {
                    printf("00    ");
                    k += 1;
                }
            }

            datacat(tmp, data+i-(i % 8), (i % 8)+1);
            tmp[8] = '\0';
            for (int m = 0; m < 8; m++) {
                tmp[m] =  tmp[m] < 0x20 || tmp[m] == 0x7f? 0x2e: tmp[m];
            }
            printf("|  %s\n", tmp);
        }
        i++;
    }
}

/* Takes twice as much tmp memory (and a little more) than hex_dump 
 * for the checking. This should all really be done in unsigned chars
 * but it isn't right now. So get over it.*/
extern int hex_check(char *expec, char *actual, size_t exsize) {
    int i = 0;
    char hexfmt[10];
    int correct = 0;
    int ret = 1; /* return true */
    while (i < exsize) {
        sprintf(hexfmt, "%02x-%02x ",
            (unsigned char) *(expec+i), (unsigned char) *(actual+i));
        if (expec[i] == actual[i]) {
            GREEN(hexfmt)
        } else {
            /* return false, something not right */
            RED(hexfmt);
            ret = 0;
        }
        printf(" | ");
        if (i % 8 == 7 || i+1 == exsize) {
            char expectmp[9];
            char actuatmp[9];

            if (i+1 == exsize) {
                int k = i;
                while (k % 8 < 7) {
                    GREEN("00-00 ");
                    printf(" | ");
                    k += 1;
                }
            }
            
            datacat(expectmp, expec+i-(i % 8), (i % 8)+1);
            datacat(actuatmp, actual+i-(i % 8), (i % 8)+1);
            expectmp[8] = '\0';
            actuatmp[8] = '\0';

            for (int m = 0; m < 8; m++) {
                expectmp[m] = expectmp[m] < 0x20 || expectmp[m] == 0x7f? 0x2e: expectmp[m];
                actuatmp[m] = actuatmp[m] < 0x20 || actuatmp[m] == 0x7f? 0x2e: actuatmp[m];
            }

            printf("| ");
            for (int n = 0; n < 8; n++) {
                if (n < i % 8 + 1) {
                    char tmpout[3] = {expectmp[n], '\0'};
                    if (expectmp[n] != actuatmp[n]) {
                        RED(tmpout);
                        correct++;
                    } else {
                        GREEN(tmpout);
                    }
                } else {
                    printf(" ");
                }
            }
            printf(" - ");

            for (int n = 0; n < i % 8 + 1; n++) {
                if (n < i % 8 + 1) {
                    char tmpout[3] = {actuatmp[n], '\0'};
                    if (expectmp[n] != actuatmp[n]) {
                        RED(tmpout);
                        correct++;
                    } else {
                        GREEN(tmpout);
                    }
                } else { 
                    printf(" ");
                }
            }
            printf("\n%d bytes are different from one another\n", correct);
            correct = 0;
        }
        i++;
    }
    return ret;
}
