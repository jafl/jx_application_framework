%top {
/*
Copyright © 2001 by John Lindal.

This scanner reads a bash file and returns CB::Text::CShell::Scanner::Tokens.
*/

#include "CBStylingScannerBase.h"
#include <jAssert.h>
}

%option namespace="CB::Text::CShell" lexer="Scanner"
%option lex="NextToken" token-type="CBStylingScannerBase::Token"
%option unicode nodefault full freespace

%class {

public:

	// these types are ordered to correspond to the type table in CBCShellStyler

	enum TokenType
	{
		kEmptyVariable = kEOF+1,
		kUnterminatedString,

		kWhitespace,	// must be the one before the first item in type table

		kID,
		kVariable,
		kReservedWord,
		kBuiltInCommand,

		kControlOperator,
		kRedirectionOperator,
		kHistoryOperator,
		kMathOperator,

		kSingleQuoteString,
		kDoubleQuoteString,
		kExecString,

		kComment,
		kError,			// place holder for CBCShellStyler

		kOtherOperator
	};
}

%x ID_STATE VAR_STATE
%x SINGLE_STRING_STATE DOUBLE_STRING_STATE EXEC_STRING_STATE

WSCHAR         ([[:space:]]|\\\n)
WS             ({WSCHAR}+)

VARNAME        ([?#%]?[[:alpha:]_][[:alnum:]_]{0,19}|%?[0-9]+)

RESERVEDWORD   (break|breaksw|case|continue|default|else|end|endif|endsw|foreach|if|then|repeat|switch|while)
BUILTINCMD     (true|false|alias|alloc|bg|bindkey|builtins|bye|cd|chdir|complete|dirs|echo|echotc|eval|exec|exit|fg|filetest|getspath|getxvers|glob|goto|hashstat|history|hup|inlib|jobs|kill|limit|log|login|logout|migrate|newgrp|nice|nohup|notify|onintr|popd|printenv|pushd|rehash|rootnode|sched|set|setenv|setpath|setspath|settc|setty|setxvers|shift|source|stop|suspend|telltc|time|umask|unalias|uncomplete|unhash|universe|unlimit|unset|unsetenv|ver|wait|warp|watchlog|where|which)

%%

"#".*\n? {
	StartToken();
	return ThisToken(kComment);
	}

\' {
	StartToken();
	start(SINGLE_STRING_STATE);
	}

\" {
	StartToken();
	start(DOUBLE_STRING_STATE);
	}

\` {
	StartToken();
	start(EXEC_STRING_STATE);
	}

"$" {
	StartToken();
	start(VAR_STATE);
	}

{RESERVEDWORD} {
	StartToken();
	return ThisToken(kReservedWord);
	}

{BUILTINCMD} {
	StartToken();
	return ThisToken(kBuiltInCommand);
	}

[^-&|;<>()!^=+*/%~#'"`$\\ \v\t\f\r\n]+ |
\\.?                           {
	StartToken();
	start(ID_STATE);
	}

"&"  |
"|"  |
";"  |
"("  |
")"  |
"&&" |
"||" {
	StartToken();
	return ThisToken(kControlOperator);
	}

"<"    |
"<<"   |
">"    |
">!"   |
">&"   |
">&!"  |
">>"   |
">>!"  |
">>&"  |
">>&!" {
	StartToken();
	return ThisToken(kRedirectionOperator);
	}

!  |
!! |
!? |
!# {
	StartToken();
	return ThisToken(kHistoryOperator);
	}

!({WSCHAR}|\() {
	StartToken();
	JStyledText::TextRange r = GetCurrentRange();
	r.charRange.last         = r.charRange.first;
	r.byteRange.last         = r.byteRange.first;	// single byte character
	SetCurrentRange(r);
	matcher().less(size()-1);
	return ThisToken(kControlOperator);
	}

"="  |
"^"  |
"==" |
"!=" |
"=~" |
"!~" |
"<=" |
">=" |
"+"  |
"++" |
"-"  |
"--" |
"*"  |
"/"  |
"%"  |
"~"  {
	StartToken();
	return ThisToken(kMathOperator);
	}

{WS} {
	StartToken();
	return ThisToken(kWhitespace);
	}

	/* Match word */



<ID_STATE>{

[^-&|;<>()!^=+*/%~#'"`$\\ \v\t\f\r\n]+ |
\\.?                                   {
	ContinueToken();
	}

.|{WS} {
	matcher().less(0);
	start(INITIAL);
	return ThisToken(kID);
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kID);
	}

}



	/* Match variable */



<VAR_STATE>{

{VARNAME}        |
"{"{VARNAME}[}[] |
[*#?$!_<]        {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kVariable);
	}

"{"  |
"{}" {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kEmptyVariable);
	}

.|\n {
	matcher().less(0);
	start(INITIAL);
	return ThisToken(kID);
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kID);
	}

}



	/* Match strings */



<SINGLE_STRING_STATE>{

\' {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kSingleQuoteString);
	}

\n {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

\\(.|\n)?  |
[^\\\n\']+ {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}

<DOUBLE_STRING_STATE>{

\" {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kDoubleQuoteString);
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

<EXEC_STRING_STATE>{

\` {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kExecString);
	}

\n {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

\\(.|\n)?  |
[^\\\n\`]+ {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Must be last, or it applies to following states as well! */



<<EOF>> {
	return ThisToken(kEOF);
	}

%%
