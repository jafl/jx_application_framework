/******************************************************************************
 SCNDIDirector.h

	Interface for the SCNDIDirector class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCNDIDirector
#define _H_SCNDIDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "SCDirectorBase.h"

class SCExprEditor;
class SCComponentMenu;
class JXTextButton;
class JXHorizPartition;
class JXVertPartition;

class SCNDIDirector : public SCDirectorBase
{
public:

	SCNDIDirector(SCCircuitDocument* supervisor);
	SCNDIDirector(istream& input, const JFileVersion vers,
				  SCCircuitDocument* supervisor);

	virtual ~SCNDIDirector();

	virtual void	StreamOut(ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXHorizPartition*	itsInputPartition;

	SCExprEditor*	itsXferFn;
	SCExprEditor*	itsNullFn;
	SCExprEditor*	itsResult;

// begin JXLayout

	JXVertPartition* itsMainPartition;
	SCComponentMenu* itsSource1;
	SCComponentMenu* itsSource2;
	JXTextButton*    itsEvalButton;

// end JXLayout

private:

	void	BuildWindow(SCCircuitDocument* doc);
	void	EvaluateNDITransferFunction();

	// not allowed

	SCNDIDirector(const SCNDIDirector& source);
	const SCNDIDirector& operator=(const SCNDIDirector& source);
};

#endif
