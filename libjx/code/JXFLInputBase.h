/******************************************************************************
 JXFLInputBase.h

	Interface for the JXFLInputBase class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXFLInputBase
#define _H_JXFLInputBase

#include "jx-af/jx/JXInputField.h"

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

	~JXFLInputBase() override;

	virtual JString	GetRegexString() const = 0;
	JError			Apply() const;

	void	HandleKeyPress(const JUtf8Character& c, const int keySym,
						   const JXKeyModifiers& modifiers) override;

protected:

	bool	OKToUnfocus() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXFileListSet*			itsFLSet;		// owns us
	JXStringHistoryMenu*	itsHistoryMenu;
};

#endif
