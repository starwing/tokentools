#define TT_LEXER
#include "lua_tokens.h"
#include <ctype.h>
#include <locale.h> /* for localeconv */


static const char *ltokens[] = {
#define X(a, b) b,
    lua_tokens
#undef X
};


/* helpers */

static void trydecpoint (tt_Lexer *L) {
  /*
  ** in case of format error, try to change decimal point separator to
  ** the one defined in the current locale and check again
  */
  lua_Lexer *ll = (lua_Lexer*)L;
  char old = ll->decpoint;
  ll->decpoint = localeconv()->decimal_point[0];
  ttL_replacebuffer(L, old, ll->decpoint);  /* try new decimal separator */
  if (!ttL_buffer2d(L)) {
    /* format error with correct decimal point: no more options */
    ttL_replacebuffer(L, ll->decpoint, '.');  /* undo change (for error message) */
    ttL_lexerror(L, "malformed number", TTK_NUMBER);
  }
}

static void read_numeral (tt_Lexer *L) {
  tt_assert(isdigit(ttL_current(L)));
  do {
    ttL_save_and_next(L);
    if (ttL_check_next(L, "EePp"))  /* exponent part? */
      ttL_check_next(L, "+-");  /* optional exponent sign */
  } while (isalnum(ttL_current(L)) || ttL_current(L) == '.');
  ttL_save(L, '\0');
  ttL_replacebuffer(L, '.', ((lua_Lexer*)L)->decpoint);  /* follow locale for decimal point */
  if (!ttL_buffer2d(L))  /* format error? */
    trydecpoint(L); /* try to update decimal point separator */
}

static int skip_sep (tt_Lexer *L) {
  /*
  ** skip a sequence '[=*[' or ']=*]' and return its number of '='s or
  ** -1 if sequence is malformed
  */
  int count = 0;
  int s = ttL_current(L);
  tt_assert(s == '[' || s == ']');
  ttL_save_and_next(L);
  while (ttL_current(L) == '=') {
    ttL_save_and_next(L);
    count++;
  }
  return (ttL_current(L) == s) ? count : (-count) - 1;
}

static void read_long_string (tt_Lexer *L, int isstring, int sep) {
  ttL_save_and_next(L);  /* skip 2nd `[' */
  if (ttL_isnewline(L))  /* string starts with a newline? */
    ttL_inclinenumber(L);  /* skip it */
  for (;;) {
    switch (ttL_current(L)) {
      case TTL_EOZ:
        ttL_lexerror(L, isstring ? "unfinished long string" :
                                 "unfinished long comment", TTK_EOS);
        break;  /* to avoid warnings */
      case ']': {
        if (skip_sep(L) == sep) {
          ttL_save_and_next(L);  /* skip 2nd `]' */
          goto endloop;
        }
        break;
      }
      case '\n': case '\r': {
        ttL_save(L, '\n');
        ttL_inclinenumber(L);
        if (!isstring) ttL_resetbuffer(L);  /* avoid wasting space */
        break;
      }
      default: {
        if (isstring) ttL_save_and_next(L);
        else ttL_next(L);
      }
    }
  } endloop:
  ttL_movebuffer(L, ttL_buffer(L)+2+sep,
                    ttL_bufflen(L)-2*(2+sep));
}

static void escerror (tt_Lexer *L, int *c, int n, const char *msg) {
  int i;
  ttL_resetbuffer(L);  /* prepare error message */
  ttL_save(L, '\\');
  for (i = 0; i < n && c[i] != TTL_EOZ; i++)
    ttL_save(L, c[i]);
  ttL_lexerror(L, msg, TTK_STRING);
}

static int hexavalue(int ch) {
    if (ch >= '0' && ch <= '9')
        return ch-'0';
    else {
        ch = tolower(ch);
        if (ch >= 'a' && ch <= 'f')
            return 10+ch-'a';
    }
    return 0;
}

static int readhexaesc (tt_Lexer *L) {
  int c[3], i;  /* keep input for error message */
  int r = 0;  /* result accumulator */
  c[0] = 'x';  /* for error message */
  for (i = 1; i < 3; i++) {  /* read two hexa digits */
    c[i] = ttL_next(L);
    if (!isxdigit(c[i]))
      escerror(L, c, i + 1, "hexadecimal digit expected");
    r = (r << 4) + hexavalue(c[i]);
  }
  return r;
}

static int readdecesc (tt_Lexer *L) {
  int c[3], i;
  int r = 0;  /* result accumulator */
  for (i = 0; i < 3 && isdigit(ttL_current(L)); i++) {  /* read up to 3 digits */
    c[i] = ttL_current(L);
    r = 10*r + c[i] - '0';
    ttL_next(L);
  }
  if (r > UCHAR_MAX)
    escerror(L, c, i, "decimal escape too large");
  return r;
}

static void read_string (tt_Lexer *L, int del) {
  ttL_save_and_next(L);  /* keep delimiter (for error messages) */
  while (ttL_current(L) != del) {
    switch (ttL_current(L)) {
      case TTL_EOZ:
        ttL_lexerror(L, "unfinished string", TTK_EOS);
        break;  /* to avoid warnings */
      case '\n':
      case '\r':
        ttL_lexerror(L, "unfinished string", TTK_STRING);
        break;  /* to avoid warnings */
      case '\\': {  /* escape sequences */
        int c;  /* final character to be saved */
        ttL_next(L);  /* do not ttL_save the `\' */
        switch (ttL_current(L)) {
          case 'a': c = '\a'; goto read_save;
          case 'b': c = '\b'; goto read_save;
          case 'f': c = '\f'; goto read_save;
          case 'n': c = '\n'; goto read_save;
          case 'r': c = '\r'; goto read_save;
          case 't': c = '\t'; goto read_save;
          case 'v': c = '\v'; goto read_save;
          case 'x': c = readhexaesc(L); goto read_save;
          case '\n': case '\r':
            ttL_inclinenumber(L); c = '\n'; goto only_save;
          case '\\': case '\"': case '\'':
            c = ttL_current(L); goto read_save;
          case TTL_EOZ: goto no_save;  /* will raise an error ttL_next loop */
          case 'z': {  /* zap following span of spaces */
            ttL_next(L);  /* skip the 'z' */
            while (isspace(ttL_current(L))) {
              if (ttL_isnewline(L)) ttL_inclinenumber(L);
              else ttL_next(L);
            }
            goto no_save;
          }
          default: {
            if (!isdigit(ttL_current(L))) {
              int ch = ttL_current(L);
              escerror(L, &ch, 1, "invalid escape sequence");
            }
            /* digital escape \ddd */
            c = readdecesc(L);
            goto only_save;
          }
        }
       read_save: ttL_next(L);  /* read next character */
       only_save: ttL_save(L, c);  /* save 'c' */
       no_save: break;
      }
      default:
        ttL_save_and_next(L);
    }
  }
  ttL_save_and_next(L);  /* skip delimiter */
  ttL_movebuffer(L, ttL_buffer(L)+1, ttL_bufflen(L)-2);
}


/* implements */

static int llex (tt_Lexer *L) {
  for (;;) {
    switch (ttL_current(L)) {
      case '\n': case '\r': {  /* line breaks */
        ttL_inclinenumber(L);
        break;
      }
      case ' ': case '\f': case '\t': case '\v': {  /* spaces */
        ttL_next(L);
        break;
      }
      case '-': {  /* '-' or '--' (comment) */
        ttL_next(L);
        if (ttL_current(L) != '-') return '-';
        /* else is a comment */
        ttL_next(L);
        if (ttL_current(L) == '[') {  /* long comment? */
          int sep = skip_sep(L);
          ttL_resetbuffer(L);  /* `skip_sep' may dirty the buffer */
          if (sep >= 0) {
            read_long_string(L, 0, sep);  /* skip long comment */
            ttL_resetbuffer(L);  /* previous call may dirty the buff. */
            break;
          }
        }
        /* else short comment */
        while (!ttL_isnewline(L) && ttL_current(L) != TTL_EOZ)
          ttL_next(L);  /* skip until end of line (or end of file) */
        break;
      }
      case '[': {  /* long string or simply '[' */
        int sep = skip_sep(L);
        if (sep >= 0) {
          read_long_string(L, 1, sep);
          return TTK_STRING;
        }
        else if (sep == -1) return '[';
        else ttL_lexerror(L, "invalid long string delimiter", TTK_STRING);
      }
      case '=': {
        ttL_next(L);
        if (ttL_current(L) != '=') return '=';
        else { ttL_next(L); return TTK_EQ; }
      }
      case '<': {
        ttL_next(L);
        if (ttL_current(L) != '=') return '<';
        else { ttL_next(L); return TTK_LE; }
      }
      case '>': {
        ttL_next(L);
        if (ttL_current(L) != '=') return '>';
        else { ttL_next(L); return TTK_GE; }
      }
      case '~': {
        ttL_next(L);
        if (ttL_current(L) != '=') return '~';
        else { ttL_next(L); return TTK_NE; }
      }
      case ':': {
        ttL_next(L);
        if (ttL_current(L) != ':') return ':';
        else { ttL_next(L); return TTK_DBCOLON; }
      }
      case '"': case '\'': {  /* short literal strings */
        read_string(L, ttL_current(L));
        return TTK_STRING;
      }
      case '.': {  /* '.', '..', '...', or number */
        ttL_save_and_next(L);
        if (ttL_check_next(L, ".")) {
          if (ttL_check_next(L, "."))
            return TTK_DOTS;   /* '...' */
          else return TTK_CONCAT;   /* '..' */
        }
        else if (!isdigit(ttL_current(L))) return '.';
        /* else go through */
      }
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9': {
        read_numeral(L);
        return TTK_NUMBER;
      }
      case TTL_EOZ: {
        return TTK_EOS;
      }
      default: {
        if (isalpha(ttL_current(L)) || ttL_current(L) == '_') {  /* identifier or reserved word? */
          int reversed;
          do {
            ttL_save_and_next(L);
          } while (isalnum(ttL_current(L)) || ttL_current(L) == '_');
          if ((reversed = ttL_iskeyword(L, ltokens, TTK_LUA_NUM_RESERVED)) != 0)
            return reversed + TTK_FIRST_REVERSED;
          return TTK_NAME;
        }
        else {  /* single-char tokens (+ - / ...) */
          int c = ttL_current(L);
          ttL_next(L);
          return c;
        }
      }
    }
  }
}

static int linitlexer(tt_Lexer *t, size_t lexersize, tt_Reader reader, void *ud) {
    lua_Lexer *ll = (lua_Lexer*)t;
    tt_assert(lexersize == sizeof(lua_Lexer));
    ll->decpoint = 0;
    return ttL_lexer_init(t, reader, ud);
}

static const char *ltostring(tt_Lexer *L, tt_TokenId tokenid) {
  if (tokenid > TTK_LUA_BEFORE_RESERVED && tokenid < TTK_LUA_AFTER_RESERVED)
    return ltokens[tokenid+1 - TTK_LUA_BEFORE_RESERVED];
  return ttL_tostring(L, tokenid);
}

static tt_LexerReg reg = {
    "lua",
    sizeof(lua_Lexer),
    linitlexer,
    NULL, /* lfreelexer */
    llex,
    ltostring,
};

tt_LexerReg *tt_lexer_lua(void) {
    return &reg;
}

/* cc: flags+='-I../../include -I..' input="*.c ../tt*.c" */
