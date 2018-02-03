/******************************************************************************
 GNBAlarmDialog.h

	Interface for the GNBAlarmDialog class

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GNBAlarmDialog
#define _H_GNBAlarmDialog

#include <JXDialogDirector.h>

#include "GCLAlarm.h"
#include "GCLUtil.h"

class JXIntegerInput;
class JXStaticText;
class JXTextButton;
class JXTextMenu;
class JXTextCheckbox;

class GNBAlarmDialog : public JXDialogDirector
{
public:

	GNBAlarmDialog(JXDirector* supervisor, const CLTime origTime);
	virtual ~GNBAlarmDialog();

	JBoolean			UseAlarm() const;
	JBoolean			GetWarnTime(JIndex* count) const;
	GCLAlarm::TimeUnit	GetTimeUnit() const;
	JBoolean			GetAlarmTime(CLTime* alarmTime) const;

protected:

	virtual void		Receive(JBroadcaster* sender, const Message& message) override;
	virtual JBoolean	OKToDeactivate();

private:

// begin JXLayout

    JXIntegerInput* itsHourInput;
    JXIntegerInput* itsMinuteInput;
    JXTextCheckbox* itsAlarmCB;
    JXIntegerInput* itsAmountInput;
    JXTextMenu*     itsTimeUnitMenu;

// end JXLayout

	JIndex			itsTimeUnit;

private:

	void BuildWindow(const CLTime origTime);

	// not allowed

	GNBAlarmDialog(const GNBAlarmDialog& source);
	const GNBAlarmDialog& operator=(const GNBAlarmDialog& source);

};

#endif
