/*
 * A toy implementation of sha3
 *  Vulnerable to side-channel and timing attacks
 */

#ifndef SHA_H
#define SHA_H

#include <stdio.h>

unsigned char *sha3_256(unsigned char *m, size_t size);

unsigned char *sha3_512(unsigned char *m, size_t size);

#endif
