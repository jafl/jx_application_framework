/******************************************************************************
 GLCreateVectorPlotDialog.cpp

	BASE CLASS = JXDialogDirector

	Copyright @ 1997 by Glenn W. Bach.

 ******************************************************************************/

#include "GLCreateVectorPlotDialog.h"
#include "GLDataDocument.h"
#include "GLRaggedFloatTableData.h"
#include "GLGlobals.h"

#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXTextMenu.h>
#include <JXInputField.h>
#include <JXStaticText.h>

#include <JPtrArray-JString.h>
#include <JUserNotification.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

GLCreateVectorPlotDialog::GLCreateVectorPlotDialog
	(
	GLDataDocument* supervisor,
	GLRaggedFloatTableData* data,
	const JIndex startX,
	const JIndex startY,
	const JIndex startX2,
	const JIndex startY2
	)
	:
	JXDialogDirector(supervisor, true)
{
	itsTableDir = supervisor;
	BuildWindow(data, startX, startY, startX2, startY2);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GLCreateVectorPlotDialog::~GLCreateVectorPlotDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GLCreateVectorPlotDialog::BuildWindow
	(
	GLRaggedFloatTableData* data,
	const JIndex startX,
	const JIndex startY,
	const JIndex startX2,
	const JIndex startY2
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 370,160, JString::empty);
	assert( window != nullptr );

	itsX1Menu =
		jnew JXTextMenu(JGetString("itsX1Menu::GLCreateVectorPlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,40, 160,30);
	assert( itsX1Menu != nullptr );

	itsY1Menu =
		jnew JXTextMenu(JGetString("itsY1Menu::GLCreateVectorPlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,40, 160,30);
	assert( itsY1Menu != nullptr );

	itsX2Menu =
		jnew JXTextMenu(JGetString("itsX2Menu::GLCreateVectorPlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,80, 160,30);
	assert( itsX2Menu != nullptr );

	itsY2Menu =
		jnew JXTextMenu(JGetString("itsY2Menu::GLCreateVectorPlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,80, 160,30);
	assert( itsY2Menu != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::GLCreateVectorPlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 280,130, 70,20);
	assert( okButton != nullptr );
	okButton->SetShortcuts(JGetString("okButton::GLCreateVectorPlotDialog::shortcuts::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::GLCreateVectorPlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 190,130, 70,20);
	assert( cancelButton != nullptr );

	itsPlotsMenu =
		jnew JXTextMenu(JGetString("itsPlotsMenu::GLCreateVectorPlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,120, 130,30);
	assert( itsPlotsMenu != nullptr );

	itsLabelInput =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 115,10, 200,20);
	assert( itsLabelInput != nullptr );

	auto* label =
		jnew JXStaticText(JGetString("label::GLCreateVectorPlotDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 65,10, 50,20);
	assert( label != nullptr );
	label->SetToLabel();

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::GLCreateVectorPlotDialog"));
	SetButtons(okButton, cancelButton);

	GLBuildColumnMenus("Column::GLGlobal", data->GetDataColCount(),
					   itsX1Menu, itsX2Menu, itsY1Menu, itsY2Menu, nullptr);

	itsStartX1 = startX;
	if (startX == 0)
		{
		itsStartX1 = 1;
		}

	itsStartX2 = startX2;
	if (startX2 == 0)
		{
		itsStartX2 = 1;
		}

	itsStartY1 = startY;
	if (startY == 0)
		{
		itsStartY1 = 1;
		}

	itsStartY2 = startY2;
	if (startY2 == 0)
		{
		itsStartY2 = 1;
		}

	JPtrArray<JString> names(JPtrArrayT::kDeleteAll);
	itsTableDir->GetPlotNames(names);

	itsPlotsMenu->AppendItem(JGetString("NewPlotItemLabel::GLGlobal"));

	for (auto* name : names)
		{
		itsPlotsMenu->AppendItem(*name);
		}

	itsPlotsMenu->ShowSeparatorAfter(1, true);

	itsPlotIndex = 1;

	itsX1Menu->SetToPopupChoice(true, itsStartX1);
	itsX2Menu->SetToPopupChoice(true, itsStartX2);
	itsY1Menu->SetToPopupChoice(true, itsStartY1);
	itsY2Menu->SetToPopupChoice(true, itsStartY2);
	itsPlotsMenu->SetToPopupChoice(true, itsPlotIndex);

	itsX1Menu->SetUpdateAction(JXMenu::kDisableNone);
	itsX2Menu->SetUpdateAction(JXMenu::kDisableNone);
	itsY1Menu->SetUpdateAction(JXMenu::kDisableNone);
	itsY2Menu->SetUpdateAction(JXMenu::kDisableNone);
	itsPlotsMenu->SetUpdateAction(JXMenu::kDisableNone);

	ListenTo(itsX1Menu);
	ListenTo(itsX2Menu);
	ListenTo(itsY1Menu);
	ListenTo(itsY2Menu);
	ListenTo(itsPlotsMenu);

	itsLabelInput->GetText()->SetText(JGetString("DefaultLabel::GLGlobal"));
}

/******************************************************************************
 GetColumns

 ******************************************************************************/

void
GLCreateVectorPlotDialog::GetColumns
	(
	JIndex* X1,
	JIndex* Y1,
	JIndex* X2,
	JIndex* Y2
	)
{
	*X1 = itsStartX1;
	*Y1 = itsStartY1;
	*X2 = itsStartX2;
	*Y2 = itsStartY2;
}

/******************************************************************************
 GetColumns

 ******************************************************************************/

void
GLCreateVectorPlotDialog::Receive
	(
	JBroadcaster* sender,
	const Message& message
	)
{
	if (sender == itsX1Menu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsStartX1 = selection->GetIndex();
		}

	else if (sender == itsX2Menu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsStartX2 = selection->GetIndex();
		}

	else if (sender == itsY1Menu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsStartY1 = selection->GetIndex();
		}

	else if (sender == itsY2Menu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsStartY2 = selection->GetIndex();
		}

	else if (sender == itsPlotsMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		itsPlotIndex = selection->GetIndex();
		}

	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 GetPlotIndex

 ******************************************************************************/

bool
GLCreateVectorPlotDialog::GetPlotIndex
	(
	JIndex* index
	)
{
	if (itsPlotIndex == 1)
		{
		return false;
		}

	*index = itsPlotIndex - 1;
	return true;
}

/******************************************************************************
 GetPlotIndex

 ******************************************************************************/

const JString&
GLCreateVectorPlotDialog::GetLabel()
{
	return itsLabelInput->GetText()->GetText();
}

/******************************************************************************
 OKToDeactivate

 ******************************************************************************/

bool
GLCreateVectorPlotDialog::OKToDeactivate()
{
	if (Cancelled())
		{
		return true;
		}
	if (GetLabel().IsEmpty())
		{
		JGetUserNotification()->ReportError(JGetString("SpecifyCurveLabel::GLGlobal"));
		return false;
		}
	return true;
}
