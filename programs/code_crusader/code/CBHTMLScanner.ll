%top{
/*
Copyright © 2001 by John Lindal.

This scanner reads an HTML/XML/PHP/JSP file and returns CB::Text::HTML::Scanner::Tokens.
*/

#include "CBStylingScannerBase.h"
#include <JRegex.h>
#include <jAssert.h>
%}

%option namespace="CB::Text::HTML" lexer="Scanner"
%option lex="NextToken" token-type="CBStylingScannerBase::Token"
%option unicode nodefault full freespace

%class {

public:

	// these types are ordered to correspond to the type table in CBHTMLStyler

	enum TokenType
	{
		kHTMLEmptyTag = kEOF+1,
		kHTMLUnterminatedTag,
		kHTMLInvalidNamedCharacter,
		kUnterminatedString,
		kUnterminatedComment,
		kUnterminatedPHPVariable,
		kUnterminatedJSRegex,
		kJavaBadCharConst,
		kPHPBadInt,
		kBadHex,
		kIllegalChar,

		kWhitespace,		// must be the one before the first item in type table

		// HTML

		kHTMLText,
		kHTMLTag,
		kHTMLScript,
		kHTMLNamedCharacter,
		kHTMLComment,
		kCDATABlock,

		// Mustache

		kMustacheCommand,

		// PHP

		kPHPStartEnd,
		kPHPID,
		kPHPVariable,
		kPHPReservedKeyword,
		kPHPBuiltInDataType,

		kPHPOperator,
		kPHPDelimiter,

		kPHPSingleQuoteString,
		kPHPDoubleQuoteString,
		kPHPHereDocString,
		kPHPNowDocString,
		kPHPExecString,

		// JSP/Java

		kJSPStartEnd,
		kJSPComment,

		kJavaID,
		kJavaReservedKeyword,
		kJavaBuiltInDataType,

		kJavaOperator,
		kJavaDelimiter,

		kJavaString,
		kJavaCharConst,

		// JavaScript

		kJSID,
		kJSReservedKeyword,

		kJSOperator,
		kJSDelimiter,

		kJSString,
		kJSTemplateString,
		kJSRegexSearch,

		// shared

		kFloat,
		kDecimalInt,
		kHexInt,
		kPHPOctalInt,

		kComment,
		kDocCommentHTMLTag,
		kDocCommentSpecialTag,

		kError			// place holder for CBHTMLStyler
	};

public:

	virtual void	BeginScan(const JStyledText* text,
							  const JStyledText::TextIndex& startIndex,
							  std::istream& input);

protected:

	bool	InTagState() const;

	virtual const JString&	GetScannedText() const = 0;

private:

	JString	itsScriptLanguage;
	JString	itsPHPHereDocTag;
	JString	itsPHPNowDocTag;
	bool	itsProbableJSOperatorFlag;	// kTrue if / is most likely operator instead of regex
	JSize	itsBraceCount;

private:

	Token	ScriptToken();
	bool	IsScript(JString* language) const;
	bool	InTagState(const int state) const;
}

%x TAG_STATE SCRIPT_STATE CHAR_ESC_STATE
%x QUOTED_ATTR_VALUE_STATE

%x CDATA_STATE MUSTACHE_STATE

%x PHP_STATE
%x PHP_QUOTED_VARIABLE_STATE PHP_CPP_COMMENT_STATE
%x PHP_SINGLE_STRING_STATE PHP_DOUBLE_STRING_STATE
%x PHP_HEREDOC_STRING_STATE PHP_NOWDOC_STRING_STATE
%x PHP_EXEC_STRING_STATE PHP_COMPLEX_VARIABLE_STATE

%x JSP_STATE JSP_CPP_COMMENT_STATE JAVA_STRING_STATE

%x HTML_COMMENT_STATE JSP_HTML_COMMENT_STATE
%x C_COMMENT_STATE DOC_COMMENT_STATE

%x JAVA_SCRIPT_STATE
%x JS_C_COMMENT_STATE JS_DOC_COMMENT_STATE JS_CPP_COMMENT_STATE
%x JS_SINGLE_STRING_STATE JS_DOUBLE_STRING_STATE JS_TEMPLATE_STRING_STATE
%x JS_REGEX_SEARCH_STATE JS_REGEX_CHAR_CLASS_STATE

WS            [[:space:]]+
PHPSTART      "<"("?"([pP][hH][pP]{WS})?|"?=")
JSPSTART      "<%"[!=@]?
JSP_COMMENT   "<%--"
SCRIPT_END    ("</"[sS][cC][rR][iI][pP][tT]">")

DOCTAG       (attribute|author|chainable|class|code|config|constructor|default|deprecated|description|docRoot|event|exception|extends|final|for|inheritDoc|link|linkplain|literal|method|module|namespace|param|private|property|protected|public|requires|return|see|serial|serialData|serialField|since|static|throws|type|uses|value|version|writeonce)


PHPKEYWORD    (__CLASS__|__DIR__|__FILE__|__FUNCTION__|__LINE__|__METHOD__|__NAMESPACE__|__TRAIT__|abstract|and|array|as|break|callable|case|catch|class|clone|const|continue|declare|default|do|echo|else|elseif|enddeclare|endfor|endforeach|endif|endswitch|endwhile|extends|final(ly)?|for|foreach|function|global|goto|if|implements|include|include_once|instanceof|insteadof|interface|list|namespace|new|or|parent|print|private|protected|public|require|require_once|return|static|switch|throw|trait|try|use|var|while|xor|yield)

PHPDATATYPE   (array|bool|boolean|double|float|int|integer|object|real|string|null|nullptr|true|TRUE|false|FALSE)

ESCCODE       ([\x7f-\xff])
IDCAR         (_|\p{L}|{ESCCODE})
IDCDR         (_|\p{L}|\d|{ESCCODE})
ID            ({IDCAR}{IDCDR}*)

DECIMAL       (0|[1-9][0-9]*)
HEX           (0[xX][[:xdigit:]]+)
OCTAL         (0[0-7]+)
%{
	/* The programmer probably meant a number, but it is invalid (match after other ints) */
%}
BADINT        ([0-9]+)
BADHEX        (0[xX][0-9A-Za-z]+)

SIGNPART      ([+-])
DIGITSEQ      ([0-9]+)
EXPONENT      ([eE]{SIGNPART}?{DIGITSEQ})
DOTDIGITS     ({DIGITSEQ}\.|{DIGITSEQ}\.{DIGITSEQ}|\.{DIGITSEQ})

FLOAT         ({DIGITSEQ}{EXPONENT}|{DOTDIGITS}{EXPONENT}?)


JAVA_KEYWORD  (abstract|attribute|autoFlush|break|buffer|byvalue|case|cast|catch|class|ClassLoader|clone|Cloneable|Compiler|const|contentType|continue|default|do|else|errorPage|extends|equals|false|final(ly)?|finalize|for|future|generic|getClass|goto|hashCode|if|include|implements|import|info|inner|instanceof|interface|isErrorPage|isThreadSafe|language|Math|NaN|native|NEGATIVE_INFINITY|new|notify(All)?|null|operator|outer|package|page|POSITIVE_INFINITY|private|Process|protected|public|Random|rest|return|Runnable|Runtime|SecurityManager|session|static|StringTokenizer|super|switch|synchronized|System|tag|taglib|this|Thread(Group)?|throws?|Throwable|toString|transient|true|try|var|void|volatile|wait|while)

JAVA_DATATYPE (BitSet|[bB]oolean|byte|char|Character|Class|Date|Dictionary|[dD]ouble|Enumeration|[fF]loat|Hashtable|int|Integer|[lL]ong|Number|Object|Observable|Observer|Properties|short|Stack|String(Buffer)?|Vector)

JAVA_UESCCODE (\\[uU][[:xdigit:]]{4})

JAVA_IDCAR    (_|\p{L}|{JAVA_UESCCODE})
JAVA_IDCDR    (_|\p{L}|\d|{JAVA_UESCCODE})

JAVA_ID       ({JAVA_IDCAR}{JAVA_IDCDR}*)

JAVA_INTSUFFIX ([lL])

JAVA_DECIMAL  ([0-9]+{JAVA_INTSUFFIX}?)
JAVA_HEX      (0[xX]0*[[:xdigit:]]{1,8}{JAVA_INTSUFFIX}?)
JAVA_BADHEX   (0[xX][[:xdigit:]]{9,}{JAVA_INTSUFFIX}?)

JAVA_FLOATSUFFIX ([fFlL])
JAVA_FLOAT       {FLOAT}{JAVA_FLOATSUFFIX}?

JAVA_CESCCODE    ([bfnrt\\'"])
JAVA_OESCCODE    ([1-3]?[0-7]{2})
JAVA_ESCCHAR     (\\({JAVA_CESCCODE}|{JAVA_OESCCODE}))
JAVA_BADESCCHAR  (\\[^bfnrt\\'"u[:digit:]])

JAVA_CHAR        ([^\n'\\]|{JAVA_ESCCHAR}|{JAVA_UESCCODE})
JAVA_CCONST      (\'{JAVA_CHAR}\')
JAVA_BADCCONST   (\'(\\|{JAVA_BADESCCHAR}|({JAVA_BADESCCHAR}|{JAVA_CHAR}){2,})\')


JS_KEYWORD    (abstract|arguments|async|await|boolean|break|byte|case|catch|char|class|const|constructor|continue|debugger|default|delete|do|double|else|enum|eval|export|extends|false|final|finally|float|for|function|goto|if|implements|import|in|instanceof|int|interface|let|long|native|new|null|of|package|private|protected|public|return|short|static|super|switch|synchronized|this|throw|throws|transient|true|try|typeof|undefined|var|void|volatile|while|with|yield)

JS_IDCAR      (_|\p{L})
JS_IDCDR      (_|\p{L}|\d)

JS_ID         ({JS_IDCAR}{JS_IDCDR}*)


JS_DECIMAL    ([0-9]+)
JS_HEX        (0[xX]0*[[:xdigit:]]{1,8})
JS_BADHEX     (0[xX][[:xdigit:]]{9,})

%%

"<" {
	StartToken();
	start(TAG_STATE);
	}

"<>" {
	StartToken();
	return ThisToken(kHTMLEmptyTag);
	}

"<![CDATA[" {
	StartToken();
	start(CDATA_STATE);
	}

"{{"\{? {
	StartToken();
	start(MUSTACHE_STATE);
	}

{PHPSTART} {
	matcher().less(0);
	push_state(PHP_STATE);
	}

{JSPSTART} {
	matcher().less(0);
	push_state(JSP_STATE);
	}

{JSP_COMMENT} {
	matcher().less(0);
	push_state(JSP_HTML_COMMENT_STATE);
	}

"<!--" {
	StartToken();
	start(HTML_COMMENT_STATE);
	}

"&" {
	StartToken();
	push_state(CHAR_ESC_STATE);
	}

[^<&{]+ |
"{"     {
	StartToken();
	return ThisToken(kHTMLText);
	}



	/* Match tag and attributes */



<TAG_STATE>{

">" {
	ContinueToken();
	const bool isScript = IsScript(&itsScriptLanguage);
	if (isScript && JString::Compare(itsScriptLanguage, "javascript", JString::kIgnoreCase) == 0)
		{
		start(JAVA_SCRIPT_STATE);
		return ScriptToken();
		}
	else if (isScript)
		{
		start(SCRIPT_STATE);
		}
	else
		{
		start(INITIAL);
		return ThisToken(kHTMLTag);
		}
	}

"="[[:space:]]*\" {
	ContinueToken();
	start(QUOTED_ATTR_VALUE_STATE);
	}

"&" {
	const Token t = ThisToken(kHTMLTag);
	StartToken();
	push_state(CHAR_ESC_STATE);
	if (!(t.range).IsEmpty())		/* may come directly from ESC/PHP */
		{
		return t;
		}
	}

{PHPSTART} {
	matcher().less(0);
	push_state(PHP_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from ESC/PHP */
		{
		return ThisToken(kHTMLTag);
		}
	}

{JSPSTART} {
	matcher().less(0);
	push_state(JSP_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from ESC/JSP */
		{
		return ThisToken(kHTMLTag);
		}
	}

{JSP_COMMENT} {
	matcher().less(0);
	push_state(JSP_HTML_COMMENT_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from ESC/JSP */
		{
		return ThisToken(kHTMLTag);
		}
	}

[^>=&<]+ |
"="      |
"<"      {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kHTMLUnterminatedTag);
	}

}



	/* Match quoted attribute value */



<QUOTED_ATTR_VALUE_STATE>{

\"  {
	ContinueToken();
	start(TAG_STATE);
	}

"&" {
	const Token t = ThisToken(kHTMLTag);
	StartToken();
	push_state(CHAR_ESC_STATE);
	if (!(t.range).IsEmpty())		/* may come directly from ESC/PHP */
		{
		return t;
		}
	}

{PHPSTART} {
	matcher().less(0);
	push_state(PHP_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from ESC/PHP */
		{
		return ThisToken(kHTMLTag);
		}
	}

{JSPSTART} {
	matcher().less(0);
	push_state(JSP_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from ESC/JSP */
		{
		return ThisToken(kHTMLTag);
		}
	}

{JSP_COMMENT} {
	matcher().less(0);
	push_state(JSP_HTML_COMMENT_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from ESC/JSP */
		{
		return ThisToken(kHTMLTag);
		}
	}

[^"&<]+ |
"<"     {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kHTMLUnterminatedTag);
	}

}



	/* Match script */



<SCRIPT_STATE>{

{SCRIPT_END} {
	ContinueToken();
	start(INITIAL);
	return ScriptToken();
	}

{PHPSTART} {
	matcher().less(0);
	push_state(PHP_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from ESC/PHP */
		{
		return ScriptToken();
		}
	}

{JSPSTART} {
	matcher().less(0);
	push_state(JSP_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from ESC/JSP */
		{
		return ScriptToken();
		}
	}

{JSP_COMMENT} {
	matcher().less(0);
	push_state(JSP_HTML_COMMENT_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from ESC/JSP */
		{
		return ScriptToken();
		}
	}

[^<]+ |
"<"   {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ScriptToken();
	}

}



	/* Match CDATA */



<CDATA_STATE>{

"]]>" {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kCDATABlock);
	}

{PHPSTART} {
	matcher().less(0);
	push_state(PHP_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from ESC/PHP */
		{
		return ThisToken(kCDATABlock);
		}
	}

{JSPSTART} {
	matcher().less(0);
	push_state(JSP_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from ESC/JSP */
		{
		return ThisToken(kCDATABlock);
		}
	}

{JSP_COMMENT} {
	matcher().less(0);
	push_state(JSP_HTML_COMMENT_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from ESC/JSP */
		{
		return ThisToken(kCDATABlock);
		}
	}

[^]<]+ |
"]"    |
"<"    {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kCDATABlock);
	}

}



	/* Match HTML comment */



<HTML_COMMENT_STATE>{

"-->" {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kHTMLComment);
	}

[^-]+ |
"-"   {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kHTMLComment);
	}

}



	/* Match JSP comment */



<JSP_HTML_COMMENT_STATE>{

{JSP_COMMENT} {
	StartToken();
	}

"--%>" {
	ContinueToken();
	pop_state();
	const Token t = ThisToken(kJSPComment);
	InitToken();	/* req'd when reentering tag */
	return t;
	}

[^-]+ |
"-"   {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kJSPComment);
	}

}



	/* Match legal character escapes (&) */
	/* Unlike the rest of HTML, these are case sensitive */



<CHAR_ESC_STATE>{

.|\n {
	matcher().less(0);
	const bool isText = top_state() == INITIAL;
	pop_state();
	const Token t = ThisToken(isText ? kHTMLText : kHTMLTag);
	InitToken();	/* req'd when reentering tag */
	return t;
	}

<<EOF>> {
	const bool isText = top_state() == INITIAL;
	start(INITIAL);
	return ThisToken(isText ? kHTMLText : kHTMLTag);
	}

#[0-9]+;?           |
#[xX][[:xdigit:]]+;? {
	ContinueToken();
	pop_state();
	JString s(text()+1, size() - (text()[size()-1] == ';' ? 2 : 1));
	if (s.GetFirstCharacter() == 'x' || s.GetFirstCharacter() == 'X')
		{
		s.Prepend("0");
		}
//	JInteger v;
	const Token t = ThisToken(// s.ConvertToInteger(&v) && 0 <= v && v <= JInteger(UCHAR_MAX) ?
							  kHTMLNamedCharacter); // : kHTMLInvalidNamedCharacter);
	InitToken();	/* req'd when reentering tag */
	return t;
	}

lt;?       |
gt;?       |
amp;?      |
quot;?     |
nbsp;?     |
iexcl;?    |
cent;?     |
pound;?    |
curren;?   |
yen;?      |
brvbar;?   |
sect;?     |
uml;?      |
copy;?     |
ordf;?     |
laquo;?    |
not;?      |
shy;?      |
reg;?      |
macr;?     |
deg;?      |
plusmn;?   |
sup2;?     |
sup3;?     |
acute;?    |
micro;?    |
para;?     |
middot;?   |
cedil;?    |
sup1;?     |
ordm;?     |
raquo;?    |
frac14;?   |
frac12;?   |
frac34;?   |
iquest;?   |
Agrave;?   |
Aacute;?   |
Acirc;?    |
Atilde;?   |
Auml;?     |
Aring;?    |
AElig;?    |
Ccedil;?   |
Egrave;?   |
Eacute;?   |
Ecirc;?    |
Euml;?     |
Igrave;?   |
Iacute;?   |
Icirc;?    |
Iuml;?     |
ETH;?      |
Ntilde;?   |
Ograve;?   |
Oacute;?   |
Ocirc;?    |
Otilde;?   |
Ouml;?     |
times;?    |
Oslash;?   |
Ugrave;?   |
Uacute;?   |
Ucirc;?    |
Uuml;?     |
Yacute;?   |
THORN;?    |
szlig;?    |
agrave;?   |
aacute;?   |
acirc;?    |
atilde;?   |
auml;?     |
aring;?    |
aelig;?    |
ccedil;?   |
egrave;?   |
eacute;?   |
ecirc;?    |
euml;?     |
igrave;?   |
iacute;?   |
icirc;?    |
iuml;?     |
eth;?      |
ntilde;?   |
ograve;?   |
oacute;?   |
ocirc;?    |
otilde;?   |
ouml;?     |
divide;?   |
oslash;?   |
ugrave;?   |
uacute;?   |
ucirc;?    |
uuml;?     |
yacute;?   |
thorn;?    |
yuml;?     |
forall;?   |
exist;?    |
ni;?       |
lowast;?   |
minus;?    |
asymp;?    |
Alpha;?    |
Beta;?     |
Chi;?      |
Delta;?    |
Epsilon;?  |
Phi;?      |
Gamma;?    |
Eta;?      |
Iota;?     |
Kappa;?    |
Lambda;?   |
Mu;?       |
Nu;?       |
Omicron;?  |
Pi;?       |
Theta;?    |
Rho;?      |
Sigma;?    |
Tau;?      |
Upsilon;?  |
sigmaf;?   |
Omega;?    |
Xi;?       |
Psi;?      |
Zeta;?     |
there4;?   |
perp;?     |
oline;?    |
alpha;?    |
beta;?     |
chi;?      |
delta;?    |
epsilon;?  |
phi;?      |
gamma;?    |
eta;?      |
iota;?     |
kappa;?    |
lambda;?   |
mu;?       |
nu;?       |
omicron;?  |
pi;?       |
piv;?      |
theta;?    |
thetasym;? |
rho;?      |
sigma;?    |
tau;?      |
upsilon;?  |
upsih;?    |
omega;?    |
xi;?       |
psi;?      |
zeta;?     |
sim;?      |
prime;?    |
le;?       |
frasl;?    |
infin;?    |
fnof;?     |
clubs;?    |
diams;?    |
hearts;?   |
spades;?   |
harr;?     |
larr;?     |
uarr;?     |
rarr;?     |
darr;?     |
Prime;?    |
ge;?       |
prop;?     |
part;?     |
bull;?     |
ne;?       |
equiv;?    |
cong;?     |
hellip;?   |
crarr;?    |
alefsym;?  |
image;?    |
real;?     |
weierp;?   |
otimes;?   |
oplus;?    |
empty;?    |
cap;?      |
cup;?      |
sup;?      |
supe;?     |
nsub;?     |
sub;?      |
sube;?     |
isin;?     |
notin;?    |
ang;?      |
nabla;?    |
trade;?    |
prod;?     |
radic;?    |
sdot;?     |
and;?      |
or;?       |
hArr;?     |
lArr;?     |
uArr;?     |
rArr;?     |
dArr;?     |
loz;?      |
lang;?     |
sum;?      |
lceil;?    |
lfloor;?   |
rang;?     |
int;?      |
rceil;?    |
rfloor;?   |
Scaron;?   |
scaron;?   |
OElig;?    |
oelig;?    |
Yuml;?     |
circ;?     |
tilde;?    |
ensp;?     |
emsp;?     |
thinsp;?   |
zwnj;?     |
zwj;?      |
lrm;?      |
rlm;?      |
ndash;?    |
mdash;?    |
lsquo;?    |
rsquo;?    |
sbquo;?    |
ldquo;?    |
rdquo;?    |
bdquo;?    |
dagger;?   |
Dagger;?   |
permil;?   |
lsaquo;?   |
rsaquo;?   |
euro;?     {
	ContinueToken();
	pop_state();
	const Token t = ThisToken(kHTMLNamedCharacter);
	InitToken();	/* req'd when reentering tag */
	return t;
	}

}



	/* Match Mustache command */



<MUSTACHE_STATE>{

"}}"\}? {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kMustacheCommand);
	}

[^}]+ |
"}"   {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kMustacheCommand);
	}

}



	/* Match PHP code */



<PHP_STATE>{

{PHPSTART} {
	StartToken();
	return ThisToken(kPHPStartEnd);
	}

"?>" {
	StartToken();
	pop_state();
	const Token t = ThisToken(kPHPStartEnd);
	InitToken();	/* req'd when reentering tag */
	return t;
	}

"="   |
"+="  |
"-="  |
"*="  |
"/="  |
".="  |
"%="  |
"<<=" |
">>=" |
"&="  |
"^="  |
"|="  |
"<<"  |
">>"  |
"++"  |
"--"  |
">"   |
"<"   |
"<="  |
">="  |
"!"   |
"=="  |
"===" |
"!="  |
"!==" |
"&&"  |
"||"  |
"and" |
"or"  |
"xor" |
"."   |
"->"  |
"/"   |
"%"   |
"+"   |
"-"   |
"~"   |
"|"   |
"^"   |
"*"   |
"&"   |
"@"   |
"::"  |
":"   |
"?"   {
	StartToken();
	return ThisToken(kPHPOperator);
	}

"(" |
")" |
"[" |
"]" |
"{" |
"}" |
"," |
";" {
	StartToken();
	return ThisToken(kPHPDelimiter);
	}

{PHPKEYWORD} {
	StartToken();
	return ThisToken(kPHPReservedKeyword);
	}

{PHPDATATYPE} {
	StartToken();
	return ThisToken(kPHPBuiltInDataType);
	}

{ID} {
	StartToken();
	return ThisToken(kPHPID);
	}

"$"+{ID} {
	StartToken();
	return ThisToken(kPHPVariable);
	}

"$"+"{" {
	StartToken();
	itsBraceCount = 1;
	start(PHP_QUOTED_VARIABLE_STATE);
	}

{DECIMAL} {
	StartToken();
	return ThisToken(kDecimalInt);
	}

{HEX} {
	StartToken();
	return ThisToken(kHexInt);
	}

{OCTAL} {
	StartToken();
	return ThisToken(kPHPOctalInt);
	}

{BADINT} {
	StartToken();
	return ThisToken(kPHPBadInt);
	}

{BADHEX} {
	StartToken();
	return ThisToken(kBadHex);
	}

{FLOAT} {
	StartToken();
	return ThisToken(kFloat);
	}



	/* Match strings */



\' {
	StartToken();
	start(PHP_SINGLE_STRING_STATE);
	}

\" {
	StartToken();
	start(PHP_DOUBLE_STRING_STATE);
	}

"<<<"{ID}[ \t]*\n {
	StartToken();
	itsPHPHereDocTag.Set(text()+3, size()-4);
	itsPHPHereDocTag.TrimWhitespace();
	start(PHP_HEREDOC_STRING_STATE);
	}

"<<<"'{ID}'[ \t]*\n {
	StartToken();
	JString s(text()+4, size()-5);
	s.TrimWhitespace();
	itsPHPNowDocTag.Set(s.GetBytes(), s.GetByteCount()-1);
	start(PHP_NOWDOC_STRING_STATE);
	}

\` {
	StartToken();
	start(PHP_EXEC_STRING_STATE);
	}



	/* Match comments */



"/*" {
	StartToken();
	push_state(C_COMMENT_STATE);
	}

"/**"/[^/] {
	StartToken();
	push_state(DOC_COMMENT_STATE);
	}

"//" |
"#"  {
	StartToken();
	start(PHP_CPP_COMMENT_STATE);
	}



	/* Miscellaneous */



{WS} {
	StartToken();
	return ThisToken(kWhitespace);
	}

. {
	StartToken();
	return ThisToken(kIllegalChar);
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kEOF);
	}

}



	/* Match complex variable constructs: ${...} */



<PHP_QUOTED_VARIABLE_STATE>{

"}" {
	ContinueToken();
	itsBraceCount--;
	if (itsBraceCount == 0)
		{
		start(PHP_STATE);
		return ThisToken(kPHPVariable);
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
	return ThisToken(kUnterminatedPHPVariable);
	}

}



	/* Match strings */



<PHP_SINGLE_STRING_STATE>{

\' {
	ContinueToken();
	start(PHP_STATE);
	return ThisToken(kPHPSingleQuoteString);
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

<PHP_DOUBLE_STRING_STATE>{

\" {
	ContinueToken();
	start(PHP_STATE);
	return ThisToken(kPHPDoubleQuoteString);
	}

"{$" {
	ContinueToken();
	itsBraceCount = 1;
	push_state(PHP_COMPLEX_VARIABLE_STATE);
	}

\\(.|\n)? |
[^\\\"{]+ |
"{"       {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}

<PHP_HEREDOC_STRING_STATE>{

^.+\n {
	ContinueToken();
	if (JString::Compare(text(), size() - (text()[size()-2] == ';' ? 2 : 1),
					   itsPHPHereDocTag.GetBytes(), itsPHPHereDocTag.GetByteCount()) == 0)
		{
		start(PHP_STATE);
		return ThisToken(kPHPHereDocString);
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

<PHP_NOWDOC_STRING_STATE>{

^.+\n {
	ContinueToken();
	if (JString::Compare(
			text(), size() - (text()[size()-2] == ';' ? 2 : 1),
			itsPHPNowDocTag.GetBytes(), itsPHPNowDocTag.GetByteCount()) == 0)
		{
		start(PHP_STATE);
		return ThisToken(kPHPNowDocString);
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

<PHP_EXEC_STRING_STATE>{

\` {
	ContinueToken();
	start(PHP_STATE);
	return ThisToken(kPHPExecString);
	}

"{$" {
	ContinueToken();
	itsBraceCount = 1;
	push_state(PHP_COMPLEX_VARIABLE_STATE);
	}

\\(.|\n)? |
[^\\`{]+  |
"{"       {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}

<PHP_COMPLEX_VARIABLE_STATE>{

"}" {
	ContinueToken();
	itsBraceCount--;
	if (itsBraceCount == 0)
		{
		pop_state();
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
	return ThisToken(kUnterminatedString);
	}

}



	/* Match JSP code */



<JSP_STATE>{

{JSPSTART} {
	StartToken();
	return ThisToken(kJSPStartEnd);
	}

"%>" {
	StartToken();
	pop_state();
	const Token t = ThisToken(kJSPStartEnd);
	InitToken();	/* req'd when reentering tag */
	return t;
	}

"++"   |
"--"   |
"+"    |
"-"    |
"~"    |
"!"    |
"*"    |
"/"    |
"%"    |
"<<"   |
">>"   |
">>>"  |
"<"    |
"<="   |
">"    |
">="   |
"=="   |
"!="   |
"&"    |
"^"    |
"|"    |
"&&"   |
"||"   |
"?"    |
":"    |
"="    |
"*="   |
"/="   |
"%="   |
"+="   |
"-="   |
"<<="  |
">>="  |
">>>=" |
"&="   |
"^="   |
"|="   |
"."    {
	StartToken();
	return ThisToken(kJavaOperator);
	}

"(" |
")" |
"[" |
"]" |
"{" |
"}" |
"," |
";" {
	StartToken();
	return ThisToken(kJavaDelimiter);
	}

{JAVA_KEYWORD} {
	StartToken();
	return ThisToken(kJavaReservedKeyword);
	}

{JAVA_DATATYPE} {
	StartToken();
	return ThisToken(kJavaBuiltInDataType);
	}

{JAVA_ID} {
	StartToken();
	return ThisToken(kJavaID);
	}

{JAVA_DECIMAL} {
	StartToken();
	return ThisToken(kDecimalInt);
	}

{JAVA_HEX} {
	StartToken();
	return ThisToken(kHexInt);
	}

{JAVA_BADHEX} {
	StartToken();
	return ThisToken(kBadHex);
	}

{JAVA_FLOAT} {
	StartToken();
	return ThisToken(kFloat);
	}

{JAVA_CCONST} {
	StartToken();
	return ThisToken(kJavaCharConst);
	}

{JAVA_BADCCONST} {
	StartToken();
	return ThisToken(kJavaBadCharConst);
	}



	/* Match strings */



\" {
	StartToken();
	start(JAVA_STRING_STATE);
	}



	/* Match comments */



"/*" {
	StartToken();
	push_state(C_COMMENT_STATE);
	}

"/**"/[^/] {
	StartToken();
	push_state(DOC_COMMENT_STATE);
	}

"//" {
	StartToken();
	start(JSP_CPP_COMMENT_STATE);
	}



	/* Miscellaneous */



{WS} {
	StartToken();
	return ThisToken(kWhitespace);
	}

. {
	StartToken();
	return ThisToken(kIllegalChar);
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kEOF);
	}

}



	/* Match strings -- based on the string scanner from the flex manual. */
	/* Java does not support \\\n to continue a string on the next line.  */



<JAVA_STRING_STATE>{

\" {
	ContinueToken();
	start(JSP_STATE);
	return ThisToken(kJavaString);
	}

\n {
	ContinueToken();
	start(JSP_STATE);
	return ThisToken(kUnterminatedString);
	}

\\.?  |
[^\\\n\"]+ {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Match C style comments */



<C_COMMENT_STATE>{

"*"+"/" {
	ContinueToken();
	pop_state();
	return ThisToken(kComment);
	}

[^*\n]*\n?      |
"*"+[^*/\n]*\n? {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedComment);
	}

}

<JS_C_COMMENT_STATE>{

"*"+"/" {
	ContinueToken();
	start(JAVA_SCRIPT_STATE);
	return ThisToken(kComment);
	}

[^*<\n]*\n?      |
"*"+[^*/<\n]*\n? |
"<"              {
	ContinueToken();
	}

{SCRIPT_END} {
	matcher().less(0);
	start(JAVA_SCRIPT_STATE);
	return ThisToken(kUnterminatedComment);
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedComment);
	}

}



	/* Match javadoc style comments */

	/* remember to update CBJavaScanner */



<DOC_COMMENT_STATE>{

[^*<@\n]*\n?      |
"*"+[^*/<@\n]*\n? |
[<@]              {
	ContinueToken();
	}

"*"+"/" {
	ContinueToken();
	pop_state();
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

<JS_DOC_COMMENT_STATE>{

[^*<@\n]*\n?      |
"*"+[^*/<@\n]*\n? |
[<@]              {
	ContinueToken();
	}

"*"+"/" {
	ContinueToken();
	start(JAVA_SCRIPT_STATE);
	return ThisToken(kComment);
	}

{SCRIPT_END} {
	matcher().less(0);
	start(JAVA_SCRIPT_STATE);
	return ThisToken(kUnterminatedComment);
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



	/* Match C++ and UNIX style comments */



<PHP_CPP_COMMENT_STATE>{

\n {
	ContinueToken();
	start(PHP_STATE);
	return ThisToken(kComment);
	}

"?>" {
	matcher().less(0);
	start(PHP_STATE);
	return ThisToken(kComment);
	}

[^?\n]+ |
"?"     {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kComment);
	}

}

<JSP_CPP_COMMENT_STATE>{

\n {
	ContinueToken();
	start(JSP_STATE);
	return ThisToken(kComment);
	}

"%>" {
	matcher().less(0);
	start(JSP_STATE);
	return ThisToken(kComment);
	}

[^%\n]+ |
"%"     {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kComment);
	}

}

<JS_CPP_COMMENT_STATE>{

\n {
	ContinueToken();
	start(JAVA_SCRIPT_STATE);
	return ThisToken(kComment);
	}

{SCRIPT_END} {
	matcher().less(0);
	start(JAVA_SCRIPT_STATE);
	return ThisToken(kComment);
	}

[^<\n]+ |
"<"     {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kComment);
	}

}



	/* JavaScript -- copied from CBJavaScriptScannerL.l */

	/* Remember to upgrade CBJavaScriptScanner, too! */



<JAVA_SCRIPT_STATE>{

{SCRIPT_END} {
	StartToken();
	start(INITIAL);
	return ScriptToken();
	}

{PHPSTART} {
	matcher().less(0);
	push_state(PHP_STATE);
	}

{JSPSTART} {
	matcher().less(0);
	push_state(JSP_STATE);
	}

{JSP_COMMENT} {
	matcher().less(0);
	push_state(JSP_HTML_COMMENT_STATE);
	}


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
	itsProbableJSOperatorFlag = false;
	return ThisToken(kJSOperator);
	}



	/* Avoid styling JSX tags as regexp */



"</" |
"/>" {
	StartToken();
	itsProbableJSOperatorFlag = false;
	return ThisToken(kJSOperator);
	}



	/* Match grouping symbols and other delimiters */



"(" |
"[" |
"{" |
"}" |
"," |
";" {
	StartToken();
	itsProbableJSOperatorFlag = false;
	return ThisToken(kJSDelimiter);
	}

")" |
"]" {
	StartToken();
	itsProbableJSOperatorFlag = true;
	return ThisToken(kJSDelimiter);
	}



	/* Match normal tokens */



{JS_KEYWORD} {
	StartToken();
	itsProbableJSOperatorFlag = false;
	return ThisToken(kJSReservedKeyword);
	}

{JS_ID} {
	StartToken();
	itsProbableJSOperatorFlag = true;
	return ThisToken(kJSID);
	}

{JS_DECIMAL} {
	StartToken();
	itsProbableJSOperatorFlag = true;
	return ThisToken(kDecimalInt);
	}

{JS_HEX} {
	StartToken();
	itsProbableJSOperatorFlag = true;
	return ThisToken(kHexInt);
	}

{JS_BADHEX} {
	StartToken();
	itsProbableJSOperatorFlag = true;
	return ThisToken(kBadHex);
	}

{FLOAT} {
	StartToken();
	itsProbableJSOperatorFlag = true;
	return ThisToken(kFloat);
	}



	/* Match strings */



\' {
	StartToken();
	itsProbableJSOperatorFlag = true;
	start(JS_SINGLE_STRING_STATE);
	}

\" {
	StartToken();
	itsProbableJSOperatorFlag = true;
	start(JS_DOUBLE_STRING_STATE);
	}

\` {
	StartToken();
	itsProbableJSOperatorFlag = true;
	start(JS_TEMPLATE_STRING_STATE);
	}



	/* Match regex search */



"/"=? {
	if (itsProbableJSOperatorFlag)
		{
		StartToken();
		itsProbableJSOperatorFlag = false;
		return ThisToken(kJSOperator);
		}
	else
		{
		StartToken();
		itsProbableJSOperatorFlag = true;
		start(JS_REGEX_SEARCH_STATE);
		}
	}



	/* Match comments */



"/*" {
	StartToken();
	start(JS_C_COMMENT_STATE);
	}

"/**"/[^/] {
	StartToken();
	start(JS_DOC_COMMENT_STATE);
	}

"//" {
	StartToken();
	start(JS_CPP_COMMENT_STATE);
	}



	/* Miscellaneous */



{WS} {
	StartToken();
	return ThisToken(kWhitespace);
	}

. {
	StartToken();
	return ThisToken(kIllegalChar);
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kEOF);
	}

}



	/* The following match tokens that are too dangerous to eat in one bite */
	/* because it is so easy to create unterminated comments and strings.   */



	/* Match strings -- based on the string scanner from the flex manual. */



<JS_SINGLE_STRING_STATE>{

\' {
	ContinueToken();
	start(JAVA_SCRIPT_STATE);
	return ThisToken(kJSString);
	}

\n {
	ContinueToken();
	start(JAVA_SCRIPT_STATE);
	return ThisToken(kUnterminatedString);
	}

\\(.|\n)?   |
"<"         |
[^\\\n\'<]+ {
	ContinueToken();
	}

{PHPSTART} {
	matcher().less(0);
	push_state(PHP_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from PHP */
		{
		return ThisToken(kJSString);
		}
	}

{JSPSTART} {
	matcher().less(0);
	push_state(JSP_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from JSP */
		{
		return ThisToken(kJSString);
		}
	}

{JSP_COMMENT} {
	matcher().less(0);
	push_state(JSP_HTML_COMMENT_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from JSP */
		{
		return ThisToken(kJSString);
		}
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}

<JS_DOUBLE_STRING_STATE>{

\" {
	ContinueToken();
	start(JAVA_SCRIPT_STATE);
	return ThisToken(kJSString);
	}

\n {
	ContinueToken();
	start(JAVA_SCRIPT_STATE);
	return ThisToken(kUnterminatedString);
	}

\\(.|\n)?   |
"<"         |
[^\\\n\"<]+ {
	ContinueToken();
	}

{PHPSTART} {
	matcher().less(0);
	push_state(PHP_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from PHP */
		{
		return ThisToken(kJSString);
		}
	}

{JSPSTART} {
	matcher().less(0);
	push_state(JSP_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from JSP */
		{
		return ThisToken(kJSString);
		}
	}

{JSP_COMMENT} {
	matcher().less(0);
	push_state(JSP_HTML_COMMENT_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from JSP */
		{
		return ThisToken(kJSString);
		}
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}

<JS_TEMPLATE_STRING_STATE>{

\` {
	ContinueToken();
	start(JAVA_SCRIPT_STATE);
	return ThisToken(kJSTemplateString);
	}

\\.?      |
"<"       |
[^\\\`<]+ {
	ContinueToken();
	}

{PHPSTART} {
	matcher().less(0);
	push_state(PHP_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from PHP */
		{
		return ThisToken(kJSTemplateString);
		}
	}

{JSPSTART} {
	matcher().less(0);
	push_state(JSP_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from JSP */
		{
		return ThisToken(kJSTemplateString);
		}
	}

{JSP_COMMENT} {
	matcher().less(0);
	push_state(JSP_HTML_COMMENT_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from JSP */
		{
		return ThisToken(kJSTemplateString);
		}
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Match regex search */



<JS_REGEX_SEARCH_STATE>{

"/"[gim]* {
	ContinueToken();
	start(JAVA_SCRIPT_STATE);
	return ThisToken(kJSRegexSearch);
	}

\[(^-?\])? {
	ContinueToken();
	start(JS_REGEX_CHAR_CLASS_STATE);
	}

\\.?      |
"<"       |
[^[\\/<]+ {
	ContinueToken();
	}

{PHPSTART} {
	matcher().less(0);
	push_state(PHP_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from PHP */
		{
		return ThisToken(kJSRegexSearch);
		}
	}

{JSPSTART} {
	matcher().less(0);
	push_state(JSP_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from JSP */
		{
		return ThisToken(kJSRegexSearch);
		}
	}

{JSP_COMMENT} {
	matcher().less(0);
	push_state(JSP_HTML_COMMENT_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from JSP */
		{
		return ThisToken(kJSRegexSearch);
		}
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedJSRegex);
	}

}

<JS_REGEX_CHAR_CLASS_STATE>{

"]" {
	ContinueToken();
	start(JS_REGEX_SEARCH_STATE);
	}

\\.?     |
"<"      |
[^]\\<]+ {
	ContinueToken();
	}

{PHPSTART} {
	matcher().less(0);
	push_state(PHP_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from PHP */
		{
		return ThisToken(kJSRegexSearch);
		}
	}

{JSPSTART} {
	matcher().less(0);
	push_state(JSP_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from JSP */
		{
		return ThisToken(kJSRegexSearch);
		}
	}

{JSP_COMMENT} {
	matcher().less(0);
	push_state(JSP_HTML_COMMENT_STATE);
	if (!GetCurrentRange().IsEmpty())	/* may come directly from JSP */
		{
		return ThisToken(kJSRegexSearch);
		}
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedJSRegex);
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
CB::Text::HTML::Scanner::BeginScan
	(
	const JStyledText*				text,
	const JStyledText::TextIndex&	startIndex,
	std::istream&					input
	)
{
	CBStylingScannerBase::BeginScan(text, startIndex, input);

	itsBraceCount = 0;
	itsScriptLanguage.Clear();
	itsPHPHereDocTag.Clear();
	itsPHPNowDocTag.Clear();
	itsProbableJSOperatorFlag = false;
}

/******************************************************************************
 ScriptToken (private)

 *****************************************************************************/

CB::Text::HTML::Scanner::Token
CB::Text::HTML::Scanner::ScriptToken()
{
	return Token(kHTMLScript, GetCurrentRange(), &itsScriptLanguage);
}

/******************************************************************************
 IsScript (private)

	Extracts the language and the start of the script range and returns
	true if the last tag was the start of a script.

 ******************************************************************************/

static JRegex scriptTagPattern1 =
	"^<[[:space:]]*script[[:space:]]+[^>]*language[[:space:]]*=[[:space:]]*\"?([^>\"[:space:]]+)";
static JRegex scriptTagPattern2 =
	"^<[[:space:]]*script[[:space:]]+[^>]*type[[:space:]]*=[[:space:]]*\"?[^/]+/([^>\"[:space:]]+)";
static JRegex scriptTagPattern3 =
	"^<[[:space:]]*script(>|[[:space:]])";

bool
CB::Text::HTML::Scanner::IsScript
	(
	JString* language
	)
	const
{
	scriptTagPattern1.SetCaseSensitive(false);
	scriptTagPattern2.SetCaseSensitive(false);
	scriptTagPattern3.SetCaseSensitive(false);

	language->Clear();

	const JString s(GetScannedText().GetRawBytes(), GetCurrentRange().byteRange, JString::kNoCopy);

	const JStringMatch m1 = scriptTagPattern1.Match(s, JRegex::kIncludeSubmatches);
	if (!m1.IsEmpty())
		{
		*language = m1.GetSubstring(1);
		return true;
		}

	const JStringMatch m2 = scriptTagPattern2.Match(s, JRegex::kIncludeSubmatches);
	if (!m2.IsEmpty())
		{
		*language = m2.GetSubstring(1);
		return true;
		}

	const JStringMatch m3 = scriptTagPattern3.Match(s, JRegex::kIgnoreSubmatches);
	if (!m3.IsEmpty())
		{
		*language = "JavaScript";
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 InTagState (protected)

 ******************************************************************************/

bool
CB::Text::HTML::Scanner::InTagState()
	const
{
	return InTagState(INITIAL);
}

// private

bool
CB::Text::HTML::Scanner::InTagState
	(
	const int state
	)
	const
{
	return (state == TAG_STATE               ||
			state == QUOTED_ATTR_VALUE_STATE ||
			(state == CHAR_ESC_STATE && InTagState(top_state())));
}
