

#ifndef UTILS_H
#define UTILS_H

/* Data dumps for char arrays.
   If the data isn't a char array then cast that sucker and make it fit */
extern void pointer_dump(char *data, size_t dsize);
extern void hex_dump(char *data, size_t dsize);

/* Checks the hex values of an actual value against an expected value */
extern int hex_check(char *expec, char *actual, size_t exsize);


#endif
