/******************************************************************************
 CBTECaretInputBase.h

	Interface for the CBTECaretInputBase class

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBTECaretInputBase
#define _H_CBTECaretInputBase

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

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers) override;

protected:

	void			ShouldAct(const JBoolean act);
	virtual void	Act(JXTEBase* te, const JIndex value) = 0;
	virtual JIndex	GetValue(JXTEBase* te) const = 0;
	virtual JIndex	GetValue(const JTextEditor::CaretLocationChanged& info) const = 0;

	virtual void		BoundsResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual JCoordinate$1GetFTCMinContentSize(const JBoolean horizontal) const override;

	virtual void		HandleFocusEvent() override;
	virtual void		HandleUnfocusEvent() override;
	virtual JBoolean	OKToUnfocus() override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXTEBase*	itsTE;				// not owned
	JInteger	itsOrigValue;
	JBoolean	itsShouldActFlag;
	JBoolean	itsOptimizeUpdateFlag;

	JXStaticText*	itsLabel;		// not owned

private:

	void	Center();

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
