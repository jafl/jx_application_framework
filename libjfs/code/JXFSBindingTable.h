/******************************************************************************
 JXFSBindingTable.h

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JXFSBindingTable
#define _H_JXFSBindingTable

#include <jx-af/jx/JXEditTable.h>

class JRegex;
class JXTextButton;
class JXTextMenu;
class JXInputField;
class JXColHeaderWidget;
class JFSBindingList;

class JXFSBindingTable : public JXEditTable
{
public:

	JXFSBindingTable(JFSBindingList* list,
					 JXTextButton* addButton, JXTextButton* removeButton,
					 JXTextButton* duplicateButton,
					 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					 const HSizingOption hSizing, const VSizingOption vSizing,
					 const JCoordinate x, const JCoordinate y,
					 const JCoordinate w, const JCoordinate h);

	virtual	~JXFSBindingTable();

	void	SetColTitles(JXColHeaderWidget* widget) const;
	void	SyncWithBindingList();

	virtual void		HandleKeyPress(const JUtf8Character& c, const int keySym,
									   const JXKeyModifiers& modifiers) override;
	virtual bool	IsEditable(const JPoint& cell) const override;

protected:

	virtual void	TableDrawCell(JPainter& p, const JPoint& cell,
								  const JRect& rect) override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual JXInputField*	CreateXInputField(const JPoint& cell,
									const JCoordinate x, const JCoordinate y,
									const JCoordinate w, const JCoordinate h) override;
	virtual bool		ExtractInputData(const JPoint& cell) override;
	virtual void			PrepareDeleteXInputField() override;

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JFSBindingList*	itsBindingList;		// not owned
	JXTextMenu*		itsTypeMenu;
	JXTextButton*	itsAddButton;
	JXTextButton*	itsRemoveButton;
	JXTextButton*	itsDuplicateButton;
	JXInputField*	itsTextInput;
	JRegex*			itsTestRegex;

private:

	void	AddPattern();
	void	RemovePattern();
	void	DuplicatePattern();

	void	UpdateTypeMenu();
	void	HandleTypeMenu(const JIndex index);

	void	UpdateButtons();
	void	UpdateColWidths();

public:

	// JBroadcaster messages

	static const JUtf8Byte* kDataChanged;

	class DataChanged : public JBroadcaster::Message
		{
		public:

			DataChanged()
				:
				JBroadcaster::Message(kDataChanged)
			{ };
		};
};

#endif
