/******************************************************************************
 test_DTreeScanner.cpp

	Test D class parser.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include "CBDTree.h"
#include "CBClass.h"
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

class TestDTree : public CBDTree
{
public:

	TestDTree()
		:
		CBDTree(nullptr, 0)
	{ };

	virtual void ParseFile(const JString& fileName, const JFAID_t id) override;
};

void
TestDTree::ParseFile
	(
	const JString&	fileName,
	const JFAID_t	id
	)
{
	CBDTree::ParseFile(fileName, id);
}

JTEST(Basic)
{
	JArray<JFAID_t> deadFileList;

	TestDTree tree;
	tree.PrepareForUpdate(false);
	tree.ParseFile(JString("./data/tree/d/a.d", false), 1);
	tree.ParseFile(JString("./data/tree/d/b.d", false), 2);
	tree.ParseFile(JString("./data/tree/d/c.d", false), 3);
	tree.ParseFile(JString("./data/tree/d/d.d", false), 4);
	tree.UpdateFinished(deadFileList);

	const TestDTree& constTree     = tree;
	const JPtrArray<CBClass>& list = constTree.GetClasses();
	JAssertEqual(8, list.GetElementCount());

	JSize found = 0;
	for (CBClass* c : list)
		{
std::cout << c->GetFullName() << std::endl;
		if (c->GetFullName() == "a.A1")
			{
			JAssertFalse(c->IsAbstract());
			JAssertFalse(c->HasParents());
			}
		else if (c->GetFullName() == "a.A2")
			{
			JAssertTrue(c->IsAbstract());
			JAssertFalse(c->HasParents());
			}
		else if (c->GetFullName() == "b.B1" ||
				 c->GetFullName() == "b.B3" ||
				 c->GetFullName() == "cc.C2")
			{
			JAssertFalse(c->IsAbstract());
			JAssertTrue(c->HasParents());
			JAssertEqual(1, c->GetParentCount());
			}
		else if (c->GetFullName() == "b.B2")
			{
			JAssertTrue(c->IsAbstract());
			JAssertTrue(c->HasParents());
			JAssertEqual(1, c->GetParentCount());
			}
		else if (c->GetFullName() == "cc.C1")
			{
			JAssertFalse(c->IsAbstract());
			JAssertTrue(c->HasParents());
			JAssertEqual(2, c->GetParentCount());
			}
		else if (c->GetFullName() == "d.D1")
			{
			JAssertFalse(c->IsAbstract());
			JAssertTrue(c->HasParents());
			JAssertEqual(3, c->GetParentCount());
			}

		for (const JUtf8Byte* n :
			{
				"a.A1",
				"a.A2",
				"b.B1",
				"b.B2",
				"b.B3",
				"cc.C1",
				"cc.C2",
				"d.D1"
			})
			{
			if (n == c->GetFullName())
				{
				found++;
				break;
				}
			}
		}
	JAssertEqual(8, found);
}

JTEST(Alias)
{
	JArray<JFAID_t> deadFileList;

	TestDTree tree;
	tree.PrepareForUpdate(false);
	tree.ParseFile(JString("./data/tree/d/alias.d", false), 1);
	tree.UpdateFinished(deadFileList);

	const TestDTree& constTree     = tree;
	const JPtrArray<CBClass>& list = constTree.GetClasses();
	JAssertEqual(5, list.GetElementCount());

	JSize found = 0;
	for (CBClass* c : list)
		{
		if (c->GetFullName() == "alias.Foo2")
			{
			JAssertTrue(c->IsTemplate());
			JAssertFalse(c->HasParents());
			}
		else if (c->GetFullName() == "alias.Foo3" ||
				 c->GetFullName() == "alias.Foo4" ||
				 c->GetFullName() == "alias.Foo5")
			{
			JAssertTrue(c->HasParents());
			JAssertEqual(1, c->GetParentCount());
			}

		for (const JUtf8Byte* n :
			{
				"alias.Foo2",
				"alias.Foo3",
				"alias.Foo4",
				"alias.Foo5",
				"T"
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
