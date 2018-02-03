/******************************************************************************
 JXWindowIcon.h

	Interface for the JXWindowIcon class

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXWindowIcon
#define _H_JXWindowIcon

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

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
	virtual void	HandleMouseEnter() override;

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source) override;
	virtual void		HandleDNDEnter() override;
	virtual void		HandleDNDLeave() override;
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source) override;

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

			DropBase(const JUtf8Byte* type,
					 const JArray<Atom>& typeList, Atom* action,
					 const JPoint& pt, const Time time, const JXWidget* source)
				:
				JBroadcaster::Message(type),
				itsTypeList(typeList), itsAction(action),
				itsPt(pt), itsTime(time), itsSource(source)
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

			const JPoint&
			GetPoint() const
			{
				return itsPt;
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
			const JPoint&		itsPt;
			const Time			itsTime;
			const JXWidget*		itsSource;
		};

public:

	// JBroadcaster messages

	static const JUtf8Byte* kAcceptDrop;
	static const JUtf8Byte* kHandleEnter;
	static const JUtf8Byte* kHandleLeave;
	static const JUtf8Byte* kHandleDrop;

	class AcceptDrop : public DropBase
		{
		public:

			AcceptDrop(const JArray<Atom>& typeList, Atom* action,
					   const JPoint& pt, const Time time, const JXWidget* source)
				:
				DropBase(kAcceptDrop, typeList, action, pt, time, source),
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
					   const JPoint& pt, const Time time, const JXWidget* source)
				:
				DropBase(kHandleDrop, typeList, action, pt, time, source)
				{ };
		};
};

#endif
