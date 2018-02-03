/******************************************************************************
 SCScratchDirector.h

	Interface for the SCScratchDirector class

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_SCScratchDirector
#define _H_SCScratchDirector

#include "SCDirectorBase.h"

class SCExprEditor;

class SCScratchDirector : public SCDirectorBase
{
public:

	SCScratchDirector(SCCircuitDocument* supervisor);
	SCScratchDirector(std::istream& input, const JFileVersion vers,
					  SCCircuitDocument* supervisor);

	virtual ~SCScratchDirector();

	virtual void	StreamOut(std::ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

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
