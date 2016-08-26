/******************************************************************************
 CBLibraryNode.h

	Copyright © 1999 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBLibraryNode
#define _H_CBLibraryNode

#include "CBFileNodeBase.h"

class CBSubprojectConfigDialog;

class CBLibraryNode : public CBFileNodeBase
{
public:

	CBLibraryNode(CBProjectTree* tree, const JCharacter* fileName);
	CBLibraryNode(istream& input, const JFileVersion vers,
				  CBProjectNode* parent);

	virtual ~CBLibraryNode();

	virtual void	OpenFile() const;
	virtual void	OpenComplementFile() const;
	void			EditSubprojectConfig();

	JBoolean	OpenProject(const JBoolean silent = kJFalse);

	virtual void		StreamOut(ostream& output) const;
	virtual JBoolean	IncludedInMakefile() const;
	virtual void		BuildMakeFiles(JString* text,
									   JPtrArray<JTreeNode>* invalidList,
									   JPtrArray<JString>* libFileList,
									   JPtrArray<JString>* libProjPathList) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JBoolean	itsIncludeInDepListFlag;	// kJTrue => include in Make.files
	JString		itsProjFileName;			// can be relative
	JBoolean	itsShouldBuildFlag;			// kJTrue => open and build sub-project

	CBSubprojectConfigDialog*	itsSubprojDialog;

private:

	void	CBLibraryNodeX();
	void	UpdateSubprojectConfig();

	// not allowed

	CBLibraryNode(const CBLibraryNode& source);
	CBLibraryNode& operator=(const CBLibraryNode& source);
};

#endif
