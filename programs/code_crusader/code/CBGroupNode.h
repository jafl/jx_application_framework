/******************************************************************************
 CBGroupNode.h

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#ifndef _H_CBGroupNode
#define _H_CBGroupNode

#include "CBProjectNode.h"

class CBGroupNode : public CBProjectNode
{
public:

	CBGroupNode(CBProjectTree* tree, const bool append = true);
	CBGroupNode(std::istream& input, const JFileVersion vers,
				CBProjectNode* parent);

	virtual ~CBGroupNode();

	virtual void	Print(JString* text) const;
	virtual void	ShowFileLocation() const;
	virtual void	ViewPlainDiffs(const bool silent) const;
	virtual void	ViewVCSDiffs(const bool silent) const;

private:

	// not allowed

	CBGroupNode(const CBGroupNode& source);
	CBGroupNode& operator=(const CBGroupNode& source);
};

#endif
