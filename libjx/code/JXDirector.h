/******************************************************************************
 JXDirector.h

	Interface for the JXDirector class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXDirector
#define _H_JXDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JBroadcaster.h>
#include <JPtrArray.h>

class JXDirector : virtual public JBroadcaster
{
public:

	JXDirector(JXDirector* supervisor);

	virtual ~JXDirector();

	JBoolean			IsClosing() const;
	virtual JBoolean	Close();

	virtual void		Activate();
	virtual JBoolean	Deactivate();
	JBoolean			IsActive() const;

	virtual void	Suspend();
	virtual void	Resume();
	JBoolean		IsSuspended() const;

	JBoolean	HasSubdirectors() const;
	JBoolean	GetSubdirectors(const JPtrArray<JXDirector>** list) const;

	// needed by JXDialogDirector until dynamic_cast works

	virtual JBoolean	IsWindowDirector() const;

protected:

	JXDirector*	GetSupervisor() const;
	JBoolean	CloseAllSubdirectors();

	virtual void	DirectorClosed(JXDirector* theDirector);

private:

	JXDirector*				itsSupervisor;
	JPtrArray<JXDirector>*	itsSubdirectors;

	JBoolean	itsActiveFlag;
	JSize		itsSuspendCount;
	JBoolean	itsClosingFlag;

private:

	void	AddDirector(JXDirector* theDirector);
	void	RemoveDirector(JXDirector* theDirector);

	// not allowed

	JXDirector(const JXDirector& source);
	const JXDirector& operator=(const JXDirector& source);
};

/******************************************************************************
 IsActive

 ******************************************************************************/

inline JBoolean
JXDirector::IsActive()
	const
{
	return itsActiveFlag;
}

/******************************************************************************
 IsSuspended

 ******************************************************************************/

inline JBoolean
JXDirector::IsSuspended()
	const
{
	return JConvertToBoolean( itsSuspendCount > 0 );
}

/******************************************************************************
 IsClosing

 ******************************************************************************/

inline JBoolean
JXDirector::IsClosing()
	const
{
	return itsClosingFlag;
}

/******************************************************************************
 HasSubdirectors

 ******************************************************************************/

inline JBoolean
JXDirector::HasSubdirectors()
	const
{
	return JI2B( itsSubdirectors != NULL && !itsSubdirectors->IsEmpty() );
}

/******************************************************************************
 GetSubdirectors

 ******************************************************************************/

inline JBoolean
JXDirector::GetSubdirectors
	(
	const JPtrArray<JXDirector>** list
	)
	const
{
	*list = itsSubdirectors;
	return HasSubdirectors();
}

/******************************************************************************
 GetSupervisor (protected)

 ******************************************************************************/

inline JXDirector*
JXDirector::GetSupervisor()
	const
{
	return itsSupervisor;
}

#endif
