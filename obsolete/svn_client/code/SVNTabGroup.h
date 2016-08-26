/******************************************************************************
 SVNTabGroup.h

	Copyright © 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SVNTabGroup
#define _H_SVNTabGroup

#include <JXTabGroup.h>

class JXImage;
class SVNBusyTabTask;

class SVNTabGroup : public JXTabGroup
{
public:

	SVNTabGroup(JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~SVNTabGroup();

	void	SetBusyIndex(const JIndex index);
	void	ClearBusyIndex();

	// called by SVNBusyTabTask

	void	IncrementSpinnerIndex();

protected:

	virtual void	DrawTab(const JIndex index, JXWindowPainter& p,
							const JRect& rect, const Edge edge);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JIndex	itsBusyIndex;

	JPtrArray<JXImage>*	itsImageList;
	SVNBusyTabTask*		itsAnimationTask;
	JIndex				itsSpinnerIndex;

private:

	// not allowed

	SVNTabGroup(const SVNTabGroup& source);
	const SVNTabGroup& operator=(const SVNTabGroup& source);
};

#endif
