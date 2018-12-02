/******************************************************************************
 GLCreateVectorPlotDialog.h

	Interface for the GLCreateVectorPlotDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLCreateVectorPlotDialog
#define _H_GLCreateVectorPlotDialog

#include <JXDialogDirector.h>

class JXTextMenu;
class GLRaggedFloatTableData;
class GLDataDocument;
class JXInputField;
class JString;

class GLCreateVectorPlotDialog : public JXDialogDirector
{
public:

	GLCreateVectorPlotDialog(GLDataDocument* supervisor, GLRaggedFloatTableData* data,
						const JIndex startX, const JIndex startY,
						const JIndex startX2, const JIndex startY2);

	virtual ~GLCreateVectorPlotDialog();

	void GetColumns(JIndex* X1, JIndex* Y1,
					JIndex* X2, JIndex* Y2);

	JBoolean GetPlotIndex(JIndex* index);

	const JString& GetLabel();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual JBoolean	OKToDeactivate() override;

private:

	JIndex itsStartX1;
	JIndex itsStartX2;
	JIndex itsStartY1;
	JIndex itsStartY2;
	JIndex itsPlotIndex;
	GLDataDocument*	itsTableDir;

// begin JXLayout

	JXTextMenu*    itsX1Menu;
	JXTextMenu*    itsY1Menu;
	JXTextMenu*    itsX2Menu;
	JXTextMenu*    itsY2Menu;
	JXTextMenu*    itsPlotsMenu;
	JXInputField*  itsLabelInput;

// end JXLayout

private:

	void	BuildWindow(GLRaggedFloatTableData* data,
						const JIndex startX, const JIndex startY,
						const JIndex startXErr, const JIndex startYErr);

	// not allowed

	GLCreateVectorPlotDialog(const GLCreateVectorPlotDialog& source);
	const GLCreateVectorPlotDialog& operator=(const GLCreateVectorPlotDialog& source);
};

#endif
