/******************************************************************************
 CBJSPCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright © 2007 by John Lindal.

 ******************************************************************************/

#include "CBJSPCompleter.h"
#include "CBHTMLCompleter.h"
#include "CBJavaScriptCompleter.h"
#include "CBJavaScriptStyler.h"
#include "CBHTMLStyler.h"
#include <jAssert.h>

CBJSPCompleter* CBJSPCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	// remember to update CBJavaCompleter

	// keywords

	"abstract", "boolean", "break", "byte", "byvalue", "case", "cast",
	"catch", "char", "class", "const", "continue", "default", "do",
	"double", "else", "enum", "extends", "false", "final", "finally", "float",
	"for", "future", "generic", "goto", "if", "implements", "import",
	"inner", "instanceof", "int", "interface", "long", "native", "new",
	"null", "operator", "outer", "package", "private", "protected",
	"public", "rest", "return", "short", "static", "super", "switch",
	"synchronized", "this", "throw", "throws", "transient", "true",
	"try", "var", "void", "volatile", "while",

	// methods

	"clone", "equals", "finalize", "getClass", "hashCode", "notify",
	"notifyAll", "toString", "wait",

	// lang package

	"Appendable", "Boolean", "Byte", "Character", "CharSequence",
	"Class", "ClassLoader", "Cloneable", "Comparable", "Compiler",
	"Double", "Enum", "Float", "InheritableThreadLocal", "Integer",
	"Iterable", "Long", "Math", "Number", "Object", "Package",
	"Process", "ProcessBuilder", "Readable", "Runnable", "Runtime",
	"RuntimePermission", "SecurityManager", "Short", "StackTraceElement",
	"StrictMath", "String", "StringBuffer", "StringBuilder",
	"System", "Thread", "ThreadGroup", "ThreadLocal", "Throwable", "Void",

	// util package

	"AbstractCollection", "AbstractList", "AbstractMap", "AbstractQueue",
	"AbstractSequentialList", "AbstractSet", "ArrayList", "Arrays", "BitSet",
	"Calendar", "Collections", "Comparator", "Currency", "Date", "Dictionary",
	"EnumMap", "EnumSet", "Enumeration", "EventListenerProxy", "EventObject",
	"FormattableFlags", "Formatter", "GregorianCalendar", "HashMap", "HashSet",
	"Hashtable", "IdentityHashMap", "Iterator", "LinkedHashMap", "LinkedHashSet",
	"LinkedList", "List", "ListIterator", "ListResourceBundle", "Locale",
	"Map", "Map.Entry", "Observable", "Observer", "PriorityQueue", "Properties",
	"PropertyPermission", "PropertyResourceBundle", "Queue", "Random",
	"RandomAccess", "ResourceBundle", "Scanner", "Set", "SimpleTimeZone",
	"SortedMap", "SortedSet", "Stack", "StringTokenizer", "Timer", "TimerTask",
	"TimeZone", "TreeMap", "TreeSet", "UUID", "Vector", "WeakHashMap"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

CBStringCompleter*
CBJSPCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = true;

		itsSelf = jnew CBJSPCompleter;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBJSPCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBJSPCompleter::CBJSPCompleter()
	:
	CBStringCompleter(kCBJSPLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
	UpdateWordList();	// include HTML and JavaScript
	ListenTo(CBHTMLStyler::Instance());
	ListenTo(CBJavaScriptStyler::Instance());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBJSPCompleter::~CBJSPCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (.) to allow completion of fully
	qualified names.

 ******************************************************************************/

bool
CBJSPCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_' || (includeNS && c == '.');
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBJSPCompleter::Receive
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
CBJSPCompleter::UpdateWordList()
{
	CBStringCompleter::UpdateWordList();

	// include HTML words

	const JUtf8Byte** htmlWordList;
	JSize count = CBHTMLCompleter::GetDefaultWordList(&htmlWordList);
	for (JUnsignedOffset i=0; i<count; i++)
		{
		Add(JString(htmlWordList[i], JString::kNoCopy));
		}

	CopyWordsFromStyler(CBHTMLStyler::Instance());

	// include JavaScript words

	const JUtf8Byte** jsWordList;
	count = CBJavaScriptCompleter::GetDefaultWordList(&jsWordList);
	for (JUnsignedOffset i=0; i<count; i++)
		{
		Add(JString(jsWordList[i], JString::kNoCopy));
		}

	CopyWordsFromStyler(CBJavaScriptStyler::Instance());
	CopySymbolsForLanguage(kCBJavaScriptLang);
}
