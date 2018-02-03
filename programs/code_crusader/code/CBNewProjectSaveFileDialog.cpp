/******************************************************************************
 CBNewProjectSaveFileDialog.cpp

	BASE CLASS = JXSaveFileDialog

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#include "CBNewProjectSaveFileDialog.h"
#include "CBProjectDocument.h"
#include "CBBuildManager.h"
#include <JXWindow.h>
#include <JXStaticText.h>
#include <JXSaveFileInput.h>
#include <JXPathInput.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXRadioGroup.h>
#include <JXTextRadioButton.h>
#include <JXPathHistoryMenu.h>
#include <JXCurrentPathMenu.h>
#include <JXScrollbarSet.h>
#include <JXNewDirButton.h>
#include <jXGlobals.h>
#include <JDirInfo.h>
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jAssert.h>

// Template menu

static const JCharacter* kTemplateMenuStr = "None %r %l";

enum
{
	kNoTemplateCmd = 1
};

static const JCharacter* kUserTemplateMarker = " (personal)";
static const JCharacter* kSysTemplateMarker  = " (global)";

// string ID's

static const JCharacter* kWarnFileExistsID = "WarnFileExists::CBNewProjectSaveFileDialog";

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

CBNewProjectSaveFileDialog*
CBNewProjectSaveFileDialog::Create
	(
	JXDirector*								supervisor,
	JDirInfo*								dirInfo,
	const JCharacter*						fileFilter,
	const JCharacter*						templateFile,
	const CBBuildManager::MakefileMethod	method,
	const JCharacter*						origName,
	const JCharacter*						prompt,
	const JCharacter*						message
	)
{
	CBNewProjectSaveFileDialog* dlog =
		jnew CBNewProjectSaveFileDialog(supervisor, dirInfo, fileFilter, method);
	assert( dlog != NULL );
	dlog->BuildWindow(origName, prompt, message);
	dlog->BuildTemplateMenu(templateFile);
	return dlog;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CBNewProjectSaveFileDialog::CBNewProjectSaveFileDialog
	(
	JXDirector*								supervisor,
	JDirInfo*								dirInfo,
	const JCharacter*						fileFilter,
	const CBBuildManager::MakefileMethod	method
	)
	:
	JXSaveFileDialog(supervisor, dirInfo, fileFilter)
{
	itsMakefileMethod = method;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBNewProjectSaveFileDialog::~CBNewProjectSaveFileDialog()
{
}

/******************************************************************************
 GetMakefileMethod

	itsMakefileMethod is only stored for use by BuildWindow().

 ******************************************************************************/

CBBuildManager::MakefileMethod
CBNewProjectSaveFileDialog::GetMakefileMethod()
	const
{
	return (CBBuildManager::MakefileMethod) itsMethodRG->GetSelectedItem();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
CBNewProjectSaveFileDialog::BuildWindow
	(
	const JCharacter*	origName,
	const JCharacter*	prompt,
	const JCharacter*	message
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 310,500, "");
	assert( window != NULL );

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,140, 180,140);
	assert( scrollbarSet != NULL );

	JXTextButton* saveButton =
		jnew JXTextButton(JGetString("saveButton::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
	assert( saveButton != NULL );
	saveButton->SetShortcuts(JGetString("saveButton::CBNewProjectSaveFileDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != NULL );

	JXTextButton* homeButton =
		jnew JXTextButton(JGetString("homeButton::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
	assert( homeButton != NULL );

	JXStaticText* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
	assert( pathLabel != NULL );
	pathLabel->SetToLabel();

	JXSaveFileInput* fileNameInput =
		jnew JXSaveFileInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,310, 180,20);
	assert( fileNameInput != NULL );

	JXPathInput* pathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 200,20);
	assert( pathInput != NULL );

	JXTextCheckbox* showHiddenCB =
		jnew JXTextCheckbox(JGetString("showHiddenCB::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
	assert( showHiddenCB != NULL );

	JXStaticText* promptLabel =
		jnew JXStaticText(JGetString("promptLabel::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,290, 180,20);
	assert( promptLabel != NULL );
	promptLabel->SetToLabel();

	JXStaticText* filterLabel =
		jnew JXStaticText(JGetString("filterLabel::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
	assert( filterLabel != NULL );
	filterLabel->SetToLabel();

	JXInputField* filterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
	assert( filterInput != NULL );

	JXStaticText* makefileTitle =
		jnew JXStaticText(JGetString("makefileTitle::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,420, 140,20);
	assert( makefileTitle != NULL );
	makefileTitle->SetToLabel();

	itsMethodRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 160,390, 132,106);
	assert( itsMethodRG != NULL );

	JXTextRadioButton* manualRB =
		jnew JXTextRadioButton(CBBuildManager::kManual, JGetString("manualRB::CBNewProjectSaveFileDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 110,20);
	assert( manualRB != NULL );

	JXTextRadioButton* qmakeRB =
		jnew JXTextRadioButton(CBBuildManager::kQMake, JGetString("qmakeRB::CBNewProjectSaveFileDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 110,20);
	assert( qmakeRB != NULL );

	JXTextRadioButton* makemakeRB =
		jnew JXTextRadioButton(CBBuildManager::kMakemake, JGetString("makemakeRB::CBNewProjectSaveFileDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 110,20);
	assert( makemakeRB != NULL );

	JXPathHistoryMenu* pathHistory =
		jnew JXPathHistoryMenu(1, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
	assert( pathHistory != NULL );

	JXStringHistoryMenu* filterHistory =
		jnew JXStringHistoryMenu(1, "", window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
	assert( filterHistory != NULL );

	JXTextButton* upButton =
		jnew JXTextButton(JGetString("upButton::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
	assert( upButton != NULL );

	JXNewDirButton* newDirButton =
		jnew JXNewDirButton(window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
	assert( newDirButton != NULL );

	JXCurrentPathMenu* currPathMenu =
		jnew JXCurrentPathMenu("/", window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,110, 180,20);
	assert( currPathMenu != NULL );

	itsTemplateMenu =
		jnew JXTextMenu(JGetString("itsTemplateMenu::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,350, 270,30);
	assert( itsTemplateMenu != NULL );

	JXTextRadioButton* cmakeRB =
		jnew JXTextRadioButton(CBBuildManager::kCMake, JGetString("cmakeRB::CBNewProjectSaveFileDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 110,20);
	assert( cmakeRB != NULL );

	JXTextButton* desktopButton =
		jnew JXTextButton(JGetString("desktopButton::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
	assert( desktopButton != NULL );

// end JXLayout

	SetObjects(scrollbarSet, promptLabel, prompt, fileNameInput, origName,
			   pathLabel, pathInput, pathHistory,
			   filterLabel, filterInput, filterHistory,
			   saveButton, cancelButton, upButton, homeButton,
			   desktopButton, newDirButton,
			   showHiddenCB, currPathMenu, message);

	itsMethodRG->SelectItem(itsMakefileMethod);
}

/******************************************************************************
 BuildTemplateMenu (private)

 ******************************************************************************/

void
CBNewProjectSaveFileDialog::BuildTemplateMenu
	(
	const JCharacter* templateFile
	)
{
	itsTemplateIndex = kNoTemplateCmd;

	itsTemplateMenu->SetMenuItems(kTemplateMenuStr);
	itsTemplateMenu->SetUpdateAction(JXMenu::kDisableNone);
	ListenTo(itsTemplateMenu);

	JPtrArray<JString> menuText(JPtrArrayT::kDeleteAll);
	menuText.SetCompareFunction(JCompareStringsCaseInsensitive);
	menuText.SetSortOrder(JListT::kSortAscending);

	JString sysDir, userDir;
	if (JXGetProgramDataDirectories(CBProjectDocument::GetTemplateDirectoryName(),
									&sysDir, &userDir))
		{
		JString* menuTextStr = NULL;
		BuildTemplateMenuItems(sysDir,  kJFalse, &menuText, templateFile, &menuTextStr);
		BuildTemplateMenuItems(userDir, kJTrue,  &menuText, templateFile, &menuTextStr);

		const JSize count = menuText.GetElementCount();
		JString itemText, nmShortcut;
		for (JIndex i=1; i<=count; i++)
			{
			// We have to extract user/sys here because otherwise we would
			// have to keep extra state while building the sorted list.

			itemText = *(menuText.GetElement(i));

			JIndex j;
			const JBoolean found = itemText.LocateLastSubstring(" (", &j);
			assert( found );

			const JIndexRange r(j, itemText.GetLength());
			nmShortcut = itemText.GetSubstring(r);
			itemText.RemoveSubstring(r);

			itsTemplateMenu->AppendItem(itemText, JXMenu::kRadioType);
			itsTemplateMenu->SetItemNMShortcut(itsTemplateMenu->GetItemCount(), nmShortcut);

			// mark item corresponding to initial template selection

			if (menuText.GetElement(i) == menuTextStr)
				{
				itsTemplateIndex = itsTemplateMenu->GetItemCount();
				}
			}
		}

	// after selecting initial template

	itsTemplateMenu->SetToPopupChoice(kJTrue, itsTemplateIndex);
	UpdateMakefileMethod();
}

/******************************************************************************
 BuildTemplateMenuItems (private)

	Builds a sorted list of the valid template files below the given path.

 ******************************************************************************/

void
CBNewProjectSaveFileDialog::BuildTemplateMenuItems
	(
	const JCharacter*	path,
	const JBoolean		isUserPath,
	JPtrArray<JString>*	menuText,
	const JCharacter*	templateFile,
	JString**			menuTextStr
	)
	const
{
	JDirInfo* info = NULL;
	if (JDirInfo::Create(path, &info))
		{
		info->ShowDirs(kJFalse);

		const JSize count = info->GetEntryCount();
		JString fullName, templateType;
		for (JIndex i=1; i<=count; i++)
			{
			fullName = (info->GetEntry(i)).GetFullName();
			if (CBProjectDocument::GetProjectTemplateType(fullName, &templateType))
				{
				JString* s = jnew JString((info->GetEntry(i)).GetName());
				assert( s != NULL );

				if (isUserPath)
					{
					*s += kUserTemplateMarker;
					}
				else
					{
					*s += kSysTemplateMarker;
					}

				menuText->InsertSorted(s);

				// save item corresponding to initial template selection

				if (JSameDirEntry(templateFile, fullName))
					{
					*menuTextStr = s;
					}
				}
			}
		}
}

/******************************************************************************
 GetProjectTemplate

 ******************************************************************************/

JBoolean
CBNewProjectSaveFileDialog::GetProjectTemplate
	(
	JString* fullName
	)
	const
{
	if (itsTemplateIndex == kNoTemplateCmd)
		{
		fullName->Clear();
		return kJFalse;
		}
	else
		{
		JString nmShortcut;
		const JBoolean ok = itsTemplateMenu->GetItemNMShortcut(itsTemplateIndex, &nmShortcut);
		assert( ok );

		JString sysDir, userDir;
		JXGetProgramDataDirectories(CBProjectDocument::GetTemplateDirectoryName(),
									&sysDir, &userDir);

		*fullName = JCombinePathAndName(
			nmShortcut == kUserTemplateMarker ? userDir : sysDir,
			itsTemplateMenu->GetItemText(itsTemplateIndex));
		return kJTrue;
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBNewProjectSaveFileDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsTemplateMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		itsTemplateMenu->CheckItem(itsTemplateIndex);
		}
	else if (sender == itsTemplateMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		itsTemplateIndex = selection->GetIndex();
		UpdateMakefileMethod();
		}

	else
		{
		JXSaveFileDialog::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateMakefileMethod (private)

 ******************************************************************************/

void
CBNewProjectSaveFileDialog::UpdateMakefileMethod()
{
	itsMethodRG->SetActive(JI2B( itsTemplateIndex == kNoTemplateCmd ));
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
CBNewProjectSaveFileDialog::OKToDeactivate()
{
	if (!JXSaveFileDialog::OKToDeactivate())
		{
		return kJFalse;
		}
	else if (Cancelled())
		{
		return kJTrue;
		}

	const CBBuildManager::MakefileMethod method = GetMakefileMethod();
	if (method == CBBuildManager::kManual)
		{
		return kJTrue;
		}

	// ask if OK to replace Make.files

	if (method == CBBuildManager::kMakemake)
		{
		const JString makeFilesName = CBBuildManager::GetMakeFilesName(GetPath());
		if (JFileExists(makeFilesName) &&
			!OKToReplaceFile(makeFilesName, JGetString("CBName")))
			{
			return kJFalse;
			}
		}

	// ask if OK to replace CMakeLists.txt

	if (method == CBBuildManager::kCMake)
		{
		JString projFileName, projRoot, projSuffix;
		GetFileName(&projFileName);
		JSplitRootAndSuffix(projFileName, &projRoot, &projSuffix);

		const JString cmakeInputName = CBBuildManager::GetCMakeInputName(GetPath(), projRoot);
		if (JFileExists(cmakeInputName) &&
			!OKToReplaceFile(cmakeInputName, JGetString("CBName")))
			{
			return kJFalse;
			}
		}

	// ask if OK to replace .pro

	if (method == CBBuildManager::kQMake)
		{
		JString projFileName, projRoot, projSuffix;
		GetFileName(&projFileName);
		JSplitRootAndSuffix(projFileName, &projRoot, &projSuffix);

		const JString qmakeInputName = CBBuildManager::GetQMakeInputName(GetPath(), projRoot);
		if (JFileExists(qmakeInputName) &&
			!OKToReplaceFile(qmakeInputName, JGetString("CBName")))
			{
			return kJFalse;
			}
		}

	// ask if OK to replace existing Makefile

	JPtrArray<JString> makefileList(JPtrArrayT::kDeleteAll);
	CBBuildManager::GetMakefileNames(GetPath(), &makefileList);

	const JSize count = makefileList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const JString* fullName = makefileList.GetElement(i);
		if (JFileExists(*fullName) &&
			!OKToReplaceFile(*fullName, CBBuildManager::GetMakefileMethodName(GetMakefileMethod())))
			{
			return kJFalse;
			}
		}

	return kJTrue;
}

/******************************************************************************
 OKToReplaceFile (private)

 ******************************************************************************/

JBoolean
CBNewProjectSaveFileDialog::OKToReplaceFile
	(
	const JCharacter* fullName,
	const JCharacter* programName
	)
{
	JString path, name;
	JSplitPathAndName(fullName, &path, &name);

	const JCharacter* map[] =
		{
		"file",   name.GetCString(),
		"method", programName
		};
	const JString msg = JGetString(kWarnFileExistsID, map, sizeof(map));

	return (JGetUserNotification())->AskUserNo(msg);
}
