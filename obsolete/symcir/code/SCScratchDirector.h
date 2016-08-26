/******************************************************************************
 SCScratchDirector.h

	Interface for the SCScratchDirector class

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCScratchDirector
#define _H_SCScratchDirector

#include "SCDirectorBase.h"

class SCExprEditor;

class SCScratchDirector : public SCDirectorBase
{
public:

	SCScratchDirector(SCCircuitDocument* supervisor);
	SCScratchDirector(istream& input, const JFileVersion vers,
					  SCCircuitDocument* supervisor);

	virtual ~SCScratchDirector();

	virtual void	StreamOut(ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	SCExprEditor*	itsFunction;
	JIndex			itsChangeTitleIndex;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(SCCircuitDocument* doc);

	void	UpdateExtraMenu();
	void	HandleExtraMenu(const JIndex item);

	// not allowed

	SCScratchDirector(const SCScratchDirector& source);
	const SCScratchDirector& operator=(const SCScratchDirector& source);
};

#endif
