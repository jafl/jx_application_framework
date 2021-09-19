%top {
/*
Copyright © 2003 by John Lindal.

This scanner reads a Ruby file and returns CB::Text::Ruby::Scanner::Tokens.
*/

#include "CBStylingScannerBase.h"
#include <JStringIterator.h>
#include <jAssert.h>
%}

%option namespace="CB::Text::Ruby" lexer="Scanner"
%option lex="NextToken" token-type="CBStylingScannerBase::Token"
%option unicode nodefault full freespace

%class {

public:

	// these types are ordered to correspond to the type table in CBRubyStyler

	enum TokenType
	{
		kBadInt = kEOF+1,
		kBadBinary,
		kBadHex,
		kUnterminatedString,
		kUnterminatedRegex,
		kUnterminatedWordList,
		kUnterminatedEmbeddedDoc,
		kIllegalChar,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kLocalVariable,
		kInstanceVariable,
		kGlobalVariable,
		kSymbol,
		kReservedKeyword,

		kOperator,
		kDelimiter,

		kSingleQuoteString,
		kDoubleQuoteString,
		kHereDocString,
		kExecString,
		kWordList,

		kFloat,
		kDecimalInt,
		kBinaryInt,
		kOctalInt,
		kHexInt,

		kRegex,

		kComment,
		kEmbeddedDoc,

		kError			// place holder for CBRubyStyler
	};

public:

	virtual void	BeginScan(const JStyledText* text,
							  const JStyledText::TextIndex& startIndex,
							  std::istream& input);

private:

	bool		itsProbableOperatorFlag;	// kTrue if /,? are most likely operators instead of regex
	JString		itsHereDocTag;
	TokenType	itsHereDocType;
}

%x SINGLE_STRING_STATE DOUBLE_STRING_STATE EXEC_STRING_STATE HEREDOC_STRING_STATE
%x REGEX_STATE EMBEDDED_DOC_STATE

WSCHAR  ([ \v\t\f\r])
WS      ([ \v\t\f\r]*)



KEYWORD      (__FILE__|__LINE__|start|END|alias|and|begin|break|case|class|def|defined|do|else|elsif|end|ensure|false|for|if|in|module|next|nil|not|or|redo|rescue|retry|return|self|super|then|true|undef|unless|until|when|while|yield)

ID           ((_|\p{L})(_|\p{L}|\d)*[?!]?)



DECIMAL      (0_*|[1-9][0-9_]*)
HEX          (0x[_[:xdigit:]]*)
BINARY       (0b[01_]*)
OCTAL        (0[0-7_]+)
%{
	/* The programmer probably meant a number, but it is invalid (match after other ints) */
%}
BADINT       ([0-9_]+)
BADHEX       (0[xX][_[:xdigit:]]+)
BADBINARY    (0[bB][_[:xdigit:]]+)



SIGNPART     ([+-])
DIGITSEQ     ([0-9][0-9_]*)
EXPONENT     ([eE]{SIGNPART}?{DIGITSEQ})
DOTDIGITS    ({DIGITSEQ}\.|{DIGITSEQ}\.{DIGITSEQ}|\.{DIGITSEQ})

FLOAT        ({DIGITSEQ}{EXPONENT}|{DOTDIGITS}{EXPONENT}?)

%%



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
	itsProbableOperatorFlag = false;
	return ThisToken(kOperator);
	}



	/* Match grouping symbols and other delimiters */



"(" |
"[" |
"{" |
"," |
";" {
	StartToken();
	itsProbableOperatorFlag = false;
	return ThisToken(kDelimiter);
	}

")" |
"]" |
"}" {
	StartToken();
	itsProbableOperatorFlag = true;
	return ThisToken(kDelimiter);
	}



	/* Match normal tokens */



{KEYWORD} {
	StartToken();
	itsProbableOperatorFlag = false;
	return ThisToken(kReservedKeyword);
	}

[$@:]?{ID} {
	StartToken();
	itsProbableOperatorFlag = true;
	return ThisToken(text()[0] == '$' ? kGlobalVariable :
					 text()[0] == '@' ? kInstanceVariable :
					 text()[0] == ':' ? kSymbol :
					 kLocalVariable);
	}

{DECIMAL} {
	StartToken();
	itsProbableOperatorFlag = true;
	return ThisToken(kDecimalInt);
	}

{BINARY} {
	StartToken();
	itsProbableOperatorFlag = true;
	return ThisToken(kBinaryInt);
	}

{OCTAL} {
	StartToken();
	itsProbableOperatorFlag = true;
	return ThisToken(kOctalInt);
	}

{HEX} {
	StartToken();
	itsProbableOperatorFlag = true;
	return ThisToken(kHexInt);
	}

{BADINT} {
	StartToken();
	itsProbableOperatorFlag = true;
	return ThisToken(kBadInt);
	}

{BADBINARY} {
	StartToken();
	itsProbableOperatorFlag = true;
	return ThisToken(kBadBinary);
	}

{BADHEX} {
	StartToken();
	itsProbableOperatorFlag = true;
	return ThisToken(kBadHex);
	}

{FLOAT} {
	StartToken();
	itsProbableOperatorFlag = true;
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
	start(EMBEDDED_DOC_STATE);
	}

<EMBEDDED_DOC_STATE>{

^=end\n? {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kEmbeddedDoc);
	}

.*\n? {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedEmbeddedDoc);
	}

}



	/* Match regex search */



"/"=? {
	if (itsProbableOperatorFlag)
		{
		StartToken();
		itsProbableOperatorFlag = false;
		return ThisToken(kOperator);
		}
	else
		{
		StartToken();
		itsProbableOperatorFlag = true;
		start(REGEX_STATE);
		}
	}

"%r" {
	StartToken();
	itsProbableOperatorFlag = true;
	if (SlurpQuoted(1, "imox"))
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
	start(INITIAL);
	return ThisToken(kRegex);
	}

\\(.|\n)? |
[^\\/]+   {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedRegex);
	}

}



	/* Match single quoted strings */



\' {
	StartToken();
	itsProbableOperatorFlag = true;
	start(SINGLE_STRING_STATE);
	}

"%q" {
	StartToken();
	itsProbableOperatorFlag = true;
	if (SlurpQuoted(1, ""))
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
	start(INITIAL);
	return ThisToken(kSingleQuoteString);
	}

\\(.|\n)? |
[^\\\']+  {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Match double quoted strings */



\" {
	StartToken();
	itsProbableOperatorFlag = true;
	start(DOUBLE_STRING_STATE);
	}

"%"Q? {
	StartToken();
	itsProbableOperatorFlag = true;
	if (SlurpQuoted(1, ""))
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
	start(INITIAL);
	return ThisToken(kDoubleQuoteString);
	}

\\(.|\n)? |
[^\\\"]+  {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Match here-document -- only the simplest types! */



"<<"[ \t]*['"`]?[^ \t\n'"`]+['"`]?;\n {
	StartToken();
	itsHereDocTag.Set(text()+2, size()-4);
	itsHereDocTag.TrimWhitespace();
	itsHereDocType = kDoubleQuoteString;

	const JUtf8Character c = itsHereDocTag.GetFirstCharacter();
	if (IsQuote(c))
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

	start(HEREDOC_STRING_STATE);
	}

<HEREDOC_STRING_STATE>{

^.+\n {
	ContinueToken();
	if (JString::Compare(text(), size() - 1,
						 itsHereDocTag.GetBytes(), itsHereDocTag.GetByteCount()) == 0)
		{
		start(INITIAL);
		return ThisToken(itsHereDocType);
		}
	}

.|\n {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Match executed strings */



` {
	StartToken();
	itsProbableOperatorFlag = true;
	start(EXEC_STRING_STATE);
	}

"%x" {
	StartToken();
	itsProbableOperatorFlag = true;
	if (SlurpQuoted(1, ""))
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
	start(INITIAL);
	return ThisToken(kExecString);
	}

\\(.|\n)? |
[^\\`]+  {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Match word list */



"%w"[ \v\t\f\r\n]* {
	StartToken();
	itsProbableOperatorFlag = true;
	if (SlurpQuoted(1, ""))
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
	itsProbableOperatorFlag = false;
	JUtf8Character c(text());
	if (c.IsPrint())
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

/******************************************************************************
 BeginScan (virtual)

 *****************************************************************************/

void
CB::Text::Ruby::Scanner::BeginScan
	(
	const JStyledText*				text,
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input
	)
{
	CBStylingScannerBase::BeginScan(text, startIndex, input);

	itsProbableOperatorFlag = false;
	itsHereDocTag.Clear();
	itsHereDocType = kDoubleQuoteString;
}
