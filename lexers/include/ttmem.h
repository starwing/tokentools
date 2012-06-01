#include "ttprivate.h"

#define ttM_reallocv(L,b,on,n,e) \
        (((size_t)(n)+1) > MAX_SIZE_T/(e) ?  /* +1 to avoid warnings */ \
                (ttM_toobig(L), (void *)0) : \
                 ttM_realloc_(L, (b), (on)*(e), (n)*(e)))

#define ttM_reallocvector(L, v,oldn,n,t) \
   ((v)=(t*)ttM_reallocv(L, v, oldn, n, sizeof(t)))

void ttM_toobig (tt_State *L);

TTI_FUNC void *ttM_realloc_ (tt_State *L, void *block, size_t oldsize, size_t size);
TTI_FUNC void *ttM_growaux_ (tt_State *L, void *block, int *size,
                              size_t size_elem, int limit,
                              const char *what);
