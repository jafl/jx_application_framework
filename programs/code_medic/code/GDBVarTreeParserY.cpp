/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     P_NAME = 258,
     P_NAME_EQ = 259,
     P_EMPTY_BRACKET = 260,
     P_EMPTY_BRACKET_EQ = 261,
     P_NO_DATA_FIELDS = 262,
     P_INTEGER = 263,
     P_HEX = 264,
     P_FLOAT = 265,
     P_CHAR = 266,
     P_STRING = 267,
     P_EMPTY_SUMMARY = 268,
     P_STATIC = 269,
     P_BRACKET = 270,
     P_BRACKET_EQ = 271,
     P_GROUP_OPEN = 272,
     P_GROUP_CLOSE = 273,
     P_PAREN_EXPR = 274,
     P_SUMMARY = 275,
     P_EOF = 276
   };
#endif
/* Tokens.  */
#define P_NAME 258
#define P_NAME_EQ 259
#define P_EMPTY_BRACKET 260
#define P_EMPTY_BRACKET_EQ 261
#define P_NO_DATA_FIELDS 262
#define P_INTEGER 263
#define P_HEX 264
#define P_FLOAT 265
#define P_CHAR 266
#define P_STRING 267
#define P_EMPTY_SUMMARY 268
#define P_STATIC 269
#define P_BRACKET 270
#define P_BRACKET_EQ 271
#define P_GROUP_OPEN 272
#define P_GROUP_CLOSE 273
#define P_PAREN_EXPR 274
#define P_SUMMARY 275
#define P_EOF 276




/* Copy the first part of user declarations.  */
#line 1 "code/GDBVarTreeParserY.y"

#include "CMVarNode.h"
#include "GDBVarGroupInfo.h"
#include "cmGlobals.h"
#include <JStringIterator.h>

// also uncomment yydebug=1; below
//#define YYERROR_VERBOSE
//#define YYDEBUG 1

inline bool
cmIsOpenablePointer
	(
	const JString& s
	)
{
	return s != "0x0";
}



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 22 "code/GDBVarTreeParserY.y"
{
	JString*				pString;
	CMVarNode*				pNode;
	JPtrArray<CMVarNode>*	pList;
	GDBVarGroupInfo*		pGroup;
}
/* Line 193 of yacc.c.  */
#line 166 "code/GDBVarTreeParserY.cpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */
#line 29 "code/GDBVarTreeParserY.y"

#define NOTAB
#include "GDBVarTreeParser.h"

#define yyparse GDBVarTreeParser::yyparse
#line 53 "code/GDBVarTreeParserY.y"


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

#line 76 "code/GDBVarTreeParserY.y"

#include <jAssert.h>	// must be last


/* Line 216 of yacc.c.  */
#line 210 "code/GDBVarTreeParserY.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  49
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   155

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  26
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  11
/* YYNRULES -- Number of rules.  */
#define YYNRULES  59
/* YYNRULES -- Number of states.  */
#define YYNSTATES  95

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   276

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    22,     2,    23,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    25,     2,
       2,     2,     2,     2,    24,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     6,    10,    13,    17,    21,    25,    29,
      34,    39,    44,    46,    50,    54,    58,    62,    67,    70,
      73,    75,    77,    79,    82,    84,    87,    90,    94,    96,
      98,   102,   107,   109,   111,   113,   115,   117,   119,   122,
     125,   127,   129,   132,   136,   139,   142,   145,   148,   151,
     153,   157,   160,   163,   166,   168,   171,   173,   176,   178
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      27,     0,    -1,    28,    21,    -1,    36,    28,    21,    -1,
      34,    21,    -1,    36,    34,    21,    -1,    17,    29,    18,
      -1,    17,    32,    18,    -1,    17,    28,    18,    -1,    20,
      17,    29,    18,    -1,    20,    17,    32,    18,    -1,    20,
      17,    28,    18,    -1,    30,    -1,    29,    22,    30,    -1,
      29,    22,    13,    -1,    28,    22,    28,    -1,    29,    22,
      28,    -1,    29,    23,    23,    23,    -1,    31,    34,    -1,
      31,    28,    -1,     7,    -1,    31,    -1,     4,    -1,    14,
       4,    -1,    16,    -1,    14,    16,    -1,     3,    16,    -1,
      14,     3,    16,    -1,     6,    -1,    33,    -1,    32,    22,
      33,    -1,    32,    23,    23,    23,    -1,    34,    -1,     3,
      -1,     5,    -1,     8,    -1,    10,    -1,     9,    -1,    24,
       9,    -1,    19,     9,    -1,    13,    -1,    12,    -1,     9,
      12,    -1,    19,     9,    12,    -1,     9,    15,    -1,     3,
      15,    -1,     8,    15,    -1,    10,    15,    -1,    11,    15,
      -1,    15,    -1,     9,    35,    19,    -1,     3,    19,    -1,
       8,    11,    -1,    17,    18,    -1,     3,    -1,    14,     3,
      -1,    15,    -1,    14,    15,    -1,     5,    -1,    19,    24,
       9,    25,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    90,    90,   104,   121,   132,   148,   154,   160,   171,
     177,   183,   197,   204,   210,   227,   238,   258,   268,   287,
     306,   314,   331,   333,   342,   344,   353,   361,   372,   377,
     384,   390,   402,   414,   416,   418,   420,   422,   428,   435,
     445,   447,   449,   458,   470,   480,   489,   498,   507,   516,
     518,   529,   538,   547,   556,   558,   567,   569,   578,   583
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "P_NAME", "P_NAME_EQ", "P_EMPTY_BRACKET",
  "P_EMPTY_BRACKET_EQ", "P_NO_DATA_FIELDS", "P_INTEGER", "P_HEX",
  "P_FLOAT", "P_CHAR", "P_STRING", "P_EMPTY_SUMMARY", "P_STATIC",
  "P_BRACKET", "P_BRACKET_EQ", "P_GROUP_OPEN", "P_GROUP_CLOSE",
  "P_PAREN_EXPR", "P_SUMMARY", "P_EOF", "','", "'.'", "'@'", "':'",
  "$accept", "top_group", "group", "node_list", "node", "name_eq",
  "value_list", "value_node", "value", "name", "reference_value", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,    44,    46,    64,    58
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    26,    27,    27,    27,    27,    28,    28,    28,    28,
      28,    28,    29,    29,    29,    29,    29,    29,    30,    30,
      30,    30,    31,    31,    31,    31,    31,    31,    31,    32,
      32,    32,    33,    34,    34,    34,    34,    34,    34,    34,
      34,    34,    34,    34,    34,    34,    34,    34,    34,    34,
      34,    34,    34,    34,    35,    35,    35,    35,    35,    36
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     3,     2,     3,     3,     3,     3,     4,
       4,     4,     1,     3,     3,     3,     3,     4,     2,     2,
       1,     1,     1,     2,     1,     2,     2,     3,     1,     1,
       3,     4,     1,     1,     1,     1,     1,     1,     2,     2,
       1,     1,     2,     3,     2,     2,     2,     2,     2,     1,
       3,     2,     2,     2,     1,     2,     1,     2,     1,     4
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    33,    34,    35,    37,    36,     0,    41,    40,    49,
       0,     0,     0,     0,     0,     0,     0,     0,    45,    51,
      52,    46,    54,    58,    42,     0,    44,     0,    47,    48,
      33,    22,    28,    20,     0,    24,    53,     0,     0,     0,
      12,    21,     0,    29,    32,    39,     0,     0,    38,     1,
       2,     4,     0,     0,    55,    57,    50,    26,     0,    23,
      25,     8,     0,     6,     0,     0,    19,    18,     7,     0,
       0,    43,     0,     0,     0,     0,     3,     5,    27,     0,
      15,     0,    14,    16,    13,     0,     0,    30,     0,    59,
      11,     9,    10,    17,    31
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,    14,    38,    39,    40,    41,    42,    43,    44,    27,
      17
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -15
static const yytype_int16 yypact[] =
{
      80,    24,   -15,    34,    41,   -10,    -8,   -15,   -15,   -15,
      16,    28,    43,     5,    63,    59,    66,    98,   -15,   -15,
     -15,   -15,   -15,   -15,   -15,    35,    65,    77,   -15,   -15,
      -3,   -15,   -15,   -15,    -1,   -15,   -15,    93,    39,   -14,
     -15,    98,   -12,   -15,   -15,   100,    96,    62,   -15,   -15,
     -15,   -15,    95,    99,   -15,   -15,   -15,   -15,   107,   -15,
     -15,   -15,    31,   -15,   135,    91,   -15,   -15,   -15,   116,
     111,   -15,   105,    76,    36,   114,   -15,   -15,   -15,    62,
     -15,   127,   -15,   -15,   -15,   121,   128,   -15,   122,   -15,
     -15,   -15,   -15,   -15,   -15
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -15,   -15,     0,   103,    83,   -15,   106,    85,     1,   -15,
     -15
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -57
static const yytype_int8 yytable[] =
{
      15,    16,    58,    59,    63,    28,    68,    29,    64,    65,
      69,    70,    18,    57,    48,    60,    19,    52,    53,    30,
      31,     2,    32,    33,     3,     4,     5,     6,     7,     8,
      34,     9,    35,    10,    36,    37,    12,    45,    54,    18,
      13,    66,    67,    19,    22,    20,    23,    73,    79,    21,
      55,    12,    46,    24,    91,    25,    26,    61,    64,    65,
      47,    62,    80,    49,    83,    30,    31,     2,    32,    33,
       3,     4,     5,     6,     7,     8,    34,     9,    35,    10,
      50,    37,    12,     1,   -56,     2,    13,    51,     3,     4,
       5,     6,     7,     8,    90,     9,    56,    10,    62,    11,
      12,     1,    45,     2,    13,    72,     3,     4,     5,     6,
       7,     8,    71,     9,    85,    10,    76,    37,    12,     1,
      77,     2,    13,    78,     3,     4,     5,     6,     7,     8,
      89,     9,    92,    86,    88,    37,    69,    70,    81,    31,
      13,    32,    33,    57,    93,    94,    36,    84,    82,    34,
      74,    35,    79,    75,    87,    12
};

static const yytype_uint8 yycheck[] =
{
       0,     0,     3,     4,    18,    15,    18,    15,    22,    23,
      22,    23,    15,    16,     9,    16,    19,    17,    17,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,     9,     3,    15,
      24,    41,    41,    19,     3,    11,     5,    47,    17,    15,
      15,    20,    24,    12,    18,    14,    15,    18,    22,    23,
      17,    22,    62,     0,    64,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      21,    19,    20,     3,    19,     5,    24,    21,     8,     9,
      10,    11,    12,    13,    18,    15,    19,    17,    22,    19,
      20,     3,     9,     5,    24,     9,     8,     9,    10,    11,
      12,    13,    12,    15,    23,    17,    21,    19,    20,     3,
      21,     5,    24,    16,     8,     9,    10,    11,    12,    13,
      25,    15,    18,    17,    23,    19,    22,    23,     3,     4,
      24,     6,     7,    16,    23,    23,    18,    64,    13,    14,
      47,    16,    17,    47,    69,    20
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     5,     8,     9,    10,    11,    12,    13,    15,
      17,    19,    20,    24,    27,    28,    34,    36,    15,    19,
      11,    15,     3,     5,    12,    14,    15,    35,    15,    15,
       3,     4,     6,     7,    14,    16,    18,    19,    28,    29,
      30,    31,    32,    33,    34,     9,    24,    17,     9,     0,
      21,    21,    28,    34,     3,    15,    19,    16,     3,     4,
      16,    18,    22,    18,    22,    23,    28,    34,    18,    22,
      23,    12,     9,    28,    29,    32,    21,    21,    16,    17,
      28,     3,    13,    28,    30,    23,    17,    33,    23,    25,
      18,    18,    18,    23,    23
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
static int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
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



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}









/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  /* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;

  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;


  /* User initialization code.  */
#line 81 "code/GDBVarTreeParserY.y"
{
	itsGroupDepth   = 0;
	itsGDBErrorFlag = false;
//	yydebug         = 1;
}
/* Line 1078 of yacc.c.  */
#line 1321 "code/GDBVarTreeParserY.cpp"
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 91 "code/GDBVarTreeParserY.y"
    {
		itsCurrentNode = (yyval.pNode) = CMGetLink()->CreateVarNode(nullptr, JString::empty, JString::empty, (yyvsp[(1) - (2)].pGroup)->GetName());
		for (JIndex i=1; i<=(yyvsp[(1) - (2)].pGroup)->list->GetElementCount(); i++)
			{
			itsCurrentNode->Append((yyvsp[(1) - (2)].pGroup)->list->GetElement(i));
			}
		itsIsPointerFlag = false;

		jdelete (yyvsp[(1) - (2)].pGroup);

		YYACCEPT;
		;}
    break;

  case 3:
#line 105 "code/GDBVarTreeParserY.y"
    {
		itsCurrentNode = (yyval.pNode) = CMGetLink()->CreateVarNode(nullptr, JString::empty, JString::empty, *(yyvsp[(1) - (3)].pString));
		for (JIndex i=1; i<=(yyvsp[(2) - (3)].pGroup)->list->GetElementCount(); i++)
			{
			itsCurrentNode->Append((yyvsp[(2) - (3)].pGroup)->list->GetElement(i));
			}
		itsIsPointerFlag = false;

		jdelete (yyvsp[(1) - (3)].pString);
		jdelete (yyvsp[(2) - (3)].pGroup);

		YYACCEPT;
		;}
    break;

  case 4:
#line 122 "code/GDBVarTreeParserY.y"
    {
		itsCurrentNode = (yyval.pNode) = CMGetLink()->CreateVarNode(nullptr, JString::empty, JString::empty, *(yyvsp[(1) - (2)].pString));
		itsCurrentNode->MakePointer(itsIsPointerFlag);
		itsIsPointerFlag = false;

		jdelete (yyvsp[(1) - (2)].pString);

		YYACCEPT;
		;}
    break;

  case 5:
#line 133 "code/GDBVarTreeParserY.y"
    {
		itsCurrentNode = (yyval.pNode) = CMGetLink()->CreateVarNode(nullptr, JString::empty, JString::empty, *(yyvsp[(1) - (3)].pString));
		CMVarNode* child = CMGetLink()->CreateVarNode(itsCurrentNode, JString::empty, JString::empty, *(yyvsp[(2) - (3)].pString));
		child->MakePointer(itsIsPointerFlag);
		itsIsPointerFlag = false;

		jdelete (yyvsp[(1) - (3)].pString);
		jdelete (yyvsp[(2) - (3)].pString);

		YYACCEPT;
		;}
    break;

  case 6:
#line 149 "code/GDBVarTreeParserY.y"
    {
		(yyval.pGroup) = jnew GDBVarGroupInfo(nullptr, (yyvsp[(2) - (3)].pList));
		assert( (yyval.pGroup) != nullptr );
		;}
    break;

  case 7:
#line 155 "code/GDBVarTreeParserY.y"
    {
		(yyval.pGroup) = jnew GDBVarGroupInfo(nullptr, (yyvsp[(2) - (3)].pList));
		assert( (yyval.pGroup) != nullptr );
		;}
    break;

  case 8:
#line 161 "code/GDBVarTreeParserY.y"
    {
		JPtrArray<CMVarNode>* list = jnew JPtrArray<CMVarNode>(JPtrArrayT::kForgetAll);
		assert( list != nullptr );
		AppendAsArrayElement(JString::empty, *((yyvsp[(2) - (3)].pGroup)->list), list);
		(yyval.pGroup) = jnew GDBVarGroupInfo(nullptr, list);
		assert( (yyval.pGroup) != nullptr );

		jdelete (yyvsp[(2) - (3)].pGroup);
		;}
    break;

  case 9:
#line 172 "code/GDBVarTreeParserY.y"
    {
		(yyval.pGroup) = jnew GDBVarGroupInfo((yyvsp[(1) - (4)].pString), (yyvsp[(3) - (4)].pList));
		assert( (yyval.pGroup) != nullptr );
		;}
    break;

  case 10:
#line 178 "code/GDBVarTreeParserY.y"
    {
		(yyval.pGroup) = jnew GDBVarGroupInfo((yyvsp[(1) - (4)].pString), (yyvsp[(3) - (4)].pList));
		assert( (yyval.pGroup) != nullptr );
		;}
    break;

  case 11:
#line 184 "code/GDBVarTreeParserY.y"
    {
		JPtrArray<CMVarNode>* list = jnew JPtrArray<CMVarNode>(JPtrArrayT::kForgetAll);
		assert( list != nullptr );
		AppendAsArrayElement((yyvsp[(3) - (4)].pGroup)->GetName(), *((yyvsp[(3) - (4)].pGroup)->list), list);
		(yyval.pGroup) = jnew GDBVarGroupInfo((yyvsp[(1) - (4)].pString), list);
		assert( (yyval.pGroup) != nullptr );

		jdelete (yyvsp[(3) - (4)].pGroup);
		;}
    break;

  case 12:
#line 198 "code/GDBVarTreeParserY.y"
    {
		JPtrArray<CMVarNode>* list = (yyval.pList) = jnew JPtrArray<CMVarNode>(JPtrArrayT::kForgetAll);
		assert( list != nullptr );
		list->Append((yyvsp[(1) - (1)].pNode));
		;}
    break;

  case 13:
#line 205 "code/GDBVarTreeParserY.y"
    {
		(yyval.pList) = (yyvsp[(1) - (3)].pList);
		(yyval.pList)->Append((yyvsp[(3) - (3)].pNode));
		;}
    break;

  case 14:
#line 211 "code/GDBVarTreeParserY.y"
    {
		(yyval.pList) = (yyvsp[(1) - (3)].pList);

		CMVarNode* node = CMGetLink()->CreateVarNode(nullptr, JString::empty, JString::empty, *(yyvsp[(3) - (3)].pString));
		if ((((yyval.pList)->GetFirstElement())->GetName()).BeginsWith(JString("[", JString::kNoCopy)))
			{
			AppendAsArrayElement(node, (yyval.pList));
			}
		else
			{
			(yyval.pList)->Append(node);
			}

		jdelete (yyvsp[(3) - (3)].pString);
		;}
    break;

  case 15:
#line 228 "code/GDBVarTreeParserY.y"
    {
		JPtrArray<CMVarNode>* list = (yyval.pList) = jnew JPtrArray<CMVarNode>(JPtrArrayT::kForgetAll);
		assert( list != nullptr );
		AppendAsArrayElement((yyvsp[(1) - (3)].pGroup)->GetName(), *((yyvsp[(1) - (3)].pGroup)->list), list);
		AppendAsArrayElement((yyvsp[(3) - (3)].pGroup)->GetName(), *((yyvsp[(3) - (3)].pGroup)->list), list);

		jdelete (yyvsp[(1) - (3)].pGroup);
		jdelete (yyvsp[(3) - (3)].pGroup);
		;}
    break;

  case 16:
#line 239 "code/GDBVarTreeParserY.y"
    {
		if ((((yyvsp[(1) - (3)].pList)->GetFirstElement())->GetName()).BeginsWith(JString("[", JString::kNoCopy)))
			{
			(yyval.pList) = (yyvsp[(1) - (3)].pList);
			}
		else
			{
			JPtrArray<CMVarNode>* list = (yyval.pList) = jnew JPtrArray<CMVarNode>(JPtrArrayT::kForgetAll);
			assert( list != nullptr );
			AppendAsArrayElement(JString::empty, *(yyvsp[(1) - (3)].pList), list);

			jdelete (yyvsp[(1) - (3)].pList);
			}

		AppendAsArrayElement((yyvsp[(3) - (3)].pGroup)->GetName(), *((yyvsp[(3) - (3)].pGroup)->list), (yyval.pList));

		jdelete (yyvsp[(3) - (3)].pGroup);
		;}
    break;

  case 17:
#line 259 "code/GDBVarTreeParserY.y"
    {
		(yyval.pList) = (yyvsp[(1) - (4)].pList);
		CMVarNode* child = CMGetLink()->CreateVarNode(nullptr, JString("...", JString::kNoCopy), JString::empty, JString::empty);
		(yyval.pList)->Append(child);
		;}
    break;

  case 18:
#line 269 "code/GDBVarTreeParserY.y"
    {
		if ((yyvsp[(1) - (2)].pString)->EndsWith("="))
			{
			JStringIterator iter((yyvsp[(1) - (2)].pString), kJIteratorStartAtEnd);
			iter.RemovePrev();
			(yyvsp[(1) - (2)].pString)->TrimWhitespace();
			}
		itsCurrentNode = (yyval.pNode) = CMGetLink()->CreateVarNode(nullptr, *(yyvsp[(1) - (2)].pString), JString::empty, *(yyvsp[(2) - (2)].pString));
		if (!(yyvsp[(1) - (2)].pString)->BeginsWith("_vptr.") && !(yyvsp[(1) - (2)].pString)->BeginsWith("_vb."))
			{
			itsCurrentNode->MakePointer(itsIsPointerFlag);
			}
		itsIsPointerFlag = false;

		jdelete (yyvsp[(1) - (2)].pString);
		jdelete (yyvsp[(2) - (2)].pString);
		;}
    break;

  case 19:
#line 288 "code/GDBVarTreeParserY.y"
    {
		if ((yyvsp[(1) - (2)].pString)->EndsWith("="))
			{
			JStringIterator iter((yyvsp[(1) - (2)].pString), kJIteratorStartAtEnd);
			iter.RemovePrev();
			(yyvsp[(1) - (2)].pString)->TrimWhitespace();
			}
		itsCurrentNode = (yyval.pNode) = CMGetLink()->CreateVarNode(nullptr, *(yyvsp[(1) - (2)].pString), JString::empty, (yyvsp[(2) - (2)].pGroup)->GetName());
		itsIsPointerFlag = false;
		for (JIndex i=1; i<=(yyvsp[(2) - (2)].pGroup)->list->GetElementCount(); i++)
			{
			itsCurrentNode->Append((yyvsp[(2) - (2)].pGroup)->list->GetElement(i));
			}

		jdelete (yyvsp[(1) - (2)].pString);
		jdelete (yyvsp[(2) - (2)].pGroup);
		;}
    break;

  case 20:
#line 307 "code/GDBVarTreeParserY.y"
    {
		itsCurrentNode = (yyval.pNode) = CMGetLink()->CreateVarNode(nullptr, *(yyvsp[(1) - (1)].pString), JString::empty, JString::empty);
		itsIsPointerFlag = false;

		jdelete (yyvsp[(1) - (1)].pString);
		;}
    break;

  case 21:
#line 315 "code/GDBVarTreeParserY.y"
    {
		if ((yyvsp[(1) - (1)].pString)->EndsWith("="))
			{
			JStringIterator iter((yyvsp[(1) - (1)].pString), kJIteratorStartAtEnd);
			iter.RemovePrev();
			(yyvsp[(1) - (1)].pString)->TrimWhitespace();
			}
		itsCurrentNode = (yyval.pNode) = CMGetLink()->CreateVarNode(nullptr, *(yyvsp[(1) - (1)].pString), JString::empty, JString("<nothing>", JString::kNoCopy));
		itsIsPointerFlag = false;

		jdelete (yyvsp[(1) - (1)].pString);
		;}
    break;

  case 23:
#line 334 "code/GDBVarTreeParserY.y"
    {
		(yyval.pString) = (yyvsp[(1) - (2)].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[(2) - (2)].pString));

		jdelete (yyvsp[(2) - (2)].pString);
		;}
    break;

  case 25:
#line 345 "code/GDBVarTreeParserY.y"
    {
		(yyval.pString) = (yyvsp[(1) - (2)].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[(2) - (2)].pString));

		jdelete (yyvsp[(2) - (2)].pString);
		;}
    break;

  case 26:
#line 354 "code/GDBVarTreeParserY.y"
    {
		(yyval.pString) = (yyvsp[(1) - (2)].pString);
		(yyval.pString)->Append(*(yyvsp[(2) - (2)].pString));

		jdelete (yyvsp[(2) - (2)].pString);
		;}
    break;

  case 27:
#line 362 "code/GDBVarTreeParserY.y"
    {
		(yyval.pString) = (yyvsp[(1) - (3)].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[(2) - (3)].pString));
		(yyval.pString)->Append(*(yyvsp[(3) - (3)].pString));

		jdelete (yyvsp[(2) - (3)].pString);
		jdelete (yyvsp[(3) - (3)].pString);
		;}
    break;

  case 29:
#line 378 "code/GDBVarTreeParserY.y"
    {
		JPtrArray<CMVarNode>* list = (yyval.pList) = jnew JPtrArray<CMVarNode>(JPtrArrayT::kForgetAll);
		assert( list != nullptr );
		AppendAsArrayElement((yyvsp[(1) - (1)].pNode), (yyval.pList));
		;}
    break;

  case 30:
#line 385 "code/GDBVarTreeParserY.y"
    {
		(yyval.pList) = (yyvsp[(1) - (3)].pList);
		AppendAsArrayElement((yyvsp[(3) - (3)].pNode), (yyval.pList));
		;}
    break;

  case 31:
#line 391 "code/GDBVarTreeParserY.y"
    {
		(yyval.pList) = (yyvsp[(1) - (4)].pList);
		CMVarNode* child = CMGetLink()->CreateVarNode(nullptr, JString("...", JString::kNoCopy), JString::empty, JString::empty);
		(yyval.pList)->Append(child);
		;}
    break;

  case 32:
#line 403 "code/GDBVarTreeParserY.y"
    {
		itsCurrentNode = (yyval.pNode) = CMGetLink()->CreateVarNode(nullptr, JString::empty, JString::empty, *(yyvsp[(1) - (1)].pString));
		itsCurrentNode->MakePointer(itsIsPointerFlag);
		itsIsPointerFlag = false;

		jdelete (yyvsp[(1) - (1)].pString);
		;}
    break;

  case 37:
#line 423 "code/GDBVarTreeParserY.y"
    {
		itsIsPointerFlag = cmIsOpenablePointer(*(yyvsp[(1) - (1)].pString));
		(yyval.pString) = (yyvsp[(1) - (1)].pString);
		;}
    break;

  case 38:
#line 429 "code/GDBVarTreeParserY.y"
    {
		itsIsPointerFlag = cmIsOpenablePointer(*(yyvsp[(2) - (2)].pString));
		(yyval.pString) = (yyvsp[(2) - (2)].pString);
		(yyval.pString)->Prepend("@");
		;}
    break;

  case 39:
#line 436 "code/GDBVarTreeParserY.y"
    {
		itsIsPointerFlag = cmIsOpenablePointer(*(yyvsp[(2) - (2)].pString));
		(yyval.pString) = (yyvsp[(1) - (2)].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[(2) - (2)].pString));

		jdelete (yyvsp[(2) - (2)].pString);
		;}
    break;

  case 42:
#line 450 "code/GDBVarTreeParserY.y"
    {
		(yyval.pString) = (yyvsp[(1) - (2)].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[(2) - (2)].pString));

		jdelete (yyvsp[(2) - (2)].pString);
		;}
    break;

  case 43:
#line 459 "code/GDBVarTreeParserY.y"
    {
		(yyval.pString) = (yyvsp[(1) - (3)].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[(2) - (3)].pString));
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[(3) - (3)].pString));

		jdelete (yyvsp[(2) - (3)].pString);
		jdelete (yyvsp[(3) - (3)].pString);
		;}
    break;

  case 44:
#line 471 "code/GDBVarTreeParserY.y"
    {
		itsIsPointerFlag = cmIsOpenablePointer(*(yyvsp[(1) - (2)].pString));
		(yyval.pString) = (yyvsp[(1) - (2)].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[(2) - (2)].pString));

		jdelete (yyvsp[(2) - (2)].pString);
		;}
    break;

  case 45:
#line 481 "code/GDBVarTreeParserY.y"
    {
		(yyval.pString) = (yyvsp[(1) - (2)].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[(2) - (2)].pString));

		jdelete (yyvsp[(2) - (2)].pString);
		;}
    break;

  case 46:
#line 490 "code/GDBVarTreeParserY.y"
    {
		(yyval.pString) = (yyvsp[(1) - (2)].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[(2) - (2)].pString));

		jdelete (yyvsp[(2) - (2)].pString);
		;}
    break;

  case 47:
#line 499 "code/GDBVarTreeParserY.y"
    {
		(yyval.pString) = (yyvsp[(1) - (2)].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[(2) - (2)].pString));

		jdelete (yyvsp[(2) - (2)].pString);
		;}
    break;

  case 48:
#line 508 "code/GDBVarTreeParserY.y"
    {
		(yyval.pString) = (yyvsp[(1) - (2)].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[(2) - (2)].pString));

		jdelete (yyvsp[(2) - (2)].pString);
		;}
    break;

  case 50:
#line 519 "code/GDBVarTreeParserY.y"
    {
		(yyval.pString) = (yyvsp[(1) - (3)].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[(2) - (3)].pString));
		(yyval.pString)->Append(*(yyvsp[(3) - (3)].pString));

		jdelete (yyvsp[(2) - (3)].pString);
		jdelete (yyvsp[(3) - (3)].pString);
		;}
    break;

  case 51:
#line 530 "code/GDBVarTreeParserY.y"
    {
		(yyval.pString) = (yyvsp[(1) - (2)].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[(2) - (2)].pString));

		jdelete (yyvsp[(2) - (2)].pString);
		;}
    break;

  case 52:
#line 539 "code/GDBVarTreeParserY.y"
    {
		(yyval.pString) = (yyvsp[(1) - (2)].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[(2) - (2)].pString));

		jdelete (yyvsp[(2) - (2)].pString);
		;}
    break;

  case 53:
#line 548 "code/GDBVarTreeParserY.y"
    {
		(yyval.pString) = jnew JString("{}");
		assert( (yyval.pString) != nullptr );
		;}
    break;

  case 55:
#line 559 "code/GDBVarTreeParserY.y"
    {
		(yyval.pString) = (yyvsp[(1) - (2)].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[(2) - (2)].pString));

		jdelete (yyvsp[(2) - (2)].pString);
		;}
    break;

  case 57:
#line 570 "code/GDBVarTreeParserY.y"
    {
		(yyval.pString) = (yyvsp[(1) - (2)].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[(2) - (2)].pString));

		jdelete (yyvsp[(2) - (2)].pString);
		;}
    break;

  case 59:
#line 584 "code/GDBVarTreeParserY.y"
    {
		(yyval.pString) = (yyvsp[(1) - (4)].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append("@");
		(yyval.pString)->Append(*(yyvsp[(3) - (4)].pString));

		jdelete (yyvsp[(3) - (4)].pString);
		;}
    break;


/* Line 1267 of yacc.c.  */
#line 2041 "code/GDBVarTreeParserY.cpp"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
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

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 594 "code/GDBVarTreeParserY.y"


