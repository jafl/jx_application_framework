/******************************************************************************
 CBDTree.h

	Interface for the CBDTree Class

	Copyright (C) 2021 John Lindal.

 ******************************************************************************/

#ifndef _H_CBDTree
#define _H_CBDTree

#include "CBTree.h"

class CBDTreeScanner;
class CBDTreeDirector;
class CBCClass;
class CBClass;

class CBDTree : public CBTree
{
public:

	CBDTree(CBDTreeDirector* director, const JSize marginWidth);
	CBDTree(std::istream& projInput, const JFileVersion projVers,
			std::istream* setInput, const JFileVersion setVers,
			std::istream* symInput, const JFileVersion symVers,
			CBDTreeDirector* director, const JSize marginWidth,
			CBDirList* dirList);

	virtual ~CBDTree();

	virtual JBoolean	UpdateFinished(const JArray<JFAID_t>& deadFileList) override;

protected:

	virtual void	ParseFile(const JString& fileName, const JFAID_t id) override;

private:

	CBDTreeScanner*	itsClassNameLexer;	// nullptr unless parsing

private:

	void	CBDTreeX();

	static CBClass* StreamInCClass(std::istream& input, const JFileVersion vers,
								   CBTree* tree);

	// not allowed

	CBDTree(const CBDTree& source);
	CBDTree& operator=(const CBDTree& source);
};

#endif
