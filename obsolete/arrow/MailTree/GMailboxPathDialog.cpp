/******************************************************************************
 GMailboxPathDialog.cc

	Clients must call BuildWindow() after constructing the object.

	BASE CLASS = JXCSFDialogBase

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include <GMailboxPathDialog.h>
#include <JXDirTable.h>
#include <JDirInfo.h>

#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXNewDirButton.h>
#include <JXTextCheckbox.h>
#include <JXPathInput.h>
#include <JXPathHistoryMenu.h>
#include <JXScrollbarSet.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>

#include <JUserNotification.h>
#include <JString.h>
#include <jDirUtil.h>
#include <jAssert.h>

extern JUserNotification*	gUserNotification;

enum
{
	kAbsolutePath = 1,
	kRelativePath
};

/******************************************************************************
 Constructor

 ******************************************************************************/

GMailboxPathDialog::GMailboxPathDialog
	(
	JXDirector*			supervisor,
	JDirInfo*			dirInfo,
	const JCharacter*	fileFilter,
	const JBoolean		showHidden,
	const JBoolean		selectOnlyWritable
	)
	:
	JXChoosePathDialog(supervisor, dirInfo, fileFilter, selectOnlyWritable)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GMailboxPathDialog::~GMailboxPathDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GMailboxPathDialog::BuildWindow
	(
	const JBoolean		showHidden,
	const JCharacter*	message
	)
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 310,375, "");
    assert( window != NULL );
    SetWindow(window);

    JXTextButton* openButton =
        new JXTextButton("Open", window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 219,249, 72,22);
    assert( openButton != NULL );
    openButton->SetShortcuts("^M");

    JXTextButton* cancelButton =
        new JXTextButton("Cancel", window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
    assert( cancelButton != NULL );
    cancelButton->SetShortcuts("^[");

    JXTextButton* upButton =
        new JXTextButton("Up", window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,110, 30,20);
    assert( upButton != NULL );
    upButton->SetShortcuts("#U");

    JXTextButton* homeButton =
        new JXTextButton("Home", window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,110, 40,20);
    assert( homeButton != NULL );
    homeButton->SetShortcuts("#H");

    JXStaticText* pathLabel =
        new JXStaticText("Path:", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
    assert( pathLabel != NULL );

    JXScrollbarSet* scrollbarSet =
        new JXScrollbarSet(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,110, 180,190);
    assert( scrollbarSet != NULL );

    JXNewDirButton* newDirButton =
        new JXNewDirButton(window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 70,20);
    assert( newDirButton != NULL );
    newDirButton->SetShortcuts("#N");

    JXTextButton* selectButton =
        new JXTextButton("Here", window,
                    JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,220, 70,20);
    assert( selectButton != NULL );
    selectButton->SetShortcuts("#E");

    JXPathInput* pathInput =
        new JXPathInput(window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,20, 200,20);
    assert( pathInput != NULL );

    JXTextCheckbox* showHiddenCB =
        new JXTextCheckbox("Show hidden files", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
    assert( showHiddenCB != NULL );
    showHiddenCB->SetShortcuts("#S");

    JXStaticText* filterLabel =
        new JXStaticText("Filter:", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
    assert( filterLabel != NULL );

    JXInputField* filterInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
    assert( filterInput != NULL );

    JXStaticText* explanText =
        new JXStaticText("Go into the directory and click on Here.", window,
                    JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,310, 270,20);
    assert( explanText != NULL );

    JXPathHistoryMenu* pathHistory =
        new JXPathHistoryMenu(1, "", window,
                    JXWidget::kHElastic, JXWidget::kFixedBottom, 260,20, 30,20);
    assert( pathHistory != NULL );

    JXStringHistoryMenu* filterHistory =
        new JXStringHistoryMenu(1, "", window,
                    JXWidget::kHElastic, JXWidget::kFixedBottom, 260,50, 30,20);
    assert( filterHistory != NULL );

// end JXLayout

	SetObjects(scrollbarSet, pathLabel, pathInput, pathHistory,
			   filterLabel, filterInput, filterHistory,
			   openButton, selectButton, cancelButton, upButton, homeButton,
			   newDirButton, showHiddenCB, message);
	itsFormatRG =
        new JXRadioGroup(window,
			JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,350, 270,55);
    assert( itsFormatRG != NULL );

    JXTextRadioButton* obj1 =
        new JXTextRadioButton(1, "Absolute path.", itsFormatRG,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,5, 105,20);
    assert( obj1 != NULL );

    JXTextRadioButton* obj2 =
        new JXTextRadioButton(3, "Path relative to makefile directory.", itsFormatRG,
                    JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,25, 220,20);
    assert( obj2 != NULL );

    itsFormatRG->SelectItem(kAbsolutePath);
}

/******************************************************************************
 IsRelative

 ******************************************************************************/

JBoolean
GMailboxPathDialog::IsRelative()
{
	JIndex item = itsFormatRG->GetSelectedItem();
	if (item == kAbsolutePath)
		{
		return kJFalse;
		}
	return kJTrue;
}
