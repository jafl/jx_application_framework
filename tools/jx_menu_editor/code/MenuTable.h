/******************************************************************************
 MenuTable.h

	Copyright © 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_MenuTable
#define _H_MenuTable

#include <jx-af/jx/JXEditTable.h>
#include <jx-af/jx/JXMenu.h>	// need ItemType
#include <jx-af/jcore/JPrefObject.h>

class JDirInfo;
class JXTextMenu;
class JXImageMenu;
class JXInputField;
class JXCharInput;
class JXColHeaderWidget;
class MenuDocument;
class ImportDialog;

class MenuTable : public JXEditTable, public JPrefObject
{
public:

	struct ItemInfo
	{
		JXMenu::ItemType	type;
		JIndex				iconIndex;
		JString*			text;
		JString*			shortcut;
		JUtf8Character		windowsKey;
		JString*			id;
		JString*			enumName;
		bool				separator;

		ItemInfo()
			:
			type(JXMenu::kPlainType), iconIndex(0),
			text(nullptr), shortcut(nullptr), windowsKey(' '), id(nullptr),
			enumName(nullptr), separator(false)
		{ };

		ItemInfo(JString* t, JString* s, JString* _id, JString* e)
			:
			type(JXMenu::kPlainType), iconIndex(0),
			text(t), shortcut(s), windowsKey(' '), id(_id),
			enumName(e), separator(false)
		{ };

		ItemInfo(const JXMenu::ItemType _type, const JIndex _icon,
				JString* t, JString* s, const JUtf8Character& wk, JString* _id,
				JString* e, const bool sep)
			:
			type(_type), iconIndex(_icon),
			text(t), shortcut(s), windowsKey(wk), id(_id),
			enumName(e), separator(sep)
		{ };

		ItemInfo	Copy() const;
		void		Free();
	};

public:

	MenuTable(MenuDocument* doc, JXMenuBar* menuBar,
			  JXTEBase* editMenuProvider, JXTextMenu* editMenu,
			  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			  const HSizingOption hSizing, const VSizingOption vSizing,
			  const JCoordinate x, const JCoordinate y,
			  const JCoordinate w, const JCoordinate h);

	~MenuTable() override;

	void	ReadGeometry(std::istream& input);
	void	WriteGeometry(std::ostream& output) const;
	void	SetColTitles(JXColHeaderWidget* widget) const;

	void	ReadMenuItems(std::istream& input);
	void	WriteMenuItems(std::ostream& output) const;

	ItemInfo	ReadMenuItem(std::istream& input, const JFileVersion vers);
	void		WriteMenuItem(std::ostream& output, const ItemInfo& item) const;

	bool	IsEditable(const JPoint& cell) const override;
	void	HandleKeyPress(const JUtf8Character& c,
						   const int keySym, const JXKeyModifiers& modifiers) override;

	void	RemoveSelectedItem();

	void	GenerateCode(std::ostream& output, const JString& className,
						 const JString& menuTitle, const JString& menuTitleShortcut,
						 std::ostream& enumOutput, const JString& enumFileName) const;
	void	GenerateStrings(std::ostream& output, const JString& className) const;

	void	FillInItemIDs(const JString& className);
	void	RebuildIconMenu();

	bool	ValidateWindowsKeys();

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;

	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;

	Atom	GetDNDAction(const JXContainer* target,
						 const JXButtonStates& buttonStates,
						 const JXKeyModifiers& modifiers) override;

	bool	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
						   const JPoint& pt, const Time time,
						   const JXWidget* source) override;
	void	HandleDNDEnter() override;
	void	HandleDNDHere(const JPoint& pt, const JXWidget* source) override;
	void	HandleDNDLeave() override;
	void	HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
						  const Atom action, const Time time,
						  const JXWidget* source) override;

	void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	bool			ExtractInputData(const JPoint& cell) override;
	void			PrepareDeleteXInputField() override;

	void	ReadPrefs(std::istream& input) override;
	void	WritePrefs(std::ostream& output) const override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	class ItemList : public JArray<ItemInfo>
	{
	public:

		void	DeleteAll();
	};

private:

	MenuDocument*	itsDoc;
	ItemList*		itsItemList;
	JXTEBase*		itsEditMenuProvider;
	JXTextMenu*		itsEditMenu;
	JXTextMenu*		itsTableMenu;
	JXTextMenu*		itsTypeMenu;
	JXImageMenu*	itsIconMenu;

	JPtrArray<JString>*	itsIconPathList;

	JXInputField*	itsTextInput;
	JXCharInput*	itsCharInput;

	JPoint	itsStartPt;
	JIndex	itsDNDRowIndex;
	Atom	itsMenuItemXAtom;

private:

	void	AddItem();
	void	DuplicateSelectedItems();

	void	Import(ImportDialog* dlog);
	void	ImportActionDefs(const JString& fullName,
							 JStringPtrMap<JString>* actionMap) const;

	void	GenerateUniqueID(ItemInfo* info);

	void	UpdateEditMenu();
	void	HandleEditMenu(const JIndex index);

	void	HandleTableMenu(const JIndex index);

	void	UpdateTypeMenu();
	void	HandleTypeMenu(const JIndex index);

	void	HandleIconMenu(const JIndex index);
	void	BuildIconMenu();
	void	LoadIcons(const JDirInfo& info);
};

#endif
