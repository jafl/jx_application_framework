/******************************************************************************
 CBJavaClass.h

	Copyright (C) 1999 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBJavaClass
#define _H_CBJavaClass

#include "CBClass.h"

class CBJavaClass : public CBClass
{
public:

	CBJavaClass(const JCharacter* name, const DeclareType declType,
				const JFAID_t fileID, CBTree* tree,
				const JBoolean isPublic, const JBoolean isFinal);
	CBJavaClass(istream& input, const JFileVersion vers, CBTree* tree);

	virtual ~CBJavaClass();

	virtual void	ViewSource() const;
	virtual void	ViewHeader() const;

	virtual JBoolean	ViewDefinition(const JCharacter* fnName,
									   const JBoolean caseSensitive,
									   const JBoolean reportNotFound = kJTrue) const;
	virtual JBoolean	ViewDeclaration(const JCharacter* fnName,
										const JBoolean caseSensitive,
										const JBoolean reportNotFound = kJTrue) const;

	virtual JBoolean	IsInherited(const JIndex index, const InheritType inherit,
									FnAccessLevel* access) const;

	virtual void	StreamOut(ostream& output) const;

	static JString	RemoveNamespace(const JCharacter* name);

protected:

	virtual CBClass*			NewGhost(const JCharacter* name, CBTree* tree);
	virtual const JCharacter*	GetNamespaceOperator() const;

	virtual void	AdjustNameStyle(const JColormap* colormap, JFontStyle* style) const;

private:

	JBoolean	itsIsPublicFlag;
	JBoolean	itsIsFinalFlag;

private:

	// not allowed

	CBJavaClass(const CBJavaClass& source);
	CBJavaClass& operator=(const CBJavaClass& source);
};

#endif
