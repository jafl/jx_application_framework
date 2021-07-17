/******************************************************************************
 GDBGetBreakpoints.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_GDBGetBreakpoints
#define _H_GDBGetBreakpoints

#include "CMGetBreakpoints.h"
#include "CMBreakpoint.h"	// need defn of Action
#include <JStringPtrMap.h>

class GDBGetBreakpoints : public CMGetBreakpoints
{
public:

	GDBGetBreakpoints();

	virtual	~GDBGetBreakpoints();

protected:

	virtual void	HandleSuccess(const JString& data) override;

private:

	void	ParseBreakpoint(JStringPtrMap<JString>& map,
							JPtrArray<CMBreakpoint>* list);
	void	ParseOther(JStringPtrMap<JString>& map,
					   JPtrArray<CMBreakpoint>* list);

	bool	ParseCommon(JStringPtrMap<JString>& map, JIndex* bpIndex,
							CMBreakpoint::Action* action, bool* enabled,
							JSize* ignoreCount);

	// not allowed

	GDBGetBreakpoints(const GDBGetBreakpoints& source);
	const GDBGetBreakpoints& operator=(const GDBGetBreakpoints& source);

};

#endif
