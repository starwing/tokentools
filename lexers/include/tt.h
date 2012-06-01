#ifndef tt_h
#define tt_h


#include <ttprivate.h>

struct tt_LexerReg {
    const char *name;
    size_t lexersize;
    int  (*init_lexer) (tt_Lexer *L, size_t lsize, tt_Reader *reader, void *ud);
    void (*free_lexer) (tt_Lexer *L);
    int  (*lexer) (tt_Lexer *L);
    const char *(*tostring) (tt_Lexer *L, tt_TokenId tokenid);
};

#ifndef tt_assert
#  include <assert.h>
#  define tt_assert(e) assert(e)
#endif

int ttL_lexer_init (tt_Lexer *L, tt_Reader *reader, void *ud);

#define TTL_EOZ (-1)
int ttL_current   (tt_Lexer *L);
int ttL_isnewline (tt_Lexer *L);

int ttL_inclinenumber (tt_Lexer *L);

int  ttL_next (tt_Lexer *L);
void ttL_save (tt_Lexer *L, int ch);
#define ttL_save_and_next(L) (ttL_save(L, ttL_current(L)), ttL_next(L))

int ttL_check_next (tt_Lexer *L, const char *setchars);

const char *ttL_tostring (tt_Lexer *L, tt_TokenId tokenid);

char  *ttL_buffer        (tt_Lexer *L);
size_t ttL_bufflen       (tt_Lexer *L);

void   ttL_resetbuffer   (tt_Lexer *L);
void   ttL_resizebuffer  (tt_Lexer *L, size_t nsize);
char  *ttL_replacebuffer (tt_Lexer *L, int oldchar, int newchar);
char  *ttL_copybuffer    (tt_Lexer *L, const char *s, size_t len);
char  *ttL_movebuffer    (tt_Lexer *L, const char *s, size_t len);
#define ttL_freebuffer(L) ttL_resetbuffer(L, 0)

tt_Number  ttL_buffer2d (tt_Lexer *L);
tt_Integer ttL_buffer2i (tt_Lexer *L);

int ttL_lexerror (tt_Lexer *L, const char *msg, tt_TokenId token);

int ttL_iskeyword (tt_Lexer *L, const char **tokens, size_t tokenlen);

int ttL_register (tt_State *S, tt_LexerReg *reg);

#if !defined(TT_CORE) || !defined(TT_RESTRICT_API)
#  define ttL_current(L)       ((L)->current)
#  define ttL_isnewline(L)     ((L)->current == '\r' || (L)->current == '\n')
#  define ttL_inclinenumber(L) ((L)->linenumber++, (L)->linecolume = 0)
#  define ttL_buffer(L)        ((L)->buffer->buffer)
#  define ttL_bufflen(L)       ((L)->buffer->bufflen)
#  define ttL_resetbuffer(L)   ((L)->buffer->bufflen = 0)
#  include "ttmem.h"
#  define ttL_resizebuffer(L,size) \
    (ttM_reallocvector((L)->S, (L)->buffer->buffer, (L)->buffer->buffsize, size, char), \
     (L)->buffer->buffsize = (size), (L)->buffer->buffer)
#  include <string.h>
#  define ttL_copybuffer(L,s,len) \
    ((len) > ttL_bufflen(L) ? (void)ttL_resizebuffer(L, len) : \
     (void)(ttL_bufflen(L) = len), \
     memcpy(ttL_buffer(L), s, len))
#  define ttL_movebuffer(L,s,len) \
    ((len) > ttL_bufflen(L) ? (void)ttL_resizebuffer(L, len) : \
     (void)(ttL_bufflen(L) = len), \
     memmove(ttL_buffer(L), s, len))

#  include <ttzio.h>
#  define ttL_next(L) (L->current = tt_zio_getc(L->z))
#endif

#endif /* tt_h */
