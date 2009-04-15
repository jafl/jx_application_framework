/******************************************************************************
 GXCreateVectorPlotDialog.h

	Interface for the GXCreateVectorPlotDialog class

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_GXCreateVectorPlotDialog
#define _H_GXCreateVectorPlotDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

class JXTextMenu;
class GRaggedFloatTableData;
class GXDataDocument;
class JXInputField;
class JString;

class GXCreateVectorPlotDialog : public JXDialogDirector
{
public:

	GXCreateVectorPlotDialog(GXDataDocument* supervisor, GRaggedFloatTableData* data,
						const JIndex startX, const JIndex startY,
						const JIndex startX2, const JIndex startY2);

	virtual ~GXCreateVectorPlotDialog();
	
	void GetColumns(JIndex* X1, JIndex* Y1, 
					JIndex* X2, JIndex* Y2);
					
	JBoolean GetPlotIndex(JIndex* index);
	
	const JString& GetLabel();
	
protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual JBoolean	OKToDeactivate();
	
private:

	JIndex itsStartX1;
	JIndex itsStartX2;
	JIndex itsStartY1;
	JIndex itsStartY2;
	JIndex itsPlotIndex;
	GXDataDocument*	itsTableDir;

// begin JXLayout

    JXTextMenu*    itsX1Menu;
    JXTextMenu*    itsY1Menu;
    JXTextMenu*    itsX2Menu;
    JXTextMenu*    itsY2Menu;
    JXTextMenu*    itsPlotsMenu;
    JXInputField*  itsLabelInput;

// end JXLayout

private:

	void	BuildWindow(GRaggedFloatTableData* data, 
						const JIndex startX, const JIndex startY,
						const JIndex startXErr, const JIndex startYErr);

	// not allowed

	GXCreateVectorPlotDialog(const GXCreateVectorPlotDialog& source);
	const GXCreateVectorPlotDialog& operator=(const GXCreateVectorPlotDialog& source);
};

#endif
