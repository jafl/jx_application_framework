%top {
/*
Copyright © 2001 by John Lindal.

This scanner reads a bash file and returns CB::Text::BourneShell::Scanner::Tokens.
*/

#include "CBStylingScannerBase.h"
#include <jAssert.h>
}

%option namespace="CB::Text::BourneShell" lexer="Scanner"
%option lex="NextToken" token-type="CBStylingScannerBase::Token"
%option unicode nodefault full freespace

%class {

public:

	// these types are ordered to correspond to the type table in CBBourneShellStyler

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

		kSingleQuoteString,
		kDoubleQuoteString,
		kExecString,

		kComment,
		kError,			// place holder for CBBourneShellStyler

		kOtherOperator
	};
}

%x ID_STATE VAR_STATE
%x DOUBLE_STRING_STATE EXEC_STRING_STATE

WSCHAR         ([[:space:]]|\\\n)
WS             ({WSCHAR}+)

VARNAME        ([[:alpha:]_][[:alnum:]_]*|[0-9]+)

RESERVEDWORD   (case|do|done|elif|else|esac|fi|for|function|if|in|select|then|until|while)
BUILTINCMD     (true|false|source|alias|bg|bind|break|builtin|cd|command|continue|declare|typeset|dirs|echo|enable|eval|exec|exit|export|fc|fg|getopts|hash|help|history|jobs|kill|let|local|logout|popd|pushd|pwd|read|readonly|return|set|shift|suspend|test|times|trap|type|ulimit|umask|unalias|unset|wait)

%%

"#".*\n? {
	StartToken();
	return ThisToken(kComment);
	}

\'[^']*\'? {
	StartToken();
	return ThisToken(size() > 1 && text()[size()-1] == '\'' ?
					 kSingleQuoteString : kUnterminatedString);
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

[^|&;()<>!#'"`=$\\ \v\t\f\r\n]+ |
\\.?                            {
	StartToken();
	start(ID_STATE);
	}

"||" |
"&"  |
"&&" |
";"  |
";;" |
"("  |
")"  |
"|"  {
	StartToken();
	return ThisToken(kControlOperator);
	}

"<"   |
"<<"  |
"<<-" |
"<&"  |
">"   |
">|"  |
">>"  |
"&>"  |
">&"  |
"<>"  {
	StartToken();
	return ThisToken(kRedirectionOperator);
	}

!    |
!!   |
!"?" |
!#   {
	StartToken();
	return ThisToken(kHistoryOperator);
	}

!(?={WSCHAR}|[=(]) {
	StartToken();
	return ThisToken(kControlOperator);
	}

= {
	StartToken();
	return ThisToken(kOtherOperator);
	}

{WS} {
	StartToken();
	return ThisToken(kWhitespace);
	}

	/* Match word */



<ID_STATE>{

[^|&;()<>!#'"`=$\\ \v\t\f\r\n]+ |
\\.?                            {
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

{VARNAME}                                                                  |
"{"[#!]?{VARNAME}("["[^]\n]+"]")?("}"|:[-=?+]?|"#"{1,2}|"%"{1,2}|"/"{1,2}) |
[-0-9*@#?$!_]                                                              {
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

<EXEC_STRING_STATE>{

\` {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kExecString);
	}

\\(.|\n)? |
[^\\\`]+   {
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
