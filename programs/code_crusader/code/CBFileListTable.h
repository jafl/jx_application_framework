/******************************************************************************
 CBFileListTable.h

	Copyright © 1999 by John Lindal.

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
class CBDTreeDirector;
class CBDTree;
class CBGoTreeDirector;
class CBGoTree;
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

	bool	Update(std::ostream& link,
					   CBProjectTree* fileTree, const CBDirList& dirList,
					   CBSymbolDirector* symbolDir,
					   CBCTreeDirector* cTreeDir, CBDTreeDirector* dTreeDir,
					   CBGoTreeDirector* goTreeDir, CBJavaTreeDirector* javaTreeDir,
					   CBPHPTreeDirector* phpTreeDir);
	void		UpdateFinished();

	const JString&	GetFileName(const JFAID_t id) const;
	bool		GetFileID(const JString& trueName, JFAID_t* id) const;

	void	ReadSetup(std::istream& projInput, const JFileVersion projVers,
					  std::istream* symInput, const JFileVersion symVers);
	void	WriteSetup(std::ostream& projOutput, std::ostream* symOutput) const;

	// for loading updated symbols

	void	ReadSetup(std::istream& input, const JFileVersion vers);

	// called by CBFileNode

	void	ParseFile(const JString& fullName, const JPtrArray<JString>& allSuffixList,
					  const time_t modTime,
					  CBSymbolList* symbolList, CBCTree* cTree, CBDTree* dTree,
					  CBGoTree* goTree, CBJavaTree* javaTree, CBPHPTree* phpTree);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

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
	JArray<bool>*	itsFileUsage;		// nullptr unless updating files; on stack
	bool			itsReparseAllFlag;	// true => flush all on next update
	bool			itsChangedDuringParseFlag;
	mutable JFAID_t		itsLastUniqueID;

private:

	void		ScanAll(CBProjectTree* fileTree, const CBDirList& dirList,
						CBSymbolList* symbolList,
						CBCTree* cTree, CBDTree* dTree, CBGoTree* goTree,
						CBJavaTree* javaTree, CBPHPTree* phpTree,
						JProgressDisplay& pg);
	void		ScanDirectory(const JString& path, const bool recurse,
							  const JPtrArray<JString>& allSuffixList,
							  CBSymbolList* symbolList,
							  CBCTree* cTree, CBDTree* dTree, CBGoTree* goTree,
							  CBJavaTree* javaTree, CBPHPTree* phpTree,
							  JProgressDisplay& pg);
	bool	AddFile(const JString& fullName, const CBTextFileType fileType,
						const time_t modTime, JFAID_t* id);

	bool	IDToIndex(const JFAID_t id, JIndex* index) const;
	JFAID_t		GetUniqueID() const;

	void	FilesAdded(const JListT::ElementsInserted& info);
	void	UpdateFileInfo(const JIndex index);

	// not allowed

	CBFileListTable(const CBFileListTable& source);
	const CBFileListTable& operator=(const CBFileListTable& source);
};

#endif
