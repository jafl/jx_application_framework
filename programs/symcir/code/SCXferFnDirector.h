/******************************************************************************
 SCXferFnDirector.h

	Interface for the SCXferFnDirector class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCXferFnDirector
#define _H_SCXferFnDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "SCDirectorBase.h"

class SCExprEditor;
class JXTextButton;
class JXVertPartition;

class SCXferFnDirector : public SCDirectorBase
{
public:

	SCXferFnDirector(SCCircuitDocument* supervisor);
	SCXferFnDirector(istream& input, const JFileVersion vers,
					 SCCircuitDocument* supervisor);

	virtual ~SCXferFnDirector();

	virtual void	StreamOut(ostream& output) const;

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
