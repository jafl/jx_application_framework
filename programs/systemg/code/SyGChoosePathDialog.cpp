/******************************************************************************
 SyGChoosePathDialog.cpp

	Clients must call BuildWindow() after constructing the object.
	Derived classes must override BuildWindow() and call SetObjects()
	at the end of their implementation.

	BASE CLASS = JXChoosePathDialog

	Copyright © 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <SyGChoosePathDialog.h>
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXPathInput.h>
#include <JXPathHistoryMenu.h>
#include <JXNewDirButton.h>
#include <JXScrollbarSet.h>
#include <JXCurrentPathMenu.h>
#include <jGlobals.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SyGChoosePathDialog::SyGChoosePathDialog
	(
	JXDirector*			supervisor,
	JDirInfo*			dirInfo,
	const JCharacter*	fileFilter,
	const JBoolean		selectOnlyWritable
	)
	:
	JXChoosePathDialog(supervisor, dirInfo, fileFilter, selectOnlyWritable)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SyGChoosePathDialog::~SyGChoosePathDialog()
{
}

/******************************************************************************
 BuildWindow

 ******************************************************************************/

void
SyGChoosePathDialog::BuildWindow
	(
	const JBoolean		newWindow,
	const JCharacter*	message
	)
{
// begin JXLayout

	JXWindow* window = new JXWindow(this, 310,400, "");
	assert( window != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::SyGChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::SyGChoosePathDialog::shortcuts::JXLayout"));

	JXTextButton* homeButton =
		new JXTextButton(JGetString("homeButton::SyGChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
	assert( homeButton != NULL );
	homeButton->SetShortcuts(JGetString("homeButton::SyGChoosePathDialog::shortcuts::JXLayout"));

	JXStaticText* pathLabel =
		new JXStaticText(JGetString("pathLabel::SyGChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
	assert( pathLabel != NULL );
	pathLabel->SetToLabel();

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,140, 180,190);
	assert( scrollbarSet != NULL );

	itsSelectButton =
		new JXTextButton(JGetString("itsSelectButton::SyGChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,250, 70,20);
	assert( itsSelectButton != NULL );
	itsSelectButton->SetShortcuts(JGetString("itsSelectButton::SyGChoosePathDialog::shortcuts::JXLayout"));

	JXPathInput* pathInput =
		new JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 200,20);
	assert( pathInput != NULL );

	JXTextCheckbox* showHiddenCB =
		new JXTextCheckbox(JGetString("showHiddenCB::SyGChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
	assert( showHiddenCB != NULL );
	showHiddenCB->SetShortcuts(JGetString("showHiddenCB::SyGChoosePathDialog::shortcuts::JXLayout"));

	JXStaticText* filterLabel =
		new JXStaticText(JGetString("filterLabel::SyGChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
	assert( filterLabel != NULL );
	filterLabel->SetToLabel();

	JXInputField* filterInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
	assert( filterInput != NULL );

	JXStaticText* explanText =
		new JXStaticText(JGetString("explanText::SyGChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,340, 270,20);
	assert( explanText != NULL );
	explanText->SetToLabel();

	JXPathHistoryMenu* pathHistory =
		new JXPathHistoryMenu(1, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
	assert( pathHistory != NULL );

	JXStringHistoryMenu* filterHistory =
		new JXStringHistoryMenu(1, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
	assert( filterHistory != NULL );

	JXTextButton* upButton =
		new JXTextButton(JGetString("upButton::SyGChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
	assert( upButton != NULL );
	upButton->SetShortcuts(JGetString("upButton::SyGChoosePathDialog::shortcuts::JXLayout"));

	JXNewDirButton* newDirButton =
		new JXNewDirButton(window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
	assert( newDirButton != NULL );

	itsNewWindowCB =
		new JXTextCheckbox(JGetString("itsNewWindowCB::SyGChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,370, 150,20);
	assert( itsNewWindowCB != NULL );
	itsNewWindowCB->SetShortcuts(JGetString("itsNewWindowCB::SyGChoosePathDialog::shortcuts::JXLayout"));

	JXCurrentPathMenu* currPathMenu =
		new JXCurrentPathMenu("/", window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 20,110, 180,20);
	assert( currPathMenu != NULL );

	itsOpenButton =
		new JXTextButton(JGetString("itsOpenButton::SyGChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
	assert( itsOpenButton != NULL );
	itsOpenButton->SetShortcuts(JGetString("itsOpenButton::SyGChoosePathDialog::shortcuts::JXLayout"));

	JXTextButton* desktopButton =
		new JXTextButton(JGetString("desktopButton::SyGChoosePathDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
	assert( desktopButton != NULL );

// end JXLayout

	itsNewWindowCB->SetState(newWindow);

	SetObjects(scrollbarSet, pathLabel, pathInput, pathHistory,
			   filterLabel, filterInput, filterHistory,
			   itsOpenButton, itsSelectButton, cancelButton,
			   upButton, homeButton, desktopButton,
			   newDirButton, showHiddenCB, currPathMenu, message);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SyGChoosePathDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXChoosePathDialog::Receive(sender, message);
}

/******************************************************************************
 OpenInNewWindow (public)

 ******************************************************************************/

JBoolean
SyGChoosePathDialog::OpenInNewWindow()
	const
{
	return itsNewWindowCB->IsChecked();
}
