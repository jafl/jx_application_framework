/******************************************************************************
 CBJavaTree.h

	Copyright (C) 1999 John Lindal.

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
	CBJavaTree(std::istream& projInput, const JFileVersion projVers,
			   std::istream* setInput, const JFileVersion setVers,
			   std::istream* symInput, const JFileVersion symVers,
			   CBJavaTreeDirector* director, const JSize marginWidth,
			   CBDirList* dirList);

	virtual ~CBJavaTree();

	virtual void	StreamOut(std::ostream& projOutput, std::ostream* setOutput,
							  std::ostream* symOutput, const CBDirList* dirList) const override;

protected:

	virtual JBoolean	UpdateFinished(const JArray<JFAID_t>& deadFileList) override;
	virtual void		ParseFile(const JString& fileName, const JFAID_t id) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBJavaTreeScanner*	itsClassNameLexer;	// nullptr unless parsing

private:

	void					ReadFunctionList(std::istream& input, CBClass* theClass);
	CBClass::FnAccessLevel	DecodeAccess(const JStringPtrMap<JString>& flags) const;

	static CBClass* StreamInJavaClass(std::istream& input, const JFileVersion vers,
									  CBTree* tree);

	// written by flex

	JBoolean	Lex(const JString& nameSpace,
					const JString& fileName, const JFAID_t fileID,
					CBClass** theClass);

	// not allowed

	CBJavaTree(const CBJavaTree& source);
	CBJavaTree& operator=(const CBJavaTree& source);
};

#endif
