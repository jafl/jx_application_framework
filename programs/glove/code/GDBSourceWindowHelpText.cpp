/******************************************************************************
 GDBSourceWindowHelpText.cpp

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include "GDBHelpText.h"

const JCharacter* kSourceWindowHelpName  = "GDBSourceWindowHelp";
const JCharacter* kSourceWindowHelpTitle = "Source Window";

const JCharacter* kSourceMainHelpName		= "GDBSourceWindowHelp#Main";
const JCharacter* kSourceStandardHelpName	= "GDBSourceWindowHelp#Standard";
const JCharacter* kSourceBreakpointHelpName	= "GDBSourceWindowHelp#Breakpoints";
const JCharacter* kSourceButtonsHelpName	= "GDBSourceWindowHelp#Buttons";
const JCharacter* kSourceMenuHelpName		= "GDBSourceWindowHelp#Menu";
const JCharacter* kSourceFileHelpName		= "GDBSourceWindowHelp#File";

const JCharacter* kSourceWindowHelpText =

"<html>"
"<h3>Source Window</h3>"

"The source window is used for two purposes - the source "
"currently being debugged is displayed in the <a href=jxhelp:#Main>"
"main source window</a>, and arbitrary project source is displayed "
"in a <a href=jxhelp:#Standard>standard source window</a>."

"<p>"

"In addition to a <a href=jxhelp:#Menu>menu</a>, both source windows "
"contain a <a href=jxhelp:#Breakpoints>breakpoint</a> list and a view of the "
"text of the source. Only the <a href=jxhelp:#Main>main source window</a> "
"contains a <a href=jxhelp:#Buttons>button bar</a>."

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
"<a href=jxhelp:#Buttons>button bar</a> is passed along to the "
"<a href=jxhelp:GDBCommandDirHelp#Input>command input</a> field."


"<p>"

"<a name=Standard>"

"<h3> Standard Source Window </h3>"

"The standard source window displays any source that was accessed by "
"selecting <i>Open file...</i> from the <a href=jxhelp:#File>"
"File</a> menu. Source that is contained in the project being debugged "
"can also be opened by double-clicking on the appropriate line in the "
"<a href=jxhelp:GDBFileListHelp>File list window</a>."

"<a name=Buttons>"

"<h3> Button Bar </h3>"

"The button bar allows you to send the specified commands to gdb. Currently "
"the available buttons are <i>Run</i>, <i>Stop</i>, <i>Kill</i>, <i>Next</i>, "
"<i>Step</i>, <i>Finish</i>, and <i>Continue</i>. Future versions of Code "
"Medic will allow you to customize the button bar. The button bar in the "
"<a href=jxhelp:GDBCommandDirHelp>command window</a> is the same as that "
"in the main source window."

"<a name=Menu>"

"<h3> Menu </h3>"

"<a name=File>"

"<b> File </b>"

"<dl>"

" <dt>Open file..."
"	<dd>Opens a source file that is displayed in a standard source window. "

"<p>"
" <dt>Find current line"
"	<dd>Scrolls the window to the line containing the currently running line. "

"<p>"
" <dt>Edit file"
"	<dd>Opens the current source in the editor specified in the edit preferences. "

"<p>"
" <dt>Restart gdb"
"	<dd>Restarts the underlying gdb process, but retains the current program, "
"	along with its <a href=jxhelp:#Breakpoints>breakpoints</a> "
"	and <a href=jxhelp:GDBVarTreeHelp>variables</a>. "

"<p>"
" <dt>Change gdb binary"
"	<dd>This allows you to select an alternate gdb binary. "

"<p>"
" <dt>Edit preferences"
"	<dd>Change the edit preferences. "

"<p>"
" <dt>Save source window size"
"	<dd>Saves the current size and position of the main source window if "
	"invoked from the main source window, or the standard source window if "
	"invoked from a standard source window. "

"<p>"
" <dt>Print"
"	<dd>Prints the currently displayed source. "

"<p>"
" <dt>Close"
"	<dd>Closes the source window. "

"<p>"
" <dt>Quit"
"	<dd>Quits Code Medic. "

"</dl>"

"<p>"

"<b> Edit </b>"

"<dl>"

" <dt>Undo"
"	<dd>Undoes the previous changes in the currently active text area."

"<p>"
" <dt>Redo"
"	<dd>Redoes the previous changes in the currently active text area."

"<p>"
" <dt>Cut"
"	<dd>Cuts the currently selected text into the clipboard."

"<p>"
" <dt>Copy"
"	<dd>Copies the currently selected text into the clipboard."

"<p>"
" <dt>Paste"
"	<dd>Pastes the contents of the clipboard into the currectly active "
	"text area."

"<p>"
" <dt>Clear"
"	<dd>Clears the currently selected text."

"<p>"
" <dt>Select all"
"	<dd>Selects all of the text in the currently active text area."

"</dl>"

"<b> Search </b>"

"<p>"

"<dl>"

" <dt>Find..."
"	<dd>Opens the find dialog."

"<p>"
" <dt>Find previous"
"	<dd>Finds the previous occurance of the search string."

"<p>"
" <dt>Find next"
"	<dd>Finds the next occurance of the search string."

"<p>"
" <dt>Enter search string"
"	<dd>Replaces the current search string with the currectly selected text."

"<p>"
" <dt>Enter replace string"
"	<dd>Replaces the current replace string with the currectly selected text."

"<p>"
" <dt>Find selection backwards"
"	<dd>Replaces the current search string with the currectly selected text, "
	"and finds the previous occurance of this string."

"<p>"
" <dt>Find selection forward"
"	<dd>Replaces the current search string with the currectly selected text, "
	"and finds the next occurance of this string."

"<p>"
" <dt>Replace"
"	<dd>Replaces the currently selected text with the current replace string."

"<p>"
" <dt>Replace and find previous"
"	<dd>Replaces the currently selected text with the current replace string, "
"	and finds the previous occurance of the current search string."	

"<p>"
" <dt>Replace and find next"
"	<dd>Replaces the currently selected text with the current replace string, "
"	and finds the next occurance of the current search string."

"<p>"
" <dt>Replace all forward"
"	<dd>Replaces all occurances of the search string with the replace string "
"	from the current cursor position forward."

"<p>"
" <dt>Replace all backward"
"	<dd>Replaces all occurances of the search string with the replace string "
"	from the current cursor position backward."


"</dl>"

"<b> Debug </b>"

"<p>"

"<dl>"

" <dt>Select binary..."
"	<dd>Selects a new program binary to debug."

"<p>"
" <dt>Select core..."
"	<dd>Selects a new core file to debug."

"<p>"
" <dt>Select process..."
"	<dd>Selects a new process to debug."

"<p>"
" <dt>Clear all breakpoints"
"	<dd>Clears all breakpoints in the current program."

"<p>"
" <dt>Display variable"
"	<dd>Copies the currently selected text into the "
"	<a href=jxhelp:GDBVarTreeHelp>variable tree</a>."

"<p>"
" <dt>Run"
"	<dd>Runs the debugged program."

"<p>"
" <dt>Stop"
"	<dd>Stops the debugged process."

"<p>"
" <dt>Kill"
"	<dd>Kills the debugged process."

"<p>"
" <dt>Next"
"	<dd>Executes the next line in the current source file."	

"<p>"
" <dt>Step"
"	<dd>Executes into the next line in the current source file."

"<p>"
" <dt>Finish"
"	<dd>Finished executing the current function and stops."

"<p>"
" <dt>Continue"
"	<dd>Resumes execution of the program."


"</dl>"

"<b> Memory </b>"

"<p>"

"<dl>"

" <dt>Examine memory..."
"	<dd>Opens a window that displays the raw memory requested."

"<p>"
" <dt>Disassemble memory..."
"	<dd>Opens a window that displays the disassembled memory requested."

"</dl>"

"<b> Windows </b>"

"<p>"

"<dl>"

" <dt>Bring windows to front"
"	<dd>Lifts all Code Medic window to the front of the screen."

"<p>"
" <dt>Close all windows"
"	<dd>Closes all subsidiary windows."

"<p>"
" <dt>Command window"
"	<dd>Opens and brings to front the <a href=jxhelp:GDBCommandDirHelp>"
"	command window</a>."

"<p>"
" <dt>Current source window"
"	<dd>Opens and bring to front the main source window."

"<p>"
" <dt>Variable tree window"
"	<dd>Opens and bring to front the <a href=jxhelp:GDBVarTreeHelp>"
"	variable tree window</a>."

"<p>"
" <dt>Local variable window"
"	<dd>Opens and bring to front the local variables window."

"<p>"
" <dt>File list window"
"	<dd>Opens and bring to front the <a href=jxhelp:GDBFileListHelp>"
"	file list window</a>."

"<p>"
" <dt>Debug window"
"	<dd>Opens a window that displays the raw text returned from gdb. "
"	This text can be used to tract down bugs in Code Medic."

"</dl>"

;
