/******************************************************************************
 cbCtagsRegex.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_cbCtagsRegex
#define _H_cbCtagsRegex

#define CBCtagsBisonDef " --langdef=jbison "
#define CBCtagsBisonNonterminalDef \
	" \"--regex-jbison=/^[ \t]*([[:alpha:]_.][[:alnum:]_.]*)[ \t\n]*:/" \
	"\\\\1/N,nontermdef/ei\" "
#define CBCtagsBisonNonterminalDecl \
	" \"--regex-jbison=/^[ \t]*%type([ \t\n]*<[^>]*>)?[ \t\n]*" \
	"([[:alpha:]_.][[:alnum:]_.]*)/\\\\2/n,nontermdecl/ei\" "
#define CBCtagsBisonTerminalDecl \
	" \"--regex-jbison=/^[ \t]*%(token|right|left|nonassoc)([ \t\n]*<[^>]*>)?[ \t\n]*" \
	"([[:alpha:]_.][[:alnum:]_.]*)/\\\\3/t,termdecl/ei\" "

#define CBCtagsHTMLDef " --langdef=jhtml "
#define CBCtagsHTMLID \
	" \"--regex-jhtml=/<[^>]+(id|name)=([^ >\\\"']+)/\\\\2/i,id/ei\" " \
	" \"--regex-jhtml=/<[^>]+(id|name)=\\\"([^\\\"']+)\\\"/\\\\2/i,id/ei\" " \
	" \"--regex-jhtml=/<[^>]+(id|name)='([^\\\"']+)'/\\\\2/i,id/ei\" "

#define CBCtagsLexDef " --langdef=jlex "
#define CBCtagsLexState \
	" \"--regex-jlex=/^<([^<>]+)>\\\\{/\\\\1/s,state/ei\" "

#define CBCtagsMakeDef " --langdef=jmake "
#define CBCtagsMakeTarget \
	" \"--regex-jmake=/^ *([A-Z0-9_]+)[ \t]*:([^=]|$)/\\\\1/t,target/ei\" "
#define CBCtagsMakeVariable \
	" \"--regex-jmake=/^[ \t]*([A-Z0-9_]+)[ \t]*:?=/\\\\1/v,variable/ei\" "

#endif
