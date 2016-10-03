/******************************************************************************
 CBJavaCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBJavaCompleter.h"
#include <ctype.h>
#include <jAssert.h>

CBJavaCompleter* CBJavaCompleter::itsSelf = NULL;

static const JCharacter* kKeywordList[] =
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

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JCharacter*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStringCompleter*
CBJavaCompleter::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = new CBJavaCompleter;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBJavaCompleter::Shutdown()
{
	delete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBJavaCompleter::CBJavaCompleter()
	:
	CBStringCompleter(kCBJavaLang, kKeywordCount, kKeywordList, kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBJavaCompleter::~CBJavaCompleter()
{
	itsSelf = NULL;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (.) to allow completion of fully
	qualified names.

 ******************************************************************************/

JBoolean
CBJavaCompleter::IsWordCharacter
	(
	const JString&	s,
	const JIndex	index,
	const JBoolean	includeNS
	)
	const
{
	const JCharacter c = s.GetCharacter(index);
	return JI2B(isalnum(c) || c == '_' || (includeNS && c == '.'));
}
