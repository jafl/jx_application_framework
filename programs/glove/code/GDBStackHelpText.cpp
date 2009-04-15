/******************************************************************************
 GDBStackHelpText.cpp

	Copyright © 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <glStdInc.h>
#include "GDBHelpText.h"

const JCharacter* kStackHelpName  = "GDBStackHelp";
const JCharacter* kStackHelpTitle = "Stack Window";

const JCharacter* kStackHelpText =

"<html>"
"<h3>Stack Window</h3>"

"The stack window contains a tree containing the current stack. "
"The location of the frames is shown at the top level of the tree, "
"with the arguments passed to the particular function dislayed in "
"the second level of the tree."

"<p>"

"<b> Changing frames </b>"

"<p>"

"You can change frames by clicking on the desired frame, or by "
"using the up and down arrow keys to move through the frames. "
"You can also continue to use the command line commands - <i>frame #</i>, "
"<i>up</i>, and <i>down</i>."

"<p>"

"<b> Arguments </b>"

"<p>"

"You can view the arguments to a particular frame's function by "
"clicking on the tree toggle. You can view all of the arguments to "
"all of the frames by meta-clicking on the tree toggle."

"<p>"

"To transfer an argument into the "
"<a href=jxhelp:GDBVartreeHelp>Variable tree window</a>, click "
"on it."

"<p>"

"<b> Viewing source </b>"

"<p>"

"To view the source associated with a particular frame's function, "
"meta-click on that frame."
;