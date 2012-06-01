#ifndef tokentools_h
#define tokentools_h


#include "ttconfig.h"
#include <stddef.h>


#define TTK_EOS            (TTK_FIRST_TOKEN+0)
#define TTK_NAME           (TTK_FIRST_TOKEN+1)
#define TTK_STRING         (TTK_FIRST_TOKEN+2)
#define TTK_NUMBER         (TTK_FIRST_TOKEN+3)
#define TTK_INTEGER        (TTK_FIRST_TOKEN+4)
#define TTK_FIRST_REVERSED (TTK_FIRST_TOKEN+10)


typedef struct tt_State tt_State;
typedef struct tt_Token tt_Token;
typedef struct tt_Lexer tt_Lexer;
typedef struct tt_LexerReg tt_LexerReg;

typedef       void *tt_Alloc  (void *ud, void *mem, size_t nsize, size_t osize);
typedef const char *tt_Reader (tt_State *S, void *ud, size_t *sz);


TT_API tt_State *tt_new   (tt_Alloc *allocf, void *ud);
TT_API void      tt_close (tt_State *S);

TT_API void *tt_uservalue    (tt_State *S);
TT_API void  tt_setuservalue (tt_State *S, void *ud);

TT_API tt_Alloc *tt_allocf    (tt_State *S, void **ud);
TT_API void      tt_setallocf (tt_State *S, tt_Alloc *allocf, void *ud);

TT_API tt_Lexer *tt_lexer_new   (tt_State *S, const char *lexername, tt_Reader *reader, void *ud);
TT_API void      tt_lexer_close (tt_State *S, tt_Lexer *L);

TT_API int  tt_lexer_init (tt_Lexer *L, tt_LexerReg *reg, tt_Reader *reader, void *ud);
TT_API void tt_lexer_free (tt_Lexer *L);

TT_API tt_Lexer *tt_lexer    (tt_State *S);
TT_API int       tt_setlexer (tt_State *S, tt_Lexer *lexer);

TT_API const char *tt_tokename (tt_State *S, tt_Token *t);

TT_API tt_TokenId tt_next      (tt_State *S);
TT_API tt_TokenId tt_lookahead (tt_State *S);

TT_API tt_Token *tt_current (tt_State *S);
TT_API tt_Token *tt_ahead   (tt_State *S);

TT_API const char *tt_filename (tt_State *S);

TT_API int tt_linenumber (tt_State *S);
TT_API int tt_linecolume (tt_State *S);

TT_API void tt_lexerror (tt_State *S, const char *msg);


TT_API tt_TokenId tt_token   (tt_Token *t);
TT_API tt_String  tt_string  (tt_Token *t);
TT_API tt_Number  tt_number  (tt_Token *t);
TT_API tt_Integer tt_integer (tt_Token *t);


#endif /* tokentools.h */
