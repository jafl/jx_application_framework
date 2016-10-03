/******************************************************************************
 SCZinDirector.h

	Interface for the SCZinDirector class

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCZinDirector
#define _H_SCZinDirector

#include "SCDirectorBase.h"

class SCNodeMenu;
class SCExprEditor;
class JXTextButton;

class SCZinDirector : public SCDirectorBase
{
public:

	SCZinDirector(SCCircuitDocument* supervisor);
	SCZinDirector(istream& input, const JFileVersion vers,
				  SCCircuitDocument* supervisor);

	virtual ~SCZinDirector();

	virtual void	StreamOut(ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	SCExprEditor*	itsResult;

// begin JXLayout

	JXTextButton* itsEvalButton;
	SCNodeMenu*   itsPosNode;
	SCNodeMenu*   itsNegNode;

// end JXLayout

private:

	void	BuildWindow(SCCircuitDocument* doc);
	void	EvaluateInputImpedance();

	// not allowed

	SCZinDirector(const SCZinDirector& source);
	const SCZinDirector& operator=(const SCZinDirector& source);
};

#endif
