/******************************************************************************
 CBTECaretInputBase.h

	Interface for the CBTECaretInputBase class

	Copyright © 1999 by John Lindal.

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
	void		ShouldOptimizeUpdate(const bool optimize);

	virtual void	HandleKeyPress(const JUtf8Character& c,
								   const int keySym, const JXKeyModifiers& modifiers) override;

protected:

	void			ShouldAct(const bool act);
	virtual void	Act(JXTEBase* te, const JIndex value) = 0;
	virtual JIndex	GetValue(JXTEBase* te) const = 0;
	virtual JIndex	GetValue(const JTextEditor::CaretLocationChanged& info) const = 0;

	virtual void		BoundsResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual JCoordinate	GetFTCMinContentSize(const bool horizontal) const override;

	virtual void		HandleFocusEvent() override;
	virtual void		HandleUnfocusEvent() override;
	virtual bool	OKToUnfocus() override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXTEBase*	itsTE;				// not owned
	JInteger	itsOrigValue;
	bool	itsShouldActFlag;
	bool	itsOptimizeUpdateFlag;

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
	const bool optimize
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
	const bool act
	)
{
	itsShouldActFlag = act;
}

#endif
