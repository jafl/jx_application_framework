%{
/*
Copyright © 1998 by John Lindal. All rights reserved.

This scanner reads an HTML file and calls the
following virtual functions:

	HandleHTMLWord
		Insert the given word.

	HandleHTMLWhitespace
		Insert whitespace as appropriate.

	HandleHTMLTag
		Handle the tag with the given name and attributes.

	HandleHTMLComment
		Handle the given comment.

	HandleHTMLError
		The specified error occurred.
*/

#define _H_JHTMLScannerL

#include <JHTMLScanner.h>
#include <JStringPtrMap.h>
#include <JRegex.h>
#include <JString.h>
#include <stdlib.h>
#include <limits.h>
#include <jAssert.h>

/* This is the easiest way to keep track of our position in the text. */

#define YY_USER_ACTION	UpdatePosition();

#define HANDLE_CHAR(c)				if (!HandleChar(c))          { return 0; }
#define HANDLE_GREEK_CHAR(c)		if (!HandleGreekChar(c))     { return 0; }
#define HANDLE_LATIN_CHAR(c,i,s)	if (!HandleLatinChar(c,i,s)) { return 0; }

#define TAG_FINISHED \
	if (!TagFinished()) \
		{ \
		return 0; \
		} \
	else if (IsScript(&scriptLanguage, &(scriptRange.first))) \
		{ \
		BEGIN(SCRIPT_STATE); \
		} \
	else \
		{ \
		BEGIN(INITIAL); \
		}
%}

%option c++ yyclass="JHTMLScanner" prefix="JHTML"
%option 8bit nodefault stack noyywrap

%x TAG_STATE SCRIPT_STATE COMMENT_STATE TOSS_TAG_STATE
%x ATTR_NAME_STATE
%x INIT_ATTR_VALUE_STATE UNQUOTED_ATTR_VALUE_STATE QUOTED_ATTR_VALUE_STATE
%x PHP_STATE PHP_C_COMMENT_STATE
%x PHP_SINGLE_STRING_STATE PHP_DOUBLE_STRING_STATE PHP_HEREDOC_STRING_STATE
%x CHAR_ESC_STATE

WS	[[:space:]]+
PHP	"<"("?"([pP][hH][pP])?|"%"=?){WS}

%%

%{
	JIndexRange commentRange, scriptRange;
	JString scriptLanguage;
	JRegex* phpHereDocPattern = NULL;

	BEGIN(INITIAL);
%}

"<" {
	NewTag();
	BEGIN(TAG_STATE);
	}

{PHP} {
	NewTag();
	yy_push_state(PHP_STATE);
	BeginPHP(yytext, yyleng);
	}

"<!--" {
	commentRange.first = itsMatchRange.first;
	BEGIN(COMMENT_STATE);
	}

"&" {
	yy_push_state(CHAR_ESC_STATE);
	}

{WS} {
	if (!HandleHTMLWhitespace(yytext, itsMatchRange))
		{
		return 0;
		}
	}

[^<&[:space:]]+ {
	if (!HandleHTMLWord(yytext, itsMatchRange))
		{
		return 0;
		}
	}

	/* This catches and ignores the newline when <pre> is on a separate line */

"<pre>"[ \t\r\f\v]*\n {
	NewTag();
	(itsTagInfo->name).Set("pre");
	TAG_FINISHED;
	}

\n[ \t\r\f\v]*"</pre>" {
	NewTag();
	(itsTagInfo->name).Set("/pre");
	TAG_FINISHED;
	}


	/* This grabs the name of the tag and looks for attributes */


<TAG_STATE>{

{WS}	/* ignore */

">" {
	TAG_FINISHED;
	}

[^>[:space:]]+ {
	(itsTagInfo->name).Set(yytext, yyleng);
	(itsTagInfo->name).ToLower();
	BEGIN(ATTR_NAME_STATE);
	}

}


	/* This grabs the attribute name and looks for a value */


<ATTR_NAME_STATE>{

{WS}	/* ignore */

">" {
	TAG_FINISHED;
	}

=[[:space:]]* {
	if ((itsTagInfo->lastAttr).IsEmpty())
		{
		if (!HandleHTMLError(kEmptyAttrName, "*** empty attribute name ***", JIndexRange()))
			{
			return 0;
			}
		BEGIN(TOSS_TAG_STATE);
		}
	else
		{
		BEGIN(INIT_ATTR_VALUE_STATE);
		}
	}

[^>=[:space:]]+ {
	(itsTagInfo->lastAttr).Set(yytext, yyleng);
	(itsTagInfo->lastAttr).ToLower();
	(itsTagInfo->attr).SetElement(itsTagInfo->lastAttr, (JString*) NULL,
								  JPtrArrayT::kDelete);
	}

}


	/* This checks whether or not the attribute value is quoted */


<INIT_ATTR_VALUE_STATE>{

">" {
	TAG_FINISHED;
	}

\" {
	BEGIN(QUOTED_ATTR_VALUE_STATE);
	}

[^">] {
	yyless(0);
	itsMatchRange.Set(itsMatchRange.first-1, itsMatchRange.first-1);
	BEGIN(UNQUOTED_ATTR_VALUE_STATE);
	}

}


	/* This grabs the attribute value */


<UNQUOTED_ATTR_VALUE_STATE>{

{WS} {
	SaveAttributeValue();
	BEGIN(ATTR_NAME_STATE);
	}

">" {
	SaveAttributeValue();
	TAG_FINISHED;
	}

"&" {
	yy_push_state(CHAR_ESC_STATE);
	}

{PHP} {
	(itsTagInfo->valueBuffer).Append(yytext, yyleng);
	yy_push_state(PHP_STATE);
	BeginPHP(yytext, yyleng);
	}

[^>&<[:space:]]+ |
"<"              {
	(itsTagInfo->valueBuffer).Append(yytext, yyleng);
	}

}


	/* This grabs the quoted attribute value */


<QUOTED_ATTR_VALUE_STATE>{

\"  {
	SaveAttributeValue();
	BEGIN(ATTR_NAME_STATE);
	}

"&" {
	yy_push_state(CHAR_ESC_STATE);
	}

{PHP} {
	(itsTagInfo->valueBuffer).Append(yytext, yyleng);
	yy_push_state(PHP_STATE);
	BeginPHP(yytext, yyleng);
	}

[^"&<]+ |
"<"     {
	(itsTagInfo->valueBuffer).Append(yytext, yyleng);
	}

}


	/* This slurps up a script */


<SCRIPT_STATE>{

"</"[sS][cC][rR][iI][pP][tT]">" {
	itsMatchRange.Set(scriptRange.first, itsMatchRange.first-1);
	scriptRange.last = itsMatchRange.last;
	if (!HandleHTMLScript(scriptLanguage, scriptRange))
		{
		return 0;
		}
	yyless(0);
	BEGIN(INITIAL);
	}

.|\n	/* ignore everything inside script */

<<EOF>> {
	scriptRange.last = itsMatchRange.last;
	HandleHTMLScript(scriptLanguage, scriptRange);
	yyterminate();
	}

}


	/* This slurps up a comment */


<COMMENT_STATE>{

"-->" {
	commentRange.last = itsMatchRange.last;
	if (!HandleHTMLComment(commentRange))
		{
		return 0;
		}
	BEGIN(INITIAL);
	}

.|\n	/* ignore everything inside comments */

<<EOF>> {
	commentRange.last = itsMatchRange.last;
	HandleHTMLComment(commentRange);
	yyterminate();
	}

}


	/* This tosses the remainder of the tag if an error occurs */


<TOSS_TAG_STATE>{

">" {
	TAG_FINISHED;	/* this goes here so JHTMLStyler gets the correct range */
	}

[^>]+	/* ignore everything inside tag */

}


	/* This converts legal character escapes (&) */
	/* Unlike the rest of HTML, these are case sensitive */


<CHAR_ESC_STATE>{

.|\n {
	yyless(0);			/* if nothing else matches, don't change the stream */
	itsMatchRange.Set(itsMatchRange.first-1, itsMatchRange.first-1);
	HANDLE_CHAR('&');
	}

<<EOF>> {
	/* itsMatchRange is correct because is isn't changed by <<EOF>> */
	HANDLE_CHAR('&');
	yyterminate();
	}

#[0-9]+;?           |
#[xX][0-9A-Fa-f]+;? {
	JString s(yytext+1, yyleng - (yytext[yyleng-1] == ';' ? 2 : 1));
	if (s.GetFirstCharacter() == 'x' || s.GetFirstCharacter() == 'X')
		{
		s.PrependCharacter('0');
		}
	JInteger v;
	if (s.ConvertToInteger(&v) && 0 <= v && v <= JInteger(UCHAR_MAX))
		{
		HANDLE_CHAR(v);
		}
	else
		{
		const JBoolean keepGoing =
			HandleHTMLError(kCharacterOutOfRange, "*** invalid character ***", itsMatchRange);
		yy_pop_state();
		if (!keepGoing)
			{
			return 0;
			}
		}
	}

lt;?      HANDLE_CHAR('<');		/* less than */
gt;?      HANDLE_CHAR('>');		/* greater than */
amp;?     HANDLE_CHAR('&');		/* ampersand */
quot;?    HANDLE_CHAR('"');		/* double quote */
nbsp;?    HANDLE_CHAR(' ');		/* non-breaking space */

iexcl;?   HANDLE_CHAR('\xA1');	/* i */
cent;?    HANDLE_CHAR('\xA2');	/* cents */
pound;?   HANDLE_CHAR('\xA3');	/* currency: pound */
curren;?  HANDLE_CHAR('\xA4');	/* currency: star */
yen;?     HANDLE_CHAR('\xA5');	/* currency: yen */
brvbar;?  HANDLE_CHAR('\xA6');	/* vertical bar */
sect;?    HANDLE_CHAR('\xA7');	/* part */
uml;?     HANDLE_CHAR('\xA8');	/* umlaut */
copy;?    HANDLE_CHAR('\xA9');	/* copyright */
ordf;?    HANDLE_CHAR('\xAA');	/* superscript underline a */
laquo;?   HANDLE_CHAR('\xAB');	/* << */
not;?     HANDLE_CHAR('\xAC');	/* logical not */
shy;?     HANDLE_CHAR('\xAD');	/* subscript dash */
reg;?     HANDLE_CHAR('\xAE');	/* registered trademark */
macr;?    HANDLE_CHAR('\xAF');	/* superscript dash */
deg;?     HANDLE_CHAR('\xB0');	/* degrees */
plusmn;?  HANDLE_CHAR('\xB1');	/* plus minus */
sup2;?    HANDLE_CHAR('\xB2');	/* superscript 2 */
sup3;?    HANDLE_CHAR('\xB3');	/* superscript 3 */
acute;?   HANDLE_CHAR('\xB4');	/* accent mark */
micro;?   HANDLE_CHAR('\xB5');	/* greek mu */
para;?    HANDLE_CHAR('\xB6');	/* paragraph */
middot;?  HANDLE_CHAR('\xB7');	/* dot */
cedil;?   HANDLE_CHAR('\xB8');	/* accent mark */
sup1;?    HANDLE_CHAR('\xB9');	/* superscript 1 */
ordm;?    HANDLE_CHAR('\xBA');	/* superscript underline o */
raquo;?   HANDLE_CHAR('\xBB');	/* >> */
frac14;?  HANDLE_CHAR('\xBC');	/* 1/4 */
frac12;?  HANDLE_CHAR('\xBD');	/* 1/2 */
frac34;?  HANDLE_CHAR('\xBE');	/* 3/4 */
iquest;?  HANDLE_CHAR('\xBF');	/* upside down question mark */
Agrave;?  HANDLE_CHAR('\xC0');
Aacute;?  HANDLE_CHAR('\xC1');
Acirc;?   HANDLE_CHAR('\xC2');	/* upper case modified characters */
Atilde;?  HANDLE_CHAR('\xC3');
Auml;?    HANDLE_CHAR('\xC4');	/* upper case umlauts */
Aring;?   HANDLE_CHAR('\xC5');
AElig;?   HANDLE_CHAR('\xC6');
Ccedil;?  HANDLE_CHAR('\xC7');
Egrave;?  HANDLE_CHAR('\xC8');
Eacute;?  HANDLE_CHAR('\xC9');
Ecirc;?   HANDLE_CHAR('\xCA');
Euml;?    HANDLE_CHAR('\xCB');
Igrave;?  HANDLE_CHAR('\xCC');
Iacute;?  HANDLE_CHAR('\xCD');
Icirc;?   HANDLE_CHAR('\xCE');
Iuml;?    HANDLE_CHAR('\xCF');
ETH;?     HANDLE_CHAR('\xD0');	/* Icelandic (capital) */
Ntilde;?  HANDLE_CHAR('\xD1');
Ograve;?  HANDLE_CHAR('\xD2');
Oacute;?  HANDLE_CHAR('\xD3');
Ocirc;?   HANDLE_CHAR('\xD4');
Otilde;?  HANDLE_CHAR('\xD5');
Ouml;?    HANDLE_CHAR('\xD6');
times;?   HANDLE_CHAR('\xD7');	/* multiplication sign */
Oslash;?  HANDLE_CHAR('\xD8');
Ugrave;?  HANDLE_CHAR('\xD9');
Uacute;?  HANDLE_CHAR('\xDA');
Ucirc;?   HANDLE_CHAR('\xDB');
Uuml;?    HANDLE_CHAR('\xDC');
Yacute;?  HANDLE_CHAR('\xDD');
THORN;?   HANDLE_CHAR('\xDE');
szlig;?   HANDLE_CHAR('\xDF');	/* German double s */
agrave;?  HANDLE_CHAR('\xE0');
aacute;?  HANDLE_CHAR('\xE1');
acirc;?   HANDLE_CHAR('\xE2');	/* lower case modified characters */
atilde;?  HANDLE_CHAR('\xE3');
auml;?    HANDLE_CHAR('\xE4');	/* lower case umlauts */
aring;?   HANDLE_CHAR('\xE5');
aelig;?   HANDLE_CHAR('\xE6');
ccedil;?  HANDLE_CHAR('\xE7');
egrave;?  HANDLE_CHAR('\xE8');
eacute;?  HANDLE_CHAR('\xE9');
ecirc;?   HANDLE_CHAR('\xEA');
euml;?    HANDLE_CHAR('\xEB');
igrave;?  HANDLE_CHAR('\xEC');
iacute;?  HANDLE_CHAR('\xED');
icirc;?   HANDLE_CHAR('\xEE');
iuml;?    HANDLE_CHAR('\xEF');
eth;?     HANDLE_CHAR('\xF0');	/* Icelandic (lowercase) */
ntilde;?  HANDLE_CHAR('\xF1');
ograve;?  HANDLE_CHAR('\xF2');
oacute;?  HANDLE_CHAR('\xF3');
ocirc;?   HANDLE_CHAR('\xF4');
otilde;?  HANDLE_CHAR('\xF5');
ouml;?    HANDLE_CHAR('\xF6');
divide;?  HANDLE_CHAR('\xF7');	/* division sign */
oslash;?  HANDLE_CHAR('\xF8');
ugrave;?  HANDLE_CHAR('\xF9');
uacute;?  HANDLE_CHAR('\xFA');
ucirc;?   HANDLE_CHAR('\xFB');
uuml;?    HANDLE_CHAR('\xFC');
yacute;?  HANDLE_CHAR('\xFD');
thorn;?   HANDLE_CHAR('\xFE');
yuml;?    HANDLE_CHAR('\xFF');

forall;?   HANDLE_GREEK_CHAR('\x22');	/* for all */
exist;?    HANDLE_GREEK_CHAR('\x24');	/* there exists */
ni;?       HANDLE_GREEK_CHAR('\x27');	/* ? contains as member */
lowast;?   HANDLE_GREEK_CHAR('\x2A');	/* asterisk */
minus;?    HANDLE_GREEK_CHAR('\x2D');	/* minus sign */
asymp;?    HANDLE_GREEK_CHAR('\x40');	/* ? asymptotic to */
Alpha;?    HANDLE_GREEK_CHAR('\x41');
Beta;?     HANDLE_GREEK_CHAR('\x42');
Chi;?      HANDLE_GREEK_CHAR('\x43');
Delta;?    HANDLE_GREEK_CHAR('\x44');
Epsilon;?  HANDLE_GREEK_CHAR('\x45');
Phi;?      HANDLE_GREEK_CHAR('\x46');
Gamma;?    HANDLE_GREEK_CHAR('\x47');
Eta;?      HANDLE_GREEK_CHAR('\x48');
Iota;?     HANDLE_GREEK_CHAR('\x49');
Kappa;?    HANDLE_GREEK_CHAR('\x4B');
Lambda;?   HANDLE_GREEK_CHAR('\x4C');
Mu;?       HANDLE_GREEK_CHAR('\x4D');
Nu;?       HANDLE_GREEK_CHAR('\x4E');
Omicron;?  HANDLE_GREEK_CHAR('\x4F');
Pi;?       HANDLE_GREEK_CHAR('\x50');
Theta;?    HANDLE_GREEK_CHAR('\x51');
Rho;?      HANDLE_GREEK_CHAR('\x52');
Sigma;?    HANDLE_GREEK_CHAR('\x53');
Tau;?      HANDLE_GREEK_CHAR('\x54');
Upsilon;?  HANDLE_GREEK_CHAR('\x55');
sigmaf;?   HANDLE_GREEK_CHAR('\x56');
Omega;?    HANDLE_GREEK_CHAR('\x57');
Xi;?       HANDLE_GREEK_CHAR('\x58');
Psi;?      HANDLE_GREEK_CHAR('\x59');
Zeta;?     HANDLE_GREEK_CHAR('\x5A');
there4;?   HANDLE_GREEK_CHAR('\x5C');	/* ? therefore */
perp;?     HANDLE_GREEK_CHAR('\x5E');	/* perpendicular to */
oline;?    HANDLE_GREEK_CHAR('\x60');	/* overline */
alpha;?    HANDLE_GREEK_CHAR('\x61');
beta;?     HANDLE_GREEK_CHAR('\x62');
chi;?      HANDLE_GREEK_CHAR('\x63');
delta;?    HANDLE_GREEK_CHAR('\x64');
epsilon;?  HANDLE_GREEK_CHAR('\x65');
phi;?      HANDLE_GREEK_CHAR('\x66');
gamma;?    HANDLE_GREEK_CHAR('\x67');
eta;?      HANDLE_GREEK_CHAR('\x68');
iota;?     HANDLE_GREEK_CHAR('\x69');
kappa;?    HANDLE_GREEK_CHAR('\x6B');
lambda;?   HANDLE_GREEK_CHAR('\x6C');
mu;?       HANDLE_GREEK_CHAR('\x6D');
nu;?       HANDLE_GREEK_CHAR('\x6E');
omicron;?  HANDLE_GREEK_CHAR('\x6F');
pi;?       HANDLE_GREEK_CHAR('\x70');
piv;?      HANDLE_GREEK_CHAR('\x70');   /* ? */
theta;?    HANDLE_GREEK_CHAR('\x71');
thetasym;? HANDLE_GREEK_CHAR('\x71');	/* ? */
rho;?      HANDLE_GREEK_CHAR('\x72');
sigma;?    HANDLE_GREEK_CHAR('\x73');
tau;?      HANDLE_GREEK_CHAR('\x74');
upsilon;?  HANDLE_GREEK_CHAR('\x75');
upsih;?    HANDLE_GREEK_CHAR('\x75');   /* ? */
omega;?    HANDLE_GREEK_CHAR('\x77');
xi;?       HANDLE_GREEK_CHAR('\x78');
psi;?      HANDLE_GREEK_CHAR('\x79');
zeta;?     HANDLE_GREEK_CHAR('\x7A');
sim;?      HANDLE_GREEK_CHAR('\x7E');	/* tilde */
prime;?    HANDLE_GREEK_CHAR('\xA2');	/* prime */
le;?       HANDLE_GREEK_CHAR('\xA3');	/* less than or equal to */
frasl;?    HANDLE_GREEK_CHAR('\xA4');	/* fraction slash */
infin;?    HANDLE_GREEK_CHAR('\xA5');	/* infinity */
fnof;?     HANDLE_GREEK_CHAR('\xA6');	/* function */
clubs;?    HANDLE_GREEK_CHAR('\xA7');
diams;?    HANDLE_GREEK_CHAR('\xA8');
hearts;?   HANDLE_GREEK_CHAR('\xA9');
spades;?   HANDLE_GREEK_CHAR('\xAA');
harr;?     HANDLE_GREEK_CHAR('\xAB');	/* left & right arrow */
larr;?     HANDLE_GREEK_CHAR('\xAC');	/* left arrow */
uarr;?     HANDLE_GREEK_CHAR('\xAD');	/* up arrow */
rarr;?     HANDLE_GREEK_CHAR('\xAE');	/* right arrow */
darr;?     HANDLE_GREEK_CHAR('\xAF');	/* down arrow */
Prime;?    HANDLE_GREEK_CHAR('\xB2');	/* double prime */
ge;?       HANDLE_GREEK_CHAR('\xB3');	/* greater than or equal to */
prop;?     HANDLE_GREEK_CHAR('\xB5');	/* proportional to */
part;?     HANDLE_GREEK_CHAR('\xB6');	/* partial differential */
bull;?     HANDLE_GREEK_CHAR('\xB7');	/* bullet */
ne;?       HANDLE_GREEK_CHAR('\xB9');	/* not equal */
equiv;?    HANDLE_GREEK_CHAR('\xBA');	/* identical */
cong;?     HANDLE_GREEK_CHAR('\xBB');	/* ? approximately equal to */
hellip;?   HANDLE_GREEK_CHAR('\xBC');	/* ellipsis */
crarr;?    HANDLE_GREEK_CHAR('\xBF');	/* carriage return */
alefsym;?  HANDLE_GREEK_CHAR('\xC0');	/* aleph */
image;?    HANDLE_GREEK_CHAR('\xC1');	/* imaginary part */
real;?     HANDLE_GREEK_CHAR('\xC2');	/* real part */
weierp;?   HANDLE_GREEK_CHAR('\xC3');	/* script capital P */
otimes;?   HANDLE_GREEK_CHAR('\xC4');	/* circled x */
oplus;?    HANDLE_GREEK_CHAR('\xC5');	/* circled plus */
empty;?    HANDLE_GREEK_CHAR('\xC6');	/* empty set */
cap;?      HANDLE_GREEK_CHAR('\xC7');	/* intersection */
cup;?      HANDLE_GREEK_CHAR('\xC8');	/* union */
sup;?      HANDLE_GREEK_CHAR('\xC9');	/* superset of */
supe;?     HANDLE_GREEK_CHAR('\xCA');	/* superset or equal */
nsub;?     HANDLE_GREEK_CHAR('\xCB');	/* not a subset of */
sub;?      HANDLE_GREEK_CHAR('\xCC');	/* subset of */
sube;?     HANDLE_GREEK_CHAR('\xCD');	/* subset or equal */
isin;?     HANDLE_GREEK_CHAR('\xCE');	/* element of */
notin;?    HANDLE_GREEK_CHAR('\xCF');	/* not element of */
ang;?      HANDLE_GREEK_CHAR('\xD0');	/* angle */
nabla;?    HANDLE_GREEK_CHAR('\xD1');	/* nabla */
trade;?    HANDLE_GREEK_CHAR('\xD4');	/* trademark */
prod;?     HANDLE_GREEK_CHAR('\xD5');	/* product */
radic;?    HANDLE_GREEK_CHAR('\xD6');	/* radical */
sdot;?     HANDLE_GREEK_CHAR('\xD7');	/* dot operator */
and;?      HANDLE_GREEK_CHAR('\xD9');	/* logical and */
or;?       HANDLE_GREEK_CHAR('\xDA');	/* logical or */
hArr;?     HANDLE_GREEK_CHAR('\xDB');	/* left & right double arrow */
lArr;?     HANDLE_GREEK_CHAR('\xDC');	/* left double arrow */
uArr;?     HANDLE_GREEK_CHAR('\xDD');	/* up double arrow */
rArr;?     HANDLE_GREEK_CHAR('\xDE');	/* right double arrow */
dArr;?     HANDLE_GREEK_CHAR('\xDF');	/* down double arrow */
loz;?      HANDLE_GREEK_CHAR('\xE0');	/* ? lozenge */
lang;?     HANDLE_GREEK_CHAR('\xE1');	/* bra */
sum;?      HANDLE_GREEK_CHAR('\xE5');	/* summation */
lceil;?    HANDLE_GREEK_CHAR('\xE9');	/* left ceiling */
lfloor;?   HANDLE_GREEK_CHAR('\xEB');	/* left floor */
rang;?     HANDLE_GREEK_CHAR('\xF1');	/* ket */
int;?      HANDLE_GREEK_CHAR('\xF2');	/* integral */
rceil;?    HANDLE_GREEK_CHAR('\xF9');	/* right ceiling */
rfloor;?   HANDLE_GREEK_CHAR('\xFB');	/* right floor */

Scaron;?   HANDLE_LATIN_CHAR('\xA9', 2, "S");
scaron;?   HANDLE_LATIN_CHAR('\xB9', 2, "s");

OElig;?    HANDLE_LATIN_CHAR('\xBC', 9, "OE");
oelig;?    HANDLE_LATIN_CHAR('\xBD', 9, "oe");
Yuml;?     HANDLE_LATIN_CHAR('\xBE', 9, "Y");

circ;?     HANDLE_CHAR('\x5E');
tilde;?    HANDLE_CHAR('\x7E');

ensp;?     HANDLE_CHAR(' ');
emsp;?     HANDLE_CHAR(' ');
thinsp;?   HANDLE_CHAR(' ');
zwnj;?     HANDLE_CHAR(' ');
	/* zwj;?      zero width joiner */
	/* lrm;? */
	/* rlm;? */
ndash;?    HANDLE_CHAR('-');	/* ? */
mdash;?    HANDLE_CHAR('-');	/* ? */
lsquo;?    HANDLE_CHAR('`');
rsquo;?    HANDLE_CHAR('\'');
	/* sbquo;? */
ldquo;?    HANDLE_CHAR('"');	/* ? */
rdquo;?    HANDLE_CHAR('"');
	/* bdquo;? */
	/* dagger;? */
	/* Dagger;? */
	/* permil;? */
lsaquo;?   HANDLE_CHAR('<');	/* ? */
rsaquo;?   HANDLE_CHAR('>');	/* ? */
euro;?     HANDLE_LATIN_CHAR('\xA4', 15, "EUR");

}


	/* This slurps up PHP code */


<PHP_STATE>{

"?>" |
"%>" {
	if (!PHPFinished(yytext, yyleng, kJTrue))
		{
		return 0;
		}
	yy_pop_state();
	}

\' {
	(itsTagInfo->valueBuffer).Append(yytext, yyleng);
	BEGIN(PHP_SINGLE_STRING_STATE);
	}

\" {
	(itsTagInfo->valueBuffer).Append(yytext, yyleng);
	BEGIN(PHP_DOUBLE_STRING_STATE);
	}

\<\<\<[A-Za-z0-9_]+[ \t]*\n {
	JString tag = yytext+3;
	tag.TrimWhitespace();
	tag.PrependCharacter('^');
	tag += ";?\n";
	assert( phpHereDocPattern == NULL );
	phpHereDocPattern = new JRegex(tag);
	assert( phpHereDocPattern != NULL );

	(itsTagInfo->valueBuffer).Append(yytext, yyleng);
	BEGIN(PHP_HEREDOC_STRING_STATE);
	}

"/*" {
	(itsTagInfo->valueBuffer).Append(yytext, yyleng);
	BEGIN(PHP_C_COMMENT_STATE);
	}

[^?%"'</]+ |
"?"      |
"%"      |
"<"      |
"/"      {
	(itsTagInfo->valueBuffer).Append(yytext, yyleng);
	}

<<EOF>> {
	PHPFinished(yytext, yyleng, kJFalse);
	yyterminate();
	}

}

<PHP_SINGLE_STRING_STATE>{

\' {
	(itsTagInfo->valueBuffer).Append(yytext, yyleng);
	BEGIN(PHP_STATE);
	}

\\(.|\n)? |
[^\\\']+  {
	(itsTagInfo->valueBuffer).Append(yytext, yyleng);
	}

<<EOF>> {
	PHPFinished(yytext, yyleng, kJFalse);
	yyterminate();
	}

}

<PHP_DOUBLE_STRING_STATE>{

\" {
	(itsTagInfo->valueBuffer).Append(yytext, yyleng);
	BEGIN(PHP_STATE);
	}

\\(.|\n)? |
[^\\\"]+  {
	(itsTagInfo->valueBuffer).Append(yytext, yyleng);
	}

<<EOF>> {
	PHPFinished(yytext, yyleng, kJFalse);
	yyterminate();
	}

}

<PHP_HEREDOC_STRING_STATE>{

^.+\n {
	(itsTagInfo->valueBuffer).Append(yytext, yyleng);
	if (phpHereDocPattern->Match(yytext))
		{
		delete phpHereDocPattern;
		phpHereDocPattern = NULL;
		BEGIN(PHP_STATE);
		}
	}

.|\n {
	(itsTagInfo->valueBuffer).Append(yytext, yyleng);
	}

<<EOF>> {
	PHPFinished(yytext, yyleng, kJFalse);
	yyterminate();
	}

}

<PHP_C_COMMENT_STATE>{

"*"+"/" {
	(itsTagInfo->valueBuffer).Append(yytext, yyleng);
	BEGIN(PHP_STATE);
	}

[^*\n]*        |
[^*\n]*\n      |
"*"+[^*/\n]*   |
"*"+[^*/\n]*\n {
	(itsTagInfo->valueBuffer).Append(yytext, yyleng);
	}

<<EOF>> {
	PHPFinished(yytext, yyleng, kJFalse);
	yyterminate();
	}

}

%%
