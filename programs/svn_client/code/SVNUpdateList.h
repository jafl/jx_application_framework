/******************************************************************************
 SVNUpdateList.h

	Copyright @ 2008 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_SVNUpdateList
#define _H_SVNUpdateList

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "SVNListBase.h"

class SVNUpdateList : public SVNListBase
{

public:

	SVNUpdateList(SVNMainDirector* director, JXTextMenu* editMenu,
				  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	virtual ~SVNUpdateList();

	virtual void	RefreshContent();

protected:

	virtual JBoolean	ShouldDisplayLine(JString* line) const;
	virtual void		StyleLine(const JIndex index, const JString& line,
								  const JFontStyle& errorStyle,
								  const JFontStyle& addStyle,
								  const JFontStyle& removeStyle);
	virtual JString		ExtractRelativePath(const JString& line) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JBoolean	itsHasRefreshedFlag;

private:

	// not allowed

	SVNUpdateList(const SVNUpdateList& source);
	const SVNUpdateList& operator=(const SVNUpdateList& source);
};

#endif
