/******************************************************************************
 CBPHPClass.h

	Copyright © 2014 John Lindal.

 ******************************************************************************/

#ifndef _H_CBPHPClass
#define _H_CBPHPClass

#include "CBClass.h"

class CBPHPClass : public CBClass
{
public:

	CBPHPClass(const JString& name, const DeclareType declType,
			   const JFAID_t fileID, CBTree* tree,
			   const bool isFinal);
	CBPHPClass(std::istream& input, const JFileVersion vers, CBTree* tree);

	virtual ~CBPHPClass();

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

	CBPHPClass(const CBPHPClass& source);
	CBPHPClass& operator=(const CBPHPClass& source);
};

#endif
