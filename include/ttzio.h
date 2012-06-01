#ifndef ttzio_h
#define ttzio_h


#include <tokentools.h>


typedef struct tt_Zio tt_ZIO;

#define tt_zio_getc(z) (((z)->n--)>0 ? (unsigned char)(*(z)->p++) : tt_zio_fill(z))

TTI_FUNC void   tt_zio_init (tt_State *S, tt_ZIO *z, tt_Reader *reader, void *ud);
TTI_FUNC size_t tt_zio_read (tt_ZIO *z, void *b, size_t n);

/* private part */

struct tt_Zio {
    size_t n;
    const char *p;
    tt_Reader *reader;
    void *ud;
    tt_State *S; /* used by tt_Reader */
};

TTI_FUNC int tt_zio_fill (tt_ZIO *z);

#endif /* ttzio_h */
