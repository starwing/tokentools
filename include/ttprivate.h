#ifndef ttpriv_h
#define ttpriv_h


#if !defined(TT_CORE) && !defined(TT_LEXER)
#error "you mustn't use restricted api!"
#endif

#include "tokentools.h"
#include <stddef.h>

typedef struct tt_Buffer tt_Buffer;

struct tt_Buffer {
    char *buffer;
    size_t bufflen;
    size_t buffsize;
};

struct tt_SemInfo {
    tt_Integer i;
    tt_Number n;
    struct tt_Buffer s;
};

struct tt_Token {
    tt_TokenId tokenid;
    struct tt_SemInfo seminfo;
};

struct tt_State {
    tt_Alloc *allocf;
    void *ud;
    void *uservalue;
    tt_Lexer *curlexer;
    tt_LexerReg *lexers;
};

struct tt_Lexer {
    tt_LexerReg *vt;
    tt_State *S;
    struct tt_Zio *z;
    int linenumber;
    int linecolume;
    int current;
    struct tt_Buffer *buffer;
    tt_Token token;
    tt_Token ahead;
};


#endif /* ttpriv_h */
