%{
/*
Copyright (C) 2003 by John Lindal.

This scanner reads a Perl file and returns CBPerlScanner::Tokens.
*/

#define _H_CBPerlScannerL

#include "CBPerlScanner.h"
#include <JStringIterator.h>
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL CBPerlScanner::Token CBPerlScanner::NextToken()

inline JBoolean
isquote
	(
	const JUtf8Character& c
	)
{
	return JI2B(c == '\'' || c == '"' || c == '`');
}
%}

%option c++ yyclass = "CBPerlScanner" prefix = "CBPerl"
%option 8bit nodefault noyywrap
%option full ecs align

%x SINGLE_STRING_STATE DOUBLE_STRING_STATE EXEC_STRING_STATE HEREDOC_STRING_STATE
%x ONE_SHOT_REGEX_SEARCH_STATE REGEX_SEARCH_STATE
%x QUOTED_VARIABLE_STATE DATA_STATE POD_STATE FILE_GLOB_STATE

WSCHAR  ([ \v\t\f\r])
WS      ([ \v\t\f\r]*)



KEYWORD      (continue|do|else|elsif|for|foreach|goto|if|last|next|redo|unless|until|while|abs|accept|alarm|atan2|bind|binmode|bless|caller|chdir|chmod|chomp|chop|chown|chr|chroot|close|closedir|connect|cos|crypt|dbmclose|dbmopen|defined|delete|die|dump|each|eof|eval|exec|exists|exit|exp|fcntl|fileno|flock|fork|format|formline|getc|getlogin|getpeername|getpgrp|getppid|getpriority|getpwnam|getgrnam|gethostbyname|getnetbyname|getprotobyname|getpwuid|getgrgid|getservbyname|gethostbyaddr|getnetbyaddr|getprotobynumber|getservbyport|getpwent|getgrent|gethostent|getnetent|getprotoent|getservent|setpwent|setgrent|sethostent|setnetent|setprotoent|setservent|endpwent|endgrent|endhostent|endnetent|endprotoent|endservent|getsockname|getsockopt|glob|gmtime|grep|hex|import|index|int|ioctl|join|keys|kill|lc|lcfirst|length|link|listen|local|localtime|lock|log|lstat|map|mkdir|msgctl|msgget|msgrcv|msgsnd|my|no|oct|open|opendir|ord|our|pack|package|pipe|pop|pos|print|printf|prototype|push|quotemeta|rand|read|readdir|readline|readlink|readpipe|recv|ref|rename|require|reset|return|reverse|rewinddir|rindex|rmdir|scalar|seek|seekdir|select|semctl|semget|semop|send|setpgrp|setpriority|setsockopt|shift|shmctl|shmget|shmread|shmwrite|shutdown|sin|sleep|socket|socketpair|sort|splice|split|sprintf|sqrt|srand|stat|study|sub|substr|symlink|syscall|sysopen|sysread|sysseek|system|syswrite|tell|telldir|tie|tied|time|times|truncate|uc|ucfirst|umask|undef|unlink|unpack|untie|unshift|use|utime|values|vec|wait|waitpid|wantarray|warn|write)

PPKEYWORD    (line)

ID           ([_[:alnum:]]+)

BAREWORD     ([_[:alpha:]][_[:alnum:]]*)



DECIMAL      (0_*|[1-9][0-9_]*)
HEX          (0x[_[:xdigit:]]*)
BINARY       (0b[01_]*)
OCTAL        (0[0-7_]+)
VERSION      (v[0-9]+(\.[0-9]+)*|([0-9]+\.){2,}[0-9]+)
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

	JSize braceCount = 0;

	if (itsResetFlag)
		{
		BEGIN(INITIAL);
		itsResetFlag = kJFalse;
		}

%}



	/* Match operators */



"->"  |
"++"  |
"--"  |
"**"  |
"!"   |
"~"   |
"\\"  |
"+"   |
"-"   |
"=~"  |
"!~"  |
"*"   |
"%"   |
"x"   |
"."   |
"<<"  |
">>"  |
">"   |
"<="  |
">="  |
"lt"  |
"gt"  |
"le"  |
"ge"  |
"=="  |
"!="  |
"<=>" |
"eq"  |
"ne"  |
"cmp" |
"~~"  |
"&"   |
"|"   |
"^"   |
"&&"  |
"||"  |
"//"  |
".."  |
"..." |
":"   |
"="   |
"**=" |
"+="  |
"-="  |
".="  |
"*="  |
"%="  |
"x="  |
"&="  |
"|="  |
"^="  |
"<<=" |
">>=" |
"&&=" |
"||=" |
"//=" |
"=>"  |
"not" |
"and" |
"or"  |
"xor" {
	StartToken();
	itsProbableOperatorFlag = kJFalse;
	return ThisToken(kOperator);
	}

-[rwxoRWXOezsfdlpSbctugkTBMAC] {
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



	/* Match argument lists */



"("(\$|\\\$|\\@|\\%|&|\\&|\*|\\\[(\$|@|%|&|\*)+\]|;)*(\$|\\\$|@|\\@|%|\\%|&|\\&|\*|\\\[(\$|@|%|&|\*)+\])")" {
	StartToken();
	itsProbableOperatorFlag = kJFalse;
	return ThisToken(kPrototypeArgList);
	}



	/* Match normal tokens */



\\?\$[&`'+*./|,\\;#%=\-~^:?!@$<>()\[\]] |
\\?\$\^[LAECDFHIMOPRSTVWX]              |
\\?\$\"\"                               |
\\?\$+{ID}                              {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(yytext[0] == '\\' ? kReference : kScalar);
	}

\\?\$+\{ {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	itsComplexVariableType  = yytext[0] == '\\' ? kReference : kScalar;
	braceCount              = 1;
	BEGIN(QUOTED_VARIABLE_STATE);
	}

\\?\@[+-] |
\\?\@\$*{ID} {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(yytext[0] == '\\' ? kReference : kList);
	}

\\?\@\$*\{ {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	itsComplexVariableType  = yytext[0] == '\\' ? kReference : kList;
	braceCount              = 1;
	BEGIN(QUOTED_VARIABLE_STATE);
	}

\\?\%\^H  |
\\?\%\$*{ID} {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(yytext[0] == '\\' ? kReference : kHash);
	}

\\?\%\$*\{ {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	itsComplexVariableType  = yytext[0] == '\\' ? kReference : kHash;
	braceCount              = 1;
	BEGIN(QUOTED_VARIABLE_STATE);
	}

\\?\&\$*{BAREWORD} {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(yytext[0] == '\\' ? kReference : kSubroutine);
	}

\\?\&\$*\{ {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	itsComplexVariableType  = yytext[0] == '\\' ? kReference : kSubroutine;
	braceCount              = 1;
	BEGIN(QUOTED_VARIABLE_STATE);
	}

[$@%&]\$*\{{WS}\} {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(kEmptyVariable);
	}

__END__  |
__DATA__ {
	StartToken();
	BEGIN(DATA_STATE);
	return ThisToken(kBareword);
	}

{KEYWORD} {
	StartToken();
	itsProbableOperatorFlag = kJFalse;
	return ThisToken(kReservedKeyword);
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

{VERSION} {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(kVersion);
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



	/* Match quoted variable constructs: ${...} */



<QUOTED_VARIABLE_STATE>{

"}" {
	ContinueToken();
	braceCount--;
	if (braceCount == 0)
		{
		BEGIN(INITIAL);
		return ThisToken(itsComplexVariableType);
		}
	}

"{" {
	ContinueToken();
	braceCount++;
	}

[^{}]+ {
	ContinueToken();	/* should actually check for ' " ` strings! */
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedVariable);
	}

}



	/* Slurp data at end of file */



<DATA_STATE>{

(.|\n)* {
	StartToken();
	return ThisToken(kModuleData);
	}

<<EOF>> {
	StartToken();
	BEGIN(INITIAL);
	return ThisToken(kEOF);
	}

}



	/* Match preprocessor directives */



^{WS}"#"{WS}{PPKEYWORD}{WSCHAR}+.*\n? {
	StartToken();
	SavePPNameRange();
	return ThisToken(kPPDirective);
	}



	/* Match comments -- must be after preprocessor since they look the same */



"#".*\n? {
	StartToken();
	return ThisToken(kComment);
	}



	/* Match POD */



^"="[^ \v\t\f\r\n] {
	StartToken();
	BEGIN(POD_STATE);
	}

<POD_STATE>{

^"=cut"({WSCHAR}+.*)?\n? {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kPOD);
	}

.*\n? {
	ContinueToken();
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedPOD);
	}

}



	/* Match readline/fileglob */



"<" {
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
		BEGIN(FILE_GLOB_STATE);
		}
	}

<FILE_GLOB_STATE>{

">" {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kFileGlob);
	}

\n {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedFileGlob);
	}

\\.?    |
[^\\>]+ {
	ContinueToken();
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedFileGlob);
	}

}



	/* Match one-shot regex search */



"?" {
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
		BEGIN(ONE_SHOT_REGEX_SEARCH_STATE);
		}
	}

<ONE_SHOT_REGEX_SEARCH_STATE>{

"?" {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kOneShotRegexSearch);
	}

\\(.|\n)? |
[^\\?]+  {
	ContinueToken();
	}

<<EOF>> {
	BEGIN(INITIAL);
	return ThisToken(kUnterminatedRegex);
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
		BEGIN(REGEX_SEARCH_STATE);
		}
	}

"m"[ \v\t\f\r\n]* {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	if (SlurpQuoted(1, "cgimosx"))
		{
		return ThisToken(kRegexSearch);
		}
	else
		{
		return ThisToken(kUnterminatedRegex);
		}
	}

<REGEX_SEARCH_STATE>{

"/"[cgimosx]* {
	ContinueToken();
	BEGIN(INITIAL);
	return ThisToken(kRegexSearch);
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

"q"[ \v\t\f\r\n]* {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
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

"qq"[ \v\t\f\r\n]* {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
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



	/* Match compiled regex */



"qr"[ \v\t\f\r\n]* {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	if (SlurpQuoted(1, "imosx"))
		{
		return ThisToken(kCompiledRegex);
		}
	else
		{
		return ThisToken(kUnterminatedRegex);
		}
	}



	/* Match executed strings */



` {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	BEGIN(EXEC_STRING_STATE);
	}

"qx"[ \v\t\f\r\n]* {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
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



"qw"[ \v\t\f\r\n]* {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	if (SlurpQuoted(1, ""))
		{
		return ThisToken(kWordList);
		}
	else
		{
		return ThisToken(kUnterminatedWordList);
		}
	}



	/* Match regex replace */



"s"[ \v\t\f\r\n]* {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	if (SlurpQuoted(2, "egimosx"))
		{
		return ThisToken(kRegexReplace);
		}
	else
		{
		return ThisToken(kUnterminatedRegex);
		}
	}



	/* Match transliteration */



(tr|y)[ \v\t\f\r\n]* {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	if (SlurpQuoted(2, "cds"))
		{
		return ThisToken(kTransliteration);
		}
	else
		{
		return ThisToken(kUnterminatedTransliteration);
		}
	}



	/* Miscellaneous */
	/* Bareword must come after m//, qq//, etc. */



{BAREWORD} {
	StartToken();
	itsProbableOperatorFlag = kJTrue;
	return ThisToken(kBareword);
	}

{WSCHAR}+ |
{WS}\n    { /* Don't eat past a line end or we'd miss PP directives */
	StartToken();
	return ThisToken(kWhitespace);
	}

. {
	StartToken();
	itsProbableOperatorFlag = kJFalse;
	JUtf8Character c(yytext);
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
