/******************************************************************************
 test_CTreeScanner.cpp

	Test C++ class parser.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include "CBCTree.h"
#include "CBClass.h"
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

class TestCTree : public CBCTree
{
public:

	TestCTree()
		:
		CBCTree(nullptr, 0)
	{ };

	virtual void ParseFile(const JString& fileName, const JFAID_t id) override;
};

void
TestCTree::ParseFile
	(
	const JString&	fileName,
	const JFAID_t	id
	)
{
	CBCTree::ParseFile(fileName, id);
}

JTEST(Basic)
{
	JArray<JFAID_t> deadFileList;

	TestCTree tree;
	tree.PrepareForUpdate(kJFalse);
	tree.ParseFile(JString("./data/tree/c/CBCTree.h", kJFalse), 1);
	tree.UpdateFinished(deadFileList);

	const TestCTree& constTree     = tree;
	const JPtrArray<CBClass>& list = constTree.GetClasses();
	JAssertEqual(3, list.GetElementCount());
	JAssertStringsEqual("CBCTree", list.GetElement(1)->GetFullName());
}

JBoolean
CBInUpdateThread()
{
	return kJTrue;
}
