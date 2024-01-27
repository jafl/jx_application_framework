/******************************************************************************
 JXToolBar.h

	Copyright (C) 1998 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_JXToolBar
#define _H_JXToolBar

#include "JXWidgetSet.h"
#include <jx-af/jcore/JPrefObject.h>
#include "JXToolBarButton.h"	// for Type

class JTree;
class JTreeNode;
class JNamedTreeNode;
class JXMenu;
class JXMenuBar;
class JXTextMenu;
class JXFunctionTask;
class JXUrgentFunctionTask;

class JXToolBar : public JXWidgetSet, public JPrefObject
{
public:

	friend class JXToolBarButton;
	friend class JXAdjustToolBarGeometryTask;

	static const JUtf8Byte* kIgnorePrefix;

	typedef void (*UpgradeFn)(JString* s);

public:

	JXToolBar(JPrefsManager* prefsMgr, const JPrefID& id, JXMenuBar* menuBar,
				JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	~JXToolBar() override;

	JXWidgetSet*	GetWidgetEnclosure() const;

	void	LoadPrefs(std::function<void(JString*)>* f);

	bool	IsEmpty() const;
	void	AppendButton(JXTextMenu* menu, const JIndex index);
	void	AppendButton(JXTextMenu* menu, const JString& id);
	void	NewGroup();

	void	Edit();

	bool	IsUsingSmallButtons() const;
	void	UseSmallButtons(const bool useSmall);

	bool	ToolBarVisible() const;
	void	ShowToolBar(const bool show);

	JXToolBarButton::Type	GetButtonType() const;
	void					SetButtonType(const JXToolBarButton::Type type);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

	void	ReadPrefs(std::istream& is) override;
	void	WritePrefs(std::ostream& os) const override;

	void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;

private:

	JCoordinate					itsNextButtonPosition;
	bool						itsInNewGroupMode;
	JPtrArray<JXToolBarButton>*	itsButtons;
	JPtrArray<JXMenu>*			itsMenus;
	JXFunctionTask*				itsTimerTask;
	JXMenuBar*					itsMenuBar;
	JSize						itsCurrentButtonHeight;
	JXWidgetSet*				itsToolBarSet;
	JXWidgetSet*				itsToolBarEnclosure;
	JArray<bool>*				itsGroupStarts;
	JCoordinate					itsCurrentLineY;
	bool						itsIsShowingButtons;
	bool						itsWasShowingButtons;
	JString						itsDialogPrefs;
	JXToolBarButton::Type		itsButtonType;
	bool						itsLoadedPrefs;
	JSize						itsResizeCount;

	std::function<void(JString*)>*	itsUpgradeFn;

private:

	void	AdjustToolBarGeometry();
	void	AdjustWindowMinSize();
	void	SendPrepareForResize();
	void	SendResizeFinished();

	void	GetGroups(JArray<JIndexRange>* groups);
	void	PlaceButton(const JIndex index);
	void	PlaceGroup(const JIndexRange& range);
	JSize	GetGroupWidth(const JIndexRange& range);
	void	NewLine();

	void	RevertBar();

	bool	ItemIsUsed(JXTextMenu* menu, const JIndex index);
	void	FindItemAndAdd(const JString& id);
	bool	FindItemAndAdd(JXTextMenu* menu, const JString& id);

	void	ExtractItemNodes(JTreeNode* menuNode);
	JTree*	BuildTree();
	void	AddMenuToTree(JTree* tree, JXTextMenu* menu,
						  JNamedTreeNode* parent, const JString& name);

	void	UpdateButtons();

private:

	// base class for JBroadcaster messages

	class DropMsg : public JBroadcaster::Message
	{
	public:

		DropMsg(const JUtf8Byte* type, const JString& id,
				const JArray<Atom>& typeList, const Atom action,
				const Time time, const JXWidget* source)
			:
			JBroadcaster::Message(type),
			itsAction(action), itsID(id), itsTypeList(typeList),
			itsTime(time), itsSource(source)
			{ };

		const JString&
		GetID() const
		{
			return itsID;
		};

		const JArray<Atom>&
		GetTypeList() const
		{
			return itsTypeList;
		};

		Atom
		GetAction() const
		{
			return itsAction;
		};

		Time
		GetTime() const
		{
			return itsTime;
		};

		bool
		GetSource(const JXWidget** w) const
		{
			*w = itsSource;
			return itsSource != nullptr;
		};

	protected:

		Atom	itsAction;		// only one derived class allows this to change

	private:

		const JString&		itsID;
		const JArray<Atom>&	itsTypeList;
		const Time			itsTime;
		const JXWidget*		itsSource;
	};

public:

	// JBroadcaster messages

	static const JUtf8Byte* kWantsToDrop;
	static const JUtf8Byte* kHandleDrop;
	static const JUtf8Byte* kPrepareForResize;
	static const JUtf8Byte* kResizeFinished;

	class WantsToDrop : public DropMsg
	{
	public:

		WantsToDrop(const JString& id,
					const JArray<Atom>& typeList, const Atom action,
					const Time time, const JXWidget* source)
			:
			DropMsg(kWantsToDrop, id, typeList, action, time, source),
			itsAcceptedFlag(false)
			{ };

		void
		SetAction(const Atom action)
		{
			itsAction = action;
		};

		bool
		WasAccepted() const
		{
			return itsAcceptedFlag;
		};

		void
		SetAccepted()
		{
			itsAcceptedFlag = true;
		};

	private:

		bool	itsAcceptedFlag;
	};

	class HandleDrop : public DropMsg
	{
	public:

		HandleDrop(const JString& id,
				   const JArray<Atom>& typeList, const Atom action,
				   const Time time, const JXWidget* source)
			:
			DropMsg(kHandleDrop, id, typeList, action, time, source),
			itsProcessedFlag(false)
			{ };

		bool
		WasProcessed() const
		{
			return itsProcessedFlag;
		};

		void
		SetProcessed()
		{
			itsProcessedFlag = true;
		};

	private:

		bool	itsProcessedFlag;
	};

	class PrepareForResize : public JBroadcaster::Message
	{
	public:

		PrepareForResize()
			:
			JBroadcaster::Message(kPrepareForResize)
			{ };
	};

	class ResizeFinished : public JBroadcaster::Message
	{
	public:

		ResizeFinished()
			:
			JBroadcaster::Message(kResizeFinished)
			{ };
	};
};


/******************************************************************************
 IsEmpty (public)

 ******************************************************************************/

inline bool
JXToolBar::IsEmpty()
	const
{
	return itsButtons->IsEmpty();
}

/******************************************************************************
 GetWidgetEnclosure (public)

 ******************************************************************************/

inline JXWidgetSet*
JXToolBar::GetWidgetEnclosure()
	const
{
	return itsToolBarEnclosure;
}

/******************************************************************************
 ToolBarVisible (public)

 ******************************************************************************/

inline bool
JXToolBar::ToolBarVisible()
	const
{
	return itsIsShowingButtons;
}

#endif
