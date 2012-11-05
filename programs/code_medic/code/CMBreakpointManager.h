/******************************************************************************
 CMBreakpointManager.h

	Copyright � 2001 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CMBreakpointManager
#define _H_CMBreakpointManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMBreakpoint.h"

class CMGetBreakpoints;

class CMBreakpointManager : virtual public JBroadcaster
{
public:

	CMBreakpointManager(CMLink* link, CMGetBreakpoints* cmd);

	virtual	~CMBreakpointManager();

	const JPtrArray<CMBreakpoint>&	GetBreakpoints() const;
	const JPtrArray<CMBreakpoint>&	GetOtherpoints() const;

	JBoolean	HasBreakpointAt(const CMLocation& loc) const;
	JBoolean	GetBreakpoints(const JCharacter* fileName,
							   JPtrArray<CMBreakpoint>* list) const;
	JBoolean	GetBreakpoints(const CMLocation& loc,
							   JPtrArray<CMBreakpoint>* list) const;

	void	ReadSetup(istream& input, const JFileVersion vers);
	void	WriteSetup(ostream& output) const;

	// called by CMGetBreakpoints, CMBreakpoint

	void	SetUpdateWhenStop(const JBoolean update);

	void	UpdateBreakpoints(const JPtrArray<CMBreakpoint>& bpList,
							  const JPtrArray<CMBreakpoint>& otherList);
	void	BreakpointFileNameResolved(CMBreakpoint* bp);
	void	BreakpointFileNameInvalid(CMBreakpoint* bp);

	static JOrderedSetT::CompareResult
		CompareBreakpointLocations(CMBreakpoint* const & bp1,
								   CMBreakpoint* const & bp2);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	CMLink*						itsLink;
	CMGetBreakpoints*			itsCmd;
	JPtrArray<CMBreakpoint>*	itsBPList;
	JPtrArray<CMBreakpoint>*	itsSavedBPList;		// NULL unless waiting to restore
	JBoolean					itsRestoreBreakpointsFlag;
	JBoolean					itsUpdateWhenStopFlag;
	JPtrArray<CMBreakpoint>*	itsOtherList;		// display only

private:

	// not allowed

	CMBreakpointManager(const CMBreakpointManager& source);
	const CMBreakpointManager& operator=(const CMBreakpointManager& source);

public:

	// JBroadcaster messages

	static const JCharacter* kBreakpointsChanged;

	class BreakpointsChanged : public JBroadcaster::Message
		{
		public:

			BreakpointsChanged()
				:
				JBroadcaster::Message(kBreakpointsChanged)
				{ };
		};
};


/******************************************************************************
 SetUpdateWhenStop

 ******************************************************************************/

inline void
CMBreakpointManager::SetUpdateWhenStop
	(
	const JBoolean update
	)
{
	itsUpdateWhenStopFlag = update;
}

/******************************************************************************
 GetBreakpoints

 *****************************************************************************/

inline const JPtrArray<CMBreakpoint>&
CMBreakpointManager::GetBreakpoints()
	const
{
	return *itsBPList;
}

/******************************************************************************
 GetOtherpoints

 *****************************************************************************/

inline const JPtrArray<CMBreakpoint>&
CMBreakpointManager::GetOtherpoints()
	const
{
	return *itsOtherList;
}

#endif
