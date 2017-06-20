/*
 * A toy implementation of sha3
 *  Vulnerable to side-channel and timing attacks
 */

#include <stdio.h>

#ifndef SHA_H
#define SHA_H

unsigned char *sha3_256(unsigned char *m, size_t size);

unsigned char *sha3_512(unsigned char *m, size_t size);

#endif
