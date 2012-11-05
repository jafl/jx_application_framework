/******************************************************************************
 CBCClass.h

	Copyright © 1996-99 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBCClass
#define _H_CBCClass

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBClass.h"

class CBCClass : public CBClass
{
public:

	CBCClass(const JCharacter* name, const DeclareType declType,
			 const JFAID_t fileID, CBTree* tree);
	CBCClass(istream& input, const JFileVersion vers, CBTree* tree);

	virtual ~CBCClass();

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

	static JString	RemoveNamespace(const JCharacter* name);

protected:

	virtual CBClass*			NewGhost(const JCharacter* name, CBTree* tree);
	virtual const JCharacter*	GetNamespaceOperator() const;

	virtual void	AdjustNameStyle(const JColormap* colormap, JFontStyle* style) const;

private:

	JBoolean	FindDefinition(const JString& headerName, const JCharacter* searchStr,
							   const JBoolean caseSensitive) const;

	// not allowed

	CBCClass(const CBCClass& source);
	CBCClass& operator=(const CBCClass& source);
};

#endif
