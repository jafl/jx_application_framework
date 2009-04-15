/******************************************************************************
 GXCreatePlotDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include <glStdInc.h>
#include "GXCreatePlotDialog.h"
#include "GXDataDocument.h"
#include "GRaggedFloatTableData.h"

#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextMenu.h>
#include <JXTextCheckbox.h>
#include <JXInputField.h>
#include <JXStaticText.h>

#include <JPtrArray.h>
#include <JString.h>
#include <JUserNotification.h>

#include <jGlobals.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GXCreatePlotDialog::GXCreatePlotDialog
	(
	GXDataDocument* supervisor, 
	GRaggedFloatTableData* data,
	const JIndex startX, 
	const JIndex startY,
	const JIndex startXErr,
	const JIndex startYErr
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	itsTableDir = supervisor;
	BuildWindow(data, startX, startXErr, startY, startYErr);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GXCreatePlotDialog::~GXCreatePlotDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GXCreatePlotDialog::BuildWindow
	(
	GRaggedFloatTableData* data,
	const JIndex startX, 
	const JIndex startY,
	const JIndex startXErr, 
	const JIndex startYErr
	)
{
	
// begin JXLayout

    JXWindow* window = new JXWindow(this, 380,160, "");
    assert( window != NULL );
    SetWindow(window);

    itsXMenu =
        new JXTextMenu("X Axis", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 10,40, 160,30);
    assert( itsXMenu != NULL );

    itsYMenu =
        new JXTextMenu("Y Axis", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 200,40, 170,30);
    assert( itsYMenu != NULL );

    itsXErrMenu =
        new JXTextMenu("X Error", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 10,80, 160,30);
    assert( itsXErrMenu != NULL );

    itsYErrMenu =
        new JXTextMenu("Y Error", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 200,80, 170,30);
    assert( itsYErrMenu != NULL );

    JXTextButton* okButton =
        new JXTextButton("OK", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 280,130, 70,20);
    assert( okButton != NULL );
    okButton->SetShortcuts("^M");

    JXTextButton* cancelButton =
        new JXTextButton("Cancel", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 190,130, 70,20);
    assert( cancelButton != NULL );
    cancelButton->SetShortcuts("^[");

    itsPlotsMenu =
        new JXTextMenu("Plot:", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 10,120, 130,30);
    assert( itsPlotsMenu != NULL );

    itsLabelInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 115,10, 200,20);
    assert( itsLabelInput != NULL );

    JXStaticText* obj1 =
        new JXStaticText("Label:", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 65,13, 45,15);
    assert( obj1 != NULL );

// end JXLayout

	window->SetTitle("Choose Plot Data Columns");
	SetButtons(okButton, cancelButton);
	
	itsXErrMenu->AppendItem("None");
	itsYErrMenu->AppendItem("None");
	
	const JSize count = data->GetDataColCount();
	for (JSize i = 1; i <= count; i++)
		{
		JString str(i);
		str.Prepend("Column "); 
		itsXMenu->AppendItem(str);
		itsXErrMenu->AppendItem(str);
		itsYMenu->AppendItem(str);
		itsYErrMenu->AppendItem(str);
		}

	
	itsStartX = startX;
	if (startX == 0)
		{
		itsStartX = 1;
		}
		
	itsStartXErr = startXErr + 1;
		
	itsStartY = startY;
	if (startY == 0)
		{
		itsStartY = 1;
		}
		
	itsStartYErr = startYErr + 1;
	
	JPtrArray<JString> names(JPtrArrayT::kDeleteAll);
	itsTableDir->GetPlotNames(names);
	
	itsPlotsMenu->AppendItem("New Plot");
	
	const JSize strCount = names.GetElementCount();
	
	for (JSize i = 1; i <= strCount; i++)
		{
		itsPlotsMenu->AppendItem(*(names.NthElement(i)));
		}

	itsPlotsMenu->ShowSeparatorAfter(1, kJTrue);

	itsPlotIndex = 1;
	
	itsXMenu->SetToPopupChoice(kJTrue, itsStartX);
	itsXErrMenu->SetToPopupChoice(kJTrue, itsStartXErr);
	itsYMenu->SetToPopupChoice(kJTrue, itsStartY);
	itsYErrMenu->SetToPopupChoice(kJTrue, itsStartYErr);
	itsPlotsMenu->SetToPopupChoice(kJTrue, itsPlotIndex);
	
	itsXMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsXErrMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsYMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsYErrMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPlotsMenu->SetUpdateAction(JXMenu::kDisableNone);
	
	ListenTo(itsXMenu);
	ListenTo(itsXErrMenu);
	ListenTo(itsYMenu);
	ListenTo(itsYErrMenu);
	ListenTo(itsPlotsMenu);

	itsLabelInput->SetText("Untitled");
}

/******************************************************************************
 GetColumns 

 ******************************************************************************/

void
GXCreatePlotDialog::GetColumns
	(
	JIndex* startX, 
	JIndex* startXErr, 
	JIndex* startY,
	JIndex* startYErr
	)
{
	*startX = itsStartX;
	*startY = itsStartY;
	*startXErr = itsStartXErr - 1;
	*startYErr = itsStartYErr - 1;
}

/******************************************************************************
 GetColumns 

 ******************************************************************************/

void
GXCreatePlotDialog::Receive
	(
	JBroadcaster* sender, 
	const Message& message
	)
{
	if (sender == itsXMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		itsStartX = selection->GetIndex();
		}
		
	else if (sender == itsXErrMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		itsStartXErr = selection->GetIndex();
		}
		
	else if (sender == itsYMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		itsStartY = selection->GetIndex();
		}
		
	else if (sender == itsYErrMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		itsStartYErr = selection->GetIndex();
		}
		
	else if (sender == itsPlotsMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		itsPlotIndex = selection->GetIndex();
		}
		
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 GetPlotIndex 

 ******************************************************************************/

JBoolean 
GXCreatePlotDialog::GetPlotIndex
	(
	JIndex* index
	)
{
	if (itsPlotIndex == 1)
		{
		return kJFalse;
		}
		
	*index = itsPlotIndex - 1;
	return kJTrue;
}

/******************************************************************************
 GetPlotIndex 

 ******************************************************************************/

const JString& 
GXCreatePlotDialog::GetLabel()
{
	return itsLabelInput->GetText();
}	

/******************************************************************************
 OKToDeactivate 

 ******************************************************************************/

JBoolean 
GXCreatePlotDialog::OKToDeactivate()
{
	if (Cancelled())
		{
		return kJTrue;
		}
	if (GetLabel().IsEmpty())
		{
		JGetUserNotification()->ReportError("You must specify a curve label.");
		return kJFalse;
		}
	return kJTrue;
}
