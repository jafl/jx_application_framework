/******************************************************************************
 CBSearchTextDialog.h

	Copyright (C) 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBSearchTextDialog
#define _H_CBSearchTextDialog

#include <JXSearchTextDialog.h>
#include <JPrefObject.h>
#include <JString.h>

class JProgressDisplay;
class JXFileListTable;
class JXPathInput;
class CBSearchPathHistoryMenu;
class CBSearchFilterHistoryMenu;
class CBListCSF;

class CBSearchTextDialog : public JXSearchTextDialog, public JPrefObject
{
public:

	static CBSearchTextDialog*	Create();

	virtual ~CBSearchTextDialog();

	virtual void	Activate();

	void	ShouldSearchFiles(const JBoolean search);
	void	AddFileToSearch(const JCharacter* fileName) const;
	void	ClearFileList();

protected:

	CBSearchTextDialog();

	virtual void	UpdateDisplay();

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXFileListTable*	itsFileList;
	CBListCSF*			itsCSF;

	mutable JString		itsFileSetName;
	JBoolean			itsOnlyListFilesFlag;
	JBoolean			itsListFilesWithoutMatchFlag;

// begin JXLayout

	JXTextCheckbox*            itsMultifileCB;
	JXTextMenu*                itsFileListMenu;
	JXTextCheckbox*            itsSearchDirCB;
	JXPathInput*               itsDirInput;
	CBSearchPathHistoryMenu*   itsDirHistory;
	JXInputField*              itsFileFilterInput;
	CBSearchFilterHistoryMenu* itsFileFilterHistory;
	JXTextCheckbox*            itsRecurseDirCB;
	JXTextButton*              itsChooseDirButton;
	JXTextCheckbox*            itsInvertFileFilterCB;
	JXInputField*              itsPathFilterInput;
	CBSearchFilterHistoryMenu* itsPathFilterHistory;

// end JXLayout

private:

	void	BuildWindow();

	void	UpdateFileListMenu();
	void	HandleFileListMenu(const JIndex item);

	void		SearchFiles() const;
	void		SearchFilesAndReplace();
	JBoolean	BuildSearchFileList(JPtrArray<JString>* fileList,
									JPtrArray<JString>* nameList) const;
	JBoolean	SearchDirectory(const JString& path, const JRegex* fileRegex,
								const JRegex* pathRegex,
								JPtrArray<JString>* fileList,
								JPtrArray<JString>* nameList,
								JProgressDisplay& pg) const;
	void		SaveFileForSearch(const JString& fullName,
								  JPtrArray<JString>* fileList,
								  JPtrArray<JString>* nameList) const;
	void		UpdateBasePath();
	JBoolean	GetSearchDirectory(JString* path, JString* fileFilter,
								   JString *pathFilter) const;

	void	LoadFileSet();
	void	SaveFileSet() const;
	void	OpenSelectedFiles() const;
	void	AddSearchFiles();

	// not allowed

	CBSearchTextDialog(const CBSearchTextDialog& source);
	const CBSearchTextDialog& operator=(const CBSearchTextDialog& source);
};

#endif
