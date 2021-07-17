%{
/*
Copyright (C) 2007 by John Lindal.

This scanner reads output from gdb --interpreter=mi2.
*/

#define _H_GDBScannerL

#include "GDBScanner.h"
#include "CMBreakpoint.h"
#include <JRegex.h>
#include <jAssert.h>

#undef YY_DECL
#define YY_DECL GDBScanner::Token GDBScanner::NextToken()

#define RETURN(x)	return Token(x, itsData)

// splitting up square brackets protects them from flex 2.5.33 on OSX
static const JRegex pidRegex1("(?:child|attached) (?:process|Pid|PID|thread) ([""[:digit:]""]+)");
static const JRegex pidRegex2("\\(LWP ([""[:digit:]""]+)");
static const JRegex pidRegex3("process ([""[:digit:]""]+)");
static const JRegex attachRegex1("(?:process|Pid|PID) ([""[:digit:]""]+)");
static const JRegex pingRegex(":Medic debugger ready:([""[:digit:]""]+)");
%}

%option c++ yyclass = "GDBScanner" prefix = "GDBOutput"
%option 8bit nodefault interactive
%option ecs align noyywrap

%x ACQUIRE_PID ATTACHING_TO_PROCESS MEDIC_COMMAND READING_SYMBOLS

WS     ([[:space:]]+)
INT    ([[:digit:]]+)
HEX    (0x[[:xdigit:]]+)
PROMPT "(gdb) "\n?

%%

%{
/************************************************************************/

	if (itsResetFlag)
		{
		BEGIN(INITIAL);
		itsResetFlag = false;
		}

	bzero(&itsData, sizeof(itsData));

%}

{PROMPT} {
	Flush();
	RETURN(kReadyForInput);
	}

(&\""echo "\\\\032\\\\032:"Medic debugger ready":{INT}:\\n\"\n)?~\"\\032\\032:"Medic debugger ready":{INT}:\"\n(\^done\n)? {

	const JString s(yytext, yyleng, JString::kNoCopy);
	const JStringMatch m = pingRegex.Match(s, JRegex::kIncludeSubmatches);
	assert( !m.IsEmpty() );
	const bool ok = m.GetSubstring(1).ConvertToUInt(&itsData.number);
	assert( ok );
	RETURN(kReadyForInput);
	}

<INITIAL,MEDIC_COMMAND>\^exit\n |
~\"\\032\\032:"Medic debugger quit":\"\n {
	RETURN(kDebuggerFinished);
	}

<INITIAL,MEDIC_COMMAND>\^done,reason=\"exited[^"]*\".*\n    |
<INITIAL,MEDIC_COMMAND>\*stopped,reason=\"exited[^"]*\".*\n |
<INITIAL,MEDIC_COMMAND>\*stopped\n                          {

	itsString.Set(yytext + 1, yyleng - 1);
	TranslateMIOutput(&itsString);
	itsData.pString = &itsString;
	RETURN(kProgramFinished);
	}

~\"\\n"Program "(exited|terminated).*\"\n  {

	itsString.Set(yytext + 4, yyleng - 6);
	TranslateMIOutput(&itsString);
	itsData.pString = &itsString;
	RETURN(kProgramFinished);
	}

~\"\\032\\032:"Medic loading file":\"\n {
	BEGIN(READING_SYMBOLS);
	RETURN(kPrepareToLoadSymbols);
	}

~\"\\032\\032:"Medic loading core":\"\n {
	RETURN(kCoreChanged);
	}

~\"\\032\\032:"Medic attaching":\"\n {
	BEGIN(ATTACHING_TO_PROCESS);
	RETURN(kDebugOutput);
	}

~\"\\032\\032:"Medic detaching":\"\n {
	RETURN(kDetachingFromProcess);
	}

~\"\\032\\032:"Medic program starting":\"\n {
	RETURN(kProgramStarting);
	}

~\"\\032\\032:"Medic acquire pid":\"\n {
	BEGIN(ACQUIRE_PID);
	RETURN(kBeginGetPID);
	}

	/* OSX prints pid when program starts */

~\""[Switching to process "{INT}.*\]\\n\"\n {

	const JString s(yytext, yyleng, JString::kNoCopy);
	const JStringMatch m = pidRegex3.Match(s, JRegex::kIncludeSubmatches);
	if (!m.IsEmpty())
		{
		const bool ok = m.GetSubstring(1).ConvertToUInt(&itsData.number);
		assert( ok );
		}
	RETURN(kProgramPID);
	}

~\"\\032\\032:"Medic program running":\"\n {
	RETURN(kProgramRunning);
	}

~\".*" has changed; re-reading symbols".*\\n\"\n {
	RETURN(kSymbolsReloaded);
	}

~\"\\032\\032:"Medic program killed":\"\n {
	RETURN(kProgramKilled);
	}

	/* result of running gdb with --fullname option: first item is full path + name */
	/* on OSX, this only works when switch frames */

~\"\\032\\032[^:\0]+:{INT}:{INT}:[^:]+:{HEX}\\n\"\n {

	JIndexRange r(11,11);
	while (yytext[r.last-1] != ':')
		{
		r.last++;
		}
	const_cast<JUtf8Byte*>(yytext)[r.last-1] = '\0';
	itsLocation.SetFileName(JString(yytext+r.first-1, JString::kNoCopy));

	r.last++;
	r.first = r.last;
	while (yytext[r.last-1] != ':')
		{
		r.last++;
		}
	const_cast<JUtf8Byte*>(yytext)[r.last-1] = '\0';
	JIndex lineNumber;
	const bool ok = JString::ConvertToUInt(yytext+r.first-1, &lineNumber);
	assert( ok );
	itsLocation.SetLineNumber(lineNumber);

	itsData.pLocation = &itsLocation;
	RETURN(kProgramStoppedAtLocation);
	}

	/* OSX result of hitting a breakpoint */

<INITIAL,MEDIC_COMMAND>\^done,reason=.+\n {

	itsString.Set(yytext + 1, yyleng - 1);
	TranslateMIOutput(&itsString);
	itsData.pString = &itsString;
	RETURN(kProgramStoppedAtLocation);
	}

~\"\\032\\032:"Medic breakpoints changed":\"\n {
	RETURN(kBreakpointsChanged);
	}

	/* frame changed to function that doesn't have a source file */

~\"\\032\\032:"Medic frame changed":\"\n/("^done,frame="|~.+\n^done\n) {
	RETURN(kFrameChangedAndProgramStoppedAtLocation);
	}

~\"\\032\\032:"Medic frame changed":\"\n {
	RETURN(kFrameChanged);
	}

~\"\\032\\032:"Medic thread changed":\"\n {
	RETURN(kThreadChanged);
	}

~\"\\032\\032:"Medic value changed":\"\n {
	RETURN(kValueChanged);
	}

~\"\\032\\032:"Medic defining script":\"\n {
	RETURN(kBeginScriptDefinition);
	}

~\"\\032\\032:"Medic command":{INT}:\"\n {
	ExtractCommandId(yytext, yyleng);
	BEGIN(MEDIC_COMMAND);
	RETURN(kBeginMedicCmd);
	}

~\"\\032\\032:"Medic ignore command":{INT}:\"\n {
	ExtractCommandId(yytext, yyleng);
	RETURN(kBeginMedicIgnoreCmd);
	}

~\"\\032\\032:"Medic ignore command done":{INT}:\"\n {
	ExtractCommandId(yytext, yyleng);
	RETURN(kEndMedicIgnoreCmd);
	}

	/* hook set by plug-in */

~\"\\032\\034:.+:\"\n {

	itsString.Set(yytext + 11, yyleng - 14);
	TranslateMIOutput(&itsString);
	itsData.pString = &itsString;
	RETURN(kPlugInMessage);
	}

	/* user output */

[~@]\".*\"\n {

	itsString.Set(yytext + 2, yyleng - 4);
	TranslateMIOutput(&itsString);
	itsData.pString = &itsString;
	RETURN(kProgramOutput);
	}

	/* mi results */

\^error,msg=\""Cannot execute this command while the target is running."\"\n |
\^error,msg=\""Cannot execute command ".+" while target running"\"\n {
	RETURN(kDebugOutput);
	}

\^error,msg=\".*\"\n {

	itsString.Set(yytext + 12, yyleng - 14);
	TranslateMIOutput(&itsString);
	itsData.pString = &itsString;
	RETURN(kErrorOutput);
	}

\^running\n\*running.*\n{PROMPT} |
[*+=^].*\n                       {
	RETURN(kDebugOutput);
	}

	/* gdb debug output */

&\".*\"\n {
	RETURN(kDebugOutput);
	}

	/* optimize to slurp as quickly as possible */

[^(^~@&*+=].*\n |
[^(^~@&*+=\n]+  |
[(^~@&*+=\n]    {

	itsString.Set(yytext, yyleng);
	itsData.pString = &itsString;
	RETURN(kProgramOutput);
	}

<*><<EOF>> {
	RETURN(kEOF);
	}



	/* Debugger is reading symbols from program */



<READING_SYMBOLS>{

~\"done\.\\n\"\n {

	itsString       = "done.\n";
	itsData.pString = &itsString;
	BEGIN(INITIAL);
	RETURN(kFinishedLoadingSymbolsFromProgram);
	}

([~@]\".*\")?\^done\n {
	BEGIN(INITIAL);
	RETURN(kFinishedLoadingSymbolsFromProgram);
	}

&\".*"No such file or directory".*\"\n |
~\""(No debugging symbols found in".*\"\n {

	itsData.pString = nullptr;
	BEGIN(INITIAL);
	RETURN(kNoSymbolsInProgram);
	}

\^error,msg=\".*\"\n {

	itsString.Set(yytext + 12, yyleng - 14);
	TranslateMIOutput(&itsString);
	itsData.pString = &itsString;
	BEGIN(INITIAL);
	RETURN(kNoSymbolsInProgram);
	}

[~@]\".*\"\n {

	itsString.Set(yytext + 2, yyleng - 4);
	TranslateMIOutput(&itsString);
	itsData.pString = &itsString;
	RETURN(kProgramOutput);
	}

.*\n? {
	RETURN(kDebugOutput);
	}

}



	/* Medic asking debugger for PID of running program */



<ACQUIRE_PID>{

~\".*(child|attached)\ (process|Pid|PID|thread)\ {INT}.*\\n\"\n |
~\".*"(LWP "{INT}.*\\n\"\n                            {

	const JString s(yytext, yyleng, JString::kNoCopy);
	const JStringMatch m1 = pidRegex1.Match(s, JRegex::kIncludeSubmatches),
					   m2 = pidRegex2.Match(s, JRegex::kIncludeSubmatches);
	if (!m1.IsEmpty())
		{
		const bool ok = m1.GetSubstring(1).ConvertToUInt(&itsData.number);
		assert( ok );
		}
	else if (!m2.IsEmpty())
		{
		const bool ok = m2.GetSubstring(1).ConvertToUInt(&itsData.number);
		assert( ok );
		}
	RETURN(kProgramPID);
	}

~\"\\032\\032:"Medic acquire pid end":\"\n {
	BEGIN(INITIAL);
	RETURN(kEndGetPID);
	}

.*\n? {
	RETURN(kDebugOutput);
	}

}



	/* Debugger attaching to process */



<ATTACHING_TO_PROCESS>{

~\".*(process|Pid|PID)\ {INT}.*\\n\"\n {

	const JString s(yytext, yyleng, JString::kNoCopy);
	const JStringMatch m = attachRegex1.Match(s, JRegex::kIncludeSubmatches);
	if (!m.IsEmpty())
		{
		const bool ok = m.GetSubstring(1).ConvertToUInt(&itsData.number);
		assert( ok );
		}
	BEGIN(READING_SYMBOLS);
	RETURN(kAttachedToProcess);
	}

.*\n? {
	RETURN(kDebugOutput);
	}

}



	/* Medic running CMCommand */



<MEDIC_COMMAND>{

~\"\\032\\032:"Medic command done":{INT}:\"\n {
	ExtractCommandId(yytext, yyleng);
	BEGIN(INITIAL);
	RETURN(kEndMedicCmd);
	}

~\".*\"\n {

	itsString.Set(yytext + 2, yyleng - 4);
	TranslateMIOutput(&itsString);
	itsData.pString = &itsString;
	RETURN(kCommandOutput);
	}

\^.*\n {

	itsString.Set(yytext + 1, yyleng - 2);
	itsData.pString = &itsString;
	RETURN(kCommandResult);
	}

@\".*\"\n {

	itsString.Set(yytext + 2, yyleng - 4);
	TranslateMIOutput(&itsString);
	itsData.pString = &itsString;
	RETURN(kProgramOutput);
	}

.*\n? {
	RETURN(kDebugOutput);
	}

}

%%
