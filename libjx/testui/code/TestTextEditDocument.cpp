/******************************************************************************
 TestTextEditDocument.cpp

	BASE CLASS = JXFileDocument

	Written by John Lindal.

 ******************************************************************************/

#include "TestTextEditDocument.h"
#include "TestTextEditor.h"

#include <JXDocumentManager.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXDocumentMenu.h>
#include <JXScrollbarSet.h>
#include <JXStandAlonePG.h>
#include <jXGlobals.h>

#include <JStyledText.h>
#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

// File menu

static const JUtf8Byte* kFileMenuStr =
	"    New %h n %k Ctrl-N"
	"  | Open... %h o %k Ctrl-O"
	"  | Save %h s %k Ctrl-S"
	"  | Save as..."
	"  | Save a copy as..."
	"  | Revert to saved"
	"%l| Page setup..."
	"  | Print... %h p %k Ctrl-P"
	"%l| Close %h c %k Ctrl-W";

enum
{
	kNewFileCmd = 1,
	kOpenFileCmd, kSaveFileCmd, kSaveFileAsCmd, kSaveCopyAsCmd, kRevertCmd,
	kPageSetupCmd, kPrintCmd,
	kCloseCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

TestTextEditDocument::TestTextEditDocument
	(
	JXDirector* supervisor
	)
	:
	JXFileDocument(supervisor,
				   (JXGetDocumentManager())->GetNewFileName(),
				   kJFalse, kJTrue, JString::empty)
{
	BuildWindow(kJTrue);
}

TestTextEditDocument::TestTextEditDocument
	(
	JXDirector*		supervisor,
	const JString&	fileName
	)
	:
	JXFileDocument(supervisor, fileName, kJTrue, kJTrue, JString::empty)
{
	assert( JFileExists(fileName) );

	BuildWindow(JFileWritable(fileName));
	ReadFile(fileName);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestTextEditDocument::~TestTextEditDocument()
{
	jdelete itsText;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
TestTextEditDocument::BuildWindow
	(
	const JBoolean fileWritable
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 400,330, JString::empty);
	assert( window != NULL );

	JXMenuBar* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != NULL );

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,300);
	assert( scrollbarSet != NULL );

// end JXLayout

	AdjustWindowTitle();
	window->SetWMClass("testjx", "TestTextEditDocument");
	window->SetMinSize(20,50);

	itsText = jnew JStyledText(kJTrue, kJTrue);
	assert( itsText != NULL );

	itsTextEditor =
		jnew TestTextEditor(itsText, kJFalse, fileWritable, menuBar, scrollbarSet,
							scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsTextEditor != NULL );
	itsTextEditor->FitToEnclosure();
	ListenTo(itsTextEditor->GetText());

	itsFileMenu = menuBar->PrependTextMenu(JGetString("FileMenuTitle::TestTextEditDocument"));
	itsFileMenu->SetShortcuts(JGetString("FileMenuShortcut::TestTextEditDocument"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	JXDocumentMenu* fileListMenu =
		jnew JXDocumentMenu(JGetString("FilesMenuTitle::TestTextEditDocument"), menuBar,
						   JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( fileListMenu != NULL );
	menuBar->PrependMenu(fileListMenu);
}

/******************************************************************************
 Receive (protected)

	Listen for menu update requests and menu selections.

 ******************************************************************************/

void
TestTextEditDocument::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateFileMenu();
		}
	else if (sender == itsFileMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		HandleFileMenu(selection->GetIndex());
		}

	else if (sender == itsTextEditor->GetText() && message.Is(JStyledText::kTextChanged))
		{
		if (itsTextEditor->GetText()->IsAtLastSaveLocation())
			{
			DataReverted();
			}
		else
			{
			DataModified();
			}
		}

	else
		{
		JXFileDocument::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
TestTextEditDocument::UpdateFileMenu()
{
	itsFileMenu->SetItemEnable(kSaveFileCmd, NeedsSave());
	itsFileMenu->SetItemEnable(kRevertCmd, CanRevert());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
TestTextEditDocument::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kNewFileCmd)
		{
		TestTextEditDocument* doc = jnew TestTextEditDocument(GetSupervisor());
		assert( doc != NULL );
		doc->Activate();
		}
	else if (index == kOpenFileCmd)
		{
		OpenFiles();
		}
	else if (index == kSaveFileCmd)
		{
		SaveInCurrentFile();
		}
	else if (index == kSaveFileAsCmd)
		{
		SaveInNewFile();
		}
	else if (index == kSaveCopyAsCmd)
		{
		SaveCopyInNewFile();
		}
	else if (index == kRevertCmd)
		{
		RevertToSaved();
		}
	else if (index == kPageSetupCmd)
		{
		itsTextEditor->HandlePSPageSetup();
		}
	else if (index == kPrintCmd)
		{
		itsTextEditor->PrintPS();
		}
	else if (index == kCloseCmd)
		{
		Close();
		}
}

/******************************************************************************
 OpenFiles

 ******************************************************************************/

void
TestTextEditDocument::OpenFiles()
{
	JChooseSaveFile* csf = JGetChooseSaveFile();
	JPtrArray<JString> fullNameList(JPtrArrayT::kDeleteAll);
	if (csf->ChooseFiles(JGetString("ChooseFilesPrompt::TestTextEditDocument"), NULL, &fullNameList))
		{
		const JSize count = fullNameList.GetElementCount();
		JXStandAlonePG pg;
		pg.RaiseWhenUpdate();
		pg.FixedLengthProcessBeginning(count, JGetString("OpenFilesProgress::TestTextEditDocument"), kJTrue, kJFalse);

		for (JIndex i=1; i<=count; i++)
			{
			const JString& fileName = *(fullNameList.GetElement(i));

			JXFileDocument* doc;
			if (!(JXGetDocumentManager())->FileDocumentIsOpen(fileName, &doc))
				{
				doc = jnew TestTextEditDocument(GetSupervisor(), fileName);
				}
			assert( doc != NULL );
			doc->Activate();

			if (!pg.IncrementProgress())
				{
				break;
				}
			}

		pg.ProcessFinished();
		}
}

/******************************************************************************
 RevertToSaved (private)

 ******************************************************************************/

void
TestTextEditDocument::RevertToSaved()
{
	if (OKToRevert())
		{
		DiscardChanges();
		}
}

/******************************************************************************
 DiscardChanges (virtual protected)

 ******************************************************************************/

void
TestTextEditDocument::DiscardChanges()
{
	JBoolean onDisk;
	const JString fullName = GetFullName(&onDisk);
	if (onDisk)
		{
		ReadFile(fullName);
		}
	else
		{
		itsTextEditor->GetText()->SetText(JString::empty);
		}

	DataReverted();
}

/******************************************************************************
 ReadFile (protected)

 ******************************************************************************/

void
TestTextEditDocument::ReadFile
	(
	const JString& fileName
	)
{
	JStyledText::PlainTextFormat format;
	itsTextEditor->GetText()->ReadPlainText(fileName, &format);
	itsTextEditor->GetText()->SetLastSaveLocation();
}

/******************************************************************************
 WriteTextFile (virtual protected)

	JTextEditor only broadcasts TextChanged when the user begins typing
	at a new position.  If the user saves and then keeps typing at the same
	position, the document will not know that more changes need to be saved.
	By calling DeactivateCurrentUndo(), we get another TextChanged message
	in this case, at the cost of interrupting the undo mechanism.  We have
	to use the optimized (i.e. minimum number of) TextChanged messages
	to avoid wasting time at every key press.

	We must not do this when safety saving, however, because safety save
	doesn't change the save status of the document, and because we shouldn't
	affect the undo mechanism when the user takes no action.

 ******************************************************************************/

void
TestTextEditDocument::WriteTextFile
	(
	std::ostream&	output,
	const JBoolean	safetySave
	)
	const
{
	itsTextEditor->GetText()->WritePlainText(output, JStyledText::kUNIXText);
	if (!safetySave)
		{
		itsTextEditor->GetText()->DeactivateCurrentUndo();
		if (output.good())
			{
			itsTextEditor->GetText()->SetLastSaveLocation();
			}
		}
}
