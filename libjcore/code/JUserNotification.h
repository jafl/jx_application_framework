/******************************************************************************
 JUserNotification.h

	Interface for the JUserNotification class.

	Copyright (C) 1994-96 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JUserNotification
#define _H_JUserNotification

#include <jTypes.h>

class JUserNotification
{
public:

	enum CloseAction
	{
		kSaveData,
		kDiscardData,
		kDontClose
	};

public:

	JUserNotification();

	virtual ~JUserNotification();

	virtual void		DisplayMessage(const JCharacter* message) = 0;
	virtual void		ReportError(const JCharacter* message) = 0;

	virtual JBoolean	AskUserYes(const JCharacter* message) = 0;
	virtual JBoolean	AskUserNo(const JCharacter* message) = 0;

	virtual CloseAction	OKToClose(const JCharacter* message) = 0;

	virtual JBoolean	AcceptLicense() = 0;

	// control of Message and Error display

	JBoolean	IsSilent() const;
	void		SetSilent(const JBoolean beQuiet);

	static JBoolean	GetBreakMessageCROnly();
	static void		SetBreakMessageCROnly();

private:

	JBoolean	itsSilenceFlag;

	static JBoolean		theBreakCROnlyFlag;

private:

	// not allowed

	JUserNotification(const JUserNotification& source);
	const JUserNotification& operator=(const JUserNotification& source);
};


/******************************************************************************
 Silence

 ******************************************************************************/

inline JBoolean
JUserNotification::IsSilent()
	const
{
	return itsSilenceFlag;
}

inline void
JUserNotification::SetSilent
	(
	const JBoolean beQuiet
	)
{
	itsSilenceFlag = beQuiet;
}

/******************************************************************************
 BreakMessageCROnly (static)

	Turn on breakCROnly for the next message only.

 ******************************************************************************/

inline JBoolean
JUserNotification::GetBreakMessageCROnly()
{
	const JBoolean result = theBreakCROnlyFlag;
	theBreakCROnlyFlag    = kJFalse;
	return result;
}

inline void
JUserNotification::SetBreakMessageCROnly()
{
	theBreakCROnlyFlag = kJTrue;
}

#endif
