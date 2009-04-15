const JCharacter* plothelptext = "
<h3>Working with plots</h3>
<p>
<b>Options</b>
<p>
The <i>Curve options</i> menu item lets you choose how to display a curve. You
can choose to show the lines, the symbols, both, or nothing for a given curve. 
You can choose whether or not to display the <i>X</i> and <i>Y</i> errors. 
You can also edit the curve label.
<p>
You can remove a plot by selecting the <i>Remove curve</i> menu item.
<p>
The <i>Scale</i> menu item lets you specify the minimum, maximum and tick 
increment for each axis. This dialog box also lets you choose whether the 
specified axis should be in a linear or logarithmic form. 
<p>
An alternate method of setting the scale is to use the mouse. The plot will
be zoomed to the region selected by the mouse. If the <i>Meta</i> key is held
down while dragging the mouse, the scale will be adjusted to sensible values 
as close as possible to those you selected with the mouse.
<p>
To return to the default scale, select the <i>Reset scale</i> menu item.
<p>
The <i>Set range</i> and <i>Clear range</i> menu items allow you to specify the
range of visible data points. To use these, zoom in to your desired range, then
select <i>Set range</i>. This range will also be used for all analysis.
<p>
The plot title and axis labels are edited under the <i>Edit labels</i> menu item.
<p>
The final menu items in the <i>Options</i> menu, allow you to show or hide the
frame, the grid, or the legend for the plot.
<p>
<b>Analysis</b>
<p>
Under the <i>Analysis</i> menu, you will find items for printing, and various 
types of fits, along with a feature for displaying an arbitrary function as 
a curve.
<p>
After fitting a curve, the fit parameters can be viewed by selecting the
<i>Fit parameters</i> menu item. The <i>Residual plots</i> menu item opens
a plot showing the difference between the data and the points calculated from
the fit parameters. It is highly recommended that after each fit, the 
residual plot is opened. This will show any systematic errors indicating
that the fit performed on the plot was not the appropriate one.
<p>
In order to save and print fit parameters, press the <i>Session</i> button from
within the <i>Fit Paramters</i> dialog box. This will copy the fit parameters onto
the session window which can then be saved or printed.
<p>
<b>Cursors</b>
<p>
The <i>Cursor</i> menu provides dual cursors in both axes. These cursors can
be moved with the arrow keys or the mouse. If you are using the arrow keys, 
hitting the
tab key will select the next available cursor. The cursor window will then
display both the cursor values and their differences. Cursor positions can
be marked with the <i>Mark cursor</i> menu item. The cursor window also displays
the current marked cursor positions. As with fit parameters, the values in
the cursor window can be added to the session window by pressing the 
<i>Session</i> button.
<p>
By using the <i>Cursor module</i> menu item, you can perform arbitrary actions
based on the current values of the cursor.
<p>
<hr>
<p>
For information on the construction of modules please see:
<br>http://www.pma.caltech.edu/~glenn/glove/modules.html
<p>
<hr>
<p>
Glove was written using the JX Application Framework:
<br>http://www.cco.caltech.edu/~jafl/jx/";
