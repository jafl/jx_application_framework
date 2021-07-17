/******************************************************************************
 CBHTMLCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBHTMLCompleter.h"
#include <jAssert.h>

CBHTMLCompleter* CBHTMLCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	// can't add tags because it is an open-ended list

	// special characters

	"lt;", "gt;",

	"amp;", "yen;", "uml;", "not;", "shy;", "reg;", "deg;", "ETH;", "eth;",

	"quot;", "nbsp;", "cent;", "sect;", "copy;", "ordf;", "macr;", "sup1;",
	"sup2;", "sup3;", "para;", "ordm;", "Auml;", "Euml;", "Iuml;", "Ouml;",
	"Uuml;", "auml;", "euml;", "iuml;", "ouml;", "uuml;", "yuml;",

	"iexcl;", "pound;", "laquo;", "acute;", "micro;", "cedil;", "raquo;",
	"times;", "Acirc;", "Aring;", "AElig;", "Ecirc;", "Icirc;", "Ocirc;",
	"Ucirc;", "THORN;", "szlig;", "acirc;", "aring;", "aelig;", "ecirc;",
	"icirc;", "ocirc;", "ucirc;", "thorn;",

	"curren;", "brvbar;", "plusmn;", "middot;", "frac14;", "frac12;", "frac34;",
	"iquest;", "Agrave;", "Aacute;", "Atilde;", "Ccedil;", "Egrave;", "Eacute;",
	"Igrave;", "Iacute;", "Ntilde;", "Ograve;", "Oacute;", "Otilde;", "Oslash;",
	"Ugrave;", "Uacute;", "Yacute;", "agrave;", "aacute;", "atilde;", "ccedil;",
	"egrave;", "eacute;", "igrave;", "iacute;", "ntilde;", "ograve;", "oacute;",
	"otilde;", "divide;", "oslash;", "ugrave;", "uacute;", "yacute;",

	"forall;", "exist;", "ni;", "lowast;", "minus;", "asymp;", "Alpha;", "Beta;",
	"Chi;", "Delta;", "Epsilon;", "Phi;", "Gamma;", "Eta;", "Iota;", "Kappa;",
	"Lambda;", "Mu;", "Nu;", "Omicron;", "Pi;", "Theta;", "Rho;", "Sigma;", "Tau;",
	"Upsilon;", "sigmaf;", "Omega;", "Xi;", "Psi;", "Zeta;", "there4;", "perp;",
	"oline;", "alpha;", "beta;", "chi;", "delta;", "epsilon;", "phi;", "gamma;",
	"eta;", "iota;", "kappa;", "lambda;", "mu;", "nu;", "omicron;", "pi;", "piv;",
	"theta;", "thetasym;", "rho;", "sigma;", "tau;", "upsilon;", "upsih;", "omega;",
	"xi;", "psi;", "zeta;", "sim;", "prime;", "le;", "frasl;", "infin;", "fnof;",
	"clubs;", "diams;", "hearts;", "spades;", "harr;", "larr;", "uarr;", "rarr;",
	"darr;", "Prime;", "ge;", "prop;", "part;", "bull;", "ne;", "equiv;", "cong;",
	"hellip;", "crarr;", "alefsym;", "image;", "real;", "weierp;", "otimes;",
	"oplus;", "empty;", "cap;", "cup;", "sup;", "supe;", "nsub;", "sub;", "sube;",
	"isin;", "notin;", "ang;", "nabla;", "trade;", "prod;", "radic;", "sdot;",
	"and;", "or;", "hArr;", "lArr;", "uArr;", "rArr;", "dArr;", "loz;", "lang;",
	"sum;", "lceil;", "lfloor;", "rang;", "int;", "rceil;", "rfloor;", "Scaron;",
	"scaron;", "OElig;", "oelig;", "Yuml;", "circ;", "tilde;", "ensp;", "emsp;",
	"thinsp;", "zwnj;", "zwj;", "lrm;", "rlm;", "ndash;", "mdash;", "lsquo;",
	"rsquo;", "sbquo;", "ldquo;", "rdquo;", "bdquo;", "dagger;", "Dagger;",
	"permil;", "lsaquo;", "rsaquo;", "euro;"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

CBStringCompleter*
CBHTMLCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = true;

		itsSelf = jnew CBHTMLCompleter;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBHTMLCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBHTMLCompleter::CBHTMLCompleter()
	:
	CBStringCompleter(kCBHTMLLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBHTMLCompleter::~CBHTMLCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

 ******************************************************************************/

bool
CBHTMLCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool				includeNS
	)
	const
{
	return c != '<' && c != '>' && c != '&' && c != '=' && !c.IsSpace();
}

/******************************************************************************
 GetDefaultWordList (static)

	Must be static because may be called before object is created.

 ******************************************************************************/

JSize
CBHTMLCompleter::GetDefaultWordList
	(
	const JUtf8Byte*** list
	)
{
	*list = kKeywordList;
	return kKeywordCount;
}
