/******************************************************************************
 CBProjectTree.h

	Copyright © 1999 John Lindal. All rights reserved.

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
class CBJavaTree;
class CBPHPTree;

class CBProjectTree : public JTree
{
public:

	CBProjectTree(CBProjectDocument* doc);
	CBProjectTree(istream& input, const JFileVersion vers,
				  CBProjectDocument* doc);

	virtual ~CBProjectTree();

	CBProjectDocument*	GetProjectDoc() const;
	CBProjectNode*		GetProjectRoot() const;

	JBoolean	BuildMakeFiles(JString* text,
							   JPtrArray<JTreeNode>* invalidList,
							   JPtrArray<JString>* libFileList,
							   JPtrArray<JString>* libProjPathList) const;
	JBoolean	BuildCMakeData(JString* src, JString* hdr,
							   JPtrArray<JTreeNode>* invalidList) const;
	JBoolean	BuildQMakeData(JString* src, JString* hdr,
							   JPtrArray<JTreeNode>* invalidList) const;
	void		ParseFiles(CBFileListTable* parser,
						   const JPtrArray<JString>& allSuffixList,
						   CBSymbolList* symbolList,
						   CBCTree* cTree, CBJavaTree* javaTree,
						   CBPHPTree* phpTree,
						   JProgressDisplay& pg) const;
	void		Print(JString* text) const;

	void	StreamOut(ostream& output) const;

	void	CreateFilesForTemplate(istream& input, const JFileVersion vers) const;
	void	SaveFilesInTemplate(ostream& output) const;

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
