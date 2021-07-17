/******************************************************************************
 GLCreatePlotDialog.h

	Interface for the GLCreatePlotDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GLCreatePlotDialog
#define _H_GLCreatePlotDialog

#include <JXDialogDirector.h>

class JXTextMenu;
class JXTextCheckbox;
class GLRaggedFloatTableData;
class GLDataDocument;
class JXInputField;
class JString;

class GLCreatePlotDialog : public JXDialogDirector
{
public:

	GLCreatePlotDialog(GLDataDocument* supervisor, GLRaggedFloatTableData* data,
						const JIndex startX, const JIndex startY,
						const JIndex startXErr, const JIndex startYErr);

	virtual ~GLCreatePlotDialog();
	
	void GetColumns(JIndex* startX, JIndex* startXErr, 
					JIndex* startY, JIndex* startYErr);
					
	bool GetPlotIndex(JIndex* index);
	
	const JString& GetLabel();
	
protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual bool	OKToDeactivate() override;
	
private:

	JIndex itsStartX;
	JIndex itsStartXErr;
	JIndex itsStartY;
	JIndex itsStartYErr;
	JIndex itsPlotIndex;
	GLDataDocument*	itsTableDir;

// begin JXLayout

	JXInputField* itsLabelInput;
	JXTextMenu*   itsXMenu;
	JXTextMenu*   itsXErrMenu;
	JXTextMenu*   itsYErrMenu;
	JXTextMenu*   itsYMenu;
	JXTextMenu*   itsPlotsMenu;

// end JXLayout

private:

	void	BuildWindow(GLRaggedFloatTableData* data, 
						const JIndex startX, const JIndex startY,
						const JIndex startXErr, const JIndex startYErr);

	// not allowed

	GLCreatePlotDialog(const GLCreatePlotDialog& source);
	const GLCreatePlotDialog& operator=(const GLCreatePlotDialog& source);
};

#endif
