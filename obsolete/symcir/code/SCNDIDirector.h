/******************************************************************************
 SCNDIDirector.h

	Interface for the SCNDIDirector class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_SCNDIDirector
#define _H_SCNDIDirector

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
	SCNDIDirector(std::istream& input, const JFileVersion vers,
				  SCCircuitDocument* supervisor);

	virtual ~SCNDIDirector();

	virtual void	StreamOut(std::ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

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
