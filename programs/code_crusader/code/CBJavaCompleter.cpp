/******************************************************************************
 CBJavaCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#include "CBJavaCompleter.h"
#include <jAssert.h>

CBJavaCompleter* CBJavaCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	// remember to update CBJSPCompleter

	// keywords

	"abstract", "boolean", "break", "byte", "byvalue", "case", "cast",
	"catch", "char", "class", "const", "continue", "default", "do",
	"double", "else", "enum", "extends", "false", "final", "finally",
	"float", "for", "future", "generic", "goto", "if", "implements",
	"import", "inner", "instanceof", "int", "interface", "long", "native",
	"new", "null", "operator", "outer", "package", "private", "protected",
	"public", "rest", "return", "short", "static", "super", "switch",
	"synchronized", "this", "throw", "throws", "transient", "true", "try",
	"var", "void", "volatile", "while",

	// methods

	"clone", "equals", "finalize", "getClass", "hashCode", "notify",
	"notifyAll", "toString", "wait",

	// lang package

	"Appendable", "Boolean", "Byte", "Character", "CharSequence", "Class",
	"ClassLoader", "Cloneable", "Comparable", "Compiler", "Double", "Enum",
	"Exception", "Float", "InheritableThreadLocal", "Integer", "Iterable",
	"Long", "Math", "Number", "Object", "Package", "Process",
	"ProcessBuilder", "Readable", "Runnable", "Runtime",
	"RuntimeException", "RuntimePermission", "SecurityManager", "Short",
	"StackTraceElement", "StrictMath", "String", "StringBuffer",
	"StringBuilder", "System", "Thread", "ThreadGroup", "ThreadLocal",
	"Throwable", "Void",

	// util package

	"AbstractCollection", "AbstractList", "AbstractMap", "AbstractQueue",
	"AbstractSequentialList", "AbstractSet", "ArrayList", "Arrays",
	"BitSet", "Calendar", "Collections", "Comparator", "Currency", "Date",
	"Dictionary", "EnumMap", "EnumSet", "Enumeration",
	"EventListenerProxy", "EventObject", "FormattableFlags", "Formatter",
	"GregorianCalendar", "HashMap", "HashSet", "Hashtable",
	"IdentityHashMap", "Iterator", "LinkedHashMap", "LinkedHashSet",
	"LinkedList", "List", "ListIterator", "ListResourceBundle", "Locale",
	"Map", "Map.Entry", "Observable", "Observer", "PriorityQueue",
	"Properties", "PropertyPermission", "PropertyResourceBundle", "Queue",
	"Random", "RandomAccess", "ResourceBundle", "Scanner", "Set",
	"SimpleTimeZone", "SortedMap", "SortedSet", "Stack", "StringTokenizer",
	"Timer", "TimerTask", "TimeZone", "TreeMap", "TreeSet", "UUID",
	"Vector", "WeakHashMap",

	// doc tags

	"author", "code", "docRoot", "deprecated", "exception", "inheritDoc",
	"link", "linkplain", "literal", "param", "return", "see", "serial",
	"serialData", "serialField", "since", "throws", "value", "version"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

CBStringCompleter*
CBJavaCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = true;

		itsSelf = jnew CBJavaCompleter;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBJavaCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBJavaCompleter::CBJavaCompleter()
	:
	CBStringCompleter(kCBJavaLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBJavaCompleter::~CBJavaCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (.) to allow completion of fully
	qualified names.

 ******************************************************************************/

bool
CBJavaCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_' || (includeNS && c == '.');
}
