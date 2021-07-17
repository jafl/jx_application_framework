/******************************************************************************
 CMBreakpointManager.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_CMBreakpointManager
#define _H_CMBreakpointManager

#include "CMBreakpoint.h"

class CMGetBreakpoints;

class CMBreakpointManager : virtual public JBroadcaster
{
public:

	CMBreakpointManager(CMLink* link, CMGetBreakpoints* cmd);

	virtual	~CMBreakpointManager();

	const JPtrArray<CMBreakpoint>&	GetBreakpoints() const;
	const JPtrArray<CMBreakpoint>&	GetOtherpoints() const;

	bool	HasBreakpointAt(const CMLocation& loc) const;
	bool	GetBreakpoints(const JString& fileName,
							   JPtrArray<CMBreakpoint>* list) const;
	bool	GetBreakpoints(const CMLocation& loc,
							   JPtrArray<CMBreakpoint>* list) const;

	void	EnableAll();
	void	DisableAll();

	void	ReadSetup(std::istream& input, const JFileVersion vers);
	void	WriteSetup(std::ostream& output) const;

	// called by CMGetBreakpoints, CMBreakpoint

	void	SetUpdateWhenStop(const bool update);

	void	UpdateBreakpoints(const JPtrArray<CMBreakpoint>& bpList,
							  const JPtrArray<CMBreakpoint>& otherList);
	void	BreakpointFileNameResolved(CMBreakpoint* bp);
	void	BreakpointFileNameInvalid(CMBreakpoint* bp);

	static JListT::CompareResult
		CompareBreakpointLocations(CMBreakpoint* const & bp1,
								   CMBreakpoint* const & bp2);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CMLink*						itsLink;
	CMGetBreakpoints*			itsCmd;
	JPtrArray<CMBreakpoint>*	itsBPList;
	JPtrArray<CMBreakpoint>*	itsSavedBPList;		// nullptr unless waiting to restore
	bool					itsRestoreBreakpointsFlag;
	bool					itsUpdateWhenStopFlag;
	JPtrArray<CMBreakpoint>*	itsOtherList;		// display only

private:

	// not allowed

	CMBreakpointManager(const CMBreakpointManager& source);
	const CMBreakpointManager& operator=(const CMBreakpointManager& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kBreakpointsChanged;

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
	const bool update
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
