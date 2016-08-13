/******************************************************************************
 GDBCommandDirHelpText.cpp

	Copyright © 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "GDBHelpText.h"

const JCharacter* kCommandDirHelpName  = "GDBCommandDirHelp";
const JCharacter* kCommandDirHelpTitle = "Command Window";

const JCharacter* kCommandHistoryHelpName	= "GDBCommandDirHelp#History";
const JCharacter* kCommandInputHelpName		= "GDBCommandDirHelp#Input";
const JCharacter* kCommandButtonsHelpName	= "GDBCommandDirHelp#Buttons";
const JCharacter* kCommandMenuHelpName		= "GDBCommandDirHelp#Menu";
const JCharacter* kCommandFileHelpName		= "GDBCommandDirHelp#File";
const JCharacter* kCommandSearchHelpName	= "GDBCommandDirHelp#Search";

const JCharacter* kCommandDirHelpText =

"<html>"
"<h3>Command Window</h3>"

"The command window is used for direct communication between the user and "
"the underlying gdb process. The command window has four parts, the "
"<a href=jxhelp:#History>command history</a>, the <a href=jxhelp:#Input>"
"command input</a>, the <a href=jxhelp:#Buttons>button bar</a>, and the "
"<a href=jxhelp:#Menu>menu</a>."

"<a name=History>"

"<h3> Command History </h3>"

"It is in the command history that the information that gdb "
"prints out is displayed. In addition, all input from the user in the "
"<a href=jxhelp:#Input>command input</a> is echoed to the command history. "
"The text in the command history may be edited and saved for further reference "
"using the <i>Save history</i> and <i>Save history as...</i> "
"<a href=jxhelp:#File>menu</a> commands."

"<p>"

"To perform a text search in the command history, use the <i>Search</i> "
"<a href=jxhelp:#Search>menu</a>."

"<a name=Input>"

"<h3> Command Input </h3>"

"Anything that the user desires to pass directly to the gdb process can "
"be typed into the command input. If the gdb process is not busy, as soon "
"as the user hits the return key everything in the command input is sent to "
"gdb. If the gdb process is currently processing the debugged program, the "
"text from the input field will be placed in a queue, and read by either "
"the debugged program if it requests command-line input, or gdb when it "
"has finished processing. The user can tell when gdb is busy by the gdb "
"prompt located on the upper left side of the command input. If gdb is busy "
"the prompt will be dimmed out, otherwise it will be black."

"<p>"

"Every command that is sent from the command input is saved. To retrieve a "
"previously submitted command, either use the up and down arrow keys "
"to scroll through the commands, or use the input history menu located on the "
"lower left side of the command input."

"<a name=Buttons>"

"<h3> Button Bar </h3>"

"The button bar allows you to send the specified commands to gdb. Currently "
"the available buttons are <i>Run</i>, <i>Stop</i>, <i>Kill</i>, <i>Next</i>, "
"<i>Step</i>, <i>Finish</i>, and <i>Continue</i>. Future versions of Code "
"Medic will allow you to customize the button bar. The button bar in the "
"command window is the same as that in the <a href=jxhelp:GDBSourceWindowHelp>"
"main source window</a>."

"<a name=Menu>"

"<h3> Menu </h3>"

"<a name=File>"

"<b> File </b>"

"<p>"

"<dl>"

" <dt>Open source file..."
"	<dd>Opens a source file that is displayed in a <a href=jxhelp:GDBSourceWindowHelp>"
	"standard source window</a>."
"<p>"
" <dt>Save history"
"	<dd>Saves the current contents of the <a href=jxhelp:#History>command history</a>."

"<p>"
" <dt>Save history as..."
"	<dd>Saves the current contents of the <a href=jxhelp:#History>"
	"command history</a> in a new file."

"<p>"
" <dt>Restart gdb"
"	<dd>Restarts the underlying gdb process, but retains the current program, "
"	along with its <a href=jxhelp:GDBSourceWindowHelp#Breakpoints>breakpoints</a> "
"	and <a href=jxhelp:GDBVarTreeHelp>variables</a>."

"<p>"
" <dt>Change gdb binary"
"	<dd>This allows you to select an alternate gdb binary."

"<p>"
" <dt>Edit preferences"
"	<dd>Change the edit preferences."

"<p>"
" <dt>Save command window size"
"	<dd>Saves the current size and position of the command window."

"<p>"
" <dt>Print"
"	<dd>Prints the current <a href=jxhelp:#History>command history</a>."

"<p>"
" <dt>Close"
"	<dd>Closes the command window."

"<p>"
" <dt>Quit"
"	<dd>Quits Code Medic."

"</dl>"

"<b> Edit </b>"

"<p>"

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

"<a name=Search>"

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
"	<dd>Opens and brings to front the command window."

"<p>"
" <dt>Current source window"
"	<dd>Opens and bring to front the <a href=jxhelp:GDBSourceWindowHelp>"
"	main source window</a>."

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