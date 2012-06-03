#define TT_CORE
#include <ttlexer.h>
#include <ttzio.h>


void tt_zio_init(tt_State *S, tt_ZIO *z, tt_Reader *reader, void *ud) {
    z->S = S;
    z->reader = reader;
    z->ud = ud;
    z->n = 0;
    z->p = NULL;
}

size_t tt_zio_read(tt_ZIO *z, void *b, size_t n) {
  while (n) {
    size_t m;
    if (z->n == 0) {  /* no bytes in buffer? */
      if (tt_zio_fill(z) == TTL_EOZ)  /* try to read more */
        return n;  /* no more input; return number of missing bytes */
      else {
        z->n++;  /* luaZ_fill consumed first byte; put it back */
        z->p--;
      }
    }
    m = (n <= z->n) ? n : z->n;  /* min. between n and z->n */
    memcpy(b, z->p, m);
    z->n -= m;
    z->p += m;
    b = (char *)b + m;
    n -= m;
  }
  return 0;
}

int tt_zio_fill(tt_ZIO *z) {
    size_t size;
    tt_State *S = z->S;
    const char *buff;
    /* tt_unlock(S); */
    buff = z->reader(S, z->ud, &size);
    /* tt_lock(S); */
    if (buff == NULL || size == 0)
        return TTL_EOZ;
    z->n = size - 1;
    z->p = buff;
    return (unsigned char)(*(z->p++));
}
