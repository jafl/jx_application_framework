%top {
/*
Copyright © 2006 by John Lindal.

This scanner reads a JavaScript file and returns CB::Text::JavaScript::Scanner::Tokens.

Remember to upgrade CBHTMLScanner, too!
*/

#include "CBStylingScannerBase.h"
#include <jAssert.h>
%}

%option namespace="CB::Text::JavaScript" lexer="Scanner"
%option lex="NextToken" token-type="CBStylingScannerBase::Token"
%option unicode nodefault full freespace

%class {

public:

	// these types are ordered to correspond to the type table in CBJavaScriptStyler

	enum TokenType
	{
		kBadHexInt = kEOF+1,
		kUnterminatedString,
		kUnterminatedRegex,
		kUnterminatedComment,
		kIllegalChar,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kID,
		kReservedKeyword,

		kOperator,
		kDelimiter,

		kString,
		kTemplateString,

		kFloat,
		kDecimalInt,
		kHexInt,

		kRegexSearch,

		kComment,
		kDocCommentHTMLTag,
		kDocCommentSpecialTag,

		kError			// place holder for CBJavaScriptStyler
		};

public:

	virtual void	BeginScan(const JStyledText* text,
							  const JStyledText::TextIndex& startIndex,
							  std::istream& input);

private:

	bool	itsProbableOperatorFlag;	// kTrue if / is most likely operator instead of regex
}

%x DOC_COMMENT_STATE C_COMMENT_STATE
%x SINGLE_STRING_STATE DOUBLE_STRING_STATE TEMPLATE_STRING_STATE
%x REGEX_SEARCH_STATE REGEX_CHAR_CLASS_STATE

WS           ([ \v\t\f\r\n]+)



KEYWORD      (abstract|arguments|async|await|boolean|break|byte|case|catch|char|class|const|constructor|continue|debugger|default|delete|do|double|else|enum|eval|export|extends|false|final|finally|float|for|function|goto|if|implements|import|in|instanceof|int|interface|let|long|native|new|null|of|package|private|protected|public|return|short|static|super|switch|synchronized|this|throw|throws|transient|true|try|typeof|undefined|var|void|volatile|while|with|yield)

DOCTAG       (async|attribute|author|beta|broadcast|bubbles|category|class|chainable|config|constructor|default|deprecated|event|example|extends|extension|extensionfor|extension_for|final|for|main|method|module|namespace|optional|param|private|property|protected|readonly|readOnly|required|requires|return|since|static|submodule|type|uses|writeonce|writeOnce)



IDCAR        (_|\p{L})
IDCDR        (_|\p{L}|\d)

ID           ({IDCAR}{IDCDR}*)



DECIMAL      ([0-9]+)
HEX          (0[xX]0*[[:xdigit:]]{1,8})
BADHEX       (0[xX][[:xdigit:]]{9,})



SIGNPART     ([+-])
DIGITSEQ     ([0-9]+)
EXPONENT     ([eE]{SIGNPART}?{DIGITSEQ})
DOTDIGITS    ({DIGITSEQ}\.|{DIGITSEQ}\.{DIGITSEQ}|\.{DIGITSEQ})

FLOAT        ({DIGITSEQ}{EXPONENT}|{DOTDIGITS}{EXPONENT}?)
%%



	/* Match operators */



"++"   |
"--"   |
"+"    |
"-"    |
"!"    |
"*"    |
"%"    |
"<<"   |
">>"   |
">>>"  |
"<"    |
"<="   |
">"    |
">="   |
"=="   |
"==="  |
"!="   |
"!=="  |
"~"    |
"&"    |
"^"    |
"|"    |
"^^"   |
"&&"   |
"||"   |
"?"    |
":"    |
"="    |
"*="   |
"%="   |
"+="   |
"-="   |
"&="   |
"&&="  |
"^="   |
"|="   |
"||="  |
"^^="  |
"<<="  |
">>="  |
">>>=" |
"."    |
"..."  |
"::"   {
	StartToken();
	itsProbableOperatorFlag = false;
	return ThisToken(kOperator);
	}



	/* Avoid styling JSX tags as regexp */



"</" |
"/>" {
	StartToken();
	itsProbableOperatorFlag = false;
	return ThisToken(kOperator);
	}



	/* Match grouping symbols and other delimiters */



"(" |
"[" |
"{" |
"}" |
"," |
";" {
	StartToken();
	itsProbableOperatorFlag = false;
	return ThisToken(kDelimiter);
	}

")" |
"]" {
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

{ID} {
	StartToken();
	itsProbableOperatorFlag = true;
	return ThisToken(kID);
	}

{DECIMAL} {
	StartToken();
	itsProbableOperatorFlag = true;
	return ThisToken(kDecimalInt);
	}

{HEX} {
	StartToken();
	itsProbableOperatorFlag = true;
	return ThisToken(kHexInt);
	}

{BADHEX} {
	StartToken();
	itsProbableOperatorFlag = true;
	return ThisToken(kBadHexInt);
	}

{FLOAT} {
	StartToken();
	itsProbableOperatorFlag = true;
	return ThisToken(kFloat);
	}



	/* Match C++ comments */



"//".*\n? {
	StartToken();
	return ThisToken(kComment);
	}



	/* The following match tokens that are too dangerous to eat in one bite */
	/* because it is so easy to create unterminated comments and strings.   */



	/* Match DOC comments -- this is based on the fast comment scanner from the flex man */
	/* page, since Vern Paxon presumably can optimize such things far better than I can. */



"/**"/[^/] {
	StartToken();
	start(DOC_COMMENT_STATE);
	}

<DOC_COMMENT_STATE>{

[^*<@\n]*\n?      |
"*"+[^*/<@\n]*\n? |
[<@]              {
	ContinueToken();
	}

"*"+"/" {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kComment);
	}

"<"[^*>]+> {		/* comment ends with <star><slash> even if in the middle of <...> */
	return DocToken(kDocCommentHTMLTag);
	}

@{DOCTAG} {
	return DocToken(kDocCommentSpecialTag);
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedComment);
	}

}



	/* Match C comments -- this is the fast comment scanner from the flex man page, */
	/* since Vern Paxon presumably can optimize such things far better than I can.  */



"/*" {
	StartToken();
	start(C_COMMENT_STATE);
	}

<C_COMMENT_STATE>{

[^*\n]*\n?      |
"*"+[^*/\n]*\n? {
	ContinueToken();
	}

"*"+"/" {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kComment);
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedComment);
	}

}



	/* Match strings -- based on the string scanner from the flex manual. */



\' {
	StartToken();
	itsProbableOperatorFlag = true;
	start(SINGLE_STRING_STATE);
	}

<SINGLE_STRING_STATE>{

\' {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kString);
	}

\n {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

\\(.|\n)? |
[^\\\n\']+  {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}

\" {
	StartToken();
	itsProbableOperatorFlag = true;
	start(DOUBLE_STRING_STATE);
	}

<DOUBLE_STRING_STATE>{

\" {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kString);
	}

\n {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

\\(.|\n)?  |
[^\\\n\"]+ {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}

\` {
	StartToken();
	itsProbableOperatorFlag = true;
	start(TEMPLATE_STRING_STATE);
	}

<TEMPLATE_STRING_STATE>{

\` {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kTemplateString);
	}

\\.?     |
[^\\\`]+ {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
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
		start(REGEX_SEARCH_STATE);
		}
	}

<REGEX_SEARCH_STATE>{

"/"[gim]* {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kRegexSearch);
	}

\[(^-?\])? {
	ContinueToken();
	start(REGEX_CHAR_CLASS_STATE);
	}

\\.?     |
[^[\\/]+ {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedRegex);
	}

}

<REGEX_CHAR_CLASS_STATE>{

"]" {
	ContinueToken();
	start(REGEX_SEARCH_STATE);
	}

\\.?    |
[^]\\]+ {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedRegex);
	}

}



	/* Clean up */



{WS} {
	StartToken();
	return ThisToken(kWhitespace);
	}

. {
	StartToken();
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
CB::Text::JavaScript::Scanner::BeginScan
	(
	const JStyledText*				text,
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input
	)
{
	CBStylingScannerBase::BeginScan(text, startIndex, input);

	itsProbableOperatorFlag = false;
}
