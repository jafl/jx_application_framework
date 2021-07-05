/******************************************************************************
 CBPHPCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#include "CBPHPCompleter.h"
#include "CBHTMLCompleter.h"
#include "CBJavaScriptCompleter.h"
#include "CBJavaScriptStyler.h"
#include "CBHTMLStyler.h"
#include <jAssert.h>

CBPHPCompleter* CBPHPCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	#include "cbPHPFunctionList.h"

	"__FILE__", "__LINE__", "__FUNCTION__", "__CLASS__", "__METHOD__",
	"abstract", "and", "array", "as",
	"bool", "boolean", "break",
	"case", "catch", "cfunction", "class", "clone", "const", "continue",
	"declare", "default", "do", "double",
	"echo", "else", "elseif", "enddeclare", "endif", "endfor", "endforeach",
	"endswitch", "endwhile", "exception", "extends",
	"if", "implements", "int", "integer", "interface",
	"false", "FALSE", "final", "float", "for", "foreach", "function",
	"global",
	"new", "null", "nullptr",
	"object", "old_function", "or",
	"parent", "php_user_filter", "print", "private", "protected", "public",
	"real", "return",
	"static", "string", "switch",
	"this", "throw", "true", "TRUE", "try",
	"use",
	"var",
	"while",
	"xor"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStringCompleter*
CBPHPCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = jnew CBPHPCompleter;
		assert( itsSelf != nullptr );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBPHPCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBPHPCompleter::CBPHPCompleter()
	:
	CBStringCompleter(kCBPHPLang, kKeywordCount, kKeywordList, kJTrue)
{
	UpdateWordList();	// include HTML and JavaScript
	ListenTo(CBHTMLStyler::Instance());
	ListenTo(CBJavaScriptStyler::Instance());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBPHPCompleter::~CBPHPCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (::) to allow completion of fully
	qualified names.

 ******************************************************************************/

JBoolean
CBPHPCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const JBoolean			includeNS
	)
	const
{
	return JI2B(c.IsAlnum() || c == '_' || (includeNS && c == ':'));
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBPHPCompleter::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if ((sender == CBHTMLStyler::Instance() ||
		 sender == CBJavaScriptStyler::Instance()) &&
		message.Is(CBStylerBase::kWordListChanged))
		{
		UpdateWordList();
		}
	else
		{
		CBStringCompleter::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateWordList (virtual protected)

 ******************************************************************************/

void
CBPHPCompleter::UpdateWordList()
{
	CBStringCompleter::UpdateWordList();

	// include HTML words

	const JUtf8Byte** htmlWordList;
	JSize count = CBHTMLCompleter::GetDefaultWordList(&htmlWordList);
	for (JUnsignedOffset i=0; i<count; i++)
		{
		Add(JString(htmlWordList[i], kJFalse));
		}

	CopyWordsFromStyler(CBHTMLStyler::Instance());

	// include JavaScript words

	const JUtf8Byte** jsWordList;
	count = CBJavaScriptCompleter::GetDefaultWordList(&jsWordList);
	for (JUnsignedOffset i=0; i<count; i++)
		{
		Add(JString(jsWordList[i], kJFalse));
		}

	CopyWordsFromStyler(CBJavaScriptStyler::Instance());
	CopySymbolsForLanguage(kCBJavaScriptLang);
}
