/******************************************************************************
 CBCTree.h

	Interface for the CBCTree Class

	Copyright (C) 1995-99 John Lindal.

 ******************************************************************************/

#ifndef _H_CBCTree
#define _H_CBCTree

#include "CBTree.h"
#include "CBClass.h"	// need definition of FnAccessLevel

class CBCTreeDirector;
class CBCPreprocessor;
class CBCClass;

class CBCTree : public CBTree
{
public:

	CBCTree(CBCTreeDirector* director, const JSize marginWidth);
	CBCTree(std::istream& projInput, const JFileVersion projVers,
			std::istream* setInput, const JFileVersion setVers,
			std::istream* symInput, const JFileVersion symVers,
			CBCTreeDirector* director, const JSize marginWidth,
			CBDirList* dirList);

	virtual ~CBCTree();

	CBCPreprocessor*	GetCPreprocessor() const;

	virtual void	StreamOut(std::ostream& projOutput, std::ostream* setOutput,
							  std::ostream* symOutput, const CBDirList* dirList) const;

protected:

	virtual void	ParseFile(const JCharacter* fileName, const JFAID_t id);

private:

	CBCPreprocessor*	itsCPP;

private:

	void	CBCTreeX();

	static CBClass* StreamInCClass(std::istream& input, const JFileVersion vers,
								   CBTree* tree);

	void	ReadHeaderFile(const JCharacter* fileName, JString* buffer);
	void	ParseClasses(JString* buffer, const JCharacter* classNamePrefix,
						 const JFAID_t fileID);
	void	ParseNamespace(const JString& nsName, const JFAID_t fileID,
						   JString* buffer, const JIndex braceIndex);
	void	ParseInheritance(const JCharacter* origNamespace,
							 CBCClass* theClass, JString* inheritance);
	void	ParseFunctions(CBCClass* theClass, const JFAID_t fileID,
						   JString* buffer, const JIndex braceIndex);
	CBClass::FnAccessLevel
	ParseFnAccessLevel(const CBClass::FnAccessLevel currLevel,
					   const JString& buffer, const JIndex endIndex);
	void	ParseEnumValues(CBCClass* theClass, JString* buffer, const JIndex braceIndex);

	JIndex	FindEndOfBlock(const JIndex blockStartIndex, const JCharacter openBlockChar,
						   const JCharacter closeBlockChar, const JString& buffer,
						   JBoolean* hasNestedBlocks);
	void	RemoveBalancedBlocks(JString* buffer, const JCharacter openChar,
								 const JCharacter closeChar);

	// not allowed

	CBCTree(const CBCTree& source);
	CBCTree& operator=(const CBCTree& source);
};


/******************************************************************************
 GetCPreprocessor

 ******************************************************************************/

inline CBCPreprocessor*
CBCTree::GetCPreprocessor()
	const
{
	return itsCPP;
}

#endif
