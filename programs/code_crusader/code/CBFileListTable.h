/******************************************************************************
 CBFileListTable.h

	Copyright © 1999 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_CBFileListTable
#define _H_CBFileListTable

#include <JXFileListTable.h>
#include "CBTextFileType.h"
#include <JFAIndex.h>
#include <JFAID.h>
#include <jTime.h>

class JProgressDisplay;
class CBProjectTree;
class CBDirList;
class CBSymbolDirector;
class CBSymbolList;
class CBCTreeDirector;
class CBCTree;
class CBJavaTreeDirector;
class CBJavaTree;
class CBPHPTreeDirector;
class CBPHPTree;

class CBFileListTable : public JXFileListTable
{
public:

	CBFileListTable(JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~CBFileListTable();

	JBoolean	Update(ostream& link,
					   CBProjectTree* fileTree, const CBDirList& dirList,
					   CBSymbolDirector* symbolDir,
					   CBCTreeDirector* cTreeDir, CBJavaTreeDirector* javaTreeDir,
					   CBPHPTreeDirector* phpTreeDir);
	void		UpdateFinished();

	const JString&	GetFileName(const JFAID_t id) const;
	JBoolean		GetFileID(const JCharacter* trueName, JFAID_t* id) const;

	void	ReadSetup(istream& projInput, const JFileVersion projVers,
					  istream* symInput, const JFileVersion symVers);
	void	WriteSetup(ostream& projOutput, ostream* symOutput) const;

	// for loading updated symbols

	void	ReadSetup(istream& input, const JFileVersion vers);

	// called by CBFileNode

	void	ParseFile(const JString& fullName, const JPtrArray<JString>& allSuffixList,
					  const time_t modTime,
					  CBSymbolList* symbolList, CBCTree* cTree, CBJavaTree* javaTree,
					  CBPHPTree* phpTree);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	struct FileInfo
	{
		JFAID_t	id;
		time_t	modTime;

		FileInfo()
			:
			id(JFAID::kInvalidID), modTime(0)
		{ };

		FileInfo(const JFAID_t id_, const time_t t)
			:
			id(id_), modTime(t)
		{ };
	};

private:

	JArray<FileInfo>*	itsFileInfo;
	JArray<JBoolean>*	itsFileUsage;		// NULL unless updating files; on stack
	JBoolean			itsReparseAllFlag;	// kJTrue => flush all on next update
	JBoolean			itsChangedDuringParseFlag;
	mutable JFAID_t		itsLastUniqueID;

private:

	void		ScanAll(CBProjectTree* fileTree, const CBDirList& dirList,
						CBSymbolList* symbolList,
						CBCTree* cTree, CBJavaTree* javaTree, CBPHPTree* phpTree,
						JProgressDisplay& pg);
	void		ScanDirectory(const JString& path, const JBoolean recurse,
							  const JPtrArray<JString>& allSuffixList,
							  CBSymbolList* symbolList,
							  CBCTree* cTree, CBJavaTree* javaTree,
							  CBPHPTree* phpTree,
							  JProgressDisplay& pg);
	JBoolean	AddFile(const JCharacter* fullName, const CBTextFileType fileType,
						const time_t modTime, JFAID_t* id);

	JBoolean	IDToIndex(const JFAID_t id, JIndex* index) const;
	JFAID_t		GetUniqueID() const;

	void	FilesAdded(const JOrderedSetT::ElementsInserted& info);
	void	UpdateFileInfo(const JIndex index);

	// not allowed

	CBFileListTable(const CBFileListTable& source);
	const CBFileListTable& operator=(const CBFileListTable& source);
};

#endif
