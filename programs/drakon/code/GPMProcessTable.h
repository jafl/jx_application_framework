/******************************************************************************
 GPMProcessTable.h

	Copyright (C) 2001 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GPMProcessTable
#define _H_GPMProcessTable

#include <JXTable.h>

class GPMProcessList;
class GPMProcessEntry;

class JXTextMenu;
class JXToolBar;
class JXTEBase;
class JXImage;

class GPMProcessTable : public JXTable
{
public:

	GPMProcessTable(GPMProcessList* list, JXTEBase* fullCmdDisplay,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~GPMProcessTable();

	JBoolean	GetSelectedProcess(const GPMProcessEntry** entry) const;
	void		SelectProcess(const GPMProcessEntry& entry);

	virtual void	HandleKeyPress(const JUtf8Character& c,
								   const int keySym, const JXKeyModifiers& modifiers) override;

	static void	DrawRowBackground(JPainter& p, const JPoint& cell, const JRect& rect,
								  const JColorID color);
	static void	DrawProcessState(const GPMProcessEntry& entry,
								 JPainter& p, const JRect& rect,
								 const JXImage& zombieImage);

	static JXTextMenu*	CreateContextMenu(JXContainer* enclosure);
	static void			UpdateContextMenu(JXTextMenu* menu, const GPMProcessEntry& entry);
	static void			HandleContextMenu(const JIndex menuIndex, const GPMProcessEntry& entry,
										  GPMProcessList* list);
	static void			ToggleProcessState(const GPMProcessEntry& entry);

protected:

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void	TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;

	virtual void	HandleFocusEvent() override;
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	GPMProcessList*			itsList;			// not owned
	JString					itsKeyBuffer;
	JXTextMenu*				itsContextMenu;
	const GPMProcessEntry* 	itsSelectedEntry;	// nullptr unless updating
	JXTEBase*				itsFullCmdDisplay;
	JXImage*				itsZombieImage;

private:

	void	UpdateContextMenu();
	void	HandleContextMenu(const JIndex index);

	void	AdjustColWidths();

	// not allowed

	GPMProcessTable(const GPMProcessTable& source);
	const GPMProcessTable& operator=(const GPMProcessTable& source);

};

#endif
