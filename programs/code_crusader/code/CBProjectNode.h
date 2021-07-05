/******************************************************************************
 CBProjectNode.h

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#ifndef _H_CBProjectNode
#define _H_CBProjectNode

#include <JNamedTreeNode.h>
#include "CBProjectNodeType.h"

class JProgressDisplay;
class JXDocument;
class CBProjectTree;
class CBProjectDocument;
class CBFileListTable;
class CBSymbolList;
class CBCTree;
class CBDTree;
class CBGoTree;
class CBJavaTree;
class CBPHPTree;

class CBProjectNode : public JNamedTreeNode
{
public:

	CBProjectNode(CBProjectTree* tree, const CBProjectNodeType type,
				  const JString& name, const JBoolean isOpenable);

	virtual ~CBProjectNode();

	CBProjectNodeType	GetType() const;
	virtual JBoolean	IncludedInMakefile() const;
	virtual JBoolean	IncludedInCMakeData() const;
	virtual JBoolean	IncludedInQMakeData() const;

	CBProjectDocument*	GetProjectDoc() const;

	virtual void		OpenFile() const;
	virtual void		OpenComplementFile() const;
	virtual JBoolean	GetFullName(JString* fullName) const;
	virtual void		ShowFileLocation() const;
	virtual void		ViewPlainDiffs(const JBoolean silent) const;
	virtual void		ViewVCSDiffs(const JBoolean silent) const;

	JBoolean	Includes(const JString& fullName) const;
	JBoolean	FindFile(const JString& fullName,
						 const CBProjectNode** node) const;
	JBoolean	FindFile(const JString& fullName, CBProjectNode** node);

	virtual void		BuildMakeFiles(JString* text,
									   JPtrArray<JTreeNode>* invalidList,
									   JPtrArray<JString>* libFileList,
									   JPtrArray<JString>* libProjPathList) const;
	virtual void		BuildCMakeData(JString* src, JString* hdr,
									   JPtrArray<JTreeNode>* invalidList) const;
	virtual void		BuildQMakeData(JString* src, JString* hdr,
									   JPtrArray<JTreeNode>* invalidList) const;

	virtual JBoolean	ParseFiles(CBFileListTable* parser,
								   const JPtrArray<JString>& allSuffixList,
								   CBSymbolList* symbolList,
								   CBCTree* cTree, CBDTree* dTree, CBGoTree* goTree,
								   CBJavaTree* javaTree, CBPHPTree* phpTree,
								   JProgressDisplay& pg) const;
	virtual void		Print(JString* text) const;

	virtual void	FileRenamed(const JString& origFullName, const JString& newFullName);

	static CBProjectNode*	StreamIn(std::istream& input, const JFileVersion vers,
									 CBProjectNode* parent);
	virtual void			StreamOut(std::ostream& output) const;

	virtual void	CreateFilesForTemplate(std::istream& input,
										   const JFileVersion vers) const;
	virtual void	SaveFilesInTemplate(std::ostream& output) const;

	CBProjectTree*			GetProjectTree();
	const CBProjectTree*	GetProjectTree() const;

	CBProjectNode*			GetProjectParent();
	const CBProjectNode*	GetProjectParent() const;
	JBoolean				GetProjectParent(CBProjectNode** parent);
	JBoolean				GetProjectParent(const CBProjectNode** parent) const;

	CBProjectNode*			GetProjectChild(const JIndex index);
	const CBProjectNode*	GetProjectChild(const JIndex index) const;

protected:

	CBProjectNode(std::istream& input, const JFileVersion vers,
				  CBProjectNode* parent, const CBProjectNodeType type,
				  const JBoolean isOpenable);

	virtual JBoolean	FindFile1(const JString& fullName,
								  CBProjectNode** node);

private:

	const CBProjectNodeType	itsType;

private:

	// not allowed

	CBProjectNode(const CBProjectNode& source);
	CBProjectNode& operator=(const CBProjectNode& source);
};


/******************************************************************************
 GetType

 ******************************************************************************/

inline CBProjectNodeType
CBProjectNode::GetType()
	const
{
	return itsType;
}

/******************************************************************************
 Includes

 ******************************************************************************/

inline JBoolean
CBProjectNode::Includes
	(
	const JString& fullName
	)
	const
{
	const CBProjectNode* node;
	return FindFile(fullName, &node);
}

#endif
