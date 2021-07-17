/******************************************************************************
 CBJavaClass.h

	Copyright © 1999 John Lindal.

 ******************************************************************************/

#ifndef _H_CBJavaClass
#define _H_CBJavaClass

#include "CBClass.h"

class CBJavaClass : public CBClass
{
public:

	CBJavaClass(const JString& name, const DeclareType declType,
				const JFAID_t fileID, CBTree* tree,
				const bool isPublic, const bool isFinal);
	CBJavaClass(std::istream& input, const JFileVersion vers, CBTree* tree);

	virtual ~CBJavaClass();

	virtual void	ViewSource() const override;
	virtual void	ViewHeader() const override;

	virtual void	StreamOut(std::ostream& output) const override;

protected:

	virtual CBClass*	NewGhost(const JString& name, CBTree* tree) override;

	virtual void	AdjustNameStyle(JFontStyle* style) const override;

private:

	bool	itsIsPublicFlag;
	bool	itsIsFinalFlag;

private:

	// not allowed

	CBJavaClass(const CBJavaClass& source);
	CBJavaClass& operator=(const CBJavaClass& source);
};

#endif
