/******************************************************************************
 JXFLInputBase.h

	Interface for the JXFLInputBase class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFLInputBase
#define _H_JXFLInputBase

#include <JXInputField.h>

class JXFileListSet;
class JXStringHistoryMenu;

class JXFLInputBase : public JXInputField
{
public:

	JXFLInputBase(JXFileListSet* flSet, JXStringHistoryMenu* historyMenu,
				  JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	virtual ~JXFLInputBase();

	virtual JString	GetRegexString() const = 0;
	JError			Apply() const;

	virtual void HandleKeyPress(const int key, const JXKeyModifiers& modifiers);														

protected:

	virtual JBoolean	OKToUnfocus() override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXFileListSet*			itsFLSet;		// owns us
	JXStringHistoryMenu*	itsHistoryMenu;

private:

	// not allowed

	JXFLInputBase(const JXFLInputBase& source);
	const JXFLInputBase& operator=(const JXFLInputBase& source);
};

#endif
