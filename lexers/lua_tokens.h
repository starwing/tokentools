#ifndef lua_tokens_h
#define lua_tokens_h


#include <ttlexer.h>


#define lua_tokens \
  X(AND,      "and"      ) \
  X(BREAK,    "break"    ) \
  X(DO,       "do"       ) \
  X(ELSE,     "else"     ) \
  X(ELSEIF,   "elseif"   ) \
  X(END,      "end"      ) \
  X(FALSE,    "false"    ) \
  X(FOR,      "for"      ) \
  X(FUNCTION, "function" ) \
  X(GOTO,     "goto"     ) \
  X(IF,       "if"       ) \
  X(IN,       "in"       ) \
  X(LOCAL,    "local"    ) \
  X(NIL,      "nil"      ) \
  X(NOT,      "not"      ) \
  X(OR,       "or"       ) \
  X(REPEAT,   "repeat"   ) \
  X(RETURN,   "return"   ) \
  X(THEN,     "then"     ) \
  X(TRUE,     "true"     ) \
  X(UNTIL,    "until"    ) \
  X(WHILE,    "while"    ) \
  \
  /* other terminal symbols */ \
  X(CONCAT,  ".."  ) \
  X(DOTS,    "..." ) \
  X(EQ,      "=="  ) \
  X(GE,      ">="  ) \
  X(LE,      "<="  ) \
  X(NE,      "~="  ) \
  X(DBCOLON, "::"  )


enum TTL_LUA_RESERVED {
    TTK_LUA_BEFORE_RESERVED = TTK_FIRST_REVERSED-1,
#define X(a, b) TTK_ ## a,
    lua_tokens
#undef X
    TTK_LUA_AFTER_RESERVED
};

#define TTK_LUA_NUM_RESERVED (TTK_WHILE-TTK_FIRST_REVERSED+1)

typedef struct {
    tt_Lexer base;
    int decpoint;
} lua_Lexer;

TT_LIBAPI int tt_lexer_lua(tt_State *S);
TT_LIBAPI int tt_lexer_lua_init (lua_Lexer *L, tt_Reader *reader, void *ud);

#endif /* lua_tokens_h */
