/******************************************************************************
 test_GoTreeScanner.cpp

	Test Go class parser.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include "CBGoTree.h"
#include "CBClass.h"
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

class TestGoTree : public CBGoTree
{
public:

	TestGoTree()
		:
		CBGoTree(nullptr, 0)
	{ };

	virtual void ParseFile(const JString& fileName, const JFAID_t id) override;
};

void
TestGoTree::ParseFile
	(
	const JString&	fileName,
	const JFAID_t	id
	)
{
	CBGoTree::ParseFile(fileName, id);
}

JTEST(Basic)
{
	JArray<JFAID_t> deadFileList;

	TestGoTree tree;
	tree.PrepareForUpdate(false);
	tree.ParseFile(JString("./data/tree/go/a.go", false), 1);
	tree.ParseFile(JString("./data/tree/go/b.go", false), 2);
	tree.ParseFile(JString("./data/tree/go/c.go", false), 3);
	tree.UpdateFinished(deadFileList);

	const TestGoTree& constTree    = tree;
	const JPtrArray<CBClass>& list = constTree.GetClasses();
	JAssertEqual(10, list.GetElementCount());

	JSize found = 0;
	for (CBClass* c : list)
		{
		if (c->GetFullName() == "a.A1" || c->GetFullName() == "c.C1")
			{
			JAssertFalse(c->IsAbstract());
			JAssertFalse(c->HasParents());
			}
		else if (c->GetFullName() == "a.A2" || c->GetFullName() == "b.b3")
			{
			JAssertTrue(c->IsAbstract());
			JAssertFalse(c->HasParents());
			}
		else if (c->GetFullName() == "b.B1" || c->GetFullName() == "b.b2")
			{
			JAssertFalse(c->IsAbstract());
			JAssertTrue(c->HasParents());
			JAssertEqual(1, c->GetParentCount());
			}
		else if (c->GetFullName() == "c.C2")
			{
			JAssertFalse(c->IsAbstract());
			JAssertTrue(c->HasParents());
			JAssertEqual(4, c->GetParentCount());
			}
		else if (c->GetFullName() == "c.c3")
			{
			JAssertTrue(c->IsAbstract());
			JAssertTrue(c->HasParents());
			JAssertEqual(3, c->GetParentCount());
			}
		else if (c->GetFullName() == "D1")
			{
			JAssertEqual(CBClass::kGhostType, c->GetDeclareType());
			}

		for (const JUtf8Byte* n :
			{
				"a.A1",
				"a.A2",
				"b.B1",
				"b.b2",
				"b.B3",
				"b.b4",
				"c.C1",
				"c.C2",
				"c.c3",
				"D1"
			})
			{
			if (n == c->GetFullName())
				{
				found++;
				break;
				}
			}
		}
	JAssertEqual(10, found);
}

bool
CBInUpdateThread()
{
	return true;
}
