/******************************************************************************
 CBTCLCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#include "CBTCLCompleter.h"
#include <jAssert.h>

CBTCLCompleter* CBTCLCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	// TCL

	"after", "append", "array", "auto_execok", "auto_import", "auto_load",
	"auto_mkindex", "auto_qualify", "auto_reset",
	"bgerror", "binary", "break",
	"catch", "cd", "clock", "close", "concat", "continue",
	"dde",
	"encoding", "env", "eof", "error", "errorCode", "errorInfo", "eval",
	"exec", "exit", "expr",
	"fblocked", "fconfigure", "fcopy", "file", "fileevent", "filename",
	"flush", "for", "foreach", "format",
	"gets", "glob", "global",
	"history",
	"if", "incr", "info", "interp",
	"join",
	"lappend", "lindex", "linsert", "list", "llength", "load", "lrange",
	"lreplace", "lsearch", "lsort",
	"memory", "msgcat",
	"namespace",
	"open",
	"package", "parray", "pid", "::pkg::create", "pkg_mkindex", "proc",
	"puts", "pwd",
	"re_syntax", "read", "regexp", "registry", "regsub", "rename",
	"resource", "return",
	"scan", "seek", "set", "socket", "source", "split", "string", "subst",
	"switch",
	"tcl_endOfWord", "tcl_findLibrary", "tcl_startOfNextWord",
	"tcl_startOfPreviousWord", "tcl_wordBreakAfter", "tcl_wordBreakBefore",
	"tcl_library", "tcl_patchLevel", "tcl_pkgPath", "tcl_platform",
	"tcl_precision", "tcl_rcFileName", "tcl_rcRsrcName", "tcl_traceCompile",
	"tcl_traceExec", "tcl_wordchars", "tcl_nonwordchars", "tcl_version",
	"tell", "time", "trace",
	"unknown", "unset", "update", "uplevel", "upvar",
	"variable", "vwait",
	"while",

	"::http::config", "::http::geturl", "::http::formatQuery", "::http::reset",
	"::http::wait", "::http::status", "::http::size", "::http::code",
	"::http::ncode", "::http::data", "::http::error", "::http::cleanup",
	"::http::register", "::http::unregister",

	"::safe::interpCreate", "::safe::interpInit", "::safe::interpConfigure",
	"::safe::interpDelete", "::safe::interpAddToAccessPath",
	"::safe::interpFindInAccessPath", "::safe::setLogCmd",
	"source", "load", "file", "encoding", "exit",

	"::tcltest::test", "::tcltest::cleanupTests", "::tcltest::getMatchingTestFiles",
	"::tcltest::loadTestedCommands", "::tcltest::makeFile", "::tcltest::removeFile",
	"::tcltest::makeDirectory", "::tcltest::removeDirectory", "::tcltest::viewFile",
	"::tcltest::normalizeMsg", "::tcltest::bytestring", "::tcltest::saveState",
	"::tcltest::restoreState", "::tcltest::threadReap",
	"::tcltest::outputChannel", "::tcltest::errorChannel", "::tcltest::mainThread",
	"::tcltest::originalEnv", "::tcltest::workingDirectory",
	"::tcltest::temporaryDirectory", "::tcltest::testsDirectory",
	"::tcltest::tcltest", "::tcltest::loadScript",
	"::tcltest::PrintUsageInfoHook", "::tcltest::processCmdLineArgsFlagHook",
	"::tcltest::processCmdLineArgsHook", "::tcltest::initConstraintsHook",
	"::tcltest::cleanupTestsHook",

	// Tk

	"bell", "bind", "bindtags", "bitmap", "button",
	"canvas", "checkbutton", "clipboard", "colors", "cursors",
	"destroy",
	"entry", "event",
	"focus", "font", "frame",
	"grab", "grid",
	"image",
	"keysyms",
	"label", "listbox", "loadTk", "lower",
	"menu", "menubutton", "message",
	"option", "options",
	"pack", "photo", "place",
	"radiobutton", "raise",
	"scale", "scrollbar", "selection", "send",
	"text", "tk", "tk_bisque", "tk_chooseColor", "tk_chooseDirectory",
	"tk_dialog", "tk_focusFollowsMouse", "tk_focusNext", "tk_focusPrev",
	"tk_getOpenFile", "tk_getSaveFile", "tk_messageBox", "tk_optionMenu",
	"tk_popup", "tk_setPalette", "tkerror", "tk_library", "tk_patchLevel",
	"tkPriv", "tk_strictMotif", "tk_version", "tkwait", "toplevel",
	"winfo", "wm"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

CBStringCompleter*
CBTCLCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
		{
		recursiveInstance = true;

		itsSelf = jnew CBTCLCompleter;
		assert( itsSelf != nullptr );

		recursiveInstance = false;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBTCLCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBTCLCompleter::CBTCLCompleter()
	:
	CBStringCompleter(kCBTCLLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBTCLCompleter::~CBTCLCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (::) to allow completion of fully
	qualified names.

 ******************************************************************************/

bool
CBTCLCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_' || (includeNS && c == ':');
}
