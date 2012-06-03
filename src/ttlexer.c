#define TT_CORE
#include <ttlexer.h>
#include <ttzio.h>

#undef ttL_next

int ttL_next(tt_Lexer *L) {
    L->current = tt_zio_getc(L->z);
    return L->current;
}

void ttL_save(tt_Lexer *L, int ch) {
    tt_Buffer *b = L->buffer;
    if (b->bufflen + 1 > b->buffsize) {
        size_t newsize;
        if (b->buffsize >= MAX_SIZE_T/2)
            ttL_lexerror(L, "lexical element too loog", 0);
        newsize = b->buffsize * 2;
        (void)ttL_resizebuffer(L, newsize);
    }
    b->buffer[b->bufflen++] = (char)ch;
}

int ttL_check_next(tt_Lexer *L, const char *setchars) {
  if (L->current == '\0' || !strchr(setchars, L->current))
    return 0;
  ttL_save_and_next(L);
  return 1;
}
