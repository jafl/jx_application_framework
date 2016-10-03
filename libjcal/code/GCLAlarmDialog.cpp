/******************************************************************************
 GCLAlarmDialog.cpp

	<Description>

	BASE CLASS = JXDialogDirector

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GCLAlarmDialog.h>
#include "GCLAlarm.h"
#include "GCLAlarmManager.h"
#include "GCLAppointment.h"
#include "GCLUtil.h"
#include "GCLGlobals.h"

#include <JXColormap.h>
#include <JXIntegerInput.h>
#include <JXRadioGroup.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextMenu.h>
#include <JXTextRadioButton.h>
#include <JXWindow.h>

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

GCLAlarmDialog::GCLAlarmDialog
	(
	JXDirector* supervisor,
	GCLAlarm*	alarm
	)
	:
	JXDialogDirector(supervisor, kJTrue),
	itsAlarm(alarm),
	itsIsLate(kJFalse)
{
	BuildWindow();
	ListenTo(GCLGetAlarmMgr());
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GCLAlarmDialog::~GCLAlarmDialog()
{
	delete itsAlarm;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
GCLAlarmDialog::BuildWindow()
{

// begin JXLayout

	JXWindow* window = new JXWindow(this, 390,250, "");
	assert( window != NULL );

	itsHelpButton =
		new JXTextButton(JGetString("itsHelpButton::GCLAlarmDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 80,220, 70,20);
	assert( itsHelpButton != NULL );

	JXTextButton* okButton =
		new JXTextButton(JGetString("okButton::GCLAlarmDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 230,220, 70,20);
	assert( okButton != NULL );
	okButton->SetShortcuts(JGetString("okButton::GCLAlarmDialog::shortcuts::JXLayout"));

	itsStatusText =
		new JXStaticText(JGetString("itsStatusText::GCLAlarmDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,140, 350,20);
	assert( itsStatusText != NULL );
	itsStatusText->SetFontSize(14);

	itsRG =
		new JXRadioGroup(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,170, 350,45);
	assert( itsRG != NULL );

	JXTextRadioButton* obj1_JXLayout =
		new JXTextRadioButton(1, JGetString("obj1_JXLayout::GCLAlarmDialog::JXLayout"), itsRG,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,0, 140,20);
	assert( obj1_JXLayout != NULL );

	JXTextRadioButton* obj2_JXLayout =
		new JXTextRadioButton(2, JGetString("obj2_JXLayout::GCLAlarmDialog::JXLayout"), itsRG,
					JXWidget::kHElastic, JXWidget::kVElastic, 10,20, 150,20);
	assert( obj2_JXLayout != NULL );

	itsAmountInput =
		new JXIntegerInput(itsRG,
					JXWidget::kHElastic, JXWidget::kVElastic, 160,0, 80,20);
	assert( itsAmountInput != NULL );

	itsTimeUnitMenu =
		new JXTextMenu(JGetString("itsTimeUnitMenu::GCLAlarmDialog::JXLayout"), itsRG,
					JXWidget::kHElastic, JXWidget::kVElastic, 240,0, 100,20);
	assert( itsTimeUnitMenu != NULL );

	JXStaticText* obj3_JXLayout =
		new JXStaticText(JGetString("obj3_JXLayout::GCLAlarmDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,10, 350,20);
	assert( obj3_JXLayout != NULL );

	JXStaticText* obj4_JXLayout =
		new JXStaticText(JGetString("obj4_JXLayout::GCLAlarmDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,40, 70,20);
	assert( obj4_JXLayout != NULL );
	obj4_JXLayout->SetFontSize(14);
	const JFontStyle obj4_JXLayout_style(kJTrue, kJFalse, 0, kJFalse, GetColormap()->GetBlackColor());
	obj4_JXLayout->SetFontStyle(obj4_JXLayout_style);

	itsSubjectText =
		new JXStaticText(JGetString("itsSubjectText::GCLAlarmDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 90,40, 280,20);
	assert( itsSubjectText != NULL );
	itsSubjectText->SetFontSize(14);
	const JFontStyle itsSubjectText_style(kJTrue, kJFalse, 0, kJFalse, GetColormap()->GetBlackColor());
	itsSubjectText->SetFontStyle(itsSubjectText_style);

	JXStaticText* obj5_JXLayout =
		new JXStaticText(JGetString("obj5_JXLayout::GCLAlarmDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,70, 70,20);
	assert( obj5_JXLayout != NULL );
	obj5_JXLayout->SetFontSize(14);
	const JFontStyle obj5_JXLayout_style(kJTrue, kJFalse, 0, kJFalse, GetColormap()->GetBlackColor());
	obj5_JXLayout->SetFontStyle(obj5_JXLayout_style);

	itsTimeText =
		new JXStaticText(JGetString("itsTimeText::GCLAlarmDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 90,70, 280,20);
	assert( itsTimeText != NULL );
	itsTimeText->SetFontSize(14);
	const JFontStyle itsTimeText_style(kJTrue, kJFalse, 0, kJFalse, GetColormap()->GetBlackColor());
	itsTimeText->SetFontStyle(itsTimeText_style);

	itsDateText =
		new JXStaticText(JGetString("itsDateText::GCLAlarmDialog::JXLayout"), window,
					JXWidget::kHElastic, JXWidget::kVElastic, 90,100, 280,20);
	assert( itsDateText != NULL );
	itsDateText->SetFontSize(14);
	const JFontStyle itsDateText_style(kJTrue, kJFalse, 0, kJFalse, GetColormap()->GetBlackColor());
	itsDateText->SetFontStyle(itsDateText_style);

// end JXLayout

	SetButtons(okButton, NULL);
	window->SetTitle("Appointment Reminder");

	itsAmountInput->SetLowerLimit(0);
	itsTimeUnitMenu->SetMenuItems(kTimeUnitMenuString);
	itsTimeUnitMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsTimeUnitMenu->SetTitleText("Minute(s)");

	itsTimeUnit	= kMinute;

	ListenTo(itsTimeUnitMenu);

	itsRG->SelectItem(kRemindAgain);
	itsAmountInput->SetValue(kTimeDefault);

	BuildText();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GCLAlarmDialog::Receive
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
	else if (sender == GCLGetAlarmMgr() && message.Is(GCLAlarmManager::kMinuteAlarm))
		{
		SetStatusText();
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
GCLAlarmDialog::GetTimeUnit()
	const
{
	return (GCLAlarm::TimeUnit)itsTimeUnit;
}

/******************************************************************************
 GetTime (public)

 ******************************************************************************/

JBoolean
GCLAlarmDialog::GetTime
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
GCLAlarmDialog::UseAlarm()
	const
{
	return JI2B(itsRG->GetSelectedItem() == kRemindAgain);
}

/******************************************************************************
 OKToDeactivate (virtual protected)

 ******************************************************************************/

JBoolean
GCLAlarmDialog::OKToDeactivate()
{
	GCLAppointment*	appt	= itsAlarm->GetAppt();
	if (itsIsLate)
		{
		AddNextAlarm();
		return kJTrue;
		}
	if (itsRG->GetSelectedItem() == kNoMoreReminds)
		{
		appt->ShouldStillRemind(kJFalse);
		AddNextAlarm();
		return kJTrue;
		}

	if (!itsAmountInput->InputValid())
		{
		return kJFalse;
		}

	JInteger value;
	if (!itsAmountInput->GetValue(&value))
		{
		return kJFalse;
		}

	GCLAlarm* alarm	= new GCLAlarm(value, GetTimeUnit(), appt, itsAlarm->GetApptDay(), itsAlarm->GetApptTime(), kJTrue);
	assert(alarm != NULL);

	GCLGetAlarmMgr()->AddAlarm(alarm);

	return kJTrue;
}

/******************************************************************************
 AddNextAlarm (private)

 ******************************************************************************/

void
GCLAlarmDialog::AddNextAlarm()
{
	GCLAppointment*	appt	= itsAlarm->GetAppt();
	GCLRecurrence r;
	CLDay day;
	if (appt->GetRecurrence(&r) && r.type != kNoRepeat &&
		GetNextAppointmentDay(appt, itsAlarm->GetApptDay(), itsAlarm->GetApptTime(), kJTrue, &day))
		{
		GCLAlarm::TimeUnit unit;
		JIndex value;
		appt->GetDefaultAlarm(&value, &unit);
		GCLAlarm* alarm	= new GCLAlarm(value, unit, appt, day, appt->GetStartTime(), kJFalse);
		assert(alarm != NULL);
		GCLGetAlarmMgr()->AddAlarm(alarm);
		appt->ShouldStillRemind(kJTrue);
		}
}


/******************************************************************************
 BuildText (private)

 ******************************************************************************/

void
GCLAlarmDialog::BuildText()
{
	itsSubjectText->SetText(itsAlarm->GetAppt()->GetSubject());

	JString str = GetTimeString(itsAlarm->GetAppt()->GetStartTime()) + " to ";
	str        += GetTimeString(itsAlarm->GetAppt()->GetEndTime());
	itsTimeText->SetText(str);

	itsDateText->SetText(GetDateString(itsAlarm->GetApptDay()));

	SetStatusText();
}

/******************************************************************************
 SetStatusText (private)

 ******************************************************************************/

void
GCLAlarmDialog::SetStatusText()
{
	time_t now			= GetCurrentEpochTime();
	CLTime apptTime		= itsAlarm->GetAppt()->GetStartTime();
	CLDay day			= itsAlarm->GetApptDay();
	time_t epochTime	= GetEpochTime(day, apptTime);
	JInteger timediff	= JRound((epochTime - now)/60.0);

	if (timediff <= 0)
		{
		JString str	= "You are late for the appointment";
		itsStatusText->SetText(str);
		AdjustForLateAppt();
		}
	else if (timediff < 60)
		{
		JString str = "The appointment is in " + JString(timediff);
		if (timediff == 1)
			{
			str += " minute";
			}
		else
			{
			str += " minutes";
			}
		itsStatusText->SetText(str);
		}
}

/******************************************************************************
 AdjustForLateAppt (private)

 ******************************************************************************/

void
GCLAlarmDialog::AdjustForLateAppt()
{
	itsIsLate	= kJTrue;
	itsRG->Hide();
}
