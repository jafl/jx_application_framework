/*
*   $Id: jscript.c,v 1.5 2006/06/25 23:38:23 jafl Exp $
*
*   Copyright (c) 2003, Darren Hiebert
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains functions for generating tags for JavaScript language
*   files.
*/

/*
*   INCLUDE FILES
*/
#include "general.h"  /* must always come first */
#include "parse.h"
#include <regex.h>

/*
*   FUNCTION DEFINITIONS
*/

/*
 * Cygwin doesn't support non-ASCII characters in character classes.
 * This isn't a good solution to the underlying problem, because we're still
 * making assumptions about the character encoding.
 * Really, these regular expressions need to concentrate on what marks the
 * end of an identifier, and we need something like iconv to take into
 * account the user's locale (or an override on the command-line.)
 */
#ifdef __CYGWIN__
#define ALPHA "[:alpha:]"
#define ALNUM "[:alnum:]"
#else
#define ALPHA "A-Za-z\x7f-\xff"
#define ALNUM "0-9A-Za-z\x7f-\xff"
#endif
#define IDENT "[" ALPHA "_][" ALNUM "_]*"
#define ARG   IDENT "(?:[ \t]*=[^,)]+)?"

static kindOption CallbackKinds [] = {
	{ TRUE, 'f', "function", "function" }
};

static regex_t keywords_pattern;

static void es6Function(const char *line, const regexMatch *matches, unsigned int count)
{
	vString *const name = vStringNew ();
	vStringNCopyS (name, line + matches[1].start, matches[1].length);

	if (regexec(&keywords_pattern, name->buffer, 0, NULL, 0) == REG_NOMATCH)
	{
		makeSimpleTag(name, CallbackKinds, 0);
	}
	else
	{
		vStringDelete(name);
	}
}

/* these patterns are duplicated in php.c */

static void installJavaScriptRegex (const langType language)
{
	addTagRegex (language, "^[ \t]*(?:async[ \t]+)?function[ \t]+(" IDENT ")[ \t]*\\(",
		"\\1", "f,function,functions", NULL);
	addTagRegex (language, "^[ \t]*var[ \t]*(" IDENT ")[ \t]*=[ \t]*(?:async[ \t]+)?function([ \t]+" IDENT ")?[ \t]*\\(",
		"\\1", "f,function,functions", NULL);
	addTagRegex (language, "^[ \t]*(" IDENT ")[ \t]*[=:][ \t]*(?:async[ \t]+)?function([ \t]+" IDENT ")?[ \t]*\\(",
		"\\1", "f,function,functions", NULL);
	addTagRegex (language, "^[ \t]*([A-Za-z0-9_.\x7f-\xff]+)\\.(" IDENT ")[ \t]*=[ \t]*(?:async[ \t]+)?function([ \t]+" IDENT ")?[ \t]*\\(",
		"\\1.\\2", "f,function,functions", NULL);
	addTagRegex (language, "^[ \t]*([A-Za-z0-9_.\x7f-\xff]+)\\.(" IDENT ")[ \t]*=[ \t]*(?:async[ \t]+)?function([ \t]+" IDENT ")?[ \t]*\\(",
		"\\2", "f,function,functions", NULL);

	addTagRegex (language, "^[ \t]*class[ \t]+(" IDENT ")(?:[ \t]*\\{)?$",
		"\\1", "c,class,classes", NULL);
	addCallbackRegex (language, "^[ \t]*(?:async[ \t]+)?(" IDENT ")[ \t]*\\((?:(?:" ARG "[ \t]*,[ \t]*)*" ARG ")?\\)(?:[ \t]*\\{)?$",
		NULL, es6Function);

	regcomp(&keywords_pattern,
			"^(abstract|arguments|async|await|boolean|break|byte|case|catch|char|class|const|constructor|continue|debugger|default|delete|do|double|else|enum|eval|export|extends|final|finally|float|for|function|goto|if|implements|import|in|instanceof|int|interface|let|long|native|new|of|package|private|protected|public|return|short|static|super|switch|synchronized|this|throw|throws|transient|try|typeof|var|void|volatile|while|with|yield)$",
			REG_EXTENDED | REG_NOSUB);
}

/* Create parser definition stucture */
extern parserDefinition* JavaScriptParser (void)
{
	static const char *const extensions [] = { "js", NULL };
	parserDefinition *const def = parserNew ("JavaScript");
	def->extensions = extensions;
	def->initialize = installJavaScriptRegex;
	def->regex      = TRUE;
	return def;
}

/* vi:set tabstop=4 shiftwidth=4: */
