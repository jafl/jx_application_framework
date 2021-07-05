/******************************************************************************
 CBNewProjectSaveFileDialog.cpp

	BASE CLASS = JXSaveFileDialog

	Copyright © 2000 by John Lindal.

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

static const JUtf8Byte* kTemplateMenuStr = "None %r %l";

enum
{
	kNoTemplateCmd = 1
};

/******************************************************************************
 Constructor function (static)

 ******************************************************************************/

CBNewProjectSaveFileDialog*
CBNewProjectSaveFileDialog::Create
	(
	JXDirector*								supervisor,
	JDirInfo*								dirInfo,
	const JString&							fileFilter,
	const JString&							templateFile,
	const CBBuildManager::MakefileMethod	method,
	const JString&							origName,
	const JString&							prompt,
	const JString&							message
	)
{
	CBNewProjectSaveFileDialog* dlog =
		jnew CBNewProjectSaveFileDialog(supervisor, dirInfo, fileFilter, method);
	assert( dlog != nullptr );
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
	const JString&							fileFilter,
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
	const JString& origName,
	const JString& prompt,
	const JString& message
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 310,500, JString::empty);
	assert( window != nullptr );

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,140, 180,140);
	assert( scrollbarSet != nullptr );

	JXTextButton* saveButton =
		jnew JXTextButton(JGetString("saveButton::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,280, 70,20);
	assert( saveButton != nullptr );
	saveButton->SetShortcuts(JGetString("saveButton::CBNewProjectSaveFileDialog::shortcuts::JXLayout"));

	JXTextButton* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,310, 70,20);
	assert( cancelButton != nullptr );

	JXTextButton* homeButton =
		jnew JXTextButton(JGetString("homeButton::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 250,140, 40,20);
	assert( homeButton != nullptr );

	JXStaticText* pathLabel =
		jnew JXStaticText(JGetString("pathLabel::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,20, 40,20);
	assert( pathLabel != nullptr );
	pathLabel->SetToLabel();

	JXSaveFileInput* fileNameInput =
		jnew JXSaveFileInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,310, 180,20);
	assert( fileNameInput != nullptr );

	JXPathInput* pathInput =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,20, 200,20);
	assert( pathInput != nullptr );

	JXTextCheckbox* showHiddenCB =
		jnew JXTextCheckbox(JGetString("showHiddenCB::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 60,80, 130,20);
	assert( showHiddenCB != nullptr );

	JXStaticText* promptLabel =
		jnew JXStaticText(JGetString("promptLabel::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,290, 180,20);
	assert( promptLabel != nullptr );
	promptLabel->SetToLabel();

	JXStaticText* filterLabel =
		jnew JXStaticText(JGetString("filterLabel::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,50, 40,20);
	assert( filterLabel != nullptr );
	filterLabel->SetToLabel();

	JXInputField* filterInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 60,50, 200,20);
	assert( filterInput != nullptr );

	JXStaticText* makefileTitle =
		jnew JXStaticText(JGetString("makefileTitle::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 20,420, 140,20);
	assert( makefileTitle != nullptr );
	makefileTitle->SetToLabel();

	itsMethodRG =
		jnew JXRadioGroup(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 160,390, 132,106);
	assert( itsMethodRG != nullptr );

	JXTextRadioButton* manualRB =
		jnew JXTextRadioButton(CBBuildManager::kManual, JGetString("manualRB::CBNewProjectSaveFileDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,10, 110,20);
	assert( manualRB != nullptr );

	JXTextRadioButton* qmakeRB =
		jnew JXTextRadioButton(CBBuildManager::kQMake, JGetString("qmakeRB::CBNewProjectSaveFileDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 110,20);
	assert( qmakeRB != nullptr );

	JXTextRadioButton* makemakeRB =
		jnew JXTextRadioButton(CBBuildManager::kMakemake, JGetString("makemakeRB::CBNewProjectSaveFileDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,30, 110,20);
	assert( makemakeRB != nullptr );

	JXPathHistoryMenu* pathHistory =
		jnew JXPathHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,20, 30,20);
	assert( pathHistory != nullptr );

	JXStringHistoryMenu* filterHistory =
		jnew JXStringHistoryMenu(1, JString::empty, window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 260,50, 30,20);
	assert( filterHistory != nullptr );

	JXTextButton* upButton =
		jnew JXTextButton(JGetString("upButton::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,140, 30,20);
	assert( upButton != nullptr );

	JXNewDirButton* newDirButton =
		jnew JXNewDirButton(window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,190, 70,20);
	assert( newDirButton != nullptr );

	JXCurrentPathMenu* currPathMenu =
		jnew JXCurrentPathMenu(JString("/", kJFalse), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,110, 180,20);
	assert( currPathMenu != nullptr );

	itsTemplateMenu =
		jnew JXTextMenu(JGetString("itsTemplateMenu::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 20,350, 270,30);
	assert( itsTemplateMenu != nullptr );

	JXTextRadioButton* cmakeRB =
		jnew JXTextRadioButton(CBBuildManager::kCMake, JGetString("cmakeRB::CBNewProjectSaveFileDialog::JXLayout"), itsMethodRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 110,20);
	assert( cmakeRB != nullptr );

	JXTextButton* desktopButton =
		jnew JXTextButton(JGetString("desktopButton::CBNewProjectSaveFileDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 220,160, 70,20);
	assert( desktopButton != nullptr );

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
	const JString& templateFile
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
		JString* menuTextStr = nullptr;
		BuildTemplateMenuItems(sysDir,  kJFalse, &menuText, templateFile, &menuTextStr);
		BuildTemplateMenuItems(userDir, kJTrue,  &menuText, templateFile, &menuTextStr);

		const JSize count = menuText.GetElementCount();
		JString itemText, nmShortcut;
		for (JIndex i=1; i<=count; i++)
			{
			// We have to extract user/sys here because otherwise we would
			// have to keep extra state while building the sorted list.

			itemText = *menuText.GetElement(i);

			JPtrArray<JString> s(JPtrArrayT::kDeleteAll);
			itemText.Split(" (", &s, 2);
			assert( s.GetElementCount() == 2 );

			itemText = *s.GetElement(1);

			nmShortcut = *s.GetElement(2);
			nmShortcut.Prepend("(");

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
	const JString&		path,
	const JBoolean		isUserPath,
	JPtrArray<JString>*	menuText,
	const JString&		templateFile,
	JString**			menuTextStr
	)
	const
{
	JDirInfo* info = nullptr;
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
				assert( s != nullptr );

				if (isUserPath)
					{
					*s += JGetString("UserTemplateMarker::CBNewProjectSaveFileDialog");
					}
				else
					{
					*s += JGetString("SysTemplateMarker::CBNewProjectSaveFileDialog");
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
			nmShortcut == JGetString("UserTemplateMarker::CBNewProjectSaveFileDialog") ? userDir : sysDir,
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
		assert( selection != nullptr );
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
	const JString& fullName,
	const JString& programName
	)
{
	JString path, name;
	JSplitPathAndName(fullName, &path, &name);

	const JUtf8Byte* map[] =
		{
		"file",   name.GetBytes(),
		"method", programName.GetBytes()
		};
	const JString msg = JGetString("WarnFileExists::CBNewProjectSaveFileDialog", map, sizeof(map));

	return JGetUserNotification()->AskUserNo(msg);
}
