/******************************************************************************
 GDBGettingStartedHelpText.cpp

	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include "GDBHelpText.h"

const JCharacter* kGettingStartedHelpName  = "GDBGettingStartedHelp";
const JCharacter* kGettingStartedHelpTitle = "GettingStarted with Code Medic";

const JCharacter* kGettingStartedHelpText =

"<html>"
"<h3>GettingStarted with Code Medic</h3>"

"Code Medic is a graphical front-end to the gdb debugger. It was designed to"
"work with <a href=http://www.cco.caltech.edu/~jafl/jcc>Code "
"Crusader</a>, but also works fine alone. "

"<p>"

"<b> Running Code Medic </b>"

"<p>"

"You start Code Medic by running <i>medic</i> at the command line. You can "
"include as arguments to medic the file to be debugged, the core file (if "
"any) to debug and any files that you want opened as well. The formats for "
"specifying files to open are the following:"

"<ul>"
"<li> -f <i>filename</i>"
"<p>"
"<li> -f +<i>line_number</i> <i>file_name</i>"
"<p>"
"<li> -f <i>file_name</i><b>:</b><i>line_number</i>"
"<p>"
"</ul>"

"<b> Selecting the Program to Debug </b>"

"<p>"

"Once Code Medic is running, and you haven't specified the file to debug "
"on the command line, you must select one by choosing <i>Select binary...</i> "
"on the <i>File</i> menu from either a "
"<a href=jxhelp:GDBSourceWindowHelp>source window</a> or the "
"<a href=jxhelp:GDBCommandDirHelp>command window</a>. " 
"After loading the file, Code Medic will fill the "
"<a href=jxhelp:GDBFileListHelp>File list window</a> with the files "
"contained in the project. It will then display the function <i>main</i> "
"in the <a href=jxhelp:GDBSourceWindowHelp>main source window</a>" 

"<p>"

"<b> Debugging Your Program </b>"

"<p>"

"You are now ready to debug your program. You can set "
"<a href=jxhelp:GDBSourceWindowHelp#Breakpoints>breakpoints</a>, "
"run the program, and step through the program using the "
"<a href=jxhelp:GDBSourceWindowHelp#Buttons>button bar</a> and "
"<a href=jxhelp:GDBSourceWindowHelp#Menu>menu</a> commands on the "
"<a href=jxhelp:GDBSourceWindowHelp>main source window</a> and "
"<a href=jxhelp:GDBCommandDirHelp>command window</a>."

"<p>"

"<b> Viewing Data </b>"

"<p>"

"To view the values of the various variables in your program, you can "
"either use raw gdb commands in the "
"<a href=jxhelp:GDBCommandDirHelp>command input</a>, or the "
"<a href=jxhelp:GDBVartreeHelp>Variable tree window</a>."

;