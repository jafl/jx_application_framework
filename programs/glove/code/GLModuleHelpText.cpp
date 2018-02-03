/******************************************************************************
 GLModuleHelpText.cpp

 	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include "GLHelpText.h"

const JCharacter* kGLModuleHelpName  		= "GLModuleHelp";
const JCharacter* kModuleHelpTitle 		= "Modules";

const JCharacter* kModuleHelpText =

"<html>"
"<h3>Modules</h3>"
"<p>"

"Modules are intended to provide extended functionality to Glove at the "
"user's control. The protocols are very simple, and involve reading and "
"writing to standard in/out. Since Glove handles this through pipes, the "
"modules may be written in any language."

"<p>"

"Below is an enum containing relevant values for the modules."

"<p>"

"<pre>"
"enum "
"{//	File import/export module values <br>"
"	kGloveDataError = 0, <br>"
"	kGloveMatrixDataFormat = 1, <br>"
"	kGloveRaggedDataFormat = 2, <br>"
"	 <br>"
"//  General module values <br>"
"	kGloveOK = 3, <br>"
"	kGloveFail = 4, <br>"
"	kGloveFinished = 5, <br>"
" <br>"
"//	Data module values <br>"
"	kGloveRequiresData = 6, <br>"
"	kGloveRequiresNoData = 7, <br>"
"	kGloveDataDump = 8, <br>"
"	kGloveDataControl = 9, <br>"
"	 <br>"
"//  Fit module values <br>"
"	kGloveXY = 10, <br>"
"	kGloveXYdY = 11, <br>"
"	kGloveXYdX = 12, <br>"
"	kGloveXYdYdX = 13 <br>"
"}; <br>"

"</pre>"

"<p>"

"<b>Import modules</b> "

"<p>"

"Import modules should be placed in the importmodule directory of one of "
"your module directories (by default /usr/local/lib/glove and ~/.glove)."

"<p>"

"The module is called with the name of the file to be imported as the argument. "
"The module then verifies that the file is in the appropriate format. If it "
"is not, the module prints the value kGloveDataError. If the file is in the "
"proper format for the module, it prints kGloveMatrixDataFormat if the file "
"is in a matrix format (ie. all columns have the same number of rows). It "
"prints kGloveRaggedDataFormat for ragged format. For matrix format, the "
"module then prints the column count, followed by the data given row by row "
"(ie. c1r1, c2r1, c3r1, c4r1, c1r2, ...). For ragged format, the module then "
"prints the column count, followed by the data column by column. Before "
"each column, the number of rows in that column are printed."

"<p>"

"<b>Export modules</b> " 

"<p>"

"Export modules should be placed in the exportmodule directory of one of "
"your module directories (by default /usr/local/lib/glove and ~/.glove)."

"<p>"

"The module is called with the name of the file to be exported as the argument. "
"The module then prints the format that it desires, either matrix "
"(kGloveMatrixDataFormat) or ragged (kGloveRaggedDataFormat). For matrix "
"format, Glove will then write the column count, the row count, and the data "
"given row by row (ie. c1r1, c2r1, c3r1, c4r1, c1r2, ...). For ragged "
"format, Glove will write out the column count data column by column. Before "
"each column, the number of rows in that column are written."

"<p>"

"<b>Data modules</b> "

"<p>"

"Data modules should be placed in the datamodule directory of one of "
"your module directories (by default /usr/local/lib/glove and ~/.glove)."

"<p>"

"Data modules serve two purposes. For transformations that require more than "
"a basic equation, a data module can be used to transform an arbitrary number "
"of columns in an arbitrary way. A data module can also act as an acquisition "
"module, serving as an interface to data acquisition hardware. "

"<p>"

"The module should begin by indicating whether or not it requires data. This "
"is done by printing either kGloveRequiresData or kGloveRequiresNoData. If "
"the module requires data, it will then print the number of columns that it "
"requires. Glove will then write out the number of rows, and the values "
"given row by row. "

"<p>"

"The module now prints whether it will just be printing all of the data "
"row by row with kGloveDataDump, or printing individual values, specified "
"by row, column, and value, with kGloveDataControl. The module then prints "
"the number of columns of data it will be printing. "

"<p>"

"The module is now ready to print the data. Each line of data is preceded "
"by either kGloveFinished, kGloveOK, or kGloveFail. If the module is using "
"kGloveDataDump, then the module should then print all of the current row's "
"values. If the module is using kGloveDataControl, it should print the row, "
"column, and value of its data. When the module is finished, it should "
"print kGloveFinished."

"<p>"

"<b>Cursor modules</b>"

"<p>"

"Cursor modules should be placed in the cursormodule directory of one of "
"your module directories (by default /usr/local/lib/glove and ~/.glove)."

"<p>"

"After the module process is started, Glove passes three booleans (T/F), "
"XCursorVisible(), YCursorVisible(), and DualCursorsVisible(). It will then "
"write the valid values in the order x1, x2, y1, y2. If dual cursors are not "
"on, for example, neither x2 or y2 will be written. "

"<p>"

"The module must now indicate if there was an error. It prints a line "
"containing either kGloveOK, or kGloveFail followed immediately by an error "
"string. Once this line is printed, the cursor module simply prints whatever "
"information it wants to print, which will be appended to the session. "

"<p>"

"<b>Fit modules</b> "

"<p>"

"Fit modules should be placed in the fitmodule directory of one of "
"your module directories (by default /usr/local/lib/glove and ~/.glove)."

"<p>"

"After starting the module process, Glove must send the data to be fit to "
"the module. Before the data is sent, Glove writes the type from, "
"kGloveXYdYdX, kGloveXYdX, kGloveXYdY, and kGloveXY. This is followed by the "
"number of data points in the data set to be fit. Then Glove send the data "
"in order x, y, dy, dx, but only if it is appropriate. For example, if there "
"are no x errors, the data will be sent x, y, dy only. "

"<p>"

"The module must now indicate if there was an error. It prints a line "
"containing either kGloveOK, or kGloveFail followed immediately by an error "
"string. Once this line is printed, the module prints the number of "
"parameters, whether or not it has errors (T/F) and whether or not it has a "
"goodness of fit value, all in one line. The module then prints the "
"individual parameter's name and value, each on a single line." 
"</html>" 
;