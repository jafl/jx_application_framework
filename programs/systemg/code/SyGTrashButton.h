/******************************************************************************
 SyGTrashButton.h

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SyGTrashButton
#define _H_SyGTrashButton

#include <JXImageButton.h>

class SyGTrashButton : public JXImageButton
{
public:

	SyGTrashButton(JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~SyGTrashButton();

	void	UpdateDisplay();

	static void	MoveFilesToTrash(const Time time, const JXWidget* source);

protected:

	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source);
	virtual void		HandleDNDEnter();
	virtual void		HandleDNDLeave();
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source);

	virtual void Receive(JBroadcaster* sender, const Message& message);

private:

	// not allowed

	SyGTrashButton(const SyGTrashButton& source);
	const SyGTrashButton& operator=(const SyGTrashButton& source);
};

#endif
