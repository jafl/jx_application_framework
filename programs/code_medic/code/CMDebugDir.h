/******************************************************************************
 CMDebugDir.h

	Copyright (C) 1997 by Glenn Bach.

 ******************************************************************************/

#ifndef _H_CMDebugDir
#define _H_CMDebugDir

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

	virtual const JString&	GetName() const override;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

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
