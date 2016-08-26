/******************************************************************************
 SVNRepoTree.h

	Copyright © 2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SVNRepoTree
#define _H_SVNRepoTree

#include <JTree.h>
#include <JPtrArray-JString.h>

class SVNRepoTreeList;
class SVNRepoTreeNode;

class SVNRepoTree : public JTree
{
public:

	SVNRepoTree(SVNRepoTreeNode* root);

	virtual ~SVNRepoTree();

	const JString&	GetRepoPath() const;
	void			Update(SVNRepoTreeList* view, const JBoolean fresh = kJTrue);
	void			ReopenIfNeeded(SVNRepoTreeNode* node);
	void			SavePathToOpen(const JCharacter* url);

	static void	SkipSetup(istream& input, JFileVersion vers);
	void		ReadSetup(istream& input, JFileVersion vers);
	void		WriteSetup(ostream& output, SVNRepoTreeList* view) const;

	SVNRepoTreeNode*		GetRepoRoot();
	const SVNRepoTreeNode*	GetRepoRoot() const;

private:

	SVNRepoTreeList*	itsView;			// latest caller to Update()
	JPtrArray<JString>*	itsSavedOpenNodes;	// repo paths

private:

	void	SaveOpenNodes() const;

	// not allowed

	SVNRepoTree(const SVNRepoTree& source);
	const SVNRepoTree& operator=(const SVNRepoTree& source);
};

#endif
