#ifndef ttconfig_h
#define ttconfig_h


#include <limits.h> /* for UCHAR_MAX */


#ifndef TT_API
#  ifdef __WIN32__
#    if !defined(TT_CORE) && !defined(TT_LEXER)
#      define TT_API __declspec(dllimport)
#    else
#      define TT_API __declspec(dllexport)
#    endif 
#  else
#    define TT_API
#  endif 
#endif 
#define TT_LIBAPI TT_API

#ifndef TTI_FUNC
#define TTI_FUNC
#endif


#ifndef TTK_FIRST_TOKEN
#define TTK_FIRST_TOKEN UCHAR_MAX
#endif

#ifndef TT_TOKENID
#define TT_TOKENID int
#endif

#ifndef TT_STRING_T 
#define TT_STRING_T const char*
#endif

#ifndef TT_NUMBER_T
#define TT_NUMBER_T double
#endif

#ifndef TT_INTEGER_T
#define TT_INTEGER_T unsigned long
#endif

#ifndef MAX_SIZE_T
#define MAX_SIZE_T ((size_t)(~(size_t)0-2))
#endif

typedef TT_TOKENID tt_TokenId;
typedef TT_STRING_T tt_String;
typedef TT_NUMBER_T tt_Number;
typedef TT_INTEGER_T tt_Integer;

#endif /* ttconfig_h */
