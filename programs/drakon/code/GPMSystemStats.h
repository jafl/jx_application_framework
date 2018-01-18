/******************************************************************************
 GPMSystemStats.h

	Interface for the GPMSystemStats class

	Copyright (C) 2011 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_GPMSystemStats
#define _H_GPMSystemStats

#include <JXWidget.h>

class GPMProcessList;

class GPMSystemStats : public JXWidget
{
public:

	GPMSystemStats(GPMProcessList* processList, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~GPMSystemStats();

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;	
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

public:

	struct CPU
	{
		JFloat user;
		JFloat other;
	};

private:

	const JIndex	itsUID;
	GPMProcessList*	itsProcessList;		// not owned
	JArray<CPU>*	itsCPUHistory;		// newest appended to the end
	JFloat			itsMaxCPU;

private:

	void	ComputeStats(JFloat* userCPUPercentage, JFloat* otherCPUPercentage,
						 JFloat* userMemoryPercentage, JFloat* otherMemoryPercentage) const;

	// not allowed

	GPMSystemStats(const GPMSystemStats& source);
	const GPMSystemStats& operator=(const GPMSystemStats& source);
};

#endif
