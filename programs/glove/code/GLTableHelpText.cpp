/******************************************************************************
 GLTableHelpText.cpp

 	Copyright © 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <glStdInc.h>
#include "GLHelpText.h"

const JCharacter* kGLTableHelpName  		= "GLTableHelp";
const JCharacter* kTableHelpTitle 		= "Data window";

const JCharacter* kGLTableDataHelpName 	= "GLTableHelp#Data";
const JCharacter* kGLTableFilesHelpName 	= "GLTableHelp#Files";
const JCharacter* kGLTablePrintHelpName	= "GLTableHelp#Print";

const JCharacter* kTableHelpText =

"<html>"
"<h3>Data window</h3>"
"<p>"
"The main window in Glove is a table which contains columns of numbers. "
"From this table, you can manipulate the data in many ways."

"<p>"
"<a name=Data>"

"<b>Manipulating data</b>"

"<p>"

"<b>Editing a cell</b> "
"To edit a cell, double-click on it with the mouse. If it is already "
"selected you can edit it by hitting the return key. To accept the "
"changes to the cell, you can either hit return, shift-return, tab, "
"or shift-tab, or click the <i>Ok</i> button in the top "
"left corner of the table. If you hit the return key, the changes will be "
"accepted and the cell below the current one will then be in edit mode. "
"Shift-return will accept the changes and begin editing on the cell above "
"the current one. Tab and shift-tab will accept the changes and shift "
"editing to the cell the the right/left of the current one. "
"To cancel the changes you made, hit the escape key. "
"All changes made can be undone/redone by selecting <i>Undo/Redo</i> "
"from the <i>Edit</i> menu."

"<p>"

"<b>Inserting, deleting, and duplicating</b> "
"You can insert, delete or duplicate rows, columns, or ranges of cells "
"by selecting the appropriate cells by click-dragging the mouse and "
"choosing <i>Insert</i>, <i>Delete</i>, or <i>Duplicate</i> from the "
"<i>Edit</i> menu. Rows or columns can be selected by clicking on the "
"row or column header number corresonding the the desired row or column. "
"To extend the selection, you can either shift-left-click or right-click. "
"The selection can be cleared by hitting the Escape key. "
"All changes made can be undone/redone by selecting <i>Undo/Redo</i> "
"from the <i>Edit</i> menu."

"<p>"

"<b>Cutting, copying, and pasting</b> "
"After selecting a group of cells, you can cut or copy them by selecting "
"<i>Cut</i> or <i>Copy</i> from the <i>Edit</i> menu. "
"After cutting or copying the desired cells, you can paste them in "
"in a few ways. If you have no cells selected when you paste, the pasted "
"cells will be placed in a new column appended to the table. "
"If you have a group of columns selected, the pasted cells will be placed "
"in columns inserted before the selected columns. "
"If you copied a single cell, you can select the destination cell and paste "
"over it, thereby changing its value. If you copied a range of cells "
"all contained within a single column, you can replace the same sized range "
"of cells elsewhere, or insert them into a column by selecting the cell "
"before which the pasted cells will be inserted. "
"All changes made can be undone/redone by selecting <i>Undo/Redo</i> "
"from the <i>Edit</i> menu."

"<p>"

"<b>Generating and transforming data</b> "
"Glove can generate a sequence of numbers for you with the "
"<i>Generate column by range</i> and <i>Generate column by increment</i> "
"menu items in the <i>Data</i> menu. The resultant column of numbers can "
"either be appended to the table, or can replace a current column. "
"This or any other column of numbers can then be transformed with "
"the transform dialog which is found in the <i>Data</i> menu."

"<p>"
"<a name=Files>"

"<b>Working with files</b>"

"<p>"

"<b>Loading files</b> "
"To start a new file, select <i>New</i> from the <i>File</i> menu. "
"To open an existing file, select <i>Open</i> from the <i>File</i> menu. "
"If the file is a Glove file, it will open immediately. If not, a dialog "
"box will pop up asking which <a href=jxhelp:GLModuleHelp>module</a> to use "
"to open it. If the file is a delimited text file, select "
"<i>Text (delimited)</i> from the menu. You will then be presented "
"with a delimiter dialog box in which you can specify how the file is "
"delimited. "

"<p>"

"<b>Saving files</b> "
"To save a file, select <i>Save</i> from the <i>File</i> menu. To save a file "
"under a different name, select <i>Save as...</i> from the <i>File</i> menu. "
"If you wish to save the file in a non-Glove format, use the desired "
"<a href=jxhelp:GLModuleHelp>module</a> from the <i>Export modules...</i> "
"menu."

"<p>"
"<a name=Print>"

"<b>Printing and plotting data</b> "

"<p>"

"<b>Printing</b> "
"To print, select <i>Print...</i> from the <i>File</i> menu. "
"Select <i>Print setup</i> from the <i>File</i> menu to specify whether "
"to print in landscape or portrait mode. In the print dialog box, you "
"can choose whether to print to a file or to a printer."

"<p>"

"<b>Plotting data</b> "
"To plot columns of data, select <i>Plot data...</i> from the "
"<i>Data</i> menu. The plot dialog will ask which columns "
"represent x, y and the errors in x and y if desired. Use the "
"<i>Plot</i> menu in the dialog to specify which plot to add this "
"curve to. By default, a new plot is created. If you have columns selected, "
"they will by default specify the x and y axis and the errors in the y and x "
"axis respectively. "

"<p>"

"To plot a vector field, select <i>Plot vector field...</i> from the "
"<i>Data</i> menu. If you have columns selected, they will by default "
"specify the x and y axis and dx and dy."
"</html>"
;