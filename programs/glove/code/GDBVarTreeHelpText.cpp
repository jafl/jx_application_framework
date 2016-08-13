/******************************************************************************
 GDBVarTreeHelpText.cpp

	Copyright © 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "GDBHelpText.h"

const JCharacter* kVarTreeHelpName  = "GDBVarTreeHelp";
const JCharacter* kVarTreeHelpTitle = "Variable Tree";

const JCharacter* kVarTreeHelpText =

"<html>"
"<h3>Variable Tree</h3>"

"The variable contains a list of variables. If any of the variables are "
"structs or pointers, then the list expands to a tree, allowing you to "
"dereference the variable to view its contents. As gdb executes the program "
"the variables in the tree are updated automatically."

"<p>"

"<b>Adding Variables</b>"

"<p>"

"To add variables select <i>Add variable</i> from the <i>Action</i> menu. "
"You can also select a variable name in the "
"<a href=jxhelp:GDBSourceWindowHelp>main source window</a> and select "
"<i>Display variable</i> from its <i>Debug</i> menu."

"<p>"

"<b>Deleting Variables</b>"

"<p>"

"To delete a variable, select the variable or variables that you want to "
"delete and either hit the <i>Backspace</i> key or select the "
"<i>Delete variable</i> from the <i>Action</i> menu."

;