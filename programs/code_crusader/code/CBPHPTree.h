/******************************************************************************
 CBPHPTree.h

	Copyright (C) 2014 John Lindal.

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
	CBPHPTree(std::istream& projInput, const JFileVersion projVers,
			  std::istream* setInput, const JFileVersion setVers,
			  std::istream* symInput, const JFileVersion symVers,
			  CBPHPTreeDirector* director, const JSize marginWidth,
			  CBDirList* dirList);

	virtual ~CBPHPTree();

	virtual void	StreamOut(std::ostream& projOutput, std::ostream* setOutput,
							  std::ostream* symOutput, const CBDirList* dirList) const override;

protected:

	virtual JBoolean	UpdateFinished(const JArray<JFAID_t>& deadFileList) override;
	virtual void		ParseFile(const JString& fileName, const JFAID_t id) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBPHPTreeScanner*	itsClassNameLexer;	// nullptr unless parsing

private:

	void					ReadFunctionList(std::istream& input, CBClass* theClass);
	CBClass::FnAccessLevel	DecodeAccess(const JStringPtrMap<JString>& flags) const;

	static CBClass* StreamInPHPClass(std::istream& input, const JFileVersion vers,
									 CBTree* tree);

	// written by flex

	JBoolean	Lex(const JString& nameSpace,
					const JString& fileName, const JFAID_t fileID,
					CBClass** theClass);

	// not allowed

	CBPHPTree(const CBPHPTree& source);
	CBPHPTree& operator=(const CBPHPTree& source);
};

#endif
