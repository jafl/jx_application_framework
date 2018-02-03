/******************************************************************************
 CMCommand.h

	Copyright (C) 1997-2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_CMCommand
#define _H_CMCommand

#include <JString.h>

class CMCommand
{
public:

	enum State
	{
		kUnassigned,
		kPending,
		kExecuting
	};

public:

	CMCommand(const JCharacter* cmd,
			  const JBoolean oneShot, const JBoolean background);
	CMCommand(const JString& cmd,
			  const JBoolean oneShot, const JBoolean background);

	virtual	~CMCommand();

	// State information

	const JString&	GetCommand() const;
	JIndex			GetTransactionID() const;
	State			GetState() const;
	JBoolean		IsOneShot() const;
	JBoolean		IsBackground() const;
	JBoolean		WillIgnoreResult() const;			// mainly for gdb-mi
	void			ShouldIgnoreResult(const JBoolean ignore);

	// Execution

	JBoolean	Send();

	// called by CMLink

	void			SetTransactionID(const JIndex id);
	virtual void	Starting();
	void			Accumulate(const JCharacter* data);
	void			SaveResult(const JCharacter* data);	// mainly for gdb-mi
	void			Finished(const JBoolean success);

protected:

	void	SetCommand(const JCharacter* cmd);

	virtual void	HandleSuccess(const JString& data) = 0;
	virtual void	HandleFailure();

	const JString&				GetLastResult() const;
	const JPtrArray<JString>&	GetResults() const;

private:

	JString				itsCommandString;
	JString				itsData;
	JPtrArray<JString>*	itsResultList;
	JIndex				itsID;
	State				itsState;
	JBoolean			itsDeleteFlag;
	JBoolean			itsBackgroundFlag;
	JBoolean			itsIgnoreResultFlag;

private:

	// not allowed

	CMCommand(const CMCommand& source);
	const CMCommand& operator=(const CMCommand& source);
};


/******************************************************************************
 GetCommand

 *****************************************************************************/

inline const JString&
CMCommand::GetCommand()
	const
{
	return itsCommandString;
}

/******************************************************************************
 SetCommand (protected)

 *****************************************************************************/

inline void
CMCommand::SetCommand
	(
	const JCharacter* cmd
	)
{
	itsCommandString = cmd;
}

/******************************************************************************
 GetTransactionID

 *****************************************************************************/

inline JIndex
CMCommand::GetTransactionID()
	const
{
	return itsID;
}

/******************************************************************************
 GetState

 *****************************************************************************/

inline CMCommand::State
CMCommand::GetState()
	const
{
	return itsState;
}

/******************************************************************************
 IsOneShot

 *****************************************************************************/

inline JBoolean
CMCommand::IsOneShot()
	const
{
	return itsDeleteFlag;
}

/******************************************************************************
 IsBackground

 *****************************************************************************/

inline JBoolean
CMCommand::IsBackground()
	const
{
	return itsBackgroundFlag;
}

/******************************************************************************
 Ignore result

 *****************************************************************************/

inline JBoolean
CMCommand::WillIgnoreResult()
	const
{
	return itsIgnoreResultFlag;
}

inline void
CMCommand::ShouldIgnoreResult
	(
	const JBoolean ignore
	)
{
	itsIgnoreResultFlag = ignore;
}

/******************************************************************************
 Accumulate

 *****************************************************************************/

inline void
CMCommand::Accumulate
	(
	const JCharacter* data
	)
{
	itsData += data;
}

#endif
