/******************************************************************************
 GLPlotHelpText.cpp

 	Copyright (C) 1998 by Glenn W. Bach.

 ******************************************************************************/

#include "GLHelpText.h"

const JCharacter* kGLPlotHelpName  		= "GLPlotHelp";
const JCharacter* kPlotHelpTitle 		= "Plot Window";

const JCharacter* kGLPlotSettingsHelpName = "GLPlotHelp#Settings";
const JCharacter* kGLPlotCursorsHelpName 	= "GLPlotHelp#Cursors";
const JCharacter* kGLPlotAnalysisHelpName	= "GLPlotHelp#Analysis";

const JCharacter* kPlotHelpText =

"<html>"

"<h3>Plot window</h3>"

"<a name=Settings>"

"<p>"
"<b>Curve settings</b> "
"<p>"

"To change the settings of a curve, either select \"Change curve options...\" from "
"the Options menu, double-click on the appropriate curve label in the legend, or "
"right-click on the curve name in the legend."

"<p>"
"<b>Scale</b>"
"<p>"

"You can change the scale of the plot in several ways. To view the scale "
"dialog box, which allows you to change the scale in both axes, you can "
"select Scale from the Settings menu, or double-click on the appropriate "
"axis. To scale without the dialog, click and drag the mouse, which will "
"create a zoom rectangle. If the Meta key is pressed, the scale will be "
"changed to show exactly the region contained by the zoom rectangle. If the "
"Meta key is not pressed, a reasonable scale will be chosen around the area "
"of interest. To return the scale to the default, select Reset scale from "
"the Settings menu."

"<p>"
"<b>Range</b> " 
"<p>"

"The range specifies what portion of the curves will be "
"drawn, and what portion of the curves will by used in the analysis. To set "
"the range, zoom in to the desired range and select Set range from the "
"Settings menu. You can then reset the range by selecting Clear range from "
"the Settings menu."

"<p>"
"<b>Labels</b> "
"<p>"

"You can edit the plot labels either by selecting Labels from the Settings "
"menu, or double-clicking on the appropriate label."

"<a name=Cursors>"

"<p>"
"<b>Cursors</b>"
"<p>"

"Cursors are a useful way of determining the position of various features "
"on a curve. If you view Dual cursors, the values of both cursors will be "
"displayed along with the distance between them. "

"<a name=Analysis>"

"<p>"
"<b>Curve fitting</b> "
"<p>"

"To fit a curve, select the curve name from one of the fit sub-menus in the "
"Analysis menu. The fit parameters and residual plots can be viewed by "
"selecting the appropriate entry in the Fit parameters and Residual plots "
"sub-menus."

"</html>"
;
