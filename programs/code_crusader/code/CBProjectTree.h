/******************************************************************************
 CBProjectTree.h

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#ifndef _H_CBProjectTree
#define _H_CBProjectTree

#include <JTree.h>
#include <JPtrArray.h>

class JString;
class JProgressDisplay;
class CBProjectDocument;
class CBProjectNode;
class CBFileListTable;
class CBSymbolList;
class CBCTree;
class CBDTree;
class CBGoTree;
class CBJavaTree;
class CBPHPTree;

class CBProjectTree : public JTree
{
public:

	CBProjectTree(CBProjectDocument* doc);
	CBProjectTree(std::istream& input, const JFileVersion vers,
				  CBProjectDocument* doc);

	virtual ~CBProjectTree();

	CBProjectDocument*	GetProjectDoc() const;
	CBProjectNode*		GetProjectRoot() const;

	bool	BuildMakeFiles(JString* text,
							   JPtrArray<JTreeNode>* invalidList,
							   JPtrArray<JString>* libFileList,
							   JPtrArray<JString>* libProjPathList) const;
	bool	BuildCMakeData(JString* src, JString* hdr,
							   JPtrArray<JTreeNode>* invalidList) const;
	bool	BuildQMakeData(JString* src, JString* hdr,
							   JPtrArray<JTreeNode>* invalidList) const;
	void		ParseFiles(CBFileListTable* parser,
						   const JPtrArray<JString>& allSuffixList,
						   CBSymbolList* symbolList,
						   CBCTree* cTree, CBDTree* dTree, CBGoTree* goTree,
						   CBJavaTree* javaTree, CBPHPTree* phpTree,
						   JProgressDisplay& pg) const;
	void		Print(JString* text) const;

	void	StreamOut(std::ostream& output) const;

	void	CreateFilesForTemplate(std::istream& input, const JFileVersion vers) const;
	void	SaveFilesInTemplate(std::ostream& output) const;

private:

	CBProjectDocument*	itsDoc;

private:

	// not allowed

	CBProjectTree(const CBProjectTree& source);
	CBProjectTree& operator=(const CBProjectTree& source);
};


/******************************************************************************
 GetProjectDoc

 ******************************************************************************/

inline CBProjectDocument*
CBProjectTree::GetProjectDoc()
	const
{
	return itsDoc;
}

#endif
