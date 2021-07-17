/******************************************************************************
 CBSymbolDirector.h

	Interface for the CBSymbolDirector class

	Copyright © 1999 by John Lindal.

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
					 CBProjectDocument* supervisor, const bool subProject);

	virtual ~CBSymbolDirector();

	void	EditPrefs();
	void	SetPrefs(const bool raiseTreeOnRightClick, const bool writePrefs);

	CBProjectDocument*	GetProjectDocument() const;
	CBSymbolList*		GetSymbolList() const;
	bool			FindSymbol(const JString& name, const JString& fileName,
								   const JXMouseButton button);

	bool	HasSymbolBrowsers() const;
	void		CloseSymbolBrowsers();

	void	StreamOut(std::ostream& projOutput,
					  std::ostream* setOutput, std::ostream* symOutput) const;

	// for loading updated symbols

	void	ReadSetup(std::istream& symInput, const JFileVersion symVers);

	// called by CBProjectDocument

	void	FileTypesChanged(const CBPrefsManager::FileTypesChanged& info);

	// called by CBFileListTable

	void		PrepareForListUpdate(const bool reparseAll, JProgressDisplay& pg);
	bool	ListUpdateFinished(const JArray<JFAID_t>& deadFileList, JProgressDisplay& pg);

	// called by CBSymbolSRDirector

	void	SRDirectorClosed(CBSymbolSRDirector* dir);

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;

private:

	CBProjectDocument*	itsProjDoc;				// not owned
	CBSymbolList*		itsSymbolList;
	CBSymbolTable*		itsSymbolTable;
	bool			itsRaiseTreeOnRightClickFlag;

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

inline bool
CBSymbolDirector::HasSymbolBrowsers()
	const
{
	return !itsSRList->IsEmpty();
}

#endif
