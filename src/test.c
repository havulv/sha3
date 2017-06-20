
#include "sha3.h"
#include "utils.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#if (defined (_WIN32) || defined (_WIN64))
#define SEP 0x2f

#else
#define SEP 0x5c

#endif


typedef struct test_vector {
    int len;
    unsigned char *msg;
    unsigned char *md;
} test_vector;

static unsigned int slen(const unsigned char *x) {
    unsigned int i = 0;
    while (x[i] != 0x00) {
        i++;
    }
    return i;
}

static unsigned char chartoint(unsigned char byte) {
    if ((((int) byte) > 0x2f) & (((int) byte) < 0x3a)) {
        return (unsigned char) ((int) byte - 48);
    } else if ((byte > 0x40) && (byte < 0x47)) {
        return (unsigned char) ((int) byte - 55);
    } else if ((byte > 0x60) && (byte < 0x67)) {
        return (unsigned char) ((int) byte - 87);
    }
    return 0x00;
}

static unsigned char *strtoint(const unsigned char *bytes) {
    int i = 0;
    int j = 0;
    int length = slen(bytes);
    unsigned char *ret = malloc(sizeof(unsigned char) * ((length / 2) + (length % 2) + 1));


    if (ret == NULL) {
        free(ret);
        return NULL;
    }

    while (bytes[i] != 0x00) {
        switch (i % 2) {
            case 0:
                ret[j] = (chartoint(bytes[i]) << 4);
                break;
            case 1:
                ret[j] |= chartoint(bytes[i]);
                j++;
                break;
        }
        i++;
    }

    return ret;
}

static int mnpow(int a, int b) {
    int ret = 1;
    if (b == 0) return 1;
    while (b > 0) {
        ret = ret * a;
        b--;
    }

    return ret;
}

static unsigned char bytestoint(unsigned char *bytes) {
    int i = 0;
    int ret = 0;
    int len = slen(bytes) - 1;
    while (bytes[i] != 0x00) {
        ret += (mnpow(10, len - i) * (int) chartoint(bytes[i]));
        i++;
    }
    return ret;
}

static unsigned char inttochar(unsigned int x) {
    if ((0 <= x) & (9 > x)) {
        return (unsigned char) (x + 48);
    } else {
        return 0x00;
    }
}

static int sha3_256_run_test(test_vector *test, unsigned char *ans) {
    unsigned char *t;
    t = sha3_256(test->msg, test->len);
    printf("\n%d, Hex: 0x", test->len);
    for (int i=31; i>=0; i--) {printf("%02x", test->md[i]);}
    puts("");
    for (int i=31; i>=0; i--) {printf("%02x", ans[i]);}
    puts("");
    int response = hex_check((char *) ans, (char *) test->md, 32);
    free(t);
    return response;
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
    test_vector *tests;
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
    } else {
        fclose(fp);
        return NULL;
    }
    fclose(fp);


    puts("File read");
    while (src[ccnt] != 0x00 ) {
        if (src[ccnt] == 0xa &&
            (strcmp("Len", &src[ccnt + 1]) ||
             strcmp("Msg", &src[ccnt + 1]) ||
             strcmp("MD", &src[ccnt + 1]))) {
            lcnt++;
        }
        ccnt++;
    }

    if ((lcnt % 3) != 0) {
        printf(\
"The line count is not divisible by 3 and thus doesn't match \
the test vector specification. Please use the correct file.\
The line count is %d", lcnt);
        free(src);
        return NULL;
    }

    puts("Lines counted");
    int *line_len = calloc(sizeof(int), lcnt + 1);

    if (line_len == NULL) {
        free(line_len);
        return NULL;
    }

    for (int i=0; i <= lcnt; i++) {
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

    puts("Line lengths counted");
    unsigned char **test_vectors = malloc( sizeof(char *) * (lcnt+2));

    if (test_vectors == NULL) {
        free(test_vectors);
        free(line_len);
        free(src);
        return NULL;
    }

    for (int i=0; i <= lcnt; i++) {
        test_vectors[i] = malloc(line_len[i] + 1);
        if (test_vectors[i] == NULL) {
            free(test_vectors[i]);
            free(test_vectors);
            free(line_len);
            free(src);
            return NULL;
        }
    }


    puts("Vectors accounted for");
    liter = 0;
    iter = 0;
    ind = 0;
    int back = 0;
    while ((iter < ccnt) && (liter <= lcnt)) {
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
            liter++;
        }
        iter++;
    }

    test_vectors[liter] = NULL;
    free(line_len);
    free(src);

    puts("Allocating structs for test vectors");

    tests = calloc(((lcnt / 3) + 1), sizeof(test_vector *));

    if (tests == NULL) {
        for (int i=0; i < lcnt; i++) {
            free(test_vectors[i]);
        }
        free(test_vectors);
        free(tests);
        return NULL;
    }

    puts("Placing test vectors into structs");

    printf("total lines = %d\n", lcnt);
    j = 0;
    for (int i=0; i < lcnt; i++) {
        switch (i % 3) {
            case 0: {
                tests[j].len = bytestoint(test_vectors[i]);
                } break;
            case 1: {
                tests[j].msg = strtoint(test_vectors[i]);
                } break;
            case 2: {
                tests[j].md = strtoint(test_vectors[i]);
                j++;
                } break;
        }
    }

    puts("Test Vectors allocated into structs");

    for (int i=0; i <= lcnt;i++) {
        printf("freeing 0x%p %s :: ", test_vectors[i], test_vectors[i]);
        free(test_vectors[i]);
        printf("%d :: %d (freed) \n", i, lcnt);
    }
    puts("here?");

    free(test_vectors);

    return tests;
}

int main(int argc, char *argv[]) {
    unsigned char *t0;

    char *file = ".\\test_vectors\\sha3_256shortmsg.rsp";

    char sep_test[2] = {SEP, 0x00};
    int z = 0;
    while (file[z] != 0x00) {
        if ((file[z] == '/') || (file[z] == '\\')) {
            file[z] = SEP;
        }
        z++;
    }


    test_vector *x = get_vec_from_file(file);
    free(x);
    return 0;

//    t0 = sha3_256(test_vec_0, 0); 
//    printf("\n0 Hex: 0x");
//    for (int i=31;i>=0;i--) {printf("%02x", t0[i]);}
//    printf("\n");
//    printf("0 Ans: 0x");
//    for (int i=31;i>=0;i--) {printf("%02x", test_vec_0_ret[i]);}
//    printf("\n");
//    hex_check((char *) t0, (char *) test_vec_0_ret, 32);
//    free(t0);
//    return 0;
}
