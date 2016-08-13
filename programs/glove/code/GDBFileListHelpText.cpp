/******************************************************************************
 GDBFileListHelpText.cpp

	Copyright © 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "GDBHelpText.h"

const JCharacter* kFileListHelpName  = "GDBFileListHelp";
const JCharacter* kFileListHelpTitle = "File List";

const JCharacter* kFileListHelpText =

"<html>"
"<h3>File List</h3>"

"The file list window contains a list of all source files found in the "
"project being debugged. In the file list you can open files, "
"filter files using a standard or regular expression filter, and search "
"for files."

"<p>"

"<b> Opening Source Files </b>"

"<p>"

"In the file list you can open a source file in the list by double-clicking on "
"the file or by selecting the file and hitting the return key."

"<p>"

"<b> Filtering Source Files </b>"

"<p>"

"The standard filter input field is made visible by selecting <i>Show filter</i> "
"from the <i>File</i> menu. If a filter expression (ie. *.cc) is input in "
"to the input field, then only those file names that match the filter will "
"be displayed. To match files using a regular expression, select <i>"
"Show regex</i> from the <i>File</i> menu. If a regular expression is "
"entered into the regex input field, then only those files that match "
"that regex will be displayed. Both input fields contain a history menu "
"that contains all expressions that have been input in the past."

"<p>"

"<b> Searching for Source Files </b>"

"<p>"

"To find a specific file in the file list, simply start typing its name in "
"the list area. As you type, the file name that most closely matches "
"the string that you've typed thus far will be selected. Hitting the "
"spacebar will clear the search string and unselect any files."
;