
#include "sha3.h"
#include "utils.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#if (defined (_WIN32) || defined (_WIN64))
#define SEP 0x5c

#else
#define SEP 0x2f

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

static int strcmp(const unsigned char *x, const unsigned char *y) {
    unsigned int length = slen(x) - 1;

    int i = 0;
    while (i < length) {
        if (x[i] != y[i]) return 0; 
        i++;
    }
    return 1;
}

static test_vector **get_vec_from_file(char *file) {
    FILE *fp = fopen(file, "r");
    unsigned char *src;
    test_vector **tests;
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
            (strcmp((unsigned char *) "Len", &src[ccnt + 1]) ||
             strcmp((unsigned char *) "Msg", &src[ccnt + 1]) ||
             strcmp((unsigned char *) "MD", &src[ccnt + 1]))) {
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
    int j = 0;
    while (iter <= ccnt && liter <= lcnt) {
        if (src[iter] == 0xa && 
            (strcmp((unsigned char *) "Len", &src[iter + 1]) ||
             strcmp((unsigned char *) "Msg", &src[iter + 1]) ||
             strcmp((unsigned char *) "MD", &src[iter + 1]))) {
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
    int back = 0;
    while ((iter < ccnt) && (liter <= lcnt)) {
        if (src[iter] == 0xa && 
            (strcmp((unsigned char *) "Len", &src[iter + 1]) ||
             strcmp((unsigned char *) "Msg", &src[iter + 1]) ||
             strcmp((unsigned char *) "MD", &src[iter + 1]))) {

            if (strcmp((unsigned char *) "Len", &src[iter + 1]) ||
                strcmp((unsigned char *) "Msg", &src[iter+1])) {
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

    tests = malloc(((lcnt / 3) + 1) * sizeof(test_vector *));

    if (tests == NULL) {
        for (int i=0; i < lcnt; i++) {
            free(test_vectors[i]);
        }
        free(test_vectors);
        free(tests);
        return NULL;
    }

    for (int i=0; i < (lcnt / 3); i++) {
        tests[i] = malloc(sizeof(test_vector));
        if (tests[i] == NULL) {
            for (int z=0; z < lcnt; z++){
                free(test_vectors[z]);
            }
            free(test_vectors);
            free(tests);
            return NULL;
        }
    }

    tests[lcnt / 3] = NULL;

    puts("Placing test vectors into structs");

    for (int i=0; i < (lcnt / 3); i++) {
        tests[i]->len = bytestoint(test_vectors[i]);
        tests[i]->msg = strtoint(test_vectors[i+1]);
        tests[i]->md = strtoint(test_vectors[i+2]);
//
//        printf("0x");
//        printf("%s", test_vectors[i+1]);
//        puts("");
//
//        j = 0;
//        printf("0x");
//        while (j < tests[i]->len) {
//            printf("%02x", tests[i]->msg[j]);
//            j++;
//        }
//        puts("");
//
//        printf("0x");
//        printf("%s", test_vectors[i+2]);
//        puts("");
//
//        j = 0;
//        printf("0x");
//        while (j < 32) {
//            printf("%02x", tests[i]->md[j]);
//            j++;
//        }
//        puts("");
//
//        break;
    }

//    return NULL;

    puts("Test Vectors allocated into structs");

    for (int i=0; i <= lcnt;i++) {
        free(test_vectors[i]);
    }
    free(test_vectors);

    return tests;
}

static int sha3_256_run_test(test_vector *test, int print) {
    unsigned char *ans = sha3_256(test->msg, test->len);
    int response = 1;

    if (print) {
        printf("\n%d, Hex: 0x", test->len);
        for (int i=0; i < 32; i++) {printf("%02x", test->md[i]);}

        printf("\n%d, Hex: 0x", test->len);
        for (int i=0; i < 32; i++) {printf("%02x", ans[i]);}
        puts("");
    }

    if (print) {
        response = hex_check((char *) ans, (char *) test->md, 32);
    } else {
        for (int i=0; i < 32; i++) {
            if (ans[i] != test->md[i]) {
                response = 0;
                break;
            }
        }
    }
    free(ans);
    return response;
}


int main(int argc, char *argv[]) {
    char file[38] = ".\\test_vectors\\SHA3_256ShortMsg.rsp";

    int z = 0;
    while (file[z] != 0x00) {
        if ((file[z] == '/') || (file[z] == '\\')) {
            file[z] = SEP;
        }
        z++;
    }
    test_vector **x = get_vec_from_file(file);
    
    if (x != NULL) {
        int j = 0;
        int pass = 0;
        int prev = pass;
        while (x[j] != NULL) {
            prev = pass;
            pass += sha3_256_run_test(x[j], 0);
            
            if (!(pass - prev)) {
                sha3_256_run_test(x[j], 1);
                j += 2;
                break;
            }
            j++;
        }

        z = 0;
        while (x[z] != NULL) {
            free(x[z]);
            z++;
        }
        free(x);

        printf("%d tests passed. %d tests failed. %d tests run.\n", 
                pass, j - pass - 1, j-1);
    } else {
        printf("Error in getting the test vector\n");
        free(x);
    }
    return 0;
}
