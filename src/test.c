
#include "sha3.h"
#include "utils.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct test_vector {
    int len;
    unsigned char *msg;
    unsigned char *md;
} test_vector;

static unsigned char test_vec_0[] = {0x00};

static unsigned char test_vec_0_ret[] = { 0xA7, 0xFF, 0xC6, 0xF8, 0xBF, 0x1E, 0xD7, 
                                 0x66, 0x51, 0xC1, 0x47, 0x56, 0xA0, 0x61, 
                                 0xD6, 0x62, 0xF5, 0x80, 0xFF, 0x4D, 0xE4, 
                                 0x3B, 0x49, 0xFA, 0x82, 0xD8, 0x0A, 0x4B, 
                                 0x80, 0xF8, 0x43, 0x4A};

/* 1 byte */
static unsigned char test_vec_5[] = { 0x19};

static unsigned char test_vec_5_ret[] = { 0x7B, 0x00, 0x47, 0xCF, 0x5A, 0x45, 0x68,
                                 0x82, 0x36, 0x3C, 0xBF, 0x0F, 0xB0, 0x53,
                                 0x22, 0xCF, 0x65, 0xF4, 0xB7, 0x05, 0x9A,
                                 0x46, 0x36, 0x5E, 0x83, 0x01, 0x32, 0xE3,
                                 0xB5, 0xD9, 0x57, 0xAF};

/* abc */
static unsigned char test_vec_abc[] = {0x61, 0x62, 0x63};

static unsigned char test_vec_abc_ret[] = {0x3a, 0x98, 0x5d, 0xa7, 0x4f, 0xe2, 0x25,
                                0xb2, 0x04, 0x5c, 0x17, 0x2d, 0x6b, 0xd3,
                                0x90, 0xbd, 0x85, 0x5f, 0x08, 0x6e, 0x3e,
                                0x9d, 0x52, 0x5b, 0x46, 0xbf, 0xe2, 0x45,
                                0x11, 0x43, 0x15, 0x32};

/* 4 bytes */
static unsigned char test_vec_30[] = { 0xa6, 0xb7, 0x86, 0x1a};

static unsigned char test_vec_30_ret[] = { 0xC8, 0x24, 0x2F, 0xEF, 0x40, 0x9E, 0x5A, 
                                  0xE9, 0xD1, 0xF1, 0xC8, 0x57, 0xAE, 0x4D, 
                                  0xC6, 0x24, 0xB9, 0x2B, 0x19, 0x80, 0x9F, 
                                  0x62, 0xAA, 0x8C, 0x07, 0x41, 0x1C, 0x54, 
                                  0xA0, 0x78, 0xB1, 0xD0};

/* 200 bytes */
static unsigned char test_vec_1600[] = { 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 
                                0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5, 0xc5 };

static unsigned char test_vec_1600_ret[] = { 0x79, 0xF3, 0x8A, 0xDE, 0xC5, 0xC2, 0x03,
                                    0x07, 0xA9, 0x8E, 0xF7, 0x6E, 0x83, 0x24, 
                                    0xAF, 0xBF, 0xD4, 0x6C, 0xFD, 0x81, 0xB2, 
                                    0x2E, 0x39, 0x73, 0xC6, 0x5F, 0xA1, 0xBD, 
                                    0x9D, 0xE3, 0x17, 0x87};

/* 201 bytes */
static unsigned char test_vec_1605[] = { 0x18, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 
                                0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8, 0xb8 };

static unsigned char test_vec_1605_ret[] = { 0x81, 0xEE, 0x76, 0x9B, 0xED, 0x09, 0x50,
                                    0x86, 0x2B, 0x1D, 0xDD, 0xED, 0x2E, 0x84,
                                    0xAA, 0xA6, 0xAB, 0x7B, 0xFD, 0xD3, 0xCE,
                                    0xAA, 0x47, 0x1B, 0xE3, 0x11, 0x63, 0xD4,
                                    0x03, 0x36, 0x36, 0x3C};

static unsigned char chartoint(unsigned char byte) {
    if ((byte > 0x2f) & (byte < 0x3a)) {
        return (unsigned char) ((int) byte - 48);
    } else if ((byte > 0x40) && (byte < 0x47)) {
        return (unsigned char) ((int) byte - 55);
    } else if ((byte > 0x61) && (byte < 0x67)) {
        return (unsigned char) ((int) byte - 87);
    }
    return 0x00;
}

static unsigned char inttochar(unsigned int x) {
    if ((0 <= x) & (9 > x)) {
        return (unsigned char) (x + 48);
    } else {
        return 0x00;
    }
}

static int sha3_256_run_test(unsigned char *vector, int len, unsigned char *ans) {
    unsigned char *t;
    t = sha3_256(vector, len);
    printf("\n%d, Hex: 0x", len);
    for (int i=31; i>=0; i--) {printf("%02x", t[i]);}
    puts("");
    for (int i=31; i>=0; i--) {printf("%02x", ans[i]);}
    puts("");
    int response = hex_check((char *) t, (char *) ans, 32);
    free(t);
    return response;
}

static unsigned int slen(const unsigned char *x) {
    unsigned int i = 0;
    while (x[i] != 0x00) {
        i++;
    }
    return i;
}

static int strcmp(const unsigned char *x, const unsigned char *y) {
    unsigned int length = slen(x) - 1;

    int i = 0;
    while (i < length) {
        if (x[i] != y[i]) return 0; 
        i++;
    }
    return 1;
}

static test_vector *get_vec_from_file(char *file) {
    FILE *fp = fopen(file, "r");
    unsigned char *src;
    unsigned char **tests;
    int ccnt = 0;
    int lcnt = 0;

    if (fp == NULL) {
        puts("Error opening file");
        return NULL;
    }

    if (fseek(fp, 0L, SEEK_END) == 0) {
        long long int buff_s = ftell(fp);
        if (buff_s == -1) {
            fclose(fp);
            return NULL;
        }

        src = malloc(sizeof(char) * (buff_s + 1));

        if (src == NULL) {
            free(src);
            fclose(fp);
            return NULL;
        }

        if (fseek(fp, 0L, SEEK_SET) != 0) {
            fclose(fp);
            return NULL;
        }

        size_t newLen = fread(src, sizeof(char), buff_s, fp);
        if (ferror(fp) != 0) {
            fputs("Error reading file", stdout);
        } else {
            src[++newLen] = 0x00;
        }
    }
    fclose(fp);

    while (src[ccnt] != 0x00 ) {
        if (src[ccnt] == 0xa &&
            (strcmp("Len", &src[ccnt + 1]) ||
             strcmp("Msg", &src[ccnt + 1]) ||
             strcmp("MD", &src[ccnt + 1]))) {
            lcnt++;
        }
        ccnt++;
    }

    int *line_len = calloc(sizeof(int), lcnt);

    if (line_len == NULL) {
        free(line_len);
        return NULL;
    }

    for (int i=0; i < lcnt; i++) {
        line_len[i] = 0;
    }

    int liter = 0;
    int iter = 0;
    int ind = 0;
    int j = 0;
    while (iter <= ccnt && liter <= lcnt) {
        if (src[iter] == 0xa && 
            (strcmp("Len", &src[iter + 1]) ||
             strcmp("Msg", &src[iter + 1]) ||
             strcmp("MD", &src[iter + 1]))) {
            line_len[liter] = 0;
            j = iter+1;
            while (src[j] != 0xa) {
                line_len[liter]++;
                j++;
            }
            liter++;
        }
        iter++;
    }
    printf("line count actual: %d\nline count recorded: %d\n\
Character Count actual: %d\nCharacter count recorded: %d\n", lcnt, liter, ccnt, iter);

    unsigned char **test_vectors = malloc(lcnt);

    if (test_vectors == NULL) {
        free(test_vectors);
        free(line_len);
        free(src);
        return NULL;
    }

    for (int i=0; i < lcnt; i++) {
        test_vectors[i] = malloc(line_len[i] + 1);
        if (test_vectors[i] == NULL) {
            free(test_vectors[i]);
            free(test_vectors);
            free(line_len);
            free(src);
            return NULL;
        }
    }

    liter = 0;
    iter = 0;
    ind = 0;
    int back = 0;
    while (iter <= ccnt && liter <= lcnt) {
        if (src[iter] == 0xa && 
            (strcmp("Len", &src[iter + 1]) ||
             strcmp("Msg", &src[iter + 1]) ||
             strcmp("MD", &src[iter + 1]))) {

            if (strcmp("Len", &src[iter + 1]) || strcmp("Msg", &src[iter+1])) {
                back = 3;
            } else {
                back = 2;
            }

            for (int i=0; i < line_len[liter] + 1 - back - 4; i++) {
                test_vectors[liter][i] = src[iter + i + back+4];
            }
            test_vectors[liter][line_len[liter] + 1 - back - 4] = 0x00;
            printf("LINE :: %s\n", test_vectors[liter]);
            liter++;
        }
        iter++;
    }
    free(src);

    for (int i=0; i < liter;i++) {
        printf("%s", test_vectors[liter]);
    }
    for (int i =0; i < liter; i++ ) {
        free(test_vectors[liter]);
    }
    free(test_vectors);
    free(line_len);

    return NULL;
}

int main(int argc, char *argv[]) {
    unsigned char *t0;
    unsigned char *t5;
    unsigned char *t30;
    unsigned char *t1600;
    unsigned char *t1605;
    unsigned char *tabc;

    test_vector *x = get_vec_from_file(".\\test_vectors\\SHA3_256ShortMsg.rsp");
    free(x);
    return 0;

    t0 = sha3_256(test_vec_0, 0); 
    printf("\n0 Hex: 0x");
    for (int i=31;i>=0;i--) {printf("%02x", t0[i]);}
    printf("\n");
    printf("0 Ans: 0x");
    for (int i=31;i>=0;i--) {printf("%02x", test_vec_0_ret[i]);}
    printf("\n");
    hex_check((char *) t0, (char *) test_vec_0_ret, 32);
    free(t0);

    tabc = sha3_256(test_vec_abc, 0); 
    printf("\n0 Hex: 0x");
    for (int i=31;i>=0;i--) {printf("%02x", tabc[i]);}
    printf("\n");
    printf("0 Ans: 0x");
    for (int i=31;i>=0;i--) {printf("%02x", test_vec_0_ret[i]);}
    printf("\n");
    hex_check((char *) tabc, (char *) test_vec_abc_ret, 32);
    free(tabc);

    t5 = sha3_256(test_vec_5, 5); 
    printf("\n5 Hex: 0x");
    for (int i=31;i>=0;i--) {printf("%02x", t5[i]);}
    printf("\n");
    printf("5 Ans: 0x");
    for (int i=31;i>=0;i--) {printf("%02x", test_vec_5_ret[i]);}
    printf("\n");
    hex_check((char *) t5, (char *) test_vec_5_ret, 32);
    free(t5);

    t30 = sha3_256(test_vec_30, 30); 
    printf("\n30 Hex: 0x");
    for (int i=31;i>=0;i--) {printf("%02x", t30[i]);}
    printf("\n");
    printf("30 Ans: 0x");
    for (int i=31;i>=0;i--) {printf("%02x", test_vec_30_ret[i]);}
    printf("\n");
    hex_check((char *) t30, (char *) test_vec_30_ret, 32);
    free(t30);

    t1600 = sha3_256(test_vec_1600, 1600); 
    printf("\n1600 Hex: 0x");
    for (int i=31;i>=0;i--) {printf("%02x", t1600[i]);}
    printf("\n");
    printf("1600 Ans: 0x");
    for (int i=31;i>=0;i--) {printf("%02x", test_vec_1600_ret[i]);}
    printf("\n");
    hex_check((char *) t1600, (char *) test_vec_1600_ret, 32);
    free(t1600);

    t1605 = sha3_256(test_vec_1605, 1605); 
    printf("\n1605 Hex: 0x");
    for (int i=31;i>=0;i--) {printf("%02x", t1605[i]);}
    printf("\n");
    printf("1605 Ans: 0x");
    for (int i=31;i>=0;i--) {printf("%02x", test_vec_1605_ret[i]);}
    printf("\n");
    hex_check((char *) t1605, (char *) test_vec_0_ret, 32);
    free(t1605);

    return 0;
}
