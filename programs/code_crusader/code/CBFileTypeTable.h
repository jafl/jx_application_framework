/******************************************************************************
 CBFileTypeTable.h

	Copyright © 1998 by John Lindal.

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

	virtual bool	IsEditable(const JPoint& cell) const override;

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect) override;
	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h) override;
	virtual bool		ExtractInputData(const JPoint& cell) override;
	virtual void			PrepareDeleteXInputField() override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JArray<CBPrefsManager::FileTypeInfo>*			itsFileTypeList;	// info.*Regex always nullptr
	const JArray<CBPrefsManager::MacroSetInfo>&		itsMacroList;
	const JArray<CBPrefsManager::CRMRuleListInfo>&	itsCRMList;

	JXInputField*	itsTextInput;			// nullptr unless editing
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
	void	BuildScriptMenuItems(const JString& path, const bool isUserPath,
								 JPtrArray<JString>* menuText) const;

	void		GetNewScriptDirectory();
	void		CreateNewScriptDirectory();
	bool	CreateDirectory(const JString& path) const;

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
