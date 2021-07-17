/******************************************************************************
 CBDClass.h

	Copyright © 2021 John Lindal.

 ******************************************************************************/

#ifndef _H_CBDClass
#define _H_CBDClass

#include "CBClass.h"

class CBDClass : public CBClass
{
public:

	CBDClass(const JString& name, const DeclareType declType,
			 const JFAID_t fileID, CBTree* tree, const bool isFinal);
	CBDClass(std::istream& input, const JFileVersion vers, CBTree* tree);

	virtual ~CBDClass();

	virtual void	ViewSource() const override;
	virtual void	ViewHeader() const override;

	virtual void	StreamOut(std::ostream& output) const override;

protected:

	virtual CBClass*	NewGhost(const JString& name, CBTree* tree) override;

	virtual void	AdjustNameStyle(JFontStyle* style) const override;

private:

	bool	itsIsFinalFlag;

private:

	// not allowed

	CBDClass(const CBDClass& source);
	CBDClass& operator=(const CBDClass& source);
};

#endif
