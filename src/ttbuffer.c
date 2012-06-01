#define TT_CORE
#include <lexer/tt.h>

#undef ttL_buffer
#undef ttL_bufflen
#undef ttL_resetbuffer
#undef ttL_resizebuffer
#undef ttL_copybuffer
#undef ttL_movebuffer

char *tt_buffer(tt_Lexer *L) {
    return L->buffer->buffer;
}

size_t ttL_bufflen(tt_Lexer *L) {
    return L->buffer->bufflen;
}

void ttL_resetbuffer(tt_Lexer *L) {
    L->buffer->bufflen = 0;
}

void ttL_resizebuffer(tt_Lexer *L, size_t size) {
    ttM_reallocvector(L->S, L->buffer->buffer, L->buffer->buffsize,
            size, char);
    L->buffer->buffsize = size;
}

char *ttL_copybuffer(tt_Lexer *L, const char *s, size_t len) {
    if (len > L->buffer->bufflen)
        ttL_resizebuffer(L, len);
    memcpy(L->buffer->buffer, s, len);
    L->buffer->bufflen = len;
    return L->buffer->buffer;
}

char *ttL_movebuffer(tt_Lexer *L, const char *s, size_t len) {
    if (len > L->buffer->bufflen)
        ttL_resizebuffer(L, len);
    memmove(L->buffer->buffer, s, len);
    L->buffer->bufflen = len;
    return L->buffer->buffer;
}

char *ttL_replacebuffer(tt_Lexer *L, int oldchar, int newchar) {
    int i;
    struct tt_Buffer *buff = L->buffer;
    for (i = 0; i < buff->bufflen; ++i) {
        if (buff->buffer[i] == oldchar)
            buff->buffer[i] = newchar;
    }
    return buff->buffer;
}

