/******************************************************************************
 CBSQLCompleter.cpp

	BASE CLASS = CBStringCompleter

	Copyright © 2002 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "CBSQLCompleter.h"
#include <ctype.h>
#include <jAssert.h>

CBSQLCompleter* CBSQLCompleter::itsSelf = NULL;

static const JCharacter* kKeywordList[] =
{
	// reserved words

	"abort", "accept", "access", "add", "all", "alter", "and", "any", "array",
	"arraylen", "as", "asc", "assert", "assign", "at", "audit", "authorization",
	"avg", "base_table", "begin", "between", "binary_integer", "body", "boolean",
	"by", "case", "char", "char_base", "check", "close", "cluster", "colauth",
	"column", "comment", "commit", "compress", "connect", "constant", "crash",
	"create", "current", "currval", "cursor", "database", "data_base", "date",
	"dba", "debugoff", "debugon", "declare", "decimal", "default", "definition",
	"delay", "delete", "delta", "desc", "digits", "dispose", "distinct", "do",
	"drop", "else", "elsif", "end", "entry", "exception", "exception_init",
	"exclusive", "exists", "exit", "false", "fetch", "file", "float", "for",
	"form", "from", "function", "generic", "goto", "grant", "group", "having",
	"identified", "if", "immediate", "in", "increment", "index", "indexes",
	"indicator", "initial", "insert", "integer", "interface", "intersect",
	"into", "is", "level", "like", "limited", "lock", "long", "loop", "max",
	"maxextents", "min", "minus", "mlslabel", "mod", "mode", "modify",
	"natural", "naturaln", "new", "nextval", "noaudit", "nocompress", "not",
	"nowait", "null", "number", "number_base", "of", "offline", "on", "online",
	"open", "option", "or", "order", "others", "out", "package", "partition",
	"pctfree", "pls_integer", "positive", "positiven", "pragma", "prior",
	"private", "privileges", "procedure", "public", "raise", "range", "raw",
	"real", "record", "ref", "release", "remr", "rename", "resource",
	"return", "reverse", "revoke", "rollback", "row", "rowid", "rowlabel",
	"rownum", "rows", "rowtype", "run", "savepoint", "schema", "select",
	"separate", "session", "set", "share", "size", "smallint", "space",
	"sql", "sqlcode", "sqlerrm", "start", "statement", "stddev", "subtype",
	"successful", "sum", "synonym", "sysdate", "tabauth", "table", "tables",
	"task", "terminate", "then", "to", "trigger", "true", "type", "uid",
	"union", "unique", "update", "use", "user", "validate", "values",
	"varchar", "varchar2", "variance", "view", "views", "when", "whenever",
	"where", "while", "with", "work", "write", "xor",

	// built-in functions

	"abs", "acos", "asin", "atan", "atan2", "ceil", "cos", "cosh", "exp",
	"floor", "ln", "log", "power", "round", "sign", "sin", "sinh", "sqrt",
	"tan", "tanh", "trunc", "ascii", "chr", "concat", "initcap", "instr",
	"instrb", "length", "lengthb", "lower", "lpad", "ltrim", "nls_initcap",
	"nls_lower", "nls_upper", "nls_sort", "replace", "rpad", "rtrim",
	"soundex", "substr", "translate", "upper", "chartorowid", "convert",
	"hextoraw", "rawtohex", "rowidtochar", "to_char", "to_date", "to_label",
	"to_multi_byte", "to_number", "to_single_byte", "add_months", "last_day",
	"months_between", "new_time", "next_day", "decode", "dump", "greatest",
	"greatest_lb", "least", "least_lb", "nvl", "userenv", "vsize"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JCharacter*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static JBoolean recursiveInstance = kJFalse;

CBStringCompleter*
CBSQLCompleter::Instance()
{
	if (itsSelf == NULL && !recursiveInstance)
		{
		recursiveInstance = kJTrue;

		itsSelf = new CBSQLCompleter;
		assert( itsSelf != NULL );

		recursiveInstance = kJFalse;
		}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CBSQLCompleter::Shutdown()
{
	delete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBSQLCompleter::CBSQLCompleter()
	:
	CBStringCompleter(kCBSQLLang, kKeywordCount, kKeywordList, kJTrue)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBSQLCompleter::~CBSQLCompleter()
{
	itsSelf = NULL;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

 ******************************************************************************/

JBoolean
CBSQLCompleter::IsWordCharacter
	(
	const JString&	s,
	const JIndex	index,
	const JBoolean	includeNS
	)
	const
{
	const JCharacter c = s.GetCharacter(index);
	return JI2B(isalnum(c) || c == '_' || c == '$');
}
