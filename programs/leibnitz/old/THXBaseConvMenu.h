/******************************************************************************
 THXBaseConvMenu.h

	Interface for the THXBaseConvMenu class

	Copyright (C) 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_THXBaseConvMenu
#define _H_THXBaseConvMenu

#include <JXTextMenu.h>

class THXBaseConvMenu : public JXTextMenu
{
public:

	THXBaseConvMenu(JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~THXBaseConvMenu();

	JSize	GetBase() const;
	void	SetBase(const JSize base);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JIndex	itsItemIndex;

private:

	void	BuildMenu();

	// not allowed

	THXBaseConvMenu(const THXBaseConvMenu& source);
	const THXBaseConvMenu& operator=(const THXBaseConvMenu& source);

public:

	// JBroadcaster messages

	static const JCharacter* kBaseChanged;

	class BaseChanged : public JBroadcaster::Message
		{
		public:

			BaseChanged(const JSize base)
				:
				JBroadcaster::Message(kBaseChanged),
				itsBase(base)
				{ };

			JSize
			GetBase() const
			{
				return itsBase;
			};

		private:

			const JSize itsBase;
		};
};

#endif
