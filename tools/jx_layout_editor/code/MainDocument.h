/******************************************************************************
 MainDocument.h

	Copyright (C) 2023 by John Lindal.

 *****************************************************************************/

#ifndef _H_MainDocument
#define _H_MainDocument

#include <jx-af/jx/JXFileDocument.h>
#include <jx-af/jcore/JStringPtrMap.h>

class JXTextMenu;
class JXToolBar;
class JXStringList;
class LayoutDirector;

class MainDocument : public JXFileDocument
{
public:

	static bool	Create(const JString& fullName, MainDocument** doc);

	~MainDocument() override;

	const JString&	GetName() const override;

	void	OpenLayout(const JString& name);
	void	SaveLayout(LayoutDirector* dir);
	void	EditLayoutName(const JString& name);

protected:

	MainDocument(const JString& fullName, const bool onDisk);

	void	WriteTextFile(std::ostream& output, const bool safetySave) const override;
	void	DiscardChanges() override;

	void	DirectorClosed(JXDirector* dir) override;

private:

	JStringPtrMap<JString>*			itsLayouts;
	JStringPtrMap<LayoutDirector>*	itsLayoutDirs;
	JPtrArray<JString>*				itsLayoutNames;
	JXStringList*					itsLayoutNameTable;
	mutable JString					itsDocName;			// so GetName() can return JString&

	JXTextMenu*	itsFileMenu;
	JXTextMenu*	itsLayoutMenu;
	JXTextMenu*	itsPrefsMenu;

// begin JXLayout

	JXToolBar* itsToolBar;

// end JXLayout

private:

	void	BuildWindow();

	void	ReadFile(std::istream& input);
	void	ImportFDesignFile(std::istream& input);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateLayoutMenu();
	void	HandleLayoutMenu(const JIndex index);

	void	HandlePrefsMenu(const JIndex index);
};

#endif
