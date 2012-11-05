/******************************************************************************
 CBTECaretInputBase.h

	Interface for the CBTECaretInputBase class

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBTECaretInputBase
#define _H_CBTECaretInputBase

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXIntegerInput.h>

class JXStaticText;

class CBTECaretInputBase : public JXIntegerInput
{
public:

	CBTECaretInputBase(JXStaticText* label, JXContainer* enclosure,
					   const HSizingOption hSizing, const VSizingOption vSizing,
					   const JCoordinate x, const JCoordinate y,
					   const JCoordinate w, const JCoordinate h);

	virtual ~CBTECaretInputBase();

	JXTEBase*	GetTE();
	void		SetTE(JXTEBase* te);
	void		ShouldOptimizeUpdate(const JBoolean optimize);

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

protected:

	void			ShouldAct(const JBoolean act);
	virtual void	Act(JXTEBase* te, const JIndex value) = 0;
	virtual JIndex	GetValue(JXTEBase* te) const = 0;
	virtual JIndex	GetValue(const JTextEditor::CaretLocationChanged& info) const = 0;

	virtual void		HandleFocusEvent();
	virtual void		HandleUnfocusEvent();
	virtual JBoolean	OKToUnfocus();

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXTEBase*	itsTE;				// not owned
	JInteger	itsOrigValue;
	JBoolean	itsShouldActFlag;
	JBoolean	itsOptimizeUpdateFlag;

	JXStaticText*	itsLabel;		// not owned

private:

	// not allowed

	CBTECaretInputBase(const CBTECaretInputBase& source);
	const CBTECaretInputBase& operator=(const CBTECaretInputBase& source);
};


/******************************************************************************
 GetTE

 ******************************************************************************/

inline JXTEBase*
CBTECaretInputBase::GetTE()
{
	return itsTE;
}

/******************************************************************************
 ShouldOptimizeUpdate

 ******************************************************************************/

inline void
CBTECaretInputBase::ShouldOptimizeUpdate
	(
	const JBoolean optimize
	)
{
	itsOptimizeUpdateFlag = optimize;
}

/******************************************************************************
 ShouldAct (protected)

 ******************************************************************************/

inline void
CBTECaretInputBase::ShouldAct
	(
	const JBoolean act
	)
{
	itsShouldActFlag = act;
}

#endif
