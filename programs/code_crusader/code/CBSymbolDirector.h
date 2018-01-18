/******************************************************************************
 CBSymbolDirector.h

	Interface for the CBSymbolDirector class

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBSymbolDirector
#define _H_CBSymbolDirector

#include <JXWindowDirector.h>
#include <JPrefObject.h>
#include "CBPrefsManager.h"		// need definition of FileTypesChanged
#include <JXButtonStates.h>

class JProgressDisplay;
class JXTextMenu;
class JXToolBar;
class CBSymbolList;
class CBSymbolTable;
class CBProjectDocument;
class CBSymbolSRDirector;
class CBCommandMenu;
class CBClass;

class CBSymbolDirector : public JXWindowDirector, public JPrefObject
{
public:

	CBSymbolDirector(CBProjectDocument* supervisor);
	CBSymbolDirector(std::istream& projInput, const JFileVersion projVers,
					 std::istream* setInput, const JFileVersion setVers,
					 std::istream* symInput, const JFileVersion symVers,
					 CBProjectDocument* supervisor, const JBoolean subProject);

	virtual ~CBSymbolDirector();

	void	EditPrefs();
	void	SetPrefs(const JBoolean raiseTreeOnRightClick, const JBoolean writePrefs);

	CBProjectDocument*	GetProjectDocument() const;
	CBSymbolList*		GetSymbolList() const;
	JBoolean			FindSymbol(const JCharacter* name, const JCharacter* fileName,
								   const JXMouseButton button);

	JBoolean	HasSymbolBrowsers() const;
	void		CloseSymbolBrowsers();

	void	StreamOut(std::ostream& projOutput,
					  std::ostream* setOutput, std::ostream* symOutput) const;

	// for loading updated symbols

	void	ReadSetup(std::istream& symInput, const JFileVersion symVers);

	// called by CBProjectDocument

	void	FileTypesChanged(const CBPrefsManager::FileTypesChanged& info);

	// called by CBFileListTable

	void		PrepareForListUpdate(const JBoolean reparseAll, JProgressDisplay& pg);
	JBoolean	ListUpdateFinished(const JArray<JFAID_t>& deadFileList, JProgressDisplay& pg);

	// called by CBSymbolSRDirector

	void	SRDirectorClosed(CBSymbolSRDirector* dir);

protected:

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;

private:

	CBProjectDocument*	itsProjDoc;				// not owned
	CBSymbolList*		itsSymbolList;
	CBSymbolTable*		itsSymbolTable;
	JBoolean			itsRaiseTreeOnRightClickFlag;

	JPtrArray<CBSymbolSRDirector>*	itsSRList;	// contents not owned

	JXTextMenu*		itsFileMenu;
	JXTextMenu*		itsSymbolMenu;
	JXTextMenu*		itsProjectMenu;
	CBCommandMenu*	itsCmdMenu;
	JXTextMenu*		itsPrefsMenu;
	JXTextMenu*		itsHelpMenu;

// begin JXLayout

	JXToolBar* itsToolBar;

// end JXLayout

private:

	void	CBSymbolDirectorX(CBProjectDocument* projDoc);
	void	BuildWindow(CBSymbolList* symbolList);
	void	AdjustWindowTitle();

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex index);

	void	UpdateSymbolMenu();
	void	HandleSymbolMenu(const JIndex index);

	void	UpdateProjectMenu();
	void	HandleProjectMenu(const JIndex index);

	void	UpdatePrefsMenu();
	void	HandlePrefsMenu(const JIndex index);

	void	BuildAncestorList(const CBClass& theClass,
							  JPtrArray<JString>* list) const;

	// not allowed

	CBSymbolDirector(const CBSymbolDirector& source);
	const CBSymbolDirector& operator=(const CBSymbolDirector& source);
};


/******************************************************************************
 GetProjectDocument

 ******************************************************************************/

inline CBProjectDocument*
CBSymbolDirector::GetProjectDocument()
	const
{
	return itsProjDoc;
}

/******************************************************************************
 GetSymbolList

 ******************************************************************************/

inline CBSymbolList*
CBSymbolDirector::GetSymbolList()
	const
{
	return itsSymbolList;
}

/******************************************************************************
 HasSymbolBrowsers

 ******************************************************************************/

inline JBoolean
CBSymbolDirector::HasSymbolBrowsers()
	const
{
	return !itsSRList->IsEmpty();
}

#endif
