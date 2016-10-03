/******************************************************************************
 CBJavaScriptCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright (C) 2006 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBJavaScriptCompleter.h"
#include <ctype.h>
#include <jAssert.h>

CBJavaScriptCompleter* CBJavaScriptCompleter::itsSelf = NULL;

static const JCharacter* kKeywordList[] =
{
	// keywords

	"abstract", "boolean", "break", "byte", "case", "catch", "char", "class",
	"continue", "const", "debugger", "default", "delete", "do", "double",
	"else", "extends","enum", "export", "false", "final", "finally", "float",
	"for", "function", "goto", "if", "implements", "import", "in",
	"instanceof", "int", "interface", "label", "long", "native", "new",
	"null", "package", "private", "protected", "public", "return", "short",
	"static", "super", "switch", "synchronized", "this", "throw", "throws",
	"transient", "true", "try", "typeof", "var", "void", "volatile",
	"while", "with",

	// methods, classes, etc.

	"alert", "anchor", "anchors", "applet", "applets" "area", "arguments",
	"array", "assign", "blur", "body", "button", "callee", "caller",
	"captureEvents", "checkbox", "clearInterval", "clearTimeout", "close",
	"closed", "confirm", "constructor", "date", "defaultStatus", "document",
	"element", "Error", "escape", "eval", "EvalError", "fileUpload", "find",
	"focus", "form", "forms", "frame", "frames", "function", "getClass",
	"hasOwnProperty", "hidden", "history", "home", "image", "images",
	"infinity", "innerHeight", "innerWidth", "isFinite", "isNaN",
	"isPrototypeOf", "java", "JavaArray", "JavaClass", "JavaObject",
	"JavaPackage", "length", "link", "links", "location", "LocationBar",
	"math", "MenuBar", "MimeType", "mimeTypes", "moveBy", "moveTo", "name",
	"NaN", "navigate", "Navigator", "netscape", "Number", "Object",
	"onblur", "onerror", "onfocus", "onload", "onunload", "open", "opener",
	"Option", "options", "outerHeight", "outerWidth", "Packages",
	"pageXOffset", "pageYOffset", "parent", "parseFloat", "parseInt",
	"password", "personalbar", "plugin", "plugins", "print", "prompt",
	"propertyIsEnum", "prototype", "Radio", "RangeError", "ReferenceError",
	"ref", "RegExp", "releaseEvents", "reset", "resizeBy", "resizeTo",
	"routeEvent", "screen", "script", "scroll", "scrollbars", "scrollBy",
	"scrollTo", "select", "self", "setTimeout", "status", "StatusBar",
	"stop", "String", "submit", "sun", "SyntaxError", "taint", "text",
	"TextArea", "Toolbar", "top", "toString", "TypeError", "undefined",
	"unescape", "untaint", "unwatch", "URIError", "valueOf", "watch",
	"window"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JCharacter*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStringCompleter*
CBJavaScriptCompleter::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = new CBJavaScriptCompleter;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBJavaScriptCompleter::Shutdown()
{
	delete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBJavaScriptCompleter::CBJavaScriptCompleter()
	:
	CBStringCompleter(kCBJavaScriptLang, kKeywordCount, kKeywordList, kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBJavaScriptCompleter::~CBJavaScriptCompleter()
{
	itsSelf = NULL;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (.) to allow completion of fully
	qualified names.

 ******************************************************************************/

JBoolean
CBJavaScriptCompleter::IsWordCharacter
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

/******************************************************************************
 MatchCase (virtual protected)

 ******************************************************************************/

void
CBJavaScriptCompleter::MatchCase
	(
	const JString&	source,
	JString*		target
	)
	const
{
	target->ToLower();
	target->MatchCase(source, JIndexRange(1, source.GetLength()));
}

/******************************************************************************
 GetDefaultWordList (static)

	Must be static because may be called before object is created.

 ******************************************************************************/

JSize
CBJavaScriptCompleter::GetDefaultWordList
	(
	const JCharacter*** list
	)
{
	*list = kKeywordList;
	return kKeywordCount;
}
