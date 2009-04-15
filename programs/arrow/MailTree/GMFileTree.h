/******************************************************************************
 GMFileTree.h

	Interface for the GMFileTree class

 ******************************************************************************/

#ifndef _H_GMFileTree
#define _H_GMFileTree

#include "JTree.h"
#include "JString.h"

class JTreeList;

class GMFileTree : public JTree
{
public:

	GMFileTree(JTreeNode* root);
	virtual ~GMFileTree();

	void BroadcastNodeHasNewMail(JTreeNode* node, const JBoolean newmail);

private:

private:

	// not allowed

	GMFileTree(const GMFileTree& source);
	const GMFileTree& operator=(const GMFileTree& source);

public:

	static const JCharacter* kNodeHasNewMail;

	class NodeHasNewMail : public JBroadcaster::Message
		{
		public:

			NodeHasNewMail(const JTreeNode* node, const JBoolean newmail)
				:
				JBroadcaster::Message(kNodeHasNewMail),
				itsNode(node),
				itsHasNewMail(newmail)
				{ };

			const JTreeNode*
			GetNode()
				const
				{
				return itsNode;
				}

			JBoolean
			HasNewMail()
				const
				{
				return itsHasNewMail;
				}

		private:

			const JTreeNode*	itsNode;
			JBoolean			itsHasNewMail;
		};
};

#endif
