/******************************************************************************
 FitModuleDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include <glStdInc.h>
#include "FitModuleDialog.h"
#include "GLPlotApp.h"
#include "GLGlobals.h"

#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextMenu.h>
#include <JXStaticText.h>
#include <JPtrArray.h>
#include <JString.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

FitModuleDialog::FitModuleDialog
	(
	JXWindowDirector* supervisor
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FitModuleDialog::~FitModuleDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
FitModuleDialog::BuildWindow()
{
	
// begin JXLayout

    JXWindow* window = new JXWindow(this, 270,100, "");
    assert( window != NULL );

    JXStaticText* obj1 =
        new JXStaticText("Please select the appropriate module.", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 30,10, 230,20);
    assert( obj1 != NULL );

    itsFilterMenu =
        new JXTextMenu("Filter:", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 70,40, 70,20);
    assert( itsFilterMenu != NULL );
	
    itsOKButton =
        new JXTextButton("OK", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 20,70, 70,20);
    assert( itsOKButton != NULL );
    itsOKButton->SetShortcuts("^M");

    JXTextButton* cancelButton =
        new JXTextButton("Cancel", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 180,70, 70,20);
    assert( cancelButton != NULL );
    cancelButton->SetShortcuts("^[");

	itsReloadButton =
        new JXTextButton("Reload", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 100,70, 70,20);
    assert( itsReloadButton != NULL );

// end JXLayout

	window->SetTitle("Choose fit module");
	SetButtons(itsOKButton, cancelButton);
		
	JPtrArray<JString>* names = (GLGetApplication())->GetFitModules();
	
	const JSize strCount = names->GetElementCount();
	
	for (JSize i = 1; i <= strCount; i++)
		{
		itsFilterMenu->AppendItem(*(names->NthElement(i)));
		}

	itsFilterIndex = 1;
	
	itsFilterMenu->SetToPopupChoice(kJTrue, itsFilterIndex);
	itsFilterMenu->SetUpdateAction(JXMenu::kDisableNone);	
	ListenTo(itsFilterMenu);
	if (strCount == 0)
		{
		itsFilterMenu->Deactivate();
		itsOKButton->Deactivate();
		}
	ListenTo(itsReloadButton);
}

/******************************************************************************
 Receive 

 ******************************************************************************/

void
FitModuleDialog::Receive
	(
	JBroadcaster* sender, 
	const Message& message
	)
{
	if (sender == itsFilterMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		itsFilterIndex = selection->GetIndex();
		}
		
	else if (sender == itsReloadButton && message.Is(JXButton::kPushed))
		{
		(GLGetApplication())->ReloadFitModules();
		itsFilterMenu->RemoveAllItems();
		JPtrArray<JString>* names = (GLGetApplication())->GetFitModules();
		const JSize strCount = names->GetElementCount();
		for (JSize i = 1; i <= strCount; i++)
			{
			itsFilterMenu->AppendItem(*(names->NthElement(i)));
			}
		itsFilterIndex = 1;
		itsFilterMenu->SetToPopupChoice(kJTrue, itsFilterIndex);
		if (strCount == 0)
			{
			itsFilterMenu->Deactivate();
			itsOKButton->Deactivate();
			}
		else
			{
			itsFilterMenu->Activate();
			itsOKButton->Activate();
			}
		}
		
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 GetFilterIndex 

 ******************************************************************************/

JIndex 
FitModuleDialog::GetFilterIndex()
{
	return itsFilterIndex;
}
