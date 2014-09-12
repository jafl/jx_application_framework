/******************************************************************************
 CBPHPClass.h

	Copyright © 2014 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBPHPClass
#define _H_CBPHPClass

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBClass.h"

class CBPHPClass : public CBClass
{
public:

	CBPHPClass(const JCharacter* name, const DeclareType declType,
			   const JFAID_t fileID, CBTree* tree,
			   const JBoolean isFinal);
	CBPHPClass(istream& input, const JFileVersion vers, CBTree* tree);

	virtual ~CBPHPClass();

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

	JBoolean	itsIsFinalFlag;

private:

	// not allowed

	CBPHPClass(const CBPHPClass& source);
	CBPHPClass& operator=(const CBPHPClass& source);
};

#endif
