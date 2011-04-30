/******************************************************************************
 GXCreateVectorPlotDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include <glStdInc.h>
#include "GXCreateVectorPlotDialog.h"
#include "GXDataDocument.h"
#include "GRaggedFloatTableData.h"

#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextMenu.h>
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

GXCreateVectorPlotDialog::GXCreateVectorPlotDialog
	(
	GXDataDocument* supervisor, 
	GRaggedFloatTableData* data,
	const JIndex startX, 
	const JIndex startY,
	const JIndex startX2,
	const JIndex startY2
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	itsTableDir = supervisor;
	BuildWindow(data, startX, startY, startX2, startY2);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GXCreateVectorPlotDialog::~GXCreateVectorPlotDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GXCreateVectorPlotDialog::BuildWindow
	(
	GRaggedFloatTableData* data,
	const JIndex startX, 
	const JIndex startY,
	const JIndex startX2, 
	const JIndex startY2
	)
{
	
// begin JXLayout

    JXWindow* window = new JXWindow(this, 380,160, "");
    assert( window != NULL );

    itsX1Menu =
        new JXTextMenu("X1", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 10,40, 160,30);
    assert( itsX1Menu != NULL );

    itsY1Menu =
        new JXTextMenu("Y1", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 200,40, 170,30);
    assert( itsY1Menu != NULL );

    itsX2Menu =
        new JXTextMenu("DX", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 10,80, 160,30);
    assert( itsX2Menu != NULL );

    itsY2Menu =
        new JXTextMenu("DY", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 200,80, 170,30);
    assert( itsY2Menu != NULL );

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

	window->SetTitle("Choose Vector Data Columns");
	SetButtons(okButton, cancelButton);
	
	const JSize count = data->GetDataColCount();
	
	for (JSize i = 1; i <= count; i++)
		{
		JString str(i);
		str.Prepend("Column "); 
		itsX1Menu->AppendItem(str);
		itsX2Menu->AppendItem(str);
		itsY1Menu->AppendItem(str);
		itsY2Menu->AppendItem(str);
		}
	
	itsStartX1 = startX;
	if (startX == 0)
		{
		itsStartX1 = 1;
		}
		
	itsStartX2 = startX2;
	if (startX2 == 0)
		{
		itsStartX2 = 1;
		}
		
	itsStartY1 = startY;
	if (startY == 0)
		{
		itsStartY1 = 1;
		}
		
	itsStartY2 = startY2;
	if (startY2 == 0)
		{
		itsStartY2 = 1;
		}
	
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
	
	itsX1Menu->SetToPopupChoice(kJTrue, itsStartX1);
	itsX2Menu->SetToPopupChoice(kJTrue, itsStartX2);
	itsY1Menu->SetToPopupChoice(kJTrue, itsStartY1);
	itsY2Menu->SetToPopupChoice(kJTrue, itsStartY2);
	itsPlotsMenu->SetToPopupChoice(kJTrue, itsPlotIndex);
	
	itsX1Menu->SetUpdateAction(JXMenu::kDisableNone);
	itsX2Menu->SetUpdateAction(JXMenu::kDisableNone);
	itsY1Menu->SetUpdateAction(JXMenu::kDisableNone);
	itsY2Menu->SetUpdateAction(JXMenu::kDisableNone);
	itsPlotsMenu->SetUpdateAction(JXMenu::kDisableNone);
	
	ListenTo(itsX1Menu);
	ListenTo(itsX2Menu);
	ListenTo(itsY1Menu);
	ListenTo(itsY2Menu);
	ListenTo(itsPlotsMenu);

	itsLabelInput->SetText("Untitled");
}

/******************************************************************************
 GetColumns 

 ******************************************************************************/

void
GXCreateVectorPlotDialog::GetColumns
	(
	JIndex* X1, 
	JIndex* Y1, 
	JIndex* X2,
	JIndex* Y2
	)
{
	*X1 = itsStartX1;
	*Y1 = itsStartY1;
	*X2 = itsStartX2;
	*Y2 = itsStartY2;
}

/******************************************************************************
 GetColumns 

 ******************************************************************************/

void
GXCreateVectorPlotDialog::Receive
	(
	JBroadcaster* sender, 
	const Message& message
	)
{
	if (sender == itsX1Menu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		itsStartX1 = selection->GetIndex();
		}
		
	else if (sender == itsX2Menu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		itsStartX2 = selection->GetIndex();
		}
		
	else if (sender == itsY1Menu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		itsStartY1 = selection->GetIndex();
		}
		
	else if (sender == itsY2Menu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		itsStartY2 = selection->GetIndex();
		}
		
	else if (sender == itsPlotsMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
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
GXCreateVectorPlotDialog::GetPlotIndex
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
GXCreateVectorPlotDialog::GetLabel()
{
	return itsLabelInput->GetText();
}	

/******************************************************************************
 OKToDeactivate 

 ******************************************************************************/

JBoolean 
GXCreateVectorPlotDialog::OKToDeactivate()
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
