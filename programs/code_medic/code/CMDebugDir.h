/******************************************************************************
 CMDebugDir.h

	Copyright © 1997 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMDebugDir
#define _H_CMDebugDir

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>
#include <fstream>

class JXTextButton;
class JXStaticText;
class CMLink;

class CMDebugDir : public JXWindowDirector
{
public:

	CMDebugDir(JXDirector* supervisor);

	virtual ~CMDebugDir();

	virtual const JString&	GetName() const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	CMLink*			itsLink;
	JXStaticText*	itsText;
	std::ofstream	itsFile;

// begin JXLayout

	JXTextButton* itsCopyButton;

// end JXLayout

private:

	void	BuildWindow();

	// not allowed

	CMDebugDir(const CMDebugDir& source);
	const CMDebugDir& operator=(const CMDebugDir& source);
};

#endif
