/******************************************************************************
 JUserNotification.h

	Interface for the JUserNotification class.

	Copyright (C) 1994-96 by John Lindal.

 ******************************************************************************/

#ifndef _H_JUserNotification
#define _H_JUserNotification

#include "jx-af/jcore/JString.h"

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

	virtual void	DisplayMessage(const JString& message) = 0;
	virtual void	ReportError(const JString& message) = 0;

	virtual bool	AskUserYes(const JString& message) = 0;
	virtual bool	AskUserNo(const JString& message) = 0;

	virtual CloseAction	OKToClose(const JString& message) = 0;

	virtual bool	AcceptLicense() = 0;

	// control of Message and Error display

	bool	IsSilent() const;
	void	SetSilent(const bool beQuiet);

	static bool	GetBreakMessageCROnly();
	static void	SetBreakMessageCROnly();

private:

	bool	itsSilenceFlag;

	static bool	theBreakCROnlyFlag;

private:

	// not allowed

	JUserNotification(const JUserNotification&) = delete;
	JUserNotification& operator=(const JUserNotification&) = delete;
};


/******************************************************************************
 Silence

 ******************************************************************************/

inline bool
JUserNotification::IsSilent()
	const
{
	return itsSilenceFlag;
}

inline void
JUserNotification::SetSilent
	(
	const bool beQuiet
	)
{
	itsSilenceFlag = beQuiet;
}

/******************************************************************************
 BreakMessageCROnly (static)

	Turn on breakCROnly for the next message only.

 ******************************************************************************/

inline bool
JUserNotification::GetBreakMessageCROnly()
{
	const bool result = theBreakCROnlyFlag;
	theBreakCROnlyFlag    = false;
	return result;
}

inline void
JUserNotification::SetBreakMessageCROnly()
{
	theBreakCROnlyFlag = true;
}

#endif
