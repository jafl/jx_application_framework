/******************************************************************************
 CBEditFileTypesDialog.cpp

	BASE CLASS = JXDialogDirector, JPrefObject

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CBEditFileTypesDialog.h"
#include "CBFileTypeTable.h"
#include "cbGlobals.h"
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXScrollbarSet.h>
#include <JXColHeaderWidget.h>
#include <JXHelpManager.h>
#include <JString.h>
#include <jAssert.h>

// setup information

const JFileVersion kCurrentSetupVersion = 0;

/******************************************************************************
 Constructor

 ******************************************************************************/

CBEditFileTypesDialog::CBEditFileTypesDialog
	(
	JXDirector*										supervisor,
	const JArray<CBPrefsManager::FileTypeInfo>&		fileTypeList,
	const JArray<CBPrefsManager::MacroSetInfo>&		macroList,
	const JArray<CBPrefsManager::CRMRuleListInfo>&	crmList,
	const bool									execOutputWordWrap,
	const bool									unknownTypeWordWrap
	)
	:
	JXDialogDirector(supervisor, true),
	JPrefObject(CBGetPrefsManager(), kCBEditFileTypesDialogID)
{
	BuildWindow(fileTypeList, macroList, crmList,
				execOutputWordWrap, unknownTypeWordWrap);
	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CBEditFileTypesDialog::~CBEditFileTypesDialog()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 BuildWindow (protected)

 ******************************************************************************/

void
CBEditFileTypesDialog::BuildWindow
	(
	const JArray<CBPrefsManager::FileTypeInfo>&		fileTypeList,
	const JArray<CBPrefsManager::MacroSetInfo>&		macroList,
	const JArray<CBPrefsManager::CRMRuleListInfo>&	crmList,
	const bool									execOutputWordWrap,
	const bool									unknownTypeWordWrap
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 700,400, JString::empty);
	assert( window != nullptr );

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::CBEditFileTypesDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 130,370, 70,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::CBEditFileTypesDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 510,370, 70,20);
	assert( okButton != nullptr );

	itsExecOutputWrapCB =
		jnew JXTextCheckbox(JGetString("itsExecOutputWrapCB::CBEditFileTypesDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 80,300, 310,20);
	assert( itsExecOutputWrapCB != nullptr );

	itsUnknownTypeWrapCB =
		jnew JXTextCheckbox(JGetString("itsUnknownTypeWrapCB::CBEditFileTypesDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 80,330, 310,20);
	assert( itsUnknownTypeWrapCB != nullptr );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::CBEditFileTypesDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 315,370, 70,20);
	assert( itsHelpButton != nullptr );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::CBEditFileTypesDialog::shortcuts::JXLayout"));

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,20, 570,260);
	assert( scrollbarSet != nullptr );

	auto* addTypeButton =
		jnew JXTextButton(JGetString("addTypeButton::CBEditFileTypesDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 610,50, 70,20);
	assert( addTypeButton != nullptr );
	addTypeButton->SetShortcuts(JGetString("addTypeButton::CBEditFileTypesDialog::shortcuts::JXLayout"));

	auto* duplicateTypeButton =
		jnew JXTextButton(JGetString("duplicateTypeButton::CBEditFileTypesDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 610,80, 70,20);
	assert( duplicateTypeButton != nullptr );
	duplicateTypeButton->SetShortcuts(JGetString("duplicateTypeButton::CBEditFileTypesDialog::shortcuts::JXLayout"));

	auto* removeTypeButton =
		jnew JXTextButton(JGetString("removeTypeButton::CBEditFileTypesDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 610,110, 70,20);
	assert( removeTypeButton != nullptr );
	removeTypeButton->SetShortcuts(JGetString("removeTypeButton::CBEditFileTypesDialog::shortcuts::JXLayout"));

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::CBEditFileTypesDialog"));
	SetButtons(okButton, cancelButton);
	UseModalPlacement(false);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();

	ListenTo(itsHelpButton);

	JXWidgetSet* encl = scrollbarSet->GetScrollEnclosure();
	const JCoordinate kColHeaderHeight = 20;
	const JCoordinate w = encl->GetApertureWidth();

	itsTable =
		jnew CBFileTypeTable(fileTypeList, macroList, crmList,
							addTypeButton, removeTypeButton,
							duplicateTypeButton, scrollbarSet, encl,
							JXWidget::kHElastic, JXWidget::kVElastic,
							0, kColHeaderHeight,
							w, encl->GetApertureHeight() - kColHeaderHeight);
	assert( itsTable != nullptr );

	auto* colHeader =
		jnew JXColHeaderWidget(itsTable, scrollbarSet, encl,
							  JXWidget::kHElastic, JXWidget::kFixedTop,
							  0,0, w, kColHeaderHeight);
	assert( colHeader != nullptr );
	colHeader->TurnOnColResizing(20);
	itsTable->SetColTitles(colHeader);

	itsExecOutputWrapCB->SetState(execOutputWordWrap);
	itsUnknownTypeWrapCB->SetState(unknownTypeWordWrap);
}

/******************************************************************************
 GetFileTypeInfo

 ******************************************************************************/

void
CBEditFileTypesDialog::GetFileTypeInfo
	(
	JArray<CBPrefsManager::FileTypeInfo>*	fileTypeList,
	bool*								execOutputWordWrap,
	bool*								unknownTypeWordWrap
	)
	const
{
	itsTable->GetFileTypeList(fileTypeList);

	*execOutputWordWrap  = itsExecOutputWrapCB->IsChecked();
	*unknownTypeWordWrap = itsUnknownTypeWrapCB->IsChecked();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CBEditFileTypesDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection("CBFileTypeHelp");
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
CBEditFileTypesDialog::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentSetupVersion)
		{
		return;
		}

	JXWindow* window = GetWindow();
	window->ReadGeometry(input);
	window->Deiconify();

	itsTable->ReadGeometry(input);
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
CBEditFileTypesDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << kCurrentSetupVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ';
	itsTable->WriteGeometry(output);
}
