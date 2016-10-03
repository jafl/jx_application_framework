/******************************************************************************
 GCLAlarmDialog.h

	Interface for the GCLAlarmDialog class

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GCLAlarmDialog
#define _H_GCLAlarmDialog

#include <JXDialogDirector.h>

#include "GCLAlarm.h"

class JXIntegerInput;
class JXRadioGroup;
class JXStaticText;
class JXTextButton;
class JXTextMenu;

class GCLAlarmDialog : public JXDialogDirector
{
public:

	GCLAlarmDialog(JXDirector* supervisor, GCLAlarm* alarm);
	virtual ~GCLAlarmDialog();

	JBoolean			UseAlarm() const;
	JBoolean			GetTime(JIndex* count) const;
	GCLAlarm::TimeUnit	GetTimeUnit() const;

protected:

	virtual void		Receive(JBroadcaster* sender, const Message& message);
	virtual JBoolean	OKToDeactivate();

private:

// begin JXLayout

    JXTextButton*   itsHelpButton;
    JXStaticText*   itsStatusText;
    JXRadioGroup*   itsRG;
    JXIntegerInput* itsAmountInput;
    JXTextMenu*     itsTimeUnitMenu;
    JXStaticText*   itsSubjectText;
    JXStaticText*   itsTimeText;
    JXStaticText*   itsDateText;

// end JXLayout

	GCLAlarm*		itsAlarm;
	JIndex			itsTimeUnit;
	JBoolean		itsIsLate;

private:

	void BuildWindow();

	void BuildText();
	void SetStatusText();

	void AdjustForLateAppt();

	void AddNextAlarm();

	// not allowed

	GCLAlarmDialog(const GCLAlarmDialog& source);
	const GCLAlarmDialog& operator=(const GCLAlarmDialog& source);

};

#endif
