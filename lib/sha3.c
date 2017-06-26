/*
 * A toy implementation of sha3
 *  Vulnerable to side-channel and timing attacks
 */

#include "sha3.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
#include <intrin.h>

int __inline clz(int val) {
    int leading_zero;
    if (!_BitScanReverse(&leading_zero, val)) {
        return 0;
    } else {
        return leading_zero;
    }
}

#else 
#define clz(x) (sizeof(int)*8) - __builtin_clz(x)

#endif

// TODO: Current assumption is that the base unit is a byte and not a bit. Change this.
//       Thus space is *4 of what it should be
//
// Arrays are done big-endian
//



static int mod(int a, int b) {
    int x = (a % b);
    return x < 0? x + b: x;
}

static int npow(int a, int b) {
    if (b == 0) return 1;

    int state = a;
    int cnt = b;
    while (cnt > 0) {
        state = a * state;
        cnt--;
    }
    return state;
}

static int nceil(int a, int b) {
    if (a % b > 0 ) {
        return (a / b + 1);
    } else {
        return ( a / b);
    }
}

/* Shifting explanation -- grab the byte, shift to the bit,
 * The mask is to clear all other bits and the last shift 
 * is to put it into the correct position */

/* s[0] is the highest bit, so you advance from the left to the right
 * in terms of computation. i.e. from 0 to n
 */

/* w is b/25, mp must be the same size as m which is w * 25*/
static unsigned char *theta(unsigned char *m, unsigned char *mp, int w) {
    for (int i = 0; i < (w * 25 / 8); i++) {mp[i] = 0x00;}
    unsigned char *C = malloc(nceil(5 * w, 8));

    if (C == NULL) {
        free(C);
        return NULL;
    }

    for (int i=0; i < nceil(5 * w, 8); i++) { C[i] = 0x00;}

    unsigned char *D = malloc(nceil(5 * w, 8));
    if (D == NULL) {
        free(D);
        return NULL;
    }

    for (int i=0; i < nceil(5 * w, 8); i++) { D[i] = 0x00;}

    for (int x=0; x < 5; x++) {
        for (int z=0; z < w; z++) {
            int pair = (x * w) + z;
            for (int y=0; y < 5; y++) {
                int bit = (w * (x+(y*5)) + z);
                C[pair / 8] ^= (-((m[bit / 8] >> (bit % 8)) & 1) 
                             ^ C[pair / 8]) & (1 << (pair % 8));
            /* C[pair / 8] ^= (-(((m[pair / 8] >> (pair % 8))
                         ^ (m[(w * (x+5) + z) / 8] >> ((w * (x+5) + z) % 8))
                         ^ (m[(w * (x+10) + z) / 8] >> ((w * (x+10) + z) % 8))
                         ^ (m[(w * (x+15) + z) / 8] >> ((w * (x+15) + z) % 8))
                         ^ (m[(w * (x+20) + z) / 8] >> ((w * (x+20) + z) % 8))) & 1)
                         ^ C[pair / 8]) & (1 << (pair % 8)); */
            }
        }
    }

    /* printf("Start D\n"); */

    for (int x=0; x < 5; x++) {
        for (int z=0; z < w; z++) {
            int pair = (x * w) + z;
            int C1 = mod(x-1, 5) * w + z;
            int C2 = mod(x+1, 5) * w + mod(z-1, w);
            /* printf("%03d = %01x ", pair, (((C[C1 / 8] >> (C1 % 8))
                         ^ (C[C2 / 8] >> (C2 % 8))) & 0x01)); */
            D[pair / 8] ^= (-(((C[C1 / 8] >> (C1 % 8))
                         ^ (C[C2 / 8] >> (C2 % 8))) & 1)
                         ^ D[pair / 8]) & (1 << (pair % 8));
            /* printf("|%02x| ", D[pair /8]);
            if ((z + 1) % 8 == 0) { printf("\n");} */
        }
    }

    /* printf("Start Assignment\n"); */

    for (int x=0; x < 5; x++) {
        for (int z=0; z < w; z++) {
            int pair = (x * w) + z;
            /* printf(" %03d = ", pair); */
            for (int y=0; y < 5; y++) {
                int bit = w * (5 * y + x) + z;
                /* printf("%01x", (((m[bit / 8] >> (bit % 8))
                             ^ (D[pair / 8] >> (pair % 8))) & 0x01)); */

                mp[bit / 8] ^= (-(((m[bit / 8] >> (bit % 8))
                             ^ (D[pair / 8] >> (pair % 8))) & 1) 
                             ^ mp[bit / 8]) & (1 << (bit % 8));
            }
            /* if ((z + 1) % 8 == 0) puts(""); */
        }
    }
    /* puts(""); */
    free(C);
    free(D);
    return mp;
}

static unsigned char *rho(unsigned char *m, unsigned char *mp, int w) {
    for (int i = 0; i < (w * 25 / 8); i++) {mp[i] = 0x00;}
    for (int z=0; z < w; z++) {
        mp[z / 8] |= ((m[z / 8] >> (z % 8)) & 1) << (z % 8); // covers the case where not 8 | w
    }
    int x = 1;
    int y = 0;
    for (int t=0; t < 23; t++) {
        for (int z=0; z < w; z++) {
            int ind = (w * (5 * y + x) + z);
            mp[ind / 8] |= ((m[(ind - mod(((t+1) * (t + 2)) / 2, w)) / 8]
                         >> ((ind - mod(((t+1) * (t + 2)) / 2, w)) % 8)
                         & 1) << (ind % 8));
        }
        int tmp = x;
        x = y;
        y = (2 * tmp + 3 * y) % 5;
    }
    return mp;
}

static unsigned char *pi(unsigned char *m, unsigned char *mp, int w) {
    for (int i = 0; i < (w * 25 / 8); i++) {mp[i] = 0x00;}
    for (int x=0; x < 5; x++) {
        for (int y=0; y < 5; y++) {
            for (int z=0; z < w; z++) {
                mp[(w * (5 * y + x) + z) / 8] |= ((m[(w * (5 * x + (x + 3 * y) % 5) + z) / 8]
                                               >> ((w * (5 * x + (x + 3 * y) % 5) + z) % 8)
                                               & 1) << ((w * (5 * y + x) + z) / 8));
            }
        }
    }
    return mp;
}

static unsigned char *chi(unsigned char *m, unsigned char *mp, int w) {
    for (int i = 0; i < (w * 25 / 8); i++) {mp[i] = 0x00;}
    for (int x=0; x < 5; x++) {
        for (int y=0; y < 5; y++) {
            for (int z=0; z < w; z++) {
                int ind = w * (5 * y + x) + z;
                mp[ind / 8] |= (((m[ind / 8] >> (ind % 8))
                             ^ ((m[(w * (5 * y + ((x + 1) % 5)) + z) / 8] 
                                >> ((w * (5 * y + ((x + 1) % 5)) + z) % 8)) 
                                ^ 1)) 
                             & (m[(w * (5 * y + ((x+2) % 5)) + z) / 8 ] 
                                >> ((w * (5 * y + ((x+2) % 5)) + z) % 8))
                             & 1) << (ind % 8);
            }
        }
    }
    return mp;
}

static unsigned char rc(int t) {
    int end = t % 255 < 0? (t % 255) + 255: (t % 255);
    if (!end) {
        return 1;
    } else {
        unsigned short R = 0x80;
        for (int i=1; i < end; i++) {
            R = R & 0x00ff;
            R |= (R ^ (R >> 8)) & 1;
            R |= (((R >> 4) ^ (R >> 8)) & 1) << 4;
            R |= (((R >> 5) ^ (R >> 8)) & 1) << 5;
            R |= (((R >> 6) ^ (R >> 8)) & 1) << 6;
            R >>= 1;
        }
        unsigned char ret = 0xff & R;
        return ret; 
    }
}

static unsigned char *iota(unsigned char *m, unsigned char *mp, int w, int ir) {
    for (int i = (w * 25) / 8; i >=0; i--) {mp[i] = 0x00;}
    int l = clz(w);
    for (int x=0; x < 5; x++) {
        for (int y=0; y < 5; y++) {
            for (int z=0; z < w; z++) {
                unsigned int ind = w * (5 * y + x) + z;
                mp[ind / 8] |= (m[ind / 8] >> (ind % 8)
                             & 1)
                             << (ind % 8);
            }
        }
    }

    unsigned char *RC = malloc(nceil(w, 8));
    if (RC == NULL) {
        free(RC);
        return NULL;
    }
    for (int i = 0; i < nceil(w, 8); i++) {
        RC[i] = 0x00;
    }

    for (int j = 0; j < l; j++) {
        RC[(npow(2, j) - 1) / 8] |= ((rc(j + 7 * ir) 
                                 & 1) << ((npow(2, j) - 1) % 8));
    }

    for (int z = 0; z < w; z++) {
        mp[z / 8] |= (((mp[z / 8] >> (z % 8)) 
                   ^ (RC[z / 8] >> (z % 8)))
                   & 1) << (z % 8);
    }
    free(RC);
    return mp;
}

/* I am doing this weird shift, because I want to avoid corrupting the process by
 * accidentally using the wrong space in memory for the operations */
static unsigned char *round(unsigned char *m, int w, int ir) {
    unsigned char *mp = malloc((w * 25) / 8);
    if (mp == NULL) {
        free(mp);
        return NULL;
    }
    for (int i=0; i < (w * 25) / 8; i++) { mp[i] = 0x00;}

    mp = theta(m, mp, w);

    printf("After theta:\n");
    for (int i=199; i >= 0; i--) {
        printf("%02x ", mp[i]);
        if ((200 - i) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");

    m = rho(mp, m, w);
    mp = chi(m, mp, w);
    m = iota(mp, m, w, ir);
    
    free(mp);
    return m;
}

static unsigned char *keccak_p(unsigned char *m, int b, int r) {

    for (int i=(12+2 * (clz(b/25))-r); 
            i < (12+2 * (clz(b/25)-1)); i++) {
        m = round(m, b/25, (12+2 * (32-clz(b/25))-r));
    }
    return m;
}

static unsigned char *keccak_f(unsigned char *m, int b) {
    return keccak_p(m, b, 12 + 2 * (clz(b/25)));
}

// Apparently this padding rule is wrong. It needs to be re written to follow Appendix section B
static unsigned char *pad10(int x, int m) {
    int j = (-m - 2) % x;
    j = j < 0? x + j: j;
    unsigned char *pad = malloc(nceil(j, 8));
    if (pad == NULL) {
        free(pad);
        return NULL;
    }
    for (int i=0; i < nceil(j, 8); i++) { pad[i] = 0x00;}
    pad[0] = 1;
    for (int i=1; i < j - 1; i++) {
        pad[i / 8] &= ~(1 << (i % 8));
    }
    pad[j / 8] |= 1 << (j % 8);
    return pad;
}

// m measured in bits -- it's up to the user to free data afterwards
static unsigned char *pad(unsigned char *data, int m, int r) {
    int q = (r / 8) - mod(m, (r / 8));
    unsigned char *ret = malloc((m / 8) + q);
    if (ret == NULL) {
        free(ret);
        return NULL;
    }
    for (int i=((m / 8) + q)-1; i >= 0; i--) { ret[i] = 0x00;}
    if ((m / 8) >= 1) {
        for (int i=((m / 8) + q)-1; i >= q; i--) { ret[i] = data[i];}
    }
    if (q) {
        ret[q-1] = 0x06;
        ret[0] |= 0x80;
    } 
    return ret;
}


/* KECCAK[512]  msze in bits*/
unsigned char *sha3_256(unsigned char *m, size_t msze) {
    unsigned char *P;
    if (!msze) {
        unsigned char tmp[200];
        for (int i=0; i < 200; i++) { tmp[i] = 0x00;}
        P = pad(tmp, 0, 1088);
    } else {
        P = pad(m, msze * 8, 1088);
    }

    int msize = msze + (136) - mod(msze * 8, 136);

//
//    int size = (int) newm_sze;
//    int j = (-size - 2) % (1088);
//    int adj_j = (j<0?1088+j:j);
//    unsigned int msize = size + adj_j;
//    unsigned char *P = malloc(nceil(msize, 8));
//    for (int i = 0; i < nceil(msize, 8); i++) {  //Need to remember to clear new allocations
//        P[i] = 0x00;
//    }
//
//    unsigned char *tmppad = pad10(1088, size);
//
//    /* The bit sets are done instead of a byte set in the case that the bits lie
//     * unevenly in the array */
//    for (int ind=msize; ind > msize - size; ind--) {
//        P[ind / 8] |= (newm[(ind - adj_j) / 8] >> ((ind - adj_j) % 8) 
//                    & 1) << (ind % 8);
//    }
//    for (int ind=(msize - size); ind >= 0; ind--) {
//        P[ind / 8] |= (tmppad[ind / 8] >> (ind % 8) 
//                    & 1) << (ind % 8);
//    }
//    free(tmppad);
//    free(newm);

    if (P == NULL) {
        free(P);
        return NULL;
    }

/*    printf("P State:\n");
    for (int i=msize-1; i >= 0; i--) {
        printf("%02x ", P[i]);
        if ((msize - i) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
*/
    int n = nceil(msize, 1088);

    unsigned char S[200];
    for (int ind=0; ind < 200; ind++) {
        S[ind] = 0x00;
    }

    unsigned char eP[200];
    for (int i=0; i < n; i++) {
        for (int k=0; k < 200; k++) { eP[k] = 0x00;}

        for (int k = 1600; k >= 512; k--) {
            eP[k / 8] = P[(((n-i) * 1088) - (1600 -k)) / 8];
        }
/*
        printf("Data to be absorbed:\n");
        for (int k=199; k >= 0; k--) {
            printf("%02x ", eP[k]);
            if ((200 - k) % 16 == 0) {
                printf("\n");
            }
        }
        printf("\n");
*/
        for (int j=0; j < 200; j++) {
            S[j] = S[j] ^ eP[j];
        }
/*
        printf("XOR'd State:\n");
        for (int k=199; k >= 0; k--) {
            printf("%02x ", S[i]);
            if ((200 - i) % 16 == 0) {
                printf("\n");
            }
        }
        printf("\n");
*/
        unsigned char *tmpS = keccak_f(S, 1600);
        for (int j = 0; j < 200; j++) {
            S[j] = tmpS[j];
        }

    }

    free(P);

    int zsize = 1088;
    unsigned char *Z = malloc(136);
    if (Z == NULL) {
        free(Z);
        return NULL;
    }
    for (int i = 0; i < 136; i++) { Z[i] = 0x00;}
    for (int i = 0; i < 1088; i++ ) {
        Z[i / 8] |= (S[i / 8] >> (i % 8) 
                  & 1) << (i % 8);
    }

    int cnt = 1;
    while (256 > zsize) {
        unsigned char *nZ = malloc(nceil(1088 * cnt, 8));
        if (nZ == NULL) {
            free(nZ);
            return NULL;
        }
        for (int i = 0; i < nceil(1088 * cnt, 8); i++) { nZ[i] = 0x00;}

        unsigned char *tmpS = keccak_f(S, 1600);
        for (int i = 0; i < 200; i++) {
            S[i] = tmpS[i];
        }

        for (int i = 0; i < 1088 * (cnt-1); i++) {
            nZ[i / 8] |= (Z[i / 8] >> (i % 8) 
                      & 1) << (i % 8);
        }
        free(Z);

        for (int i = 1088 * (cnt - 1); i < (1088 * cnt); i++) {
            nZ[i / 8] |= (S[(i - (1088 * (cnt - 1))) / 8] >> ((i - (1088 * (cnt - 1))) % 8)
                       & 1) << (i % 8);
        }
        zsize = 1088 * cnt;

        Z = malloc(nceil(1088 * cnt, 8));
        if (Z == NULL) {
            free(Z);
            return NULL;
        }

        for (int i = 0; i < nceil(1088 * cnt, 8); i++) { Z[i] = 0x00;}
        for (int i = 0; i < (1088 * cnt) / 8; i++) {
            Z[i] = nZ[i];
        }
        free(nZ);
        cnt++;
    }

    unsigned char *ret = malloc(32);
    if (ret == NULL) {
        free(ret);
        return NULL;
    }

    for (int i=0; i < 32; i++) {ret[i] = 0x00;}
    for (int i = 0; i < 32; i++) {
        ret[i] = Z[i];
    }
    free(Z);

    return ret;
};


/* KECCAK[1024] */
unsigned char *sha3_512(unsigned char *m, size_t msze) {
    unsigned char *newm = malloc(msze);
    if (newm == NULL) {
        free(newm);
        return NULL;
    }
    for (int i = 0; i < msze * 8; i++) {
        newm[(i + 2) / 8] |= (m[i / 8] >> (i % 8)) 
                           << ((i + 2) % 8);
    }
    newm[0] |= 0x01;
    int size = (int) (msze) * 8;
    int r = 1600 - 1024;
    int j = (-size - 2) % (576);
    unsigned int msize = size + (j<0?576+j:j);
    unsigned char *P = malloc(nceil(msize, 8));
    if (P == NULL) {
        free(P);
        return NULL;
    }
    unsigned char *tmppad = pad10(576, size);
    /* The bit sets are done instead of a byte set in the case that the bits lie
     * unevenly in the array */
    for (int ind=msize; ind > msize - size; ind--) {
        P[ind / 8] |= (newm[ind / 8] >> (ind % 8) 
                    & 1) << (ind % 8);
    }
    for (int ind=(msize - size); ind > 0; ind--) {
        P[ind / 8] |= (tmppad[(ind - size) / 8] >> ((ind - size) % 8) 
                    & 1) << (ind % 8);
    }
    free(tmppad);
    free(newm);

    int n = msize / 576;

    unsigned char S[200];
    for (int ind=0; ind < 200; ind++) {
        S[ind] = 0x00;
    }

    unsigned char eP[200];
    for (int i=0; i < n-1; i++) {

        for (int k=0; k < 200; k++) {eP[k] = 0x00;}

        for (int k = 0; k < 576; k++) {
            eP[k / 8] |= (P[(i * 576+k) / 8] >> (((i * 576)+k) % 8) 
                       & 1) << (k % 8);
        }
        for (int k = 576; k < 576 + 1024; k++) {
            eP[k / 8] |= ~(1 << (k % 8));
        }

        for (int k=0; k < 200; k++) {
            S[k] = S[k] ^ eP[k];
        }
        unsigned char *tmpS = keccak_f(S, 1600);
        for (int k = 0; k < 200; k++) {
            S[k] = tmpS[k];
        }
    }

    free(P);

    int zsize = 576;
    unsigned char *Z = malloc(72);
    if (Z == NULL) {
        free(Z);
        return NULL;
    }
    for (int i = 0; i < 576; i++ ) {
        Z[i / 8] |= (S[i / 8] >> (i % 8) 
                  & 1) << (i % 8);
    }

    int cnt = 1;
    while (512 > zsize) {
        unsigned char *nZ = malloc(nceil(576 * cnt, 8));
        if (nZ == NULL) {
            free(nZ);
            return NULL;
        }

        unsigned char *tmpS = keccak_f(S, 1600);
        for (int i = 0; i < 200; i++) {
            S[i] = tmpS[i];
        }

        for (int i = 0; i < 576 * (cnt-1); i++) {
            nZ[i / 8] |= (Z[i / 8] >> (i % 8) 
                      & 1) << (i % 8);
        }
        free(Z);

        for (int i = 576 * (cnt - 1); i < (576 * cnt); i++) {
            nZ[i / 8] |= (S[(i - (576 * (cnt - 1))) / 8] >> ((i - (576 * (cnt - 1))) % 8)
                       & 1) << (i % 8);
        }
        zsize = 576 * cnt;

        Z = malloc(nceil(576 * cnt, 8));
        if (Z == NULL) {
            free(Z);
            return NULL;
        }
        for (int i = 0; i < (576 * cnt) / 8; i++) {
            Z[i] = nZ[i];
        }
        free(nZ);
        cnt++;
    }

    unsigned char *ret = malloc(64);
    if (ret == NULL) {
        free(ret);
        return NULL;
    }

    for (int i = 0; i < 64; i++) {
        ret[i] = Z[i];
    }
    free(Z);

    return ret;   
}
