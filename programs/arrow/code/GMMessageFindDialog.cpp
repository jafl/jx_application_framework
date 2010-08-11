/******************************************************************************
 GMMessageFindDialog.cc

	BASE CLASS = JXDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "GMMessageFindDialog.h"
#include "GMessageTableDir.h"

#include <JXInputField.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXWindow.h>

#include <JString.h>

#include <jXGlobals.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GMMessageFindDialog::GMMessageFindDialog
	(
	JXDirector* supervisor
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMMessageFindDialog::~GMMessageFindDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GMMessageFindDialog::BuildWindow()
{

// begin JXLayout

    JXWindow* window = new JXWindow(this, 350,120, "");
    assert( window != NULL );

    itsSearchInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 85,20, 200,20);
    assert( itsSearchInput != NULL );

    JXStaticText* obj1_JXLayout =
        new JXStaticText(JGetString("obj1_JXLayout::GMMessageFindDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 25,20, 60,20);
    assert( obj1_JXLayout != NULL );
    obj1_JXLayout->SetToLabel();

    itsMessageBodyCB =
        new JXTextCheckbox(JGetString("itsMessageBodyCB::GMMessageFindDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 85,50, 200,20);
    assert( itsMessageBodyCB != NULL );
    itsMessageBodyCB->SetShortcuts(JGetString("itsMessageBodyCB::GMMessageFindDialog::shortcuts::JXLayout"));

    JXTextButton* cancelButton =
        new JXTextButton(JGetString("cancelButton::GMMessageFindDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 30,90, 70,20);
    assert( cancelButton != NULL );
    cancelButton->SetShortcuts(JGetString("cancelButton::GMMessageFindDialog::shortcuts::JXLayout"));

    itsHelpButton =
        new JXTextButton(JGetString("itsHelpButton::GMMessageFindDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 140,90, 70,20);
    assert( itsHelpButton != NULL );

    JXTextButton* searchButton =
        new JXTextButton(JGetString("searchButton::GMMessageFindDialog::JXLayout"), window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 250,90, 70,20);
    assert( searchButton != NULL );
    searchButton->SetShortcuts(JGetString("searchButton::GMMessageFindDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle("Find message");
	SetButtons(searchButton, cancelButton);
}

/******************************************************************************
 GetSearchString

 ******************************************************************************/

const JString&
GMMessageFindDialog::GetSearchString()
	const
{
	return itsSearchInput->GetText();
}

/******************************************************************************
 SearchMessage (public)

 ******************************************************************************/

JBoolean
GMMessageFindDialog::SearchMessage()
	const
{
	return itsMessageBodyCB->IsChecked();
}
