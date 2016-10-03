/******************************************************************************
 CBJavaTree.h

	Copyright (C) 1999 John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBJavaTree
#define _H_CBJavaTree

#include "CBTree.h"
#include "CBClass.h"		// need definition of FnAccessLevel
#include "CBCtagsUser.h"

class CBJavaTreeScanner;
class CBJavaTreeDirector;
class CBJavaClass;

class CBJavaTree : public CBTree, public CBCtagsUser
{
public:

	CBJavaTree(CBJavaTreeDirector* director, const JSize marginWidth);
	CBJavaTree(istream& projInput, const JFileVersion projVers,
			   istream* setInput, const JFileVersion setVers,
			   istream* symInput, const JFileVersion symVers,
			   CBJavaTreeDirector* director, const JSize marginWidth,
			   CBDirList* dirList);

	virtual ~CBJavaTree();

	virtual void	StreamOut(ostream& projOutput, ostream* setOutput,
							  ostream* symOutput, const CBDirList* dirList) const;

protected:

	virtual JBoolean	UpdateFinished(const JArray<JFAID_t>& deadFileList);
	virtual void		ParseFile(const JCharacter* fileName, const JFAID_t id);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	CBJavaTreeScanner*	itsClassNameLexer;	// NULL unless parsing

private:

	void					ReadFunctionList(istream& input, CBClass* theClass);
	CBClass::FnAccessLevel	DecodeAccess(const JStringPtrMap<JString>& flags) const;

	static CBClass* StreamInJavaClass(istream& input, const JFileVersion vers,
									  CBTree* tree);

	// written by flex

	JBoolean	Lex(const JCharacter* nameSpace,
					const JCharacter* fileName, const JFAID_t fileID,
					CBClass** theClass);

	// not allowed

	CBJavaTree(const CBJavaTree& source);
	CBJavaTree& operator=(const CBJavaTree& source);
};

#endif
