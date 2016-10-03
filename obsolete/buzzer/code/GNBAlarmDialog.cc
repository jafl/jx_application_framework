/******************************************************************************
 GNBAlarmDialog.cc

	<Description>

	BASE CLASS = JXDialogDirector

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GNBAlarmDialog.h>
#include "GCLAlarm.h"
#include "GCLAlarmManager.h"
#include "GCLAppointment.h"
#include "GCLUtil.h"
#include "GCLGlobals.h"

#include <JXColormap.h>
#include <JXIntegerInput.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextCheckbox.h>
#include <JXTextMenu.h>
#include <JXTextRadioButton.h>
#include <JXWindow.h>

#include <JString.h>

#include <jMath.h>

#include <jAssert.h>

static const JCharacter* kTimeUnitMenuString	=
	"Minute(s)|Hour(s)|Day(s)|Week(s)|Month(s)";

const JIndex kMinute		= 1;

const JIndex kRemindAgain	= 1;
const JIndex kNoMoreReminds	= 2;

const JIndex kTimeDefault	= 30;

/******************************************************************************
 Constructor

 *****************************************************************************/

GNBAlarmDialog::GNBAlarmDialog
	(
	JXDirector* supervisor,
	const CLTime origTime
	)
	:
	JXDialogDirector(supervisor, kJTrue)
{
	BuildWindow(origTime);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GNBAlarmDialog::~GNBAlarmDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GNBAlarmDialog::BuildWindow
	(
	const CLTime origTime
	)
{

// begin JXLayout

	JXWindow* window = new JXWindow(this, 430,170, "");
	assert( window != NULL );

	JXTextButton* cancelButton =
		new JXTextButton(JGetString("cancelButton::GNBAlarmDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 80,130, 75,20);
	assert( cancelButton != NULL );
	cancelButton->SetShortcuts(JGetString("cancelButton::GNBAlarmDialog::shortcuts::JXLayout"));

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::GNBAlarmDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 280,130, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::GNBAlarmDialog::shortcuts::JXLayout"));

	itsHourInput =
		new JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 120,35, 30,20);
	assert( itsHourInput != NULL );

	itsMinuteInput =
		new JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 160,35, 40,20);
	assert( itsMinuteInput != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::GNBAlarmDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 150,35, 10,20);
	assert( obj1_JXLayout != NULL );

	itsAlarmCB =
		new JXTextCheckbox(JGetString("itsAlarmCB::GNBAlarmDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,85, 100,20);
	assert( itsAlarmCB != NULL );

	itsAmountInput =
		new JXIntegerInput(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 120,85, 80,20);
	assert( itsAmountInput != NULL );

	itsTimeUnitMenu =
		new JXTextMenu(JGetString("itsTimeUnitMenu::GNBAlarmDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 200,85, 100,20);
	assert( itsTimeUnitMenu != NULL );

	JXStaticText* obj2_JXLayout =
		new JXStaticText(JGetString("obj2_JXLayout::GNBAlarmDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 310,85, 80,20);
	assert( obj2_JXLayout != NULL );

	JXStaticText* obj3_JXLayout =
		new JXStaticText(JGetString("obj3_JXLayout::GNBAlarmDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 30,35, 75,20);
	assert( obj3_JXLayout != NULL );

// end JXLayout

	SetButtons(okButton, cancelButton);
	window->SetTitle("Appointment Reminder");

	itsAmountInput->SetLowerLimit(0);
	itsTimeUnitMenu->SetMenuItems(kTimeUnitMenuString);
	itsTimeUnitMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsTimeUnitMenu->SetTitleText("Minute(s)");

	itsTimeUnit	= kMinute;

	ListenTo(itsTimeUnitMenu);

	itsAlarmCB->SetState(kJTrue);
	itsAmountInput->SetValue(kTimeDefault);

	itsHourInput->SetText(JString(origTime.hour));
	itsMinuteInput->SetText(JString(origTime.minute));

	itsHourInput->SetLimits(0, 23);
	itsMinuteInput->SetLimits(0, 59);

}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GNBAlarmDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsTimeUnitMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* info =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert(info != NULL);
		itsTimeUnit	= info->GetIndex();
		itsTimeUnitMenu->SetTitleText(itsTimeUnitMenu->GetItemText(info->GetIndex()));
		}
	else
		{
		JXDialogDirector::Receive(sender, message);
		}
}

/******************************************************************************
 GetTimeUnit (public)

 ******************************************************************************/

GCLAlarm::TimeUnit
GNBAlarmDialog::GetTimeUnit()
	const
{
	return (GCLAlarm::TimeUnit)itsTimeUnit;
}

/******************************************************************************
 GetWarnTime (public)

 ******************************************************************************/

JBoolean
GNBAlarmDialog::GetWarnTime
	(
	JIndex* count
	)
	const
{
	JInteger iTime;
	JBoolean ok	= itsAmountInput->GetValue(&iTime);
	if (ok)
		{
		*count = iTime;
		return kJTrue;
		}
	return kJFalse;
}

/******************************************************************************
 UseAlarm (public)

 ******************************************************************************/

JBoolean
GNBAlarmDialog::UseAlarm()
	const
{
	return itsAlarmCB->IsChecked();
}

/******************************************************************************
 GetAlarmTime (public)

 ******************************************************************************/

JBoolean
GNBAlarmDialog::GetAlarmTime
	(
	CLTime* alarmTime
	)
	const
{
	JString hourStr		= itsHourInput->GetText();
	JInteger hourVal;
	if (!hourStr.ConvertToInteger(&hourVal))
		{
		return kJFalse;
		}
	JString minStr		= itsMinuteInput->GetText();
	JInteger minVal;
	if (!minStr.ConvertToInteger(&minVal))
		{
		return kJFalse;
		}
	alarmTime->hour		= hourVal;
	alarmTime->minute	= minVal;

	return kJTrue;
}


/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
GNBAlarmDialog::OKToDeactivate()
{
	JString hourStr		= itsHourInput->GetText();
	JInteger hourVal;
	if (!hourStr.ConvertToInteger(&hourVal))
		{
		return kJFalse;
		}
	JString minStr		= itsMinuteInput->GetText();
	JInteger minVal;
	if (!minStr.ConvertToInteger(&minVal))
		{
		return kJFalse;
		}

	if (!itsHourInput->InputValid())
		{
		return kJFalse;
		}

	if (!itsMinuteInput->InputValid())
		{
		return kJFalse;
		}

	return kJTrue;
}

