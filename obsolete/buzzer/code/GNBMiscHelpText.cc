/******************************************************************************
 GNBMiscHelpText.cc

	Copyright © 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "GNBHelpText.h"

const JCharacter* kMiscHelpName  = "GNBMiscHelp";
const JCharacter* kMiscHelpTitle = "Miscellaneous notes";

const JCharacter* kWMHelpName	= "GNBMainHelp#WM";

const JCharacter* kMiscHelpText =

"<html>"
"<h3>Working with you window manager</h3>"
"<a name=WM>"

"Window managers like fvwm2 and fvwm95 that provide a taskbar listing all "
"the currently open windows allow you to filter this list based on the "
"window's title or WM_CLASS attribute. The Buzzer Electronic Notebook "
"sets WM_CLASS as follows: "

"<blockquote>"
"<dl>"
"<dt> Main notebook window"
"<dd> Buzzer_Notebook"
"<p>"
"<dt> Trash window"
"<dd> Buzzer_Trash"
"<p>"
"<dt> Note window"
"<dd> Buzzer_Editor"
"</dl>"
"</blockquote>"


;