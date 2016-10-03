/******************************************************************************
 GNBPTPrintSetupDialog.cc

    BASE CLASS = JXPTPrintSetupDialog

    Copyright (C) 2000 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "GNBPTPrintSetupDialog.h"

#include <JString.h>

#include <JXIntegerInput.h>
#include <JXRadioGroup.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXTextRadioButton.h>
#include <JXWindow.h>

#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GNBPTPrintSetupDialog::GNBPTPrintSetupDialog()
    :
    JXPTPrintSetupDialog()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GNBPTPrintSetupDialog::~GNBPTPrintSetupDialog()
{
}

/******************************************************************************
 PrintSelection

 ******************************************************************************/

JBoolean
GNBPTPrintSetupDialog::WillPrintSelection()
    const
{
    return itsPrintSelectionCB->IsChecked();
}

void
GNBPTPrintSetupDialog::ShouldPrintSelection
    (
    const JBoolean print
    )
{
    itsPrintSelectionCB->SetState(print);
}

/******************************************************************************
 PrintNotes

 ******************************************************************************/

JBoolean
GNBPTPrintSetupDialog::WillPrintNotes()
    const
{
    return itsPrintNotesCB->IsChecked();
}

void
GNBPTPrintSetupDialog::ShouldPrintNotes
    (
    const JBoolean notes
    )
{
    itsPrintNotesCB->SetState(notes);
}

/******************************************************************************
 PrintClosed

 ******************************************************************************/

JBoolean
GNBPTPrintSetupDialog::WillPrintClosed()
    const
{
    return itsPrintClosedCB->IsChecked();
}

void
GNBPTPrintSetupDialog::ShouldPrintClosed
    (
    const JBoolean closed
    )
{
    itsPrintClosedCB->SetState(closed);
}

/******************************************************************************
 BuildWindow

 ******************************************************************************/

void
GNBPTPrintSetupDialog::BuildWindow
    (
    const JXPTPrinter::Destination	dest,
    const JCharacter*				printCmd,
    const JCharacter*				fileName,
    const JBoolean					printLineNumbers,
    const JBoolean					printSel,
    const JBoolean 					printNotes,
    const JBoolean 					printClosed,
    const JBoolean 					hasSelection
    )
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 370,280, "");
    assert( window != NULL );

    JXStaticText* printCmdLabel =
        new JXStaticText("Print command:", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 10,70, 100,19);
    assert( printCmdLabel != NULL );

    JXTextButton* okButton =
        new JXTextButton("Print", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 219,249, 72,22);
    assert( okButton != NULL );
    okButton->SetShortcuts("^M");

    JXTextButton* cancelButton =
        new JXTextButton("Cancel", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 70,250, 70,20);
    assert( cancelButton != NULL );

    JXStaticText* obj1 =
        new JXStaticText("Destination:", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 50,30, 80,20);
    assert( obj1 != NULL );

    JXRadioGroup* destination =
        new JXRadioGroup(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 135,20, 139,39);
    assert( destination != NULL );

    JXTextRadioButton* obj2 =
        new JXTextRadioButton(1, "Printer", destination,
                    JXWidget::kHElastic, JXWidget::kVElastic, 5,8, 70,20);
    assert( obj2 != NULL );
    obj2->SetShortcuts("#P");

    JXTextRadioButton* obj3 =
        new JXTextRadioButton(2, "File", destination,
                    JXWidget::kHElastic, JXWidget::kVElastic, 75,8, 50,20);
    assert( obj3 != NULL );
    obj3->SetShortcuts("#F");

    JXInputField* printCmdInput =
        new JXInputField(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 110,70, 240,20);
    assert( printCmdInput != NULL );

    JXTextButton* chooseFileButton =
        new JXTextButton("Choose file", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 30,70, 80,20);
    assert( chooseFileButton != NULL );
    chooseFileButton->SetShortcuts("#O");

    JXIntegerInput* copyCount =
        new JXIntegerInput(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 140,120, 40,20);
    assert( copyCount != NULL );

    JXIntegerInput* firstPageIndex =
        new JXIntegerInput(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 230,210, 40,20);
    assert( firstPageIndex != NULL );

    JXIntegerInput* lastPageIndex =
        new JXIntegerInput(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 290,210, 40,20);
    assert( lastPageIndex != NULL );

    JXTextCheckbox* printAllCB =
        new JXTextCheckbox("Print all pages", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 40,210, 120,20);
    assert( printAllCB != NULL );
    printAllCB->SetShortcuts("#A");

    JXStaticText* firstPageIndexLabel =
        new JXStaticText("Print page", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 163,213, 66,20);
    assert( firstPageIndexLabel != NULL );

    JXStaticText* lastPageIndexLabel =
        new JXStaticText("to", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 273,213, 16,20);
    assert( lastPageIndexLabel != NULL );

    JXStaticText* obj4 =
        new JXStaticText("Number of copies:", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 25,120, 115,20);
    assert( obj4 != NULL );

    itsPrintSelectionCB =
        new JXTextCheckbox("Print selection", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 200,110, 130,20);
    assert( itsPrintSelectionCB != NULL );
    itsPrintSelectionCB->SetShortcuts("#S");

    JXTextCheckbox* printLineNumbersCB =
        new JXTextCheckbox("Print line numbers", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 200,170, 130,20);
    assert( printLineNumbersCB != NULL );
    printLineNumbersCB->SetShortcuts("#L");

    itsPrintNotesCB =
        new JXTextCheckbox("Print notes", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 200,150, 130,20);
    assert( itsPrintNotesCB != NULL );
    itsPrintNotesCB->SetShortcuts("#N");

    itsPrintClosedCB =
        new JXTextCheckbox("Print closed categories", window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 200,130, 160,20);
    assert( itsPrintClosedCB != NULL );

// end JXLayout

    SetObjects(okButton, cancelButton, destination, dest,
               printCmdLabel, printCmdInput, printCmd,
               chooseFileButton, fileName, copyCount,
               printAllCB, firstPageIndexLabel,
               firstPageIndex, lastPageIndexLabel, lastPageIndex,
               printLineNumbersCB, printLineNumbers);

    if (hasSelection)
        {
        itsPrintSelectionCB->SetState(printSel);
        }
    else
        {
        itsPrintSelectionCB->Deactivate();
        itsPrintSelectionCB->SetState(kJFalse);
        }
    itsPrintNotesCB->SetState(printNotes);
    itsPrintClosedCB->SetState(printClosed);
}
