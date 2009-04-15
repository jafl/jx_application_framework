/******************************************************************************
 SCFeedbackDirector.h

	Interface for the SCFeedbackDirector class

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCFeedbackDirector
#define _H_SCFeedbackDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "SCDirectorBase.h"

class SCExprEditor;
class SCComponentMenu;
class JXTextButton;
class JXHorizPartition;
class JXVertPartition;

class SCFeedbackDirector : public SCDirectorBase
{
public:

	SCFeedbackDirector(SCCircuitDocument* supervisor);
	SCFeedbackDirector(istream& input, const JFileVersion vers,
					   SCCircuitDocument* supervisor);

	virtual ~SCFeedbackDirector();

	virtual void	StreamOut(ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXHorizPartition*	itsHPartition;
	JXHorizPartition*	itsTPartition;

	SCExprEditor*	itsOutputFn;
	SCExprEditor*	itsH0;
	SCExprEditor*	itsHinf;
	SCExprEditor*	itsT;
	SCExprEditor*	itsTn;
	SCExprEditor*	itsScratchFn;

	JIndex	itsBuildH0Index;
	JIndex	itsBuildHinfIndex;

// begin JXLayout

    JXVertPartition* itsMainPartition;
    SCComponentMenu* itsInputSource;
    SCComponentMenu* itsDepSource;
    JXTextButton*    itsEvalButton;

// end JXLayout

private:

	void	BuildWindow(SCCircuitDocument* doc);
	void	EvaluateFeedbackParameters();

	void	UpdateExtraMenu();
	void	HandleExtraMenu(const JIndex item);

	// not allowed

	SCFeedbackDirector(const SCFeedbackDirector& source);
	const SCFeedbackDirector& operator=(const SCFeedbackDirector& source);
};

#endif
