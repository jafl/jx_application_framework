/******************************************************************************
 CBGoTree.h

	Copyright (C) 2021 John Lindal.

 ******************************************************************************/

#ifndef _H_CBGoTree
#define _H_CBGoTree

#include "CBTree.h"

class CBGoTreeScanner;
class CBGoTreeDirector;
class CBGoClass;
class CBClass;

class CBGoTree : public CBTree
{
public:

	CBGoTree(CBGoTreeDirector* director, const JSize marginWidth);
	CBGoTree(std::istream& projInput, const JFileVersion projVers,
			 std::istream* setInput, const JFileVersion setVers,
			 std::istream* symInput, const JFileVersion symVers,
			 CBGoTreeDirector* director, const JSize marginWidth,
			   CBDirList* dirList);

	virtual ~CBGoTree();

	virtual void	StreamOut(std::ostream& projOutput, std::ostream* setOutput,
							  std::ostream* symOutput, const CBDirList* dirList) const override;

	virtual JBoolean	UpdateFinished(const JArray<JFAID_t>& deadFileList) override;

protected:

	virtual void	ParseFile(const JString& fileName, const JFAID_t id) override;

private:

	CBGoTreeScanner*	itsClassNameLexer;	// nullptr unless parsing

private:

	static CBClass* StreamInGoClass(std::istream& input, const JFileVersion vers,
									CBTree* tree);

	// not allowed

	CBGoTree(const CBGoTree& source);
	CBGoTree& operator=(const CBGoTree& source);
};

#endif
