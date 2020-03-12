/*
*   $Id: php.c 557 2007-06-15 17:20:04Z elliotth $
*
*   Copyright (c) 2000, Jesus Castagnetto <jmcastagnetto@zkey.com>
*
*   This source code is released for free distribution under the terms of the
*   GNU General Public License.
*
*   This module contains functions for generating tags for the PHP web page
*   scripting language. Only recognizes functions and classes, not methods or
*   variables.
*
*   Parsing PHP defines by Pavel Hlousek <pavel.hlousek@seznam.cz>, Apr 2003.
*/

/*
*   INCLUDE FILES
*/
#include "general.h"  /* must always come first */

#include <string.h>
#include <regex.h>

#include "parse.h"
#include "read.h"
#include "vstring.h"

/*
*   DATA DEFINITIONS
*/
#if 0
typedef enum {
	K_CLASS, K_DEFINE, K_FUNCTION, K_VARIABLE
} phpKind;

static kindOption PhpKinds [] = {
	{ TRUE, 'c', "class",    "classes" },
	{ TRUE, 'd', "define",   "constant definitions" },
	{ TRUE, 'f', "function", "functions" },
	{ TRUE, 'v', "variable", "variables" }
};
#endif

/*
*   FUNCTION DEFINITIONS
*/

/* JavaScript patterns are duplicated in jscript.c */

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

static kindOption CallbackKinds [] = {
	{ TRUE, 'j', "jsfunction", "javascript function" }
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

static void installPHPRegex (const langType language)
{
	addTagRegex(language, "^[ \t]*((final|abstract)[ \t]+)*class[ \t]+(" IDENT ")",
		"\\3", "c,class,classes", NULL);
	addTagRegex(language, "^[ \t]*interface[ \t]+(" IDENT ")",
		"\\1", "i,interface,interfaces", NULL);
	addTagRegex(language, "^[ \t]*define[ \t]*\\([ \t]*['\"]?(" IDENT ")",
		"\\1", "d,define,constant definitions", NULL);
	addTagRegex(language, "^[ \t]*((abstract|static|public|protected|private)[ \t]+)*function[ \t]+&?[ \t]*(" IDENT ")",
		"\\3", "f,function,functions", NULL);
	addTagRegex(language, "^[ \t]*(\\$|::\\$|\\$this->)(" IDENT ")[ \t]*=",
		"\\2", "v,variable,variables", NULL);
	addTagRegex(language, "^[ \t]*((var|public|protected|private|static)[ \t]+)+\\$(" IDENT ")[ \t]*[=;]",
		"\\3", "v,variable,variables", NULL);

	/* javascript - plain function is covered by php */

	addTagRegex (language, "^[ \t]*async[ \t]+function[ \t]+(" IDENT ")[ \t]*\\(",
		"\\1", "j,jsfunction,javascript functions", NULL);
	addTagRegex (language, "^[ \t]*var[ \t]*(" IDENT ")[ \t]*=[ \t]*(?:async[ \t]+)?function([ \t]+" IDENT ")?[ \t]*\\(",
		"\\1", "j,jsfunction,javascript functions", NULL);
	addTagRegex (language, "^[ \t]*(" IDENT ")[ \t]*[=:][ \t]*(?:async[ \t]+)?function([ \t]+" IDENT ")?[ \t]*\\(",
		"\\1", "j,jsfunction,javascript functions", NULL);
	addTagRegex (language, "^[ \t]*([A-Za-z0-9_.\x7f-\xff]+)\\.(" IDENT ")[ \t]*=[ \t]*(?:async[ \t]+)?function([ \t]+" IDENT ")?[ \t]*\\(",
		"\\1.\\2", "j,jsfunction,javascript functions", NULL);
	addTagRegex (language, "^[ \t]*([A-Za-z0-9_.\x7f-\xff]+)\\.(" IDENT ")[ \t]*=[ \t]*(?:async[ \t]+)?function([ \t]+" IDENT ")?[ \t]*\\(",
		"\\2", "j,jsfunction,javascript functions", NULL);

	addCallbackRegex (language, "^[ \t]*(?:async[ \t]+)?(" IDENT ")[ \t]*\\((?:" IDENT "[ \t]*,[ \t]*)*" IDENT "\\)(?:[ \t]*\\{)?$",
		NULL, es6Function);

	regcomp(&keywords_pattern,
			"^(abstract|arguments|async|await|boolean|break|byte|case|catch|char|class|const|constructor|continue|debugger|default|delete|do|double|else|enum|eval|export|extends|false|final|finally|float|for|function|goto|if|implements|import|in|instanceof|int|interface|let|long|native|new|null|of|package|private|protected|public|return|short|static|super|switch|synchronized|this|throw|throws|transient|true|try|typeof|var|void|volatile|while|with|yield)$",
			REG_EXTENDED | REG_NOSUB);
}

/* Create parser definition structure */
extern parserDefinition* PhpParser (void)
{
	static const char *const extensions [] = { "php", "php3", "phtml", NULL };
	parserDefinition* def = parserNew ("PHP");
	def->extensions = extensions;
	def->initialize = installPHPRegex;
	def->regex      = TRUE;
	return def;
}

#if 0

static boolean isLetter(const int c)
{
	return (boolean)(isalpha(c) || (c >= 127  &&  c <= 255));
}

static boolean isVarChar1(const int c)
{
	return (boolean)(isLetter (c)  ||  c == '_');
}

static boolean isVarChar(const int c)
{
	return (boolean)(isVarChar1 (c) || isdigit (c));
}

static void findPhpTags (void)
{
	vString *name = vStringNew ();
	const unsigned char *line;

	while ((line = fileReadLine ()) != NULL)
	{
		const unsigned char *cp = line;
		const char* f;

		while (isspace (*cp))
			cp++;

		if (*(const char*)cp == '$'  &&  isVarChar1 (*(const char*)(cp+1)))
		{
			cp += 1;
			vStringClear (name);
			while (isVarChar ((int) *cp))
			{
				vStringPut (name, (int) *cp);
				++cp;
			}
			while (isspace ((int) *cp))
				++cp;
			if (*(const char*) cp == '=')
			{
				vStringTerminate (name);
				makeSimpleTag (name, PhpKinds, K_VARIABLE);
				vStringClear (name);
			}
		}
		else if ((f = strstr ((const char*) cp, "function")) != NULL &&
			(f == (const char*) cp || isspace ((int) f [-1])) &&
			isspace ((int) f [8]))
		{
			cp = ((const unsigned char *) f) + 8;

			while (isspace ((int) *cp))
				++cp;

			if (*cp == '&')	/* skip reference character and following whitespace */
			{
				cp++;

				while (isspace ((int) *cp))
					++cp; 
			}

			vStringClear (name);
			while (isalnum ((int) *cp)  ||  *cp == '_')
			{
				vStringPut (name, (int) *cp);
				++cp;
			}
			vStringTerminate (name);
			makeSimpleTag (name, PhpKinds, K_FUNCTION);
			vStringClear (name);
		} 
		else if (strncmp ((const char*) cp, "class", (size_t) 5) == 0 &&
				 isspace ((int) cp [5]))
		{
			cp += 5;

			while (isspace ((int) *cp))
				++cp;
			vStringClear (name);
			while (isalnum ((int) *cp)  ||  *cp == '_')
			{
				vStringPut (name, (int) *cp);
				++cp;
			}
			vStringTerminate (name);
			makeSimpleTag (name, PhpKinds, K_CLASS);
			vStringClear (name);
		}
		else if (strncmp ((const char*) cp, "define", (size_t) 6) == 0 &&
				 ! isalnum ((int) cp [6]))
		{
			cp += 6;

			while (isspace ((int) *cp))
				++cp;
			if (*cp != '(')
				continue;
			++cp;

			while (isspace ((int) *cp))
				++cp;
			if ((*cp == '\'') || (*cp == '"'))
				++cp;
			else if (! ((*cp == '_')  || isalnum ((int) *cp)))
				continue;

			vStringClear (name);
			while (isalnum ((int) *cp)  ||  *cp == '_')
			{
				vStringPut (name, (int) *cp);
				++cp;
			}
			vStringTerminate (name);
			makeSimpleTag (name, PhpKinds, K_DEFINE);
			vStringClear (name);
		}
	}
	vStringDelete (name);
}

extern parserDefinition* PhpParser (void)
{
	static const char *const extensions [] = { "php", "php3", "phtml", NULL };
	parserDefinition* def = parserNew ("PHP");
	def->kinds      = PhpKinds;
	def->kindCount  = KIND_COUNT (PhpKinds);
	def->extensions = extensions;
	def->parser     = findPhpTags;
	return def;
}

#endif

/* vi:set tabstop=4 shiftwidth=4: */
