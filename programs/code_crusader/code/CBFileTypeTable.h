/******************************************************************************
 CBFileTypeTable.h

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBFileTypeTable
#define _H_CBFileTypeTable

#include <JXEditTable.h>
#include "CBPrefsManager.h"		// need definition of FileTypeInfo

class JXTextButton;
class JXTextMenu;
class JXInputField;
class JXColHeaderWidget;
class JXGetNewDirDialog;

class CBFileTypeTable : public JXEditTable
{
public:

	CBFileTypeTable(const JArray<CBPrefsManager::FileTypeInfo>& fileTypeList,
					const JArray<CBPrefsManager::MacroSetInfo>& macroList,
					const JArray<CBPrefsManager::CRMRuleListInfo>& crmList,
					JXTextButton* addTypeButton, JXTextButton* removeTypeButton,
					JXTextButton* duplicateTypeButton,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~CBFileTypeTable();

	void	GetFileTypeList(JArray<CBPrefsManager::FileTypeInfo>* fileTypeList) const;

	void	ReadGeometry(std::istream& input);
	void	WriteGeometry(std::ostream& output) const;
	void	SetColTitles(JXColHeaderWidget* widget) const;

	virtual JBoolean	IsEditable(const JPoint& cell) const;

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h);
	virtual JBoolean		ExtractInputData(const JPoint& cell);
	virtual void			PrepareDeleteXInputField();

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JArray<CBPrefsManager::FileTypeInfo>*			itsFileTypeList;	// info.*Regex always NULL
	const JArray<CBPrefsManager::MacroSetInfo>&		itsMacroList;
	const JArray<CBPrefsManager::CRMRuleListInfo>&	itsCRMList;

	JXInputField*	itsTextInput;			// NULL unless editing
	JFont			itsFont;
	JRegex*			itsTestRegex;

	JXTextMenu*		itsTypeMenu;
	JXTextMenu*		itsMacroMenu;
	JXTextMenu*		itsScriptMenu;
	JXTextMenu*		itsCRMMenu;

	JXTextButton*	itsAddTypeButton;
	JXTextButton*	itsRemoveTypeButton;
	JXTextButton*	itsDuplicateTypeButton;

	JXGetNewDirDialog*	itsNewDirDialog;

private:

	void	CleanOutFileTypeList(JArray<CBPrefsManager::FileTypeInfo>* fileTypeList) const;
	void	FinishFileTypeListCopy(JArray<CBPrefsManager::FileTypeInfo>* fileTypeList) const;

	void	AddType();
	void	RemoveType();
	void	DuplicateType();

	void	UpdateTypeMenu();
	void	HandleTypeMenu(const JIndex index);

	JXTextMenu*	CreateMacroMenu();
	void		UpdateMacroMenu();
	void		HandleMacroMenu(const JIndex index);
	JIndex		MacroIDToMenuIndex(const JIndex id) const;

	void	UpdateScriptMenu();
	void	HandleScriptMenu(const JIndex index);
	void	BuildScriptMenuItems(const JCharacter* path, const JBoolean isUserPath,
								 JPtrArray<JString>* menuText) const;

	void		GetNewScriptDirectory();
	void		CreateNewScriptDirectory();
	JBoolean	CreateDirectory(const JCharacter* path) const;

	JXTextMenu*	CreateCRMMenu();
	void		UpdateCRMMenu();
	void		HandleCRMMenu(const JIndex index);
	JIndex		CRMIDToMenuIndex(const JIndex id) const;

	void	UpdateButtons();

	// not allowed

	CBFileTypeTable(const CBFileTypeTable& source);
	const CBFileTypeTable& operator=(const CBFileTypeTable& source);
};

#endif
