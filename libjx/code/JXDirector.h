/******************************************************************************
 JXDirector.h

	Interface for the JXDirector class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXDirector
#define _H_JXDirector

#include <JBroadcaster.h>
#include <JPtrArray.h>

class JXDirector : virtual public JBroadcaster
{
public:

	JXDirector(JXDirector* supervisor);

	virtual ~JXDirector();

	bool			IsClosing() const;
	virtual bool	Close();

	virtual void		Activate();
	virtual bool	Deactivate();
	bool			IsActive() const;

	virtual void	Suspend();
	virtual void	Resume();
	bool		IsSuspended() const;

	bool	HasSubdirectors() const;
	bool	GetSubdirectors(const JPtrArray<JXDirector>** list) const;

	// needed by JXDialogDirector until dynamic_cast works

	virtual bool	IsWindowDirector() const;

protected:

	JXDirector*	GetSupervisor() const;
	bool	CloseAllSubdirectors();

	virtual void	DirectorClosed(JXDirector* theDirector);

private:

	JXDirector*				itsSupervisor;
	JPtrArray<JXDirector>*	itsSubdirectors;

	bool	itsActiveFlag;
	JSize		itsSuspendCount;
	bool	itsClosingFlag;

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

inline bool
JXDirector::IsActive()
	const
{
	return itsActiveFlag;
}

/******************************************************************************
 IsSuspended

 ******************************************************************************/

inline bool
JXDirector::IsSuspended()
	const
{
	return itsSuspendCount > 0;
}

/******************************************************************************
 IsClosing

 ******************************************************************************/

inline bool
JXDirector::IsClosing()
	const
{
	return itsClosingFlag;
}

/******************************************************************************
 HasSubdirectors

 ******************************************************************************/

inline bool
JXDirector::HasSubdirectors()
	const
{
	return itsSubdirectors != nullptr && !itsSubdirectors->IsEmpty();
}

/******************************************************************************
 GetSubdirectors

 ******************************************************************************/

inline bool
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
