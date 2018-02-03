/******************************************************************************
 SCFeedbackDirector.h

	Interface for the SCFeedbackDirector class

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_SCFeedbackDirector
#define _H_SCFeedbackDirector

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
	SCFeedbackDirector(std::istream& input, const JFileVersion vers,
					   SCCircuitDocument* supervisor);

	virtual ~SCFeedbackDirector();

	virtual void	StreamOut(std::ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

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
