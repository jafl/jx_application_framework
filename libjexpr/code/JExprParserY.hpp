/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_CODE_JEXPRPARSERY_HPP_INCLUDED
# define YY_YY_CODE_JEXPRPARSERY_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    P_NUMBER = 258,                /* P_NUMBER  */
    P_HEX = 259,                   /* P_HEX  */
    P_VARIABLE = 260,              /* P_VARIABLE  */
    P_PI = 261,                    /* P_PI  */
    P_E = 262,                     /* P_E  */
    P_I = 263,                     /* P_I  */
    P_INPUT = 264,                 /* P_INPUT  */
    P_FN_ABS = 265,                /* P_FN_ABS  */
    P_FN_ARCCOS = 266,             /* P_FN_ARCCOS  */
    P_FN_ARCCOSH = 267,            /* P_FN_ARCCOSH  */
    P_FN_ARCSIN = 268,             /* P_FN_ARCSIN  */
    P_FN_ARCSINH = 269,            /* P_FN_ARCSINH  */
    P_FN_ARCTAN = 270,             /* P_FN_ARCTAN  */
    P_FN_ARCTAN2 = 271,            /* P_FN_ARCTAN2  */
    P_FN_ARCTANH = 272,            /* P_FN_ARCTANH  */
    P_FN_CONJUGATE = 273,          /* P_FN_CONJUGATE  */
    P_FN_COS = 274,                /* P_FN_COS  */
    P_FN_COSH = 275,               /* P_FN_COSH  */
    P_FN_IM = 276,                 /* P_FN_IM  */
    P_FN_LN = 277,                 /* P_FN_LN  */
    P_FN_LOG = 278,                /* P_FN_LOG  */
    P_FN_LOG2 = 279,               /* P_FN_LOG2  */
    P_FN_LOG10 = 280,              /* P_FN_LOG10  */
    P_FN_MAX = 281,                /* P_FN_MAX  */
    P_FN_MIN = 282,                /* P_FN_MIN  */
    P_FN_PHASE = 283,              /* P_FN_PHASE  */
    P_FN_RE = 284,                 /* P_FN_RE  */
    P_FN_ROTATE = 285,             /* P_FN_ROTATE  */
    P_FN_SIN = 286,                /* P_FN_SIN  */
    P_FN_SINH = 287,               /* P_FN_SINH  */
    P_FN_SQRT = 288,               /* P_FN_SQRT  */
    P_FN_TAN = 289,                /* P_FN_TAN  */
    P_FN_TANH = 290,               /* P_FN_TANH  */
    P_FN_SIGN = 291,               /* P_FN_SIGN  */
    P_FN_ROUND = 292,              /* P_FN_ROUND  */
    P_FN_TRUNCATE = 293,           /* P_FN_TRUNCATE  */
    P_FN_PARALLEL = 294,           /* P_FN_PARALLEL  */
    P_EOF = 295,                   /* P_EOF  */
    UMINUS = 296,                  /* UMINUS  */
    UPLUS = 297                    /* UPLUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 56 "code/JExprParserY.y"

	JString*				pString;
	JFunction*				pFunction;
	JPtrArray<JFunction>*	pList;

#line 112 "code/JExprParserY.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif




int yyparse (void);


#endif /* !YY_YY_CODE_JEXPRPARSERY_HPP_INCLUDED  */
