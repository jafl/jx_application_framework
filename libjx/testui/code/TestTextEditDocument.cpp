/******************************************************************************
 TestTextEditDocument.cpp

	BASE CLASS = JXFileDocument

	Written by John Lindal.

 ******************************************************************************/

#include "TestTextEditDocument.h"
#include "TestTextEditor.h"

#include <jx-af/jx/JXStyledText.h>
#include <jx-af/jx/JXVIKeyHandler.h>
#include <jx-af/jx/JXDocumentManager.h>
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXDocumentMenu.h>
#include <jx-af/jx/JXHorizPartition.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXStandAlonePG.h>
#include <jx-af/jx/JXDeleteObjectTask.h>
#include <jx-af/jx/JXChooseFileDialog.h>
#include <jx-af/jx/jXGlobals.h>

#include <jx-af/jcore/JUndoRedoChain.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

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

	itsText->SetText("ABC Ж Җ ζ Ǽ ậ ϖ Ӝ ἆ Ɽ 转 燜 ㄊ 먄 욶 א ݣ ﺺ Բարեւ");
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

#include "TestTextEditDocument-File.h"
#include "TestTextEditDocument-Emulator.h"

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
		sizes.AppendItem(140);
		minSizes.AppendItem(50);
	}

// begin JXLayout

	auto* window = jnew JXWindow(this, 400,330, JString::empty);

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != nullptr );

	auto* partition =
		jnew JXHorizPartition(sizes, 1, minSizes, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,300);
	assert( partition != nullptr );

// end JXLayout

	AdjustWindowTitle();
	window->SetWMClass("testjx", "TestTextEditDocument");
	window->SetMinSize(20,50);

	itsText = jnew JXStyledText(true, true, GetDisplay()->GetFontManager());
	assert( itsText != nullptr );
	ListenTo(itsText, std::function([this](const JStyledText::TextChanged&)
	{
		if (itsText->GetUndoRedoChain()->IsAtLastSaveLocation())
		{
			DataReverted();
		}
		else
		{
			DataModified();
		}
	}));

	JXContainer* compartment = partition->GetCompartment(1);

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(compartment, JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 100,100);
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
	scrollbarSet->FitToEnclosure(true, true);

	itsTextEditor2 =
		jnew TestTextEditor(itsText, false, fileWritable, nullptr, scrollbarSet,
							scrollbarSet->GetScrollEnclosure(),
							JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsTextEditor2 != nullptr );
	itsTextEditor2->FitToEnclosure();
	itsTextEditor2->ShouldAlwaysShowSelection(true);
	itsTextEditor2->ShareMenus(itsTextEditor1);

	itsFileMenu = menuBar->PrependTextMenu(JGetString("MenuTitle::TestTextEditDocument_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&TestTextEditDocument::UpdateFileMenu,
		&TestTextEditDocument::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);

	JXDocumentMenu* fileListMenu =
		jnew JXDocumentMenu(JGetString("FilesMenuTitle::TestTextEditDocument"), menuBar,
						   JXWidget::kFixedLeft, JXWidget::kVElastic, 0,0, 10,10);
	assert( fileListMenu != nullptr );
	menuBar->PrependMenu(fileListMenu);

	itsEmulatorMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::TestTextEditDocument_Emulator"));
	itsEmulatorMenu->SetMenuItems(kEmulatorMenuStr);
	itsEmulatorMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsEmulatorMenu->AttachHandlers(this,
		&TestTextEditDocument::UpdateEmulatorMenu,
		&TestTextEditDocument::HandleEmulatorMenu);
	ConfigureEmulatorMenu(itsEmulatorMenu);
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
TestTextEditDocument::UpdateFileMenu()
{
	itsFileMenu->SetItemEnabled(kSaveFileCmd, NeedsSave());
	itsFileMenu->SetItemEnabled(kRevertCmd, CanRevert());
	itsFileMenu->SetItemEnabled(kWritePrivateFmtCmd, NeedsSave());
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
	auto* dlog = JXChooseFileDialog::Create(JXChooseFileDialog::kSelectMultipleFiles);

	if (dlog->DoDialog())
	{
		JPtrArray<JString> fullNameList(JPtrArrayT::kDeleteAll);
		dlog->GetFullNames(&fullNameList);

		const JSize count = fullNameList.GetItemCount();
		JXStandAlonePG pg;
		pg.RaiseWhenUpdate();
		pg.FixedLengthProcessBeginning(count, JGetString("OpenFilesProgress::TestTextEditDocument"), true, true);

		for (auto* fileName : fullNameList)
		{
			JXFileDocument* doc;
			if (!JXGetDocumentManager()->FileDocumentIsOpen(*fileName, &doc))
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
	itsText->GetUndoRedoChain()->SetLastSaveLocation();
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
			itsText->GetUndoRedoChain()->SetLastSaveLocation();
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
