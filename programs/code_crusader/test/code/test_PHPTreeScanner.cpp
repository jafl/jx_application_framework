/******************************************************************************
 test_PHPTreeScanner.cpp

	Test PHP class parser.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include "CBPHPTree.h"
#include "CBClass.h"
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

class TestPHPTree : public CBPHPTree
{
public:

	TestPHPTree()
		:
		CBPHPTree(nullptr, 0)
	{ };

	virtual void ParseFile(const JString& fileName, const JFAID_t id) override;
};

void
TestPHPTree::ParseFile
	(
	const JString&	fileName,
	const JFAID_t	id
	)
{
	CBPHPTree::ParseFile(fileName, id);
}

JTEST(Basic)
{
	JArray<JFAID_t> deadFileList;

	TestPHPTree tree;
	tree.PrepareForUpdate(false);
	tree.ParseFile(JString("./data/tree/php/A1.php", false), 1);
	tree.ParseFile(JString("./data/tree/php/B1.php", false), 2);
	tree.ParseFile(JString("./data/tree/php/InvalidArgumentException.php", false), 3);
	tree.UpdateFinished(deadFileList);

	const TestPHPTree& constTree   = tree;
	const JPtrArray<CBClass>& list = constTree.GetClasses();
	JAssertEqual(5, list.GetElementCount());

	JSize found = 0;
	for (CBClass* c : list)
		{
		if (c->GetFullName() == "Zend\\Authentication\\Adapter\\DbTable\\Exception\\InvalidArgumentException")
			{
			JAssertTrue(c->HasParents());
			JAssertEqual(2, c->GetParentCount());
			}
		else if (c->GetFullName() == "foo\\B\\B1")
			{
			JAssertTrue(c->HasParents());
			JAssertEqual(1, c->GetParentCount());
			}
		else if (c->GetFullName() == "A\\A1")
			{
			JAssertTrue(c->IsAbstract());
			}
		else
			{
			JAssertEqual(CBClass::kGhostType, c->GetDeclareType());
			}

		for (const JUtf8Byte* n :
			{
				"A\\A1",
				"Exception\\InvalidArgumentException",
				"ExceptionInterface",
				"foo\\B\\B1",
				"Zend\\Authentication\\Adapter\\DbTable\\Exception\\InvalidArgumentException"
			})
			{
			if (n == c->GetFullName())
				{
				found++;
				break;
				}
			}
		}
	JAssertEqual(5, found);
}

bool
CBInUpdateThread()
{
	return true;
}
