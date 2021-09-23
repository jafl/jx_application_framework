/******************************************************************************
 JXTabGroup.h

	Copyright (C) 2002-08 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXTabGroup
#define _H_JXTabGroup

#include "JXCardFile.h"
#include <JPtrArray-JString.h>
#include <JFont.h>

class JXTextMenu;
class JXImage;

class JXTabGroup : public JXWidget
{
public:

	enum Edge
	{
		kTop,
		kLeft,
		kBottom,
		kRight
	};

public:

	JXTabGroup(JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	virtual ~JXTabGroup();

	virtual void	Activate() override;		// must call inherited

	JSize	GetTabCount() const;
	bool	GetCurrentTabIndex(JIndex* index) const;

	JXContainer*	InsertTab(const JIndex index, const JString& title,
							  const bool closeable = false);
	JXContainer*	PrependTab(const JString& title, const bool closeable = false);
	JXContainer*	AppendTab(const JString& title, const bool closeable = false);

	void			InsertTab(const JIndex index, const JString& title,
							  JXWidgetSet* card, const bool closeable = false);
	void			PrependTab(const JString& title, JXWidgetSet* card,
							   const bool closeable = false);
	void			AppendTab(const JString& title, JXWidgetSet* card,
							  const bool closeable = false);

	JXContainer*	RemoveTab(const JIndex index);
	void			DeleteTab(const JIndex index);
	void			KillFocusOnCurrentTab();

	const JString&	GetTabTitle(const JIndex index) const;
	void			SetTabTitle(const JIndex index, const JString& title);

	bool	TabCanClose(const JIndex index) const;
	void	SetTabCanClose(const JIndex index, const bool closable);

	bool	ShowTab(const JIndex index);
	bool	ShowTab(JXContainer* card);
	void	ShowPreviousTab();
	void	ShowNextTab();
	void	ScrollTabsIntoView();

	Edge	GetTabEdge() const;
	void	SetTabEdge(const Edge edge);

	const JFont&	GetFont() const;

	void	SetFontName(const JString& name);
	void	SetFontSize(const JSize size);
	void	SetFontStyle(const JFontStyle& style);
	void	SetFont(const JFont& font);

	JCoordinate	GetTabTitlePreMargin(const JIndex index) const;
	void		SetTabTitlePreMargin(const JIndex index, const JCoordinate margin);
	JCoordinate	GetTabTitlePostMargin(const JIndex index) const;
	void		SetTabTitlePostMargin(const JIndex index, const JCoordinate margin);

	JXContainer*	GetCardEnclosure();

	void		ReadSetup(std::istream& input);
	static void	SkipSetup(std::istream& input);
	void		WriteSetup(std::ostream& output) const;

protected:

	bool			FindTab(const JPoint& pt, JIndex* index, JRect* rect) const;
	bool			GetMouseTabIndex(JIndex* index) const;
	virtual bool	OKToDeleteTab(const JIndex index);
	bool			ScrollForWheel(const JXMouseButton button,
								   const JXKeyModifiers& modifiers);

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;
	virtual void	DrawBorder(JXWindowPainter& p, const JRect& frame) override;
	virtual void	DrawTab(const JIndex index, JXWindowPainter& p,
							const JRect& rect, const Edge edge);

	virtual void	BoundsResized(const JCoordinate dw, const JCoordinate dh) override;

	virtual void	HandleMouseHere(const JPoint& pt, const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseLeave() override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	virtual bool	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
								   const JPoint& pt, const Time time,
								   const JXWidget* source) override;
	virtual void	HandleDNDHere(const JPoint& pt, const JXWidget* source) override;
	virtual void	HandleDNDScroll(const JPoint& pt, const JXMouseButton scrollButton,
									const JXKeyModifiers& modifiers) override;

	virtual bool	NeedsInternalFTC() const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	struct TabInfo
	{
		bool	closable;
		JCoordinate	preMargin;
		JCoordinate	postMargin;

		TabInfo();
		TabInfo(const bool closable);

		TabInfo(const bool c, const JCoordinate pre, const JCoordinate post)
			:
			closable(c), preMargin(pre), postMargin(post)
		{ };
	};

	enum DragAction
	{
		kInvalidClick,
		kScrollUp,
		kScrollDown,
		kClose
	};

private:

	Edge				itsEdge;
	JFont				itsFont;
	JPtrArray<JString>*	itsTitles;
	JArray<TabInfo>*	itsTabInfoList;
	bool				itsCanScrollUpFlag;
	JRect				itsScrollUpRect;
	bool				itsCanScrollDownFlag;
	JRect				itsScrollDownRect;
	JIndex				itsFirstDrawIndex;
	JIndex				itsLastDrawIndex;
	JArray<JRect>*		itsTabRects;			// 1 <=> itsFirstDrawIndex
	JXCardFile*			itsCardFile;
	JXTextMenu*			itsContextMenu;			// nullptr until first used
	JXImage*			itsCloseImage;			// not owned
	JXImage*			itsClosePushedImage;	// not owned
	JIndex				itsPrevTabIndex;		// 0 => none

	// used during dragging

	DragAction	itsDragAction;
	bool		itsScrollUpPushedFlag;
	bool		itsScrollDownPushedFlag;

	JIndex	itsMouseIndex;
	JRect	itsCloseRect;
	bool	itsClosePushedFlag;

private:

	void	UpdateAppearance();
	void	PlaceCardFile();
	void	DrawTabBorder(JXWindowPainter& p, const JRect& rect,
						  const bool isSelected);
	void	DrawCloseButton(const JIndex index, JXWindowPainter& p,
							const JRect& rect);
	void	DrawScrollButtons(JXWindowPainter& p, const JCoordinate lineHeight);
	void	ScrollWait(const JFloat delta) const;

	void	CreateContextMenu();
	void	UpdateContextMenu();
	void	HandleContextMenu(const JIndex index);

	void	ScrollUpToTab(const JIndex index);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kAppearanceChanged;

	class AppearanceChanged : public JBroadcaster::Message
		{
		public:

			AppearanceChanged()
				:
				JBroadcaster::Message(kAppearanceChanged)
				{ };
		};
};


/******************************************************************************
 GetTabCount

 ******************************************************************************/

inline JSize
JXTabGroup::GetTabCount()
	const
{
	return itsTitles->GetElementCount();
}

/******************************************************************************
 GetCurrentTabIndex

 ******************************************************************************/

inline bool
JXTabGroup::GetCurrentTabIndex
	(
	JIndex* index
	)
	const
{
	return itsCardFile->GetCurrentCardIndex(index);
}

/******************************************************************************
 GetMouseTabIndex (protected)

 ******************************************************************************/

inline bool
JXTabGroup::GetMouseTabIndex
	(
	JIndex* index
	)
	const
{
	*index = itsMouseIndex;
	return itsMouseIndex > 0;
}

/******************************************************************************
 PrependTab

 ******************************************************************************/

inline JXContainer*
JXTabGroup::PrependTab
	(
	const JString&	title,
	const bool	closeable
	)
{
	return InsertTab(1, title, closeable);
}

inline void
JXTabGroup::PrependTab
	(
	const JString&	title,
	JXWidgetSet*	card,
	const bool	closeable
	)
{
	InsertTab(1, title, card, closeable);
}

/******************************************************************************
 AppendTab

 ******************************************************************************/

inline JXContainer*
JXTabGroup::AppendTab
	(
	const JString&	title,
	const bool	closeable
	)
{
	return InsertTab(GetTabCount()+1, title, closeable);
}

inline void
JXTabGroup::AppendTab
	(
	const JString&	title,
	JXWidgetSet*	card,
	const bool	closeable
	)
{
	InsertTab(GetTabCount()+1, title, card, closeable);
}

/******************************************************************************
 DeleteTab

	Deletes the specified tab and everything on it.

 ******************************************************************************/

inline void
JXTabGroup::DeleteTab
	(
	const JIndex index
	)
{
	JXContainer* card = RemoveTab(index);
	jdelete card;
}

/******************************************************************************
 KillFocusOnCurrentTab

	If one of the widgets on the current card has focus, we call KillFocus().
	This is useful when you want to discard the current card without saving
	its contents.

 ******************************************************************************/

inline void
JXTabGroup::KillFocusOnCurrentTab()
{
	itsCardFile->KillFocusOnCurrentCard();
}

/******************************************************************************
 Tab edge

 ******************************************************************************/

inline JXTabGroup::Edge
JXTabGroup::GetTabEdge()
	const
{
	return itsEdge;
}

inline void
JXTabGroup::SetTabEdge
	(
	const Edge edge
	)
{
	itsEdge = edge;
	UpdateAppearance();
}

/******************************************************************************
 Tab title

 ******************************************************************************/

inline const JString&
JXTabGroup::GetTabTitle
	(
	const JIndex index
	)
	const
{
	return *(itsTitles->GetElement(index));
}

inline void
JXTabGroup::SetTabTitle
	(
	const JIndex	index,
	const JString&	title
	)
{
	*(itsTitles->GetElement(index)) = title;
	Refresh();
}

/******************************************************************************
 Tab closable

 ******************************************************************************/

inline bool
JXTabGroup::TabCanClose
	(
	const JIndex index
	)
	const
{
	return (itsTabInfoList->GetElement(index)).closable;
}

inline void
JXTabGroup::SetTabCanClose
	(
	const JIndex	index,
	const bool	closable
	)
{
	TabInfo info = itsTabInfoList->GetElement(index);
	if (closable != info.closable)
		{
		info.closable = closable;
		itsTabInfoList->SetElement(index, info);
		Refresh();
		}
}

/******************************************************************************
 Font info

 ******************************************************************************/

inline const JFont&
JXTabGroup::GetFont()
	const
{
	return itsFont;
}

inline void
JXTabGroup::SetFontName
	(
	const JString& name
	)
{
	itsFont.SetName(name);
	UpdateAppearance();
}

inline void
JXTabGroup::SetFontSize
	(
	const JSize size
	)
{
	itsFont.SetSize(size);
	UpdateAppearance();
}

inline void
JXTabGroup::SetFontStyle
	(
	const JFontStyle& style
	)
{
	itsFont.SetStyle(style);
	UpdateAppearance();
}

inline void
JXTabGroup::SetFont
	(
	const JFont& font
	)
{
	itsFont = font;
	UpdateAppearance();
}

/******************************************************************************
 GetCardEnclosure

	Returns the enclosure that must be used for all cards.

 ******************************************************************************/

inline JXContainer*
JXTabGroup::GetCardEnclosure()
{
	return itsCardFile;
}

/******************************************************************************
 UpdateAppearance (private)

 ******************************************************************************/

inline void
JXTabGroup::UpdateAppearance()
{
	itsFirstDrawIndex = 1;
	PlaceCardFile();
	Refresh();
	Broadcast(AppearanceChanged());
}

#endif
