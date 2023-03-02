/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* "%code top" blocks.  */
#line 3 "../../include/jx-af/jexpr/JExprParserY.y"

/*
Copyright (C) 2018 by John Lindal.
*/

#include "JExprParser.h"
#include "JAbsValue.h"
#include "JAlgSign.h"
#include "JArcCosine.h"
#include "JArcHypCosine.h"
#include "JArcHypSine.h"
#include "JArcHypTangent.h"
#include "JArcSine.h"
#include "JArcTangent.h"
#include "JArcTangent2.h"
#include "JConjugate.h"
#include "JConstantValue.h"
#include "JCosine.h"
#include "JDivision.h"
#include "JExponent.h"
#include "JHypCosine.h"
#include "JHypSine.h"
#include "JHypTangent.h"
#include "JImagPart.h"
#include "JLogB.h"
#include "JLogE.h"
#include "JMaxFunc.h"
#include "JMinFunc.h"
#include "JNamedConstant.h"
#include "JNegation.h"
#include "JParallel.h"
#include "JPhaseAngle.h"
#include "JProduct.h"
#include "JRealPart.h"
#include "JRotateComplex.h"
#include "JRoundToInt.h"
#include "JSine.h"
#include "JSquareRoot.h"
#include "JSummation.h"
#include "JTangent.h"
#include "JTruncateToInt.h"
#include "JUserInputFunction.h"
#include "JVariableValue.h"
#include "JVariableList.h"
#include <jx-af/jcore/JPtrArray-JString.h>

#define yyparse JExprParser::yyparse

// also uncomment yydebug=1; below
//#define YYERROR_VERBOSE
//#define YYDEBUG 1

#line 121 "../../include/jx-af/jexpr/JExprParserY.cpp"




# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "JExprParserY.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_P_NUMBER = 3,                   /* P_NUMBER  */
  YYSYMBOL_P_HEX = 4,                      /* P_HEX  */
  YYSYMBOL_P_VARIABLE = 5,                 /* P_VARIABLE  */
  YYSYMBOL_P_PI = 6,                       /* P_PI  */
  YYSYMBOL_P_E = 7,                        /* P_E  */
  YYSYMBOL_P_I = 8,                        /* P_I  */
  YYSYMBOL_P_INPUT = 9,                    /* P_INPUT  */
  YYSYMBOL_P_FN_ABS = 10,                  /* P_FN_ABS  */
  YYSYMBOL_P_FN_ARCCOS = 11,               /* P_FN_ARCCOS  */
  YYSYMBOL_P_FN_ARCCOSH = 12,              /* P_FN_ARCCOSH  */
  YYSYMBOL_P_FN_ARCSIN = 13,               /* P_FN_ARCSIN  */
  YYSYMBOL_P_FN_ARCSINH = 14,              /* P_FN_ARCSINH  */
  YYSYMBOL_P_FN_ARCTAN = 15,               /* P_FN_ARCTAN  */
  YYSYMBOL_P_FN_ARCTAN2 = 16,              /* P_FN_ARCTAN2  */
  YYSYMBOL_P_FN_ARCTANH = 17,              /* P_FN_ARCTANH  */
  YYSYMBOL_P_FN_CONJUGATE = 18,            /* P_FN_CONJUGATE  */
  YYSYMBOL_P_FN_COS = 19,                  /* P_FN_COS  */
  YYSYMBOL_P_FN_COSH = 20,                 /* P_FN_COSH  */
  YYSYMBOL_P_FN_IM = 21,                   /* P_FN_IM  */
  YYSYMBOL_P_FN_LN = 22,                   /* P_FN_LN  */
  YYSYMBOL_P_FN_LOG = 23,                  /* P_FN_LOG  */
  YYSYMBOL_P_FN_LOG2 = 24,                 /* P_FN_LOG2  */
  YYSYMBOL_P_FN_LOG10 = 25,                /* P_FN_LOG10  */
  YYSYMBOL_P_FN_MAX = 26,                  /* P_FN_MAX  */
  YYSYMBOL_P_FN_MIN = 27,                  /* P_FN_MIN  */
  YYSYMBOL_P_FN_PHASE = 28,                /* P_FN_PHASE  */
  YYSYMBOL_P_FN_RE = 29,                   /* P_FN_RE  */
  YYSYMBOL_P_FN_ROTATE = 30,               /* P_FN_ROTATE  */
  YYSYMBOL_P_FN_SIN = 31,                  /* P_FN_SIN  */
  YYSYMBOL_P_FN_SINH = 32,                 /* P_FN_SINH  */
  YYSYMBOL_P_FN_SQRT = 33,                 /* P_FN_SQRT  */
  YYSYMBOL_P_FN_TAN = 34,                  /* P_FN_TAN  */
  YYSYMBOL_P_FN_TANH = 35,                 /* P_FN_TANH  */
  YYSYMBOL_P_FN_SIGN = 36,                 /* P_FN_SIGN  */
  YYSYMBOL_P_FN_ROUND = 37,                /* P_FN_ROUND  */
  YYSYMBOL_P_FN_TRUNCATE = 38,             /* P_FN_TRUNCATE  */
  YYSYMBOL_P_FN_PARALLEL = 39,             /* P_FN_PARALLEL  */
  YYSYMBOL_P_EOF = 40,                     /* P_EOF  */
  YYSYMBOL_41_ = 41,                       /* '+'  */
  YYSYMBOL_42_ = 42,                       /* '-'  */
  YYSYMBOL_43_ = 43,                       /* '*'  */
  YYSYMBOL_44_ = 44,                       /* '/'  */
  YYSYMBOL_45_ = 45,                       /* '^'  */
  YYSYMBOL_UMINUS = 46,                    /* UMINUS  */
  YYSYMBOL_UPLUS = 47,                     /* UPLUS  */
  YYSYMBOL_48_ = 48,                       /* '['  */
  YYSYMBOL_49_ = 49,                       /* ']'  */
  YYSYMBOL_50_ = 50,                       /* '('  */
  YYSYMBOL_51_ = 51,                       /* ')'  */
  YYSYMBOL_52_ = 52,                       /* ','  */
  YYSYMBOL_YYACCEPT = 53,                  /* $accept  */
  YYSYMBOL_expression = 54,                /* expression  */
  YYSYMBOL_e = 55,                         /* e  */
  YYSYMBOL_arglist = 56                    /* arglist  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 79 "../../include/jx-af/jexpr/JExprParserY.y"


// debugging output

#define YYPRINT(file, type, value)	yyprint(file, type, value)
#include <stdio.h>

inline void
yyprint
	(
	FILE*	file,
	int		type,
	YYSTYPE	value
	)
{
	if (value.pString != nullptr)
	{
		fprintf(file, "string:  %s", (value.pString)->GetBytes());
	}
}

// must be last
#include <jx-af/jcore/jAssert.h>

#line 240 "../../include/jx-af/jexpr/JExprParserY.cpp"

#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  78
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   360

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  53
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  4
/* YYNRULES -- Number of rules.  */
#define YYNRULES  52
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  166

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   297


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      50,    51,    43,    41,    52,    42,     2,    44,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    48,     2,    49,    45,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    46,    47
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   119,   119,   127,   159,   173,   187,   198,   204,   210,
     216,   227,   238,   251,   256,   261,   266,   271,   277,   283,
     289,   294,   300,   306,   312,   318,   324,   330,   336,   342,
     348,   354,   360,   366,   372,   378,   384,   390,   396,   402,
     408,   414,   420,   426,   432,   438,   444,   450,   456,   462,
     468,   476,   483
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "P_NUMBER", "P_HEX",
  "P_VARIABLE", "P_PI", "P_E", "P_I", "P_INPUT", "P_FN_ABS", "P_FN_ARCCOS",
  "P_FN_ARCCOSH", "P_FN_ARCSIN", "P_FN_ARCSINH", "P_FN_ARCTAN",
  "P_FN_ARCTAN2", "P_FN_ARCTANH", "P_FN_CONJUGATE", "P_FN_COS",
  "P_FN_COSH", "P_FN_IM", "P_FN_LN", "P_FN_LOG", "P_FN_LOG2", "P_FN_LOG10",
  "P_FN_MAX", "P_FN_MIN", "P_FN_PHASE", "P_FN_RE", "P_FN_ROTATE",
  "P_FN_SIN", "P_FN_SINH", "P_FN_SQRT", "P_FN_TAN", "P_FN_TANH",
  "P_FN_SIGN", "P_FN_ROUND", "P_FN_TRUNCATE", "P_FN_PARALLEL", "P_EOF",
  "'+'", "'-'", "'*'", "'/'", "'^'", "UMINUS", "UPLUS", "'['", "']'",
  "'('", "')'", "','", "$accept", "expression", "e", "arglist", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-62)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      82,    -5,   -62,   -30,   -62,   -62,   -62,   -62,   -20,    -7,
      72,    80,    81,    83,    92,    93,    94,    97,    98,   111,
     112,   113,   127,   128,   129,   143,   144,   145,   159,   160,
     161,   175,   176,   177,   191,   192,   193,   207,    82,    82,
      82,   164,    34,    -9,    82,    82,    82,    82,    82,    82,
      82,    82,    82,    82,    82,    82,    82,    82,    82,    82,
      82,    82,    82,    82,    82,    82,    82,    82,    82,    82,
      82,    82,    82,    82,    82,   -62,   -62,   -16,   -62,   -62,
      82,    82,    82,    82,    82,    31,   209,   306,    84,    95,
     109,   114,   125,   130,   -38,   141,   146,   157,   162,   173,
     178,   -33,   189,   194,   315,   -15,   -10,   205,   210,   -21,
     221,   226,   237,   242,   253,   258,   269,   274,    10,   -62,
     -28,   -28,   135,   135,   135,   -62,   -62,   -62,   -62,   -62,
     -62,   -62,   -62,   -62,    82,   -62,   -62,   -62,   -62,   -62,
     -62,    82,   -62,   -62,   -62,    82,   -62,   -62,   -62,    82,
     -62,   -62,   -62,   -62,   -62,   -62,   -62,   -62,   -62,   285,
     290,   315,   301,   -62,   -62,   -62
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     3,     6,    10,     8,     7,     9,    12,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    20,    19,     0,     1,     2,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    51,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    13,
      14,    15,    16,    17,    18,     4,     5,    11,    21,    39,
      49,    38,    48,    40,     0,    50,    23,    43,    46,    26,
      37,     0,    35,    36,    28,     0,    27,    22,    25,     0,
      42,    45,    29,    44,    47,    31,    32,    33,    30,     0,
       0,    52,     0,    41,    34,    24
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -62,   -62,     0,   -61
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,    41,   104,   105
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      42,   106,    43,    80,    81,    82,    83,    84,    80,    81,
      82,    83,    84,   118,   134,    82,    83,    84,    44,   141,
      80,    81,    82,    83,    84,    80,    81,    82,    83,    84,
      45,   149,    85,    86,   125,   119,   144,   145,    75,    76,
      77,   146,   145,    46,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   158,   145,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,    79,    80,    81,    82,    83,    84,
     120,   121,   122,   123,   124,     1,     2,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    47,    38,    39,    80,    81,    82,    83,    84,
      48,    49,    40,    50,   159,   128,    80,    81,    82,    83,
      84,   160,    51,    52,    53,   161,   129,    54,    55,   162,
      80,    81,    82,    83,    84,    80,    81,    82,    83,    84,
     130,    56,    57,    58,    78,   131,    80,    81,    82,    83,
      84,    80,    81,    82,    83,    84,   132,    59,    60,    61,
      84,   133,    80,    81,    82,    83,    84,    80,    81,    82,
      83,    84,   135,    62,    63,    64,     0,   136,    80,    81,
      82,    83,    84,    80,    81,    82,    83,    84,   137,    65,
      66,    67,   126,   138,    80,    81,    82,    83,    84,    80,
      81,    82,    83,    84,   139,    68,    69,    70,     0,   140,
      80,    81,    82,    83,    84,    80,    81,    82,    83,    84,
     142,    71,    72,    73,     0,   143,    80,    81,    82,    83,
      84,    80,    81,    82,    83,    84,   147,    74,     0,     0,
       0,   148,    80,    81,    82,    83,    84,    80,    81,    82,
      83,    84,   150,     0,     0,     0,     0,   151,    80,    81,
      82,    83,    84,    80,    81,    82,    83,    84,   152,     0,
       0,     0,     0,   153,    80,    81,    82,    83,    84,    80,
      81,    82,    83,    84,   154,     0,     0,     0,     0,   155,
      80,    81,    82,    83,    84,    80,    81,    82,    83,    84,
     156,     0,     0,     0,     0,   157,    80,    81,    82,    83,
      84,    80,    81,    82,    83,    84,   163,     0,     0,     0,
       0,   164,    80,    81,    82,    83,    84,    80,    81,    82,
      83,    84,   165,     0,     0,   127,    80,    81,    82,    83,
      84
};

static const yytype_int16 yycheck[] =
{
       0,    62,     7,    41,    42,    43,    44,    45,    41,    42,
      43,    44,    45,    74,    52,    43,    44,    45,    48,    52,
      41,    42,    43,    44,    45,    41,    42,    43,    44,    45,
      50,    52,    41,    42,     3,    51,    51,    52,    38,    39,
      40,    51,    52,    50,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    51,    52,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    40,    41,    42,    43,    44,    45,
      80,    81,    82,    83,    84,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    50,    41,    42,    41,    42,    43,    44,    45,
      50,    50,    50,    50,   134,    51,    41,    42,    43,    44,
      45,   141,    50,    50,    50,   145,    51,    50,    50,   149,
      41,    42,    43,    44,    45,    41,    42,    43,    44,    45,
      51,    50,    50,    50,     0,    51,    41,    42,    43,    44,
      45,    41,    42,    43,    44,    45,    51,    50,    50,    50,
      45,    51,    41,    42,    43,    44,    45,    41,    42,    43,
      44,    45,    51,    50,    50,    50,    -1,    51,    41,    42,
      43,    44,    45,    41,    42,    43,    44,    45,    51,    50,
      50,    50,     3,    51,    41,    42,    43,    44,    45,    41,
      42,    43,    44,    45,    51,    50,    50,    50,    -1,    51,
      41,    42,    43,    44,    45,    41,    42,    43,    44,    45,
      51,    50,    50,    50,    -1,    51,    41,    42,    43,    44,
      45,    41,    42,    43,    44,    45,    51,    50,    -1,    -1,
      -1,    51,    41,    42,    43,    44,    45,    41,    42,    43,
      44,    45,    51,    -1,    -1,    -1,    -1,    51,    41,    42,
      43,    44,    45,    41,    42,    43,    44,    45,    51,    -1,
      -1,    -1,    -1,    51,    41,    42,    43,    44,    45,    41,
      42,    43,    44,    45,    51,    -1,    -1,    -1,    -1,    51,
      41,    42,    43,    44,    45,    41,    42,    43,    44,    45,
      51,    -1,    -1,    -1,    -1,    51,    41,    42,    43,    44,
      45,    41,    42,    43,    44,    45,    51,    -1,    -1,    -1,
      -1,    51,    41,    42,    43,    44,    45,    41,    42,    43,
      44,    45,    51,    -1,    -1,    49,    41,    42,    43,    44,
      45
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    41,    42,
      50,    54,    55,     7,    48,    50,    50,    50,    50,    50,
      50,    50,    50,    50,    50,    50,    50,    50,    50,    50,
      50,    50,    50,    50,    50,    50,    50,    50,    50,    50,
      50,    50,    50,    50,    50,    55,    55,    55,     0,    40,
      41,    42,    43,    44,    45,    41,    42,    55,    55,    55,
      55,    55,    55,    55,    55,    55,    55,    55,    55,    55,
      55,    55,    55,    55,    55,    56,    56,    55,    55,    55,
      55,    55,    55,    55,    55,    55,    55,    55,    56,    51,
      55,    55,    55,    55,    55,     3,     3,    49,    51,    51,
      51,    51,    51,    51,    52,    51,    51,    51,    51,    51,
      51,    52,    51,    51,    51,    52,    51,    51,    51,    52,
      51,    51,    51,    51,    51,    51,    51,    51,    51,    55,
      55,    55,    55,    51,    51,    51
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    53,    54,    55,    55,    55,    55,    55,    55,    55,
      55,    55,    55,    55,    55,    55,    55,    55,    55,    55,
      55,    55,    55,    55,    55,    55,    55,    55,    55,    55,
      55,    55,    55,    55,    55,    55,    55,    55,    55,    55,
      55,    55,    55,    55,    55,    55,    55,    55,    55,    55,
      55,    56,    56
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     4,     4,     1,     1,     1,     1,
       1,     4,     1,     3,     3,     3,     3,     3,     3,     2,
       2,     4,     4,     4,     6,     4,     4,     4,     4,     4,
       4,     4,     4,     4,     6,     4,     4,     4,     4,     4,
       4,     6,     4,     4,     4,     4,     4,     4,     4,     4,
       4,     1,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */


/* User initialization code.  */
#line 105 "../../include/jx-af/jexpr/JExprParserY.y"
{
//	yydebug = 1;
}

#line 1139 "../../include/jx-af/jexpr/JExprParserY.cpp"

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* expression: e P_EOF  */
#line 120 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		itsParseResult = (yyvsp[-1].pFunction);
		YYACCEPT;
	}
#line 1345 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 3: /* e: P_NUMBER  */
#line 128 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		JPtrArray<JString> s(JPtrArrayT::kDeleteAll);
		(yyvsp[0].pString)->ToLower();
		(yyvsp[0].pString)->Split("e", &s, 2);

		if (s.GetElementCount() == 2)
		{
			JFloat v, e;
			if (!s.GetElement(1)->ConvertToFloat(&v) ||
				!s.GetElement(2)->ConvertToFloat(&e))
			{
				YYERROR;
			}
			JProduct* p = jnew JProduct();
			assert( p != nullptr );
			p->AppendArg(jnew JConstantValue(v));
			p->AppendArg(jnew JExponent(jnew JConstantValue(10), jnew JConstantValue(e)));
			(yyval.pFunction) = p;
		}
		else
		{
			JFloat v;
			if (!(yyvsp[0].pString)->ConvertToFloat(&v))
			{
				YYERROR;
			}
			(yyval.pFunction) = jnew JConstantValue(v);
			assert( (yyval.pFunction) != nullptr );
		}
	}
#line 1380 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 4: /* e: P_NUMBER P_E '+' P_NUMBER  */
#line 160 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		JFloat v, e;
		if (!(yyvsp[-3].pString)->ConvertToFloat(&v) || !(yyvsp[0].pString)->ConvertToFloat(&e))
		{
			YYERROR;
		}
		JProduct* p = jnew JProduct();
		assert( p != nullptr );
		p->AppendArg(jnew JConstantValue(v));
		p->AppendArg(jnew JExponent(jnew JConstantValue(10), jnew JConstantValue(e)));
		(yyval.pFunction) = p;
	}
#line 1397 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 5: /* e: P_NUMBER P_E '-' P_NUMBER  */
#line 174 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		JFloat v, e;
		if (!(yyvsp[-3].pString)->ConvertToFloat(&v) || !(yyvsp[0].pString)->ConvertToFloat(&e))
		{
			YYERROR;
		}
		JProduct* p = jnew JProduct();
		assert( p != nullptr );
		p->AppendArg(jnew JConstantValue(v));
		p->AppendArg(jnew JExponent(jnew JConstantValue(10), jnew JNegation(jnew JConstantValue(e))));
		(yyval.pFunction) = p;
	}
#line 1414 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 6: /* e: P_HEX  */
#line 188 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		JFloat v;
		if (!(yyvsp[0].pString)->ConvertToFloat(&v))
		{
			YYERROR;
		}
		(yyval.pFunction) = jnew JConstantValue(v);
		assert( (yyval.pFunction) != nullptr );
	}
#line 1428 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 7: /* e: P_E  */
#line 199 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JNamedConstant(kEJNamedConstIndex);
		assert( (yyval.pFunction) != nullptr );
	}
#line 1437 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 8: /* e: P_PI  */
#line 205 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JNamedConstant(kPiJNamedConstIndex);
		assert( (yyval.pFunction) != nullptr );
	}
#line 1446 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 9: /* e: P_I  */
#line 211 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JNamedConstant(kIJNamedConstIndex);
		assert( (yyval.pFunction) != nullptr );
	}
#line 1455 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 10: /* e: P_VARIABLE  */
#line 217 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		JIndex i;
		if (!itsVarList->ParseVariableName(*(yyvsp[0].pString), &i))
		{
			YYERROR;
		}
		(yyval.pFunction) = jnew JVariableValue(itsVarList, i, nullptr);
		assert( (yyval.pFunction) != nullptr );
	}
#line 1469 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 11: /* e: P_VARIABLE '[' e ']'  */
#line 228 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		JIndex i;
		if (!itsVarList->ParseVariableName(*(yyvsp[-3].pString), &i))
		{
			YYERROR;
		}
		(yyval.pFunction) = jnew JVariableValue(itsVarList, i, (yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1483 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 12: /* e: P_INPUT  */
#line 239 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		if (itsEditor != nullptr)
		{
			(yyval.pFunction) = jnew JUserInputFunction(itsEditor);
		}
		else
		{
			YYERROR;
		}
		assert( (yyval.pFunction) != nullptr );
	}
#line 1499 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 13: /* e: '(' e ')'  */
#line 252 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = (yyvsp[-1].pFunction);
	}
#line 1507 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 14: /* e: e '+' e  */
#line 257 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = UpdateSum((yyvsp[-2].pFunction), (yyvsp[0].pFunction));
	}
#line 1515 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 15: /* e: e '-' e  */
#line 262 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = UpdateSum((yyvsp[-2].pFunction), jnew JNegation((yyvsp[0].pFunction)));
	}
#line 1523 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 16: /* e: e '*' e  */
#line 267 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = UpdateProduct((yyvsp[-2].pFunction), (yyvsp[0].pFunction));
	}
#line 1531 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 17: /* e: e '/' e  */
#line 272 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JDivision((yyvsp[-2].pFunction), (yyvsp[0].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1540 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 18: /* e: e '^' e  */
#line 278 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JExponent((yyvsp[-2].pFunction), (yyvsp[0].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1549 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 19: /* e: '-' e  */
#line 284 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JNegation((yyvsp[0].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1558 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 20: /* e: '+' e  */
#line 290 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = (yyvsp[0].pFunction);
	}
#line 1566 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 21: /* e: P_FN_ABS '(' e ')'  */
#line 295 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JAbsValue((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1575 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 22: /* e: P_FN_PHASE '(' e ')'  */
#line 301 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JPhaseAngle((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1584 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 23: /* e: P_FN_CONJUGATE '(' e ')'  */
#line 307 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JConjugate((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1593 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 24: /* e: P_FN_ROTATE '(' e ',' e ')'  */
#line 313 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JRotateComplex((yyvsp[-3].pFunction), (yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1602 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 25: /* e: P_FN_RE '(' e ')'  */
#line 319 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JRealPart((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1611 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 26: /* e: P_FN_IM '(' e ')'  */
#line 325 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JImagPart((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1620 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 27: /* e: P_FN_MIN '(' arglist ')'  */
#line 331 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JMinFunc((yyvsp[-1].pList));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1629 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 28: /* e: P_FN_MAX '(' arglist ')'  */
#line 337 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JMaxFunc((yyvsp[-1].pList));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1638 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 29: /* e: P_FN_SQRT '(' e ')'  */
#line 343 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JSquareRoot((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1647 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 30: /* e: P_FN_PARALLEL '(' arglist ')'  */
#line 349 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JParallel((yyvsp[-1].pList));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1656 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 31: /* e: P_FN_SIGN '(' e ')'  */
#line 355 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JAlgSign((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1665 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 32: /* e: P_FN_ROUND '(' e ')'  */
#line 361 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JRoundToInt((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1674 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 33: /* e: P_FN_TRUNCATE '(' e ')'  */
#line 367 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JTruncateToInt((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1683 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 34: /* e: P_FN_LOG '(' e ',' e ')'  */
#line 373 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JLogB((yyvsp[-3].pFunction), (yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1692 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 35: /* e: P_FN_LOG2 '(' e ')'  */
#line 379 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JLogB(jnew JConstantValue(2), (yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1701 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 36: /* e: P_FN_LOG10 '(' e ')'  */
#line 385 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JLogB(jnew JConstantValue(10), (yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1710 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 37: /* e: P_FN_LN '(' e ')'  */
#line 391 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JLogE((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1719 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 38: /* e: P_FN_ARCSIN '(' e ')'  */
#line 397 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JArcSine((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1728 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 39: /* e: P_FN_ARCCOS '(' e ')'  */
#line 403 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JArcCosine((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1737 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 40: /* e: P_FN_ARCTAN '(' e ')'  */
#line 409 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JArcTangent((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1746 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 41: /* e: P_FN_ARCTAN2 '(' e ',' e ')'  */
#line 415 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JArcTangent2((yyvsp[-3].pFunction), (yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1755 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 42: /* e: P_FN_SIN '(' e ')'  */
#line 421 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JSine((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1764 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 43: /* e: P_FN_COS '(' e ')'  */
#line 427 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JCosine((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1773 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 44: /* e: P_FN_TAN '(' e ')'  */
#line 433 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JTangent((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1782 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 45: /* e: P_FN_SINH '(' e ')'  */
#line 439 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JHypSine((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1791 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 46: /* e: P_FN_COSH '(' e ')'  */
#line 445 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JHypCosine((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1800 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 47: /* e: P_FN_TANH '(' e ')'  */
#line 451 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JHypTangent((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1809 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 48: /* e: P_FN_ARCSINH '(' e ')'  */
#line 457 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JArcHypSine((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1818 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 49: /* e: P_FN_ARCCOSH '(' e ')'  */
#line 463 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JArcHypCosine((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1827 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 50: /* e: P_FN_ARCTANH '(' e ')'  */
#line 469 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pFunction) = jnew JArcHypTangent((yyvsp[-1].pFunction));
		assert( (yyval.pFunction) != nullptr );
	}
#line 1836 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 51: /* arglist: e  */
#line 477 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pList) = jnew JPtrArray<JFunction>(JPtrArrayT::kDeleteAll);
		assert( (yyval.pList) != nullptr );
		(yyval.pList)->AppendElement((yyvsp[0].pFunction));
	}
#line 1846 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;

  case 52: /* arglist: arglist ',' e  */
#line 484 "../../include/jx-af/jexpr/JExprParserY.y"
        {
		(yyval.pList) = (yyvsp[-2].pList); (yyval.pList)->AppendElement((yyvsp[0].pFunction));
	}
#line 1854 "../../include/jx-af/jexpr/JExprParserY.cpp"
    break;


#line 1858 "../../include/jx-af/jexpr/JExprParserY.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

