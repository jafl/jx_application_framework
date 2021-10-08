/******************************************************************************
 JXToolBar.h

	Copyright (C) 1998 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_JXToolBar
#define _H_JXToolBar

#include "jx-af/jx/JXWidgetSet.h"
#include <jx-af/jcore/JPrefObject.h>
#include "jx-af/jx/JXToolBarButton.h"	// need defn of Type

class JTree;
class JTreeNode;
class JNamedTreeNode;
class JXMenu;
class JXMenuBar;
class JXTextMenu;
class JXToolBarEditDir;
class JXTimerTask;
class JXAdjustToolBarGeometryTask;

class JXToolBar : public JXWidgetSet, public JPrefObject
{
public:

	friend class JXToolBarButton;
	friend class JXAdjustToolBarGeometryTask;

public:

	JXToolBar(JPrefsManager* prefsMgr, const JPrefID& id, JXMenuBar* menuBar,
				JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	~JXToolBar() override;

	JXWidgetSet*	GetWidgetEnclosure() const;

	void	LoadPrefs();

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

	JCoordinate						itsNextButtonPosition;
	bool						itsInNewGroupMode;
	JPtrArray<JXToolBarButton>*		itsButtons;
	JPtrArray<JXMenu>*				itsMenus;
	JXTimerTask*					itsTimerTask;
	JXAdjustToolBarGeometryTask*	itsAdjustTask;
	JXToolBarEditDir*				itsEditDialog;
	JTree*							itsMenuTree;
	JXMenuBar*						itsMenuBar;
	JSize							itsCurrentButtonHeight;
	JXWidgetSet*					itsToolBarSet;
	JXWidgetSet*					itsToolBarEnclosure;
	JArray<bool>*				itsGroupStarts;
	JCoordinate						itsCurrentLineY;
	bool						itsIsShowingButtons;
	bool						itsWasShowingButtons;
	JString							itsDialogPrefs;
	JXToolBarButton::Type			itsButtonType;
	bool						itsLoadedPrefs;

private:

	void		AdjustToolBarGeometry();
	void		AdjustGeometryIfNeeded();
	void		AdjustWindowMinSize();

	void		GetGroups(JArray<JIndexRange>* groups);
	void		PlaceButton(const JIndex index);
	void		PlaceGroup(const JIndexRange& range);
	JSize		GetGroupWidth(const JIndexRange& range);
	void		NewLine();

	void		RevertBar();

	bool	ItemIsUsed(JXTextMenu* menu, const JIndex index);
	void		FindItemAndAdd(const JString& id);
	void		FindItemAndAdd(JXTextMenu* menu, const JString& id);

	void		ExtractChanges();
	void		ExtractItemNodes(JTreeNode* menuNode);
	void		BuildTree();
	void		AddMenuToTree(JXTextMenu* menu, JNamedTreeNode* parent, const JString& name);

	void		UpdateButtons();

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
};

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
