/******************************************************************************
 WidgetParametersDialog.cpp

	BASE CLASS = JXModalDialogDirector

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "WidgetParametersDialog.h"
#include "WidgetPanelBase.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXInputField.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXRadioGroup.h>
#include <jx-af/jx/JXImageRadioButton.h>
#include <jx-af/jx/JXAtMostOneCBGroup.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

WidgetParametersDialog::WidgetParametersDialog
	(
	const JString&					varName,
	const bool						isMemberVar,
	const bool						isPredeclaredVar,
	const JXWidget::HSizingOption	hSizing,
	const JXWidget::VSizingOption	vSizing
	)
	:
	JXModalDialogDirector(),
	itsPanelList(jnew JPtrArray<WidgetPanelBase>(JPtrArrayT::kForgetAll))
{
	BuildWindow(varName, isMemberVar, isPredeclaredVar, hSizing, vSizing);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

WidgetParametersDialog::~WidgetParametersDialog()
{
	jdelete itsPanelList;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
WidgetParametersDialog::BuildWindow
	(
	const JString&					varName,
	const bool						isMemberVar,
	const bool						isPredeclaredVar,
	const JXWidget::HSizingOption	hSizing,
	const JXWidget::VSizingOption	vSizing
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 460,150, JGetString("WindowTitle::WidgetParametersDialog::JXLayout"));

	itsLatestContainer =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,110);

	auto* variableNameLabel =
		jnew JXStaticText(JGetString("variableNameLabel::WidgetParametersDialog::JXLayout"), itsLatestContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,20, 110,20);
	variableNameLabel->SetToLabel(false);

	itsHSizingRG =
		jnew JXRadioGroup(itsLatestContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 40,50, 90,50);
	itsHSizingRG->SetBorderWidth(0);

	itsVSizingRG =
		jnew JXRadioGroup(itsLatestContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 180,50, 90,50);
	itsVSizingRG->SetBorderWidth(0);

	itsMemberVarCB =
		jnew JXTextCheckbox(JGetString("itsMemberVarCB::WidgetParametersDialog::JXLayout"), itsLatestContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 310,50, 130,20);
	itsMemberVarCB->SetShortcuts(JGetString("itsMemberVarCB::shortcuts::WidgetParametersDialog::JXLayout"));

	auto* hSizingLabel =
		jnew JXStaticText(JGetString("hSizingLabel::WidgetParametersDialog::JXLayout"), itsLatestContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,60, 20,30);
	hSizingLabel->SetToLabel(false);

	auto* vSizingLabel =
		jnew JXStaticText(JGetString("vSizingLabel::WidgetParametersDialog::JXLayout"), itsLatestContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 160,60, 20,30);
	vSizingLabel->SetToLabel(false);

	auto* fixedLeftRB =
		jnew JXImageRadioButton(JXWidget::kFixedLeft, itsHSizingRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,10, 30,30);
#ifndef _H_align_left
#define _H_align_left
#include "align_left.xpm"
#endif
	fixedLeftRB->SetXPM(align_left);

	auto* hElasticRB =
		jnew JXImageRadioButton(JXWidget::kHElastic, itsHSizingRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,10, 30,30);
#ifndef _H_expand_horiz
#define _H_expand_horiz
#include "expand_horiz.xpm"
#endif
	hElasticRB->SetXPM(expand_horiz);

	auto* fixedRightRB =
		jnew JXImageRadioButton(JXWidget::kFixedRight, itsHSizingRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,10, 30,30);
#ifndef _H_align_right
#define _H_align_right
#include "align_right.xpm"
#endif
	fixedRightRB->SetXPM(align_right);

	auto* fixedTopRB =
		jnew JXImageRadioButton(JXWidget::kFixedTop, itsVSizingRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,10, 30,30);
#ifndef _H_align_top
#define _H_align_top
#include "align_top.xpm"
#endif
	fixedTopRB->SetXPM(align_top);

	auto* vElasticRB =
		jnew JXImageRadioButton(JXWidget::kVElastic, itsVSizingRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 30,10, 30,30);
#ifndef _H_expand_vert
#define _H_expand_vert
#include "expand_vert.xpm"
#endif
	vElasticRB->SetXPM(expand_vert);

	auto* fixedBottomRB =
		jnew JXImageRadioButton(JXWidget::kFixedBottom, itsVSizingRG,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 60,10, 30,30);
#ifndef _H_align_bottom
#define _H_align_bottom
#include "align_bottom.xpm"
#endif
	fixedBottomRB->SetXPM(align_bottom);

	itsPredeclaredVarCB =
		jnew JXTextCheckbox(JGetString("itsPredeclaredVarCB::WidgetParametersDialog::JXLayout"), itsLatestContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 310,80, 130,20);
	itsPredeclaredVarCB->SetShortcuts(JGetString("itsPredeclaredVarCB::shortcuts::WidgetParametersDialog::JXLayout"));

	auto* cancelButton =
		jnew JXTextButton(JGetString("cancelButton::WidgetParametersDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 120,120, 60,20);
	assert( cancelButton != nullptr );

	auto* okButton =
		jnew JXTextButton(JGetString("okButton::WidgetParametersDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 289,119, 62,22);
	okButton->SetShortcuts(JGetString("okButton::shortcuts::WidgetParametersDialog::JXLayout"));

	itsVarNameInput =
		jnew JXInputField(itsLatestContainer,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 130,20, 310,20);
	itsVarNameInput->SetIsRequired();
	itsVarNameInput->SetValidationPattern(jnew JRegex("^[_a-z][_a-z0-9:\\[\\]]+$", "i"), "itsVarNameInput::validation::WidgetParametersDialog::JXLayout");

// end JXLayout

	SetButtons(okButton, cancelButton);

	itsVarNameInput->SetFont(JFontManager::GetDefaultMonospaceFont());
	itsVarNameInput->GetText()->SetText(varName);

	itsMemberVarCB->SetState(isMemberVar);
	itsPredeclaredVarCB->SetState(isPredeclaredVar);
	jnew JXAtMostOneCBGroup(2, itsMemberVarCB, itsPredeclaredVarCB);

	itsHSizingRG->SelectItem(hSizing);
	itsVSizingRG->SelectItem(vSizing);
}

/******************************************************************************
 AddPanel

 ******************************************************************************/

void
WidgetParametersDialog::AddPanel
	(
	WidgetPanelBase*	panel,
	JXWidgetSet*		container
	)
{
	itsPanelList->Append(panel);

	GetWindow()->AdjustSize(0, container->GetFrameHeight());
	container->Place(0, itsLatestContainer->GetFrame().bottom);
	itsLatestContainer = container;
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

bool
WidgetParametersDialog::OKToDeactivate()
{
	if (!JXModalDialogDirector::OKToDeactivate())
	{
		return false;
	}
	else if (Cancelled())
	{
		return true;
	}

	for (auto* panel : *itsPanelList)
	{
		if (!panel->Validate())
		{
			return false;
		}
	}

	return true;
}

/******************************************************************************
 GetVarName

 ******************************************************************************/

const JString&
WidgetParametersDialog::GetVarName
	(
	bool* isMemberData,
	bool* isPredeclared
	)
	const
{
	*isMemberData  = itsMemberVarCB->IsChecked();
	*isPredeclared = itsPredeclaredVarCB->IsChecked();
	return itsVarNameInput->GetText()->GetText();
}

/******************************************************************************
 Get sizing

 ******************************************************************************/

JXWidget::HSizingOption
WidgetParametersDialog::GetHSizing()
	const
{
	return (JXWidget::HSizingOption) itsHSizingRG->GetSelectedItem();
}

JXWidget::VSizingOption
WidgetParametersDialog::GetVSizing()
	const
{
	return (JXWidget::VSizingOption) itsVSizingRG->GetSelectedItem();
}
