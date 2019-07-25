%{
/*
Copyright (C) 2003 by John Lindal.

This scanner reads a Ruby file and returns CBRubyScanner::Tokens.
*/

#define _H_CBRubyScannerL

#include "CBRubyScanner.h"
#include <JStringIterator.h>
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL CBRubyScanner::Token CBRubyScanner::NextToken()

inline JBoolean
isquote
	(
	const JUtf8Character& c
	)
{
	return JI2B(c == '\'' || c == '"' || c == '`');
}
%}

%option c++ yyclass = "CBRubyScanner" prefix = "CBRuby"
%option 8bit nodefault noyywrap
%option full ecs align

%x SINGLE_STRING_STATE DOUBLE_STRING_STATE EXEC_STRING_STATE HEREDOC_STRING_STATE
%x REGEX_STATE EMBEDDED_DOC_STATE

WSCHAR  ([ \v\t\f\r])
WS      ([ \v\t\f\r]*)



KEYWORD      (__FILE__|__LINE__|BEGIN|END|alias|and|begin|break|case|class|def|defined|do|else|elsif|end|ensure|false|for|if|in|module|next|nil|not|or|redo|rescue|retry|return|self|super|then|true|undef|unless|until|when|while|yield)

ID           ([_[:alpha:]][_[:alnum:]]*[?!]?)



DECIMAL      (0_*|[1-9][0-9_]*)
HEX          (0x[0-9A-Fa-f_]*)
BINARY       (0b[01_]*)
OCTAL        (0[0-7_]+)
	/* The programmer probably meant a number, but it is invalid (match after other ints) */
BADINT       ([0-9_]+)
BADHEX       (0[xX][0-9A-Za-z_]+)
BADBINARY    (0[bB][0-9A-Za-z_]+)



SIGNPART     ([+-])
DIGITSEQ     ([0-9][0-9_]*)
EXPONENT     ([eE]{SIGNPART}?{DIGITSEQ})
DOTDIGITS    ({DIGITSEQ}\.|{DIGITSEQ}\.{DIGITSEQ}|\.{DIGITSEQ})

FLOAT        ({DIGITSEQ}{EXPONENT}|{DOTDIGITS}{EXPONENT}?)

%%

%{
/************************************************************************/

	if (itsResetFlag)
		{
		BEGIN(INITIAL);
		itsResetFlag = kJFalse;
		}

%}



	/* Match operators */



"**"  |
"!"   |
"~"   |
"+"   |
"-"   |
"=~"  |
"!~"  |
"*"   |
"%"   |
"."   |
"<<"  |
">>"  |
"<"   |
">"   |
"<="  |
">="  |
"=="  |
"===" |
"!="  |
"<=>" |
"&"   |
"|"   |
"^"   |
"&&"  |
"||"  |
".."  |
"..." |
"::"  |
"="   |
"**=" |
"+="  |
"-="  |
"*="  |
"%="  |
"&="  |
"|="  |
"^="  |
"<<=" |
">>=" |
"&&=" |
"||=" |
"=>"  |
"not" |
"and" |
"or"  {
	StartToken();
	itsProbableOperatorFlag = kJFalse;
	return ThisToken(kOperator);
	}



	/* Match grouping symbols and other delimiters */



"(" |
"[" |
"{" |
"," |
";" {
	StartToken();
	itsProbableOperatorFlag = kJFalse;
	return ThisToken(kDelimiter);
	}

")" |
"]" |
"}" {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(kDelimiter);
	}



	/* Match normal tokens */



{KEYWORD} {
	StartToken();
	itsProbableOperatorFlag = kJFalse;
	return ThisToken(kReservedKeyword);
	}

[$@:]?{ID} {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(yytext[0] == '$' ? kGlobalVariable :
					 yytext[0] == '@' ? kInstanceVariable :
					 yytext[0] == ':' ? kSymbol :
					 kLocalVariable);
	}

{DECIMAL} {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(kDecimalInt);
	}

{BINARY} {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(kBinaryInt);
	}

{OCTAL} {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(kOctalInt);
	}

{HEX} {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(kHexInt);
	}

{BADINT} {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(kBadInt);
	}

{BADBINARY} {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(kBadBinary);
	}

{BADHEX} {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(kBadHex);
	}

{FLOAT} {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(kFloat);
	}



	/* Match comments */



"#".*\n? {
	StartToken();
	return ThisToken(kComment);
	}



	/* Match embedded documentation */



^=begin {
	StartToken();
	BEGIN(EMBEDDED_DOC_STATE);
	}

<EMBEDDED_DOC_STATE>{

^=end\n? {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kEmbeddedDoc);
	}

.*\n? {
	ContinueToken();
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedEmbeddedDoc);
	}

}



	/* Match regex search */



"/"=? {
	if (itsProbableOperatorFlag)
		{
		StartToken();
		itsProbableOperatorFlag = kJFalse;
		return ThisToken(kOperator);
		}
	else
		{
		StartToken();
		itsProbableOperatorFlag = kJTrue;
		BEGIN(REGEX_STATE);
		}
	}

"%r" {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	if (SlurpQuoted("imox"))
		{
		return ThisToken(kRegex);
		}
	else
		{
		return ThisToken(kUnterminatedRegex);
		}
	}

<REGEX_STATE>{

"/"[imox]* {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kRegex);
	}

\\(.|\n)? |
[^\\/]+   {
	ContinueToken();
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedRegex);
	}

}



	/* Match single quoted strings */



\' {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	BEGIN(SINGLE_STRING_STATE);
	}

"%q" {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	if (SlurpQuoted(""))
		{
		return ThisToken(kSingleQuoteString);
		}
	else
		{
		return ThisToken(kUnterminatedString);
		}
	}

<SINGLE_STRING_STATE>{

\' {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kSingleQuoteString);
	}

\\(.|\n)? |
[^\\\']+  {
	ContinueToken();
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Match double quoted strings */



\" {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	BEGIN(DOUBLE_STRING_STATE);
	}

"%"Q? {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	if (SlurpQuoted(""))
		{
		return ThisToken(kDoubleQuoteString);
		}
	else
		{
		return ThisToken(kUnterminatedString);
		}
	}

<DOUBLE_STRING_STATE>{

\" {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kDoubleQuoteString);
	}

\\(.|\n)? |
[^\\\"]+  {
	ContinueToken();
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Match here-document -- only the simplest types! */



\<\<[ \t]*['"`]?[^ \t\n'"`]+['"`]?;\n {
	StartToken();
	itsHereDocTag.Set(yytext+2, yyleng-4);
	itsHereDocTag.TrimWhitespace();
	itsHereDocType = kDoubleQuoteString;

	const JUtf8Character c = itsHereDocTag.GetFirstCharacter();
	if (isquote(c))
		{
		if (c == '\'')
			{
			itsHereDocType = kSingleQuoteString;
			}
		else if (c == '`')
			{
			itsHereDocType = kExecString;
			}

		JStringIterator iter(&itsHereDocTag);
		iter.RemoveNext();
		if (itsHereDocTag.GetLastCharacter() != c)
			{
			return ThisToken(kUnterminatedString);
			}
		iter.MoveTo(kJIteratorStartAtEnd, 0);
		iter.RemovePrev();
		}

	BEGIN(HEREDOC_STRING_STATE);
	}

<HEREDOC_STRING_STATE>{

^.+\n {
	ContinueToken();
	if (JString::Compare(yytext, yyleng - 1,
						 itsHereDocTag.GetBytes(), itsHereDocTag.GetByteCount()) == 0)
		{
		BEGIN(INITIAL);
		return ThisToken(itsHereDocType);
		}
	}

.|\n {
	ContinueToken();
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Match executed strings */



` {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	BEGIN(EXEC_STRING_STATE);
	}

"%x" {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	if (SlurpQuoted(""))
		{
		return ThisToken(kExecString);
		}
	else
		{
		return ThisToken(kUnterminatedString);
		}
	}

<EXEC_STRING_STATE>{

` {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kExecString);
	}

\\(.|\n)? |
[^\\`]+  {
	ContinueToken();
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Match word list */



"%w"[ \v\t\f\r\n]* {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	if (SlurpQuoted(""))
		{
		return ThisToken(kWordList);
		}
	else
		{
		return ThisToken(kUnterminatedWordList);
		}
	}



	/* Miscellaneous */



{WSCHAR}+ |
{WS}\n    { /* Don't eat past a line end or we'd miss PP directives */
	StartToken();
	return ThisToken(kWhitespace);
	}

. {
	StartToken();
	itsProbableOperatorFlag = kJFalse;
	if ( isprint(YYText()[0]) )
		{
		return ThisToken(kIllegalChar);
		}
	else
		{
		return ThisToken(kNonPrintChar);
		}
	}



	/* Must be last, or it applies to following states as well! */



<<EOF>> {
	return ThisToken(kEOF);
	}

%%
