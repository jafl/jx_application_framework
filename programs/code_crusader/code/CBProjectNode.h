/******************************************************************************
 CBProjectNode.h

	Copyright © 1999 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBProjectNode
#define _H_CBProjectNode

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JNamedTreeNode.h>
#include "CBProjectNodeType.h"

class JProgressDisplay;
class JXDocument;
class CBProjectTree;
class CBProjectDocument;
class CBFileListTable;
class CBSymbolList;
class CBCTree;
class CBJavaTree;
class CBPHPTree;

class CBProjectNode : public JNamedTreeNode
{
public:

	CBProjectNode(CBProjectTree* tree, const CBProjectNodeType type,
				  const JCharacter* name, const JBoolean isOpenable);

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

	JBoolean	Includes(const JCharacter* fullName) const;
	JBoolean	FindFile(const JCharacter* fullName,
						 const CBProjectNode** node) const;
	JBoolean	FindFile(const JCharacter* fullName, CBProjectNode** node);

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
								   CBCTree* cTree, CBJavaTree* javaTree,
								   CBPHPTree* phpTree,
								   JProgressDisplay& pg) const;
	virtual void		Print(JString* text) const;

	virtual void	FileRenamed(const JCharacter* origFullName, const JCharacter* newFullName);

	static CBProjectNode*	StreamIn(istream& input, const JFileVersion vers,
									 CBProjectNode* parent);
	virtual void			StreamOut(ostream& output) const;

	virtual void	CreateFilesForTemplate(istream& input,
										   const JFileVersion vers) const;
	virtual void	SaveFilesInTemplate(ostream& output) const;

	CBProjectTree*			GetProjectTree();
	const CBProjectTree*	GetProjectTree() const;

	CBProjectNode*			GetProjectParent();
	const CBProjectNode*	GetProjectParent() const;
	JBoolean				GetProjectParent(CBProjectNode** parent);
	JBoolean				GetProjectParent(const CBProjectNode** parent) const;

	CBProjectNode*			GetProjectChild(const JIndex index);
	const CBProjectNode*	GetProjectChild(const JIndex index) const;

protected:

	CBProjectNode(istream& input, const JFileVersion vers,
				  CBProjectNode* parent, const CBProjectNodeType type,
				  const JBoolean isOpenable);

	virtual JBoolean	FindFile1(const JCharacter* fullName,
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
	const JCharacter* fullName
	)
	const
{
	const CBProjectNode* node;
	return FindFile(fullName, &node);
}

#endif
