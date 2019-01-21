/******************************************************************************
 CBCClass.h

	Copyright (C) 1996-99 John Lindal.

 ******************************************************************************/

#ifndef _H_CBCClass
#define _H_CBCClass

#include "CBClass.h"

class CBCClass : public CBClass
{
public:

	CBCClass(const JString& name, const DeclareType declType,
			 const JFAID_t fileID, CBTree* tree);
	CBCClass(std::istream& input, const JFileVersion vers, CBTree* tree);

	virtual ~CBCClass();

	virtual JBoolean	IsInherited(const JIndex index, const InheritType inherit,
									FnAccessLevel* access) const override;

	virtual void	ViewSource() const override;
	virtual void	ViewHeader() const override;

protected:

	virtual CBClass*	NewGhost(const JString& name, CBTree* tree) override;

	virtual void	AdjustNameStyle(JFontStyle* style) const override;

private:

	// not allowed

	CBCClass(const CBCClass& source);
	CBCClass& operator=(const CBCClass& source);
};

#endif
