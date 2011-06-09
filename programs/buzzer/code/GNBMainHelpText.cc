/******************************************************************************
 GNBMainHelpText.cc

	Copyright © 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "GNBHelpText.h"

const JCharacter* kMainHelpName  	= "GNBMainHelp";
const JCharacter* kMainHelpTitle 	= "Main Window";

const JCharacter* kNotesHelpName	= "GNBMainHelp#Notes";
const JCharacter* kToDoHelpName		= "GNBMainHelp#ToDo";

const JCharacter* kMainHelpText =

"<html>"
"<h3>Main Window</h3>"

"The main window is where most of the work is done. This window contains "
"a hierarchy of groups, notes and to-do list items. You can change the name "
"of any of the items by clicking on its name."
"<p>"

"The button bar below the menu is configurable. To change it, "
"Select <i>Edit tool bar...</i> from the <i>Notebook</i> menu."

"<p>"

"All items can be rearranged by click-dragging their icon. All items can be "
"removed by selecting the item by clicking on its icon and selecting "
"<i>Remove selected</i> from the <i>Notebook</i> menu. Any item removed "
"is placed in the Discards window which can be accessed by selecting "
"<i>View discards</i> from the <i>Notebook</i> menu."

"<h3> Notes </h3>"
"<a name=Notes>"

"You create a new note by selecting <i>New note</i> from the "
"<i>Notebook</i> menu, or by clicking on the corresponding button. "
"If you wish to have a note created within a group, select the group "
"by clicking on the icon before creating the note. If you wish to have "
"the new note placed after a current item, select that item by clicking "
"its icon before creating the note. To edit a note, double-click on its icon."

"<p>"

"You do not need to name notes. If you haven't named it, the first line "
"of the note's text will be used as the title."

"<p>"

"<a name=ToDo>"

"<h3> To-Do list items </h3>"

"You create a new to-do item by selecting <i>New to-do</i> from the "
"<i>Notebook</i> menu, or by clicking on the corresponding button. "
"If you wish to have a to-do item created within a group, select the group "
"by clicking on the icon before creating the to-do item. If you wish to have "
"the new to-do item placed after a current item, select that item by clicking "
"its icon before creating the to-do item. "

"<p>"

"You can set a to-do list item to be urgent by selecting <i>Toggle urgency</i> "
"from the <i>Notebook</i> menu. This menu item can also be used to remove the "
"urgency from a to-do list item. To mark a to-do list item as complete, "
"Meta-click on the icon. You can add a note to a to-do list item by selecting "
"<i>Add note</i> from the <i>Notebook</i> menu."
;