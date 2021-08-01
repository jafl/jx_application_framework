/******************************************************************************
 TestTextEditDocument.cpp

	BASE CLASS = JXFileDocument

	Written by John Lindal.

 ******************************************************************************/

#include "TestTextEditDocument.h"
#include "TestTextEditor.h"

#include <JXStyledText.h>
#include <JXVIKeyHandler.h>
#include <JXDocumentManager.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXDocumentMenu.h>
#include <JXHorizPartition.h>
#include <JXScrollbarSet.h>
#include <JXStandAlonePG.h>
#include <JXDeleteObjectTask.h>
#include <jXGlobals.h>

#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <sstream>
#include <jAssert.h>

// File menu

static const JUtf8Byte* kFileMenuStr =
	"    New     %h n %k Ctrl-N"
	"  | Open... %h o %k Ctrl-O"
	"  | Save    %h s %k Ctrl-S"
	"  | Save as..."
	"  | Save a copy as..."
	"  | Revert to saved"
	"%l| Load private format... %k Ctrl-Shift-O"
	"  | Save private format... %k Ctrl-Shift-S"
	"%l| Page setup..."
	"  | Print... %h p %k Ctrl-P"
	"%l| Close    %h c %k Ctrl-W";

enum
{
	kNewFileCmd = 1,
	kOpenFileCmd, kSaveFileCmd, kSaveFileAsCmd, kSaveCopyAsCmd, kRevertCmd,
	kReadPrivateFmtCmd, kWritePrivateFmtCmd,
	kPageSetupCmd, kPrintCmd,
	kCloseCmd
};

// Emulator menu

static const JUtf8Byte* kEmulatorMenuStr =
	"    None %r"
	"  | vi   %r";

enum
{
	kNoEmulatorCmd = 1,
	kVIEmulatorCmd
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
				   false, true, ""),
	itsEmulatorType(kNoEmulator),
	itsWritePrivateFmtFlag(false)
{
	BuildWindow(true);

	// start with some challenging glyphs

	itsText->SetText(JString("ABC Ж Җ ζ Ǽ ậ ϖ Ӝ ἆ Ɽ 转 燜 ㄊ 먄 욶 א ݣ ﺺ Բարեւ", JString::kNoCopy));
}

TestTextEditDocument::TestTextEditDocument
	(
	JXDirector*		supervisor,
	const JString&	fileName,
	const bool		privateFmt
	)
	:
	JXFileDocument(supervisor, fileName, true, true, ""),
	itsEmulatorType(kNoEmulator),
	itsWritePrivateFmtFlag(privateFmt)
{
	assert( JFileExists(fileName) );

	BuildWindow(JFileWritable(fileName));
	ReadFile(fileName);
	itsWritePrivateFmtFlag = false;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestTextEditDocument::~TestTextEditDocument()
{
	JXDeleteObjectTask<JStyledText>::Delete(itsText);
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
TestTextEditDocument::BuildWindow
	(
	const bool fileWritable
	)
{
	JArray<JCoordinate> sizes;
	JArray<JCoordinate> minSizes;

	for (JIndex i=1; i<=2; i++)
		{
		sizes.AppendElement(140);
		minSizes.AppendElement(50);
		}

// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 400,330, JString::empty);
	assert( window != nullptr );

	JXMenuBar* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != nullptr );

	JXHorizPartition* partition =
		jnew JXHorizPartition(sizes, 1, minSizes, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,300);
	assert( partition != nullptr );

// end JXLayout

	AdjustWindowTitle();
	window->SetWMClass("testjx", "TestTextEditDocument");
	window->SetMinSize(20,50);

	itsText = jnew JXStyledText(true, true, GetDisplay()->GetFontManager());
	assert( itsText != nullptr );
	ListenTo(itsText);

	JXContainer* compartment = partition->GetCompartment(1);

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(compartment, JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( scrollbarSet != nullptr );
	scrollbarSet->FitToEnclosure();

	itsTextEditor1 =
		jnew TestTextEditor(itsText, false, fileWritable, menuBar, scrollbarSet,
							scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsTextEditor1 != nullptr );
	itsTextEditor1->FitToEnclosure();
	itsTextEditor1->ShouldAlwaysShowSelection(true);

	compartment = partition->GetCompartment(2);

	scrollbarSet =
		jnew JXScrollbarSet(compartment, JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
	assert( scrollbarSet != nullptr );
	scrollbarSet->FitToEnclosure(true, true);

	itsTextEditor2 =
		jnew TestTextEditor(itsText, false, fileWritable, nullptr, scrollbarSet,
							scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsTextEditor2 != nullptr );
	itsTextEditor2->FitToEnclosure();
	itsTextEditor2->ShouldAlwaysShowSelection(true);
	itsTextEditor2->ShareMenus(itsTextEditor1);

	itsFileMenu = menuBar->PrependTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetShortcuts(JGetString("FileMenuShortcut::TestTextEditDocument"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsFileMenu);

	JXDocumentMenu* fileListMenu =
		jnew JXDocumentMenu(JGetString("FilesMenuTitle::TestTextEditDocument"), menuBar,
						   JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( fileListMenu != nullptr );
	menuBar->PrependMenu(fileListMenu);

	itsEmulatorMenu = menuBar->AppendTextMenu(JGetString("EmulatorMenuTitle::TestTextEditDocument"));
	itsEmulatorMenu->SetMenuItems(kEmulatorMenuStr);
	itsEmulatorMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsEmulatorMenu);
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
		assert( selection != nullptr );
		HandleFileMenu(selection->GetIndex());
		}

	else if (sender == itsEmulatorMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateEmulatorMenu();
		}
	else if (sender == itsEmulatorMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleEmulatorMenu(selection->GetIndex());
		}

	else if (sender == itsText && message.Is(JStyledText::kTextChanged))
		{
		if (itsText->IsAtLastSaveLocation())
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
	itsFileMenu->SetItemEnable(kWritePrivateFmtCmd, NeedsSave());
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
		assert( doc != nullptr );
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

	else if (index == kReadPrivateFmtCmd)
		{
		itsWritePrivateFmtFlag = true;
		OpenFiles();
		itsWritePrivateFmtFlag = false;
		}
	else if (index == kWritePrivateFmtCmd)
		{
		itsWritePrivateFmtFlag = true;
		SaveCopyInNewFile();
		itsWritePrivateFmtFlag = false;
		}

	else if (index == kPageSetupCmd)
		{
		itsTextEditor1->HandlePSPageSetup();
		}
	else if (index == kPrintCmd)
		{
		itsTextEditor1->PrintPS();
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
	if (csf->ChooseFiles(JGetString("ChooseFilesPrompt::TestTextEditDocument"), JString::empty, &fullNameList))
		{
		const JSize count = fullNameList.GetElementCount();
		JXStandAlonePG pg;
		pg.RaiseWhenUpdate();
		pg.FixedLengthProcessBeginning(count, JGetString("OpenFilesProgress::TestTextEditDocument"), true, false);

		for (auto* fileName : fullNameList)
			{
			JXFileDocument* doc;
			if (!(JXGetDocumentManager())->FileDocumentIsOpen(*fileName, &doc))
				{
				doc = jnew TestTextEditDocument(GetSupervisor(), *fileName, itsWritePrivateFmtFlag);
				}
			assert( doc != nullptr );
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
	bool onDisk;
	const JString fullName = GetFullName(&onDisk);
	if (onDisk)
		{
		ReadFile(fullName);
		}
	else
		{
		itsText->SetText(JString::empty);
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
	if (itsWritePrivateFmtFlag)
		{
		std::ifstream input(fileName.GetBytes());
		itsText->ReadPrivateFormat(input);
		}
	else
		{
		JStyledText::PlainTextFormat format;
		itsText->ReadPlainText(fileName, &format);
		}
	itsText->SetLastSaveLocation();
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
	const bool	safetySave
	)
	const
{
	if (itsWritePrivateFmtFlag)
		{
		std::ostringstream data;
		itsText->WritePrivateFormat(data);
		output << data.str().c_str();
		}
	else
		{
		itsText->WritePlainText(output, JStyledText::kUNIXText);
		}

	if (!safetySave)
		{
		itsText->DeactivateCurrentUndo();
		if (output.good())
			{
			itsText->SetLastSaveLocation();
			}
		}
}

/******************************************************************************
 UpdateEmulatorMenu (private)

 ******************************************************************************/

void
TestTextEditDocument::UpdateEmulatorMenu()
{
	itsEmulatorMenu->CheckItem(itsEmulatorType);
}

/******************************************************************************
 HandleEmulatorMenu (private)

 ******************************************************************************/

void
TestTextEditDocument::HandleEmulatorMenu
	(
	const JIndex index
	)
{
	if (index == kNoEmulatorCmd)
		{
		itsTextEditor2->SetKeyHandler(nullptr);
		itsEmulatorType = kNoEmulator;
		}
	else if (index == kVIEmulatorCmd)
		{
		itsTextEditor2->SetKeyHandler(jnew JXVIKeyHandler);
		itsEmulatorType = kVIEmulator;
		}
}
