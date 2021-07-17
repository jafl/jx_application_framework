/******************************************************************************
 CBGoClass.h

	Copyright © 2021 John Lindal.

 ******************************************************************************/

#ifndef _H_CBGoClass
#define _H_CBGoClass

#include "CBClass.h"

class CBGoClass : public CBClass
{
public:

	CBGoClass(const JString& name, const DeclareType declType,
			  const JFAID_t fileID, CBTree* tree,
			  const bool isPublic);
	CBGoClass(std::istream& input, const JFileVersion vers, CBTree* tree);

	virtual ~CBGoClass();

	virtual void	ViewSource() const override;
	virtual void	ViewHeader() const override;

	virtual void	StreamOut(std::ostream& output) const override;

protected:

	virtual CBClass*	NewGhost(const JString& name, CBTree* tree) override;

	virtual void	AdjustNameStyle(JFontStyle* style) const override;

private:

	bool	itsIsPublicFlag;

private:

	// not allowed

	CBGoClass(const CBGoClass& source);
	CBGoClass& operator=(const CBGoClass& source);
};

#endif
