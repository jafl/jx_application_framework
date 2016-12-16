/******************************************************************************
 SCXferFnDirector.h

	Interface for the SCXferFnDirector class

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCXferFnDirector
#define _H_SCXferFnDirector

#include "SCDirectorBase.h"

class SCExprEditor;
class JXTextButton;
class JXVertPartition;

class SCXferFnDirector : public SCDirectorBase
{
public:

	SCXferFnDirector(SCCircuitDocument* supervisor);
	SCXferFnDirector(std::istream& input, const JFileVersion vers,
					 SCCircuitDocument* supervisor);

	virtual ~SCXferFnDirector();

	virtual void	StreamOut(std::ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	SCExprEditor*	itsXferFn;
	SCExprEditor*	itsResult;

// begin JXLayout

	JXVertPartition* itsPartition;
	JXTextButton*    itsEvalButton;

// end JXLayout

private:

	void	BuildWindow(SCCircuitDocument* doc);
	void	EvaluateTransferFunction();

	// not allowed

	SCXferFnDirector(const SCXferFnDirector& source);
	const SCXferFnDirector& operator=(const SCXferFnDirector& source);
};

#endif
