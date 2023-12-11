/******************************************************************************
 MenuDocument.h

	Copyright (C) 2023 by John Lindal.

 *****************************************************************************/

#ifndef _H_MenuDocument
#define _H_MenuDocument

#include <jx-af/jx/JXFileDocument.h>
#include <jx-af/jx/JXWidget.h>	// need sizing

class JXTextMenu;
class JXToolBar;
class JXInputField;
class JXCharInput;
class MenuTable;

class MenuDocument : public JXFileDocument
{
public:

	static bool	Create(MenuDocument** doc);
	static bool	Create(const JString& fullName, MenuDocument** doc);

	~MenuDocument() override;

	const JString&	GetName() const override;
	const JString&	GetClassName() const;

	static bool	FindProjectRoot(const JString& path, JString* root);

protected:

	MenuDocument(const JString& fullName, const bool onDisk);

	JError	WriteFile(const JString& fullName, const bool safetySave) const override;
	void	WriteTextFile(std::ostream& output, const bool safetySave) const override;
	void	DiscardChanges() override;

private:

	MenuTable*			itsTable;
	mutable JString		itsDocName;		// so GetName() can return JString&

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsPrefsMenu;

// begin JXLayout

	JXToolBar* itsToolBar;

// end JXLayout

// begin EditorLayout

	JXInputField* itsClassNameInput;
	JXInputField* itsMenuTitleInput;
	JXCharInput*  itsWindowsKeyInput;

// end EditorLayout

private:

	void	BuildWindow();
	void	UpdateUnsavedFileName();
	void	ReadFile(std::istream& input);

	void	GenerateCode() const;

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	HandlePrefsMenu(const JIndex index);
};

#endif
