/******************************************************************************
 GDBSourceWindowHelpText.cc

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include "GDBHelpText.h"

const JCharacter* kSourceWindowHelpName  = "GDBSourceWindowHelp";
const JCharacter* kSourceWindowHelpTitle = "Source Window";

const JCharacter* kSourceMainHelpName		= "GDBSourceWindowHelp#Main";
const JCharacter* kSourceStandardHelpName	= "GDBSourceWindowHelp#Standard";
const JCharacter* kSourceBreakpointHelpName	= "GDBSourceWindowHelp#Breakpoints";

const JCharacter* kSourceWindowHelpText =

"<html>"
"<h3>Source Window</h3>"

"The source window is used for two purposes - the source "
"currently being debugged is displayed in the <a href=jxhelp:#Main>"
"main source window</a>, and arbitrary project source is displayed "
"in a <a href=jxhelp:#Standard>standard source window</a>."

"<p>"

"In addition to a <a href=jxhelp:GDBMenuHelp>menu</a>, both source windows "
"contain a <a href=jxhelp:#Breakpoints>breakpoint</a> list and a view of the "
"text of the source. Only the <a href=jxhelp:#Main>main source window</a> "
"contains a <a href=jxhelp:GDBMiscHelpName#Buttons>button bar</a>."

"<p>"

"<a name=Breakpoints>"

"<h3> Setting Breakpoints </h3>"

"Setting and unsetting breakpoints can be done in either source window. "
"To set a breakpoint, click in the area containing the line number of "
"the source where you want the program to break. If there is already a "
"breakpoint at that location, it will be marked by a red circle. Clicking "
"on a line containing a breakpoint will unset the breakpoint at that "
"location. Breakpoints can also be set and unset in the "
"<a href=jxhelp:GDBCommandDirHelp#Input>command input</a> field."


"<p>"

"<a name=Main>"

"<h3> Main Source Window </h3>"


"The main source window contains the code that is currently being debugged. "
"An arrow beside the source line number indicates the currently running "
"line. On startup, the function <i>main()</i> should be displayed "
"in this window."

"<p>"

"Any text that is typed into this window that is not a shortcut for the "
"<a href=jxhelp:GDBMiscHelpName#Buttons>button bar</a> is passed along to the"
"<a href=jxhelp:GDBCommandDirHelp#Input>command input</a> field."


"<p>"

"<a name=Standard>"

"<h3> Standard Source Window </h3>"

"The standard source window displays any source that was accessed by "
"selecting <i>Open file...</i> from the <a href=jxhelp:GDBMenuHelp#File>"
"file</a> menu. Source that is contained in the project being debugged "
"can also be opened by double-clicking on the appropriate line in the "
"<a href=jxhelp:GDBFileListHelp>File list window</a>."
;