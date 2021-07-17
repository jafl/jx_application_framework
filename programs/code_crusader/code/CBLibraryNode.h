/******************************************************************************
 CBLibraryNode.h

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#ifndef _H_CBLibraryNode
#define _H_CBLibraryNode

#include "CBFileNodeBase.h"

class CBSubprojectConfigDialog;

class CBLibraryNode : public CBFileNodeBase
{
public:

	CBLibraryNode(CBProjectTree* tree, const JString& fileName);
	CBLibraryNode(std::istream& input, const JFileVersion vers,
				  CBProjectNode* parent);

	virtual ~CBLibraryNode();

	virtual void	OpenFile() const override;
	virtual void	OpenComplementFile() const override;
	void			EditSubprojectConfig();

	bool	OpenProject(const bool silent = false);

	virtual void		StreamOut(std::ostream& output) const override;
	virtual bool	IncludedInMakefile() const override;
	virtual void		BuildMakeFiles(JString* text,
									   JPtrArray<JTreeNode>* invalidList,
									   JPtrArray<JString>* libFileList,
									   JPtrArray<JString>* libProjPathList) const override;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	bool	itsIncludeInDepListFlag;	// true => include in Make.files
	JString		itsProjFileName;			// can be relative
	bool	itsShouldBuildFlag;			// true => open and build sub-project

	CBSubprojectConfigDialog*	itsSubprojDialog;

private:

	void	CBLibraryNodeX();
	void	UpdateSubprojectConfig();

	// not allowed

	CBLibraryNode(const CBLibraryNode& source);
	CBLibraryNode& operator=(const CBLibraryNode& source);
};

#endif
