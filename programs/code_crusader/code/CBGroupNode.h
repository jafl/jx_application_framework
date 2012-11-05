/******************************************************************************
 CBGroupNode.h

	Copyright © 1999 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBGroupNode
#define _H_CBGroupNode

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBProjectNode.h"

class CBGroupNode : public CBProjectNode
{
public:

	CBGroupNode(CBProjectTree* tree, const JBoolean append = kJTrue);
	CBGroupNode(istream& input, const JFileVersion vers,
				CBProjectNode* parent);

	virtual ~CBGroupNode();

	virtual void	Print(JString* text) const;
	virtual void	ShowFileLocation() const;
	virtual void	ViewPlainDiffs(const JBoolean silent) const;
	virtual void	ViewVCSDiffs(const JBoolean silent) const;

private:

	// not allowed

	CBGroupNode(const CBGroupNode& source);
	CBGroupNode& operator=(const CBGroupNode& source);
};

#endif
