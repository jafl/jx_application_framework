/******************************************************************************
 JXRadioGroup.h

	Interface for the JXRadioGroup class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXRadioGroup
#define _H_JXRadioGroup

#include "JXWidget.h"
#include <JPtrArray.h>

class JXRadioButton;

class JXRadioGroup : public JXWidget
{
	friend class JXRadioButton;

public:

	JXRadioGroup(JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~JXRadioGroup();

	JIndex	GetSelectedItem() const;
	void	SelectItem(const JIndex id);

	bool	GetRadioButton(const JIndex id, JXRadioButton** rb) const;

protected:

	virtual void		Draw(JXWindowPainter& p, const JRect& rect) override;
	virtual void		DrawBorder(JXWindowPainter& p, const JRect& frame) override;
	virtual bool	NeedsInternalFTC() const override;

private:

	JPtrArray<JXRadioButton>*	itsButtons;
	JXRadioButton*				itsSelection;

private:

	// called by JXRadioButton

	void	NewButton(JXRadioButton* button);
	void	NewSelection(JXRadioButton* button);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kSelectionChanged;

	class SelectionChanged : public JBroadcaster::Message
		{
		public:

			SelectionChanged(const JIndex id)
				:
				JBroadcaster::Message(kSelectionChanged),
				itsID(id)
				{ };

			JIndex
			GetID()
				const
			{
				return itsID;
			};

		private:

			JIndex itsID;
		};
};

#endif
