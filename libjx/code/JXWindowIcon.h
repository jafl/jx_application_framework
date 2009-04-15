/******************************************************************************
 JXWindowIcon.h

	Interface for the JXWindowIcon class

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXWindowIcon
#define _H_JXWindowIcon

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWidget.h>

class JXImage;

class JXWindowIcon : public JXWidget
{
public:

	JXWindowIcon(JXImage* normalImage, JXImage* dropImage,
				 JXWindow* mainWindow, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~JXWindowIcon();

	void	SetIcons(JXImage* normalImage, JXImage* dropImage);

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame);
	virtual void	HandleMouseEnter();

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const Time time, const JXWidget* source);
	virtual void		HandleDNDEnter();
	virtual void		HandleDNDLeave();
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source);

private:

	JXWindow*	itsMainWindow;
	JXImage*	itsNormalImage;
	JXImage*	itsDropImage;

private:

	// not allowed

	JXWindowIcon(const JXWindowIcon& source);
	const JXWindowIcon& operator=(const JXWindowIcon& source);

protected:

	class DropBase : public JBroadcaster::Message
		{
		public:

			DropBase(const JCharacter* type,
					 const JArray<Atom>& typeList, Atom* action,
					 const Time time, const JXWidget* source)
				:
				JBroadcaster::Message(type),
				itsTypeList(typeList), itsAction(action),
				itsTime(time), itsSource(source)
				{ };

			const JArray<Atom>&
			GetTypeList() const
			{
				return itsTypeList;
			};

			Atom
			GetAction() const
			{
				return *itsAction;
			};

			Atom*
			GetActionPtr() const	// for passing directly to WillAcceptDrop()
			{
				return itsAction;
			};

			void
			SetAction(const Atom action)
			{
				*itsAction = action;
			};

			Time
			GetTime() const
			{
				return itsTime;
			};

			JBoolean
			GetSource(const JXWidget** source) const
			{
				*source = itsSource;
				return JI2B( itsSource != NULL );
			};

			const JXWidget*
			GetSource() const	// for passing directly to HandleDNDDrop()
			{
				return itsSource;
			};

		private:

			const JArray<Atom>&	itsTypeList;
			Atom*				itsAction;
			const Time			itsTime;
			const JXWidget*		itsSource;
		};

public:

	// JBroadcaster messages

	static const JCharacter* kAcceptDrop;
	static const JCharacter* kHandleEnter;
	static const JCharacter* kHandleLeave;
	static const JCharacter* kHandleDrop;

	class AcceptDrop : public DropBase
		{
		public:

			AcceptDrop(const JArray<Atom>& typeList, Atom* action,
					   const Time time, const JXWidget* source)
				:
				DropBase(kAcceptDrop, typeList, action, time, source),
				itsAcceptFlag(kJFalse)
				{ };

			JBoolean
			WillAcceptDrop() const
			{
				return itsAcceptFlag;
			};

			void
			ShouldAcceptDrop(const JBoolean accept)
			{
				itsAcceptFlag = accept;
			};

		private:

			JBoolean	itsAcceptFlag;
		};

	class HandleEnter : public JBroadcaster::Message
		{
		public:

			HandleEnter()
				:
				JBroadcaster::Message(kHandleEnter)
				{ };
		};

	class HandleLeave : public JBroadcaster::Message
		{
		public:

			HandleLeave()
				:
				JBroadcaster::Message(kHandleLeave)
				{ };
		};

	class HandleDrop : public DropBase
		{
		public:

			HandleDrop(const JArray<Atom>& typeList, Atom* action,
					   const Time time, const JXWidget* source)
				:
				DropBase(kHandleDrop, typeList, action, time, source)
				{ };
		};
};

#endif
