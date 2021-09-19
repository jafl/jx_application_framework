%top {
/*
Copyright © 2003 by John Lindal.

This scanner reads a Perl file and returns CB::Text::Perl::Scanner::Tokens.
*/

#include "CBStylingScannerBase.h"
#include <JStringIterator.h>
#include <jAssert.h>
%}

%option namespace="CB::Text::Perl" lexer="Scanner"
%option lex="NextToken" token-type="CBStylingScannerBase::Token"
%option unicode nodefault full freespace

%class {

public:

	// these types are ordered to correspond to the type table in CBPerlStyler

	enum TokenType
	{
		kBadInt = kEOF+1,
		kBadBinary,
		kBadHex,
		kEmptyVariable,
		kUnterminatedVariable,
		kUnterminatedString,
		kUnterminatedRegex,
		kUnterminatedWordList,
		kUnterminatedTransliteration,
		kUnterminatedFileGlob,
		kUnterminatedPOD,
		kIllegalChar,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kScalar,
		kList,
		kHash,
		kSubroutine,
		kPrototypeArgList,
		kReference,
		kReservedKeyword,
		kBareword,

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
		kVersion,

		kRegexSearch,
		kRegexReplace,
		kOneShotRegexSearch,
		kCompiledRegex,
		kTransliteration,

		kFileGlob,

		kComment,
		kPOD,
		kPPDirective,
		kModuleData,

		kError			// place holder for CBPerlStyler
	};

public:

	virtual void	BeginScan(const JStyledText* text,
							  const JStyledText::TextIndex& startIndex,
							  std::istream& input);

private:

	bool		itsProbableOperatorFlag;	// kTrue if /,? are most likely operators instead of regex
	TokenType	itsComplexVariableType;
	JString		itsHereDocTag;
	TokenType	itsHereDocType;
	JSize		itsBraceCount;
}

%x SINGLE_STRING_STATE DOUBLE_STRING_STATE EXEC_STRING_STATE HEREDOC_STRING_STATE
%x ONE_SHOT_REGEX_SEARCH_STATE REGEX_SEARCH_STATE
%x QUOTED_VARIABLE_STATE DATA_STATE POD_STATE FILE_GLOB_STATE

WSCHAR  ([ \v\t\f\r])
WS      ([ \v\t\f\r]*)



KEYWORD      (continue|do|else|elsif|for|foreach|goto|if|last|next|redo|unless|until|while|abs|accept|alarm|atan2|bind|binmode|bless|caller|chdir|chmod|chomp|chop|chown|chr|chroot|close|closedir|connect|cos|crypt|dbmclose|dbmopen|defined|delete|die|dump|each|eof|eval|exec|exists|exit|exp|fcntl|fileno|flock|fork|format|formline|getc|getlogin|getpeername|getpgrp|getppid|getpriority|getpwnam|getgrnam|gethostbyname|getnetbyname|getprotobyname|getpwuid|getgrgid|getservbyname|gethostbyaddr|getnetbyaddr|getprotobynumber|getservbyport|getpwent|getgrent|gethostent|getnetent|getprotoent|getservent|setpwent|setgrent|sethostent|setnetent|setprotoent|setservent|endpwent|endgrent|endhostent|endnetent|endprotoent|endservent|getsockname|getsockopt|glob|gmtime|grep|hex|import|index|int|ioctl|join|keys|kill|lc|lcfirst|length|link|listen|local|localtime|lock|log|lstat|map|mkdir|msgctl|msgget|msgrcv|msgsnd|my|no|oct|open|opendir|ord|our|pack|package|pipe|pop|pos|print|printf|prototype|push|quotemeta|rand|read|readdir|readline|readlink|readpipe|recv|ref|rename|require|reset|return|reverse|rewinddir|rindex|rmdir|scalar|seek|seekdir|select|semctl|semget|semop|send|setpgrp|setpriority|setsockopt|shift|shmctl|shmget|shmread|shmwrite|shutdown|sin|sleep|socket|socketpair|sort|splice|split|sprintf|sqrt|srand|stat|study|sub|substr|symlink|syscall|sysopen|sysread|sysseek|system|syswrite|tell|telldir|tie|tied|time|times|truncate|uc|ucfirst|umask|undef|unlink|unpack|untie|unshift|use|utime|values|vec|wait|waitpid|wantarray|warn|write)

PPKEYWORD    (line)

ID           ((_|\p{L}|\d)+)

BAREWORD     ((_|\p{L})(_|\p{L}|\d)*)



DECIMAL      (0_*|[1-9][0-9_]*)
HEX          (0x[_[:xdigit:]]*)
BINARY       (0b[01_]*)
OCTAL        (0[0-7_]+)
VERSION      (v[0-9]+(\.[0-9]+)*|([0-9]+\.){2,}[0-9]+)
%{
	/* The programmer probably meant a number, but it is invalid (match after other ints) */
%}
BADINT       ([0-9_]+)
BADHEX       (0[xX][0-9A-Za-z_]+)
BADBINARY    (0[bB][0-9A-Za-z_]+)



SIGNPART     ([+-])
DIGITSEQ     ([0-9][0-9_]*)
EXPONENT     ([eE]{SIGNPART}?{DIGITSEQ})
DOTDIGITS    ({DIGITSEQ}\.|{DIGITSEQ}\.{DIGITSEQ}|\.{DIGITSEQ})

FLOAT        ({DIGITSEQ}{EXPONENT}|{DOTDIGITS}{EXPONENT}?)

%%



	/* Match operators */



"->"  |
"++"  |
"--"  |
"**"  |
"!"   |
"~"   |
\\    |
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
	itsProbableOperatorFlag = false;
	return ThisToken(kOperator);
	}

-[rwxoRWXOezsfdlpSbctugkTBMAC] {
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



	/* Match argument lists */



"("(\$|\\\$|\\@|\\%|&|\\&|\*|\\\[(\$|@|%|&|\*)+\]|;)*(\$|\\\$|@|\\@|%|\\%|&|\\&|\*|\\\[(\$|@|%|&|\*)+\])")" {
	StartToken();
	itsProbableOperatorFlag = false;
	return ThisToken(kPrototypeArgList);
	}



	/* Match normal tokens */



\\?\$[&`'+*./|,\\;#%=\-~^:?!@$<>()\[\]] |
\\?\$\^[LAECDFHIMOPRSTVWX]              |
\\?\$\"\"                               |
\\?\$+{ID}                              {
	StartToken();
	itsProbableOperatorFlag = true;
	return ThisToken(text()[0] == '\\' ? kReference : kScalar);
	}

\\?\$+\{ {
	StartToken();
	itsProbableOperatorFlag = true;
	itsComplexVariableType  = text()[0] == '\\' ? kReference : kScalar;
	itsBraceCount           = 1;
	start(QUOTED_VARIABLE_STATE);
	}

\\?\@[+-] |
\\?\@\$*{ID} {
	StartToken();
	itsProbableOperatorFlag = true;
	return ThisToken(text()[0] == '\\' ? kReference : kList);
	}

\\?\@\$*\{ {
	StartToken();
	itsProbableOperatorFlag = true;
	itsComplexVariableType  = text()[0] == '\\' ? kReference : kList;
	itsBraceCount           = 1;
	start(QUOTED_VARIABLE_STATE);
	}

\\?\%\^H  |
\\?\%\$*{ID} {
	StartToken();
	itsProbableOperatorFlag = true;
	return ThisToken(text()[0] == '\\' ? kReference : kHash);
	}

\\?\%\$*\{ {
	StartToken();
	itsProbableOperatorFlag = true;
	itsComplexVariableType  = text()[0] == '\\' ? kReference : kHash;
	itsBraceCount           = 1;
	start(QUOTED_VARIABLE_STATE);
	}

\\?\&\$*{BAREWORD} {
	StartToken();
	itsProbableOperatorFlag = true;
	return ThisToken(text()[0] == '\\' ? kReference : kSubroutine);
	}

\\?\&\$*\{ {
	StartToken();
	itsProbableOperatorFlag = true;
	itsComplexVariableType  = text()[0] == '\\' ? kReference : kSubroutine;
	itsBraceCount           = 1;
	start(QUOTED_VARIABLE_STATE);
	}

[$@%&]\$*\{{WS}\} {
	StartToken();
	itsProbableOperatorFlag = true;
	return ThisToken(kEmptyVariable);
	}

__END__  |
__DATA__ {
	StartToken();
	start(DATA_STATE);
	return ThisToken(kBareword);
	}

{KEYWORD} {
	StartToken();
	itsProbableOperatorFlag = false;
	return ThisToken(kReservedKeyword);
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

{VERSION} {
	StartToken();
	itsProbableOperatorFlag = true;
	return ThisToken(kVersion);
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



	/* Match quoted variable constructs: ${...} */



<QUOTED_VARIABLE_STATE>{

"}" {
	ContinueToken();
	itsBraceCount--;
	if (itsBraceCount == 0)
		{
		start(INITIAL);
		return ThisToken(itsComplexVariableType);
		}
	}

"{" {
	ContinueToken();
	itsBraceCount++;
	}

[^{}]+ {
	ContinueToken();	/* should actually check for ' " ` strings! */
	}

<<EOF>> {
	start(INITIAL);
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
	start(INITIAL);
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
	start(POD_STATE);
	}

<POD_STATE>{

^"=cut"({WSCHAR}+.*)?\n? {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kPOD);
	}

.*\n? {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedPOD);
	}

}



	/* Match readline/fileglob */



"<" {
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
		start(FILE_GLOB_STATE);
		}
	}

<FILE_GLOB_STATE>{

">" {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kFileGlob);
	}

\n {
	start(INITIAL);
	return ThisToken(kUnterminatedFileGlob);
	}

\\.?    |
[^\\>]+ {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedFileGlob);
	}

}



	/* Match one-shot regex search */



"?" {
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
		start(ONE_SHOT_REGEX_SEARCH_STATE);
		}
	}

<ONE_SHOT_REGEX_SEARCH_STATE>{

"?" {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kOneShotRegexSearch);
	}

\\(.|\n)? |
[^\\?]+  {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedRegex);
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

"m"[ \v\t\f\r\n]* {
	StartToken();
	itsProbableOperatorFlag = true;
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
	start(INITIAL);
	return ThisToken(kRegexSearch);
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

"q"[ \v\t\f\r\n]* {
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

"qq"[ \v\t\f\r\n]* {
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



	/* Match compiled regex */



"qr"[ \v\t\f\r\n]* {
	StartToken();
	itsProbableOperatorFlag = true;
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
	itsProbableOperatorFlag = true;
	start(EXEC_STRING_STATE);
	}

"qx"[ \v\t\f\r\n]* {
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



"qw"[ \v\t\f\r\n]* {
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



	/* Match regex replace */



"s"[ \v\t\f\r\n]* {
	StartToken();
	itsProbableOperatorFlag = true;
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
	itsProbableOperatorFlag = true;
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
	itsProbableOperatorFlag = true;
	return ThisToken(kBareword);
	}

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
CB::Text::Perl::Scanner::BeginScan
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
