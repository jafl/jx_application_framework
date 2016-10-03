/******************************************************************************
 CBPHPTree.h

	Copyright (C) 2014 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBPHPTree
#define _H_CBPHPTree

#include "CBTree.h"
#include "CBClass.h"		// need definition of FnAccessLevel
#include "CBCtagsUser.h"

class CBPHPTreeScanner;
class CBPHPTreeDirector;
class CBPHPClass;

class CBPHPTree : public CBTree, public CBCtagsUser
{
public:

	CBPHPTree(CBPHPTreeDirector* director, const JSize marginWidth);
	CBPHPTree(istream& projInput, const JFileVersion projVers,
			  istream* setInput, const JFileVersion setVers,
			  istream* symInput, const JFileVersion symVers,
			  CBPHPTreeDirector* director, const JSize marginWidth,
			  CBDirList* dirList);

	virtual ~CBPHPTree();

	virtual void	StreamOut(ostream& projOutput, ostream* setOutput,
							  ostream* symOutput, const CBDirList* dirList) const;

protected:

	virtual JBoolean	UpdateFinished(const JArray<JFAID_t>& deadFileList);
	virtual void		ParseFile(const JCharacter* fileName, const JFAID_t id);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	CBPHPTreeScanner*	itsClassNameLexer;	// NULL unless parsing

private:

	void					ReadFunctionList(istream& input, CBClass* theClass);
	CBClass::FnAccessLevel	DecodeAccess(const JStringPtrMap<JString>& flags) const;

	static CBClass* StreamInPHPClass(istream& input, const JFileVersion vers,
									 CBTree* tree);

	// written by flex

	JBoolean	Lex(const JCharacter* nameSpace,
					const JCharacter* fileName, const JFAID_t fileID,
					CBClass** theClass);

	// not allowed

	CBPHPTree(const CBPHPTree& source);
	CBPHPTree& operator=(const CBPHPTree& source);
};

#endif
