/******************************************************************************
 test_CTreeScanner.cpp

	Test C++ class parser.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include "CBCTree.h"
#include "CBClass.h"
#include "CBCPreprocessor.h"
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
	tree.GetCPreprocessor()->DefineMacro(
		JString("TEST_MACRO", false),
		JString("CBCtagsUser", false));

	tree.PrepareForUpdate(false);
	tree.ParseFile(JString("./data/tree/c/CBTestTree.h", false), 1);
	tree.UpdateFinished(deadFileList);

	const TestCTree& constTree     = tree;
	const JPtrArray<CBClass>& list = constTree.GetClasses();
	JAssertEqual(3, list.GetElementCount());

	JSize found = 0;
	for (CBClass* c : list)
		{
		if (c->GetFullName() == "CBTestTree")
			{
			JAssertTrue(c->HasParents());
			JAssertEqual(2, c->GetParentCount());
			}
		else
			{
			JAssertEqual(CBClass::kGhostType, c->GetDeclareType());
			}

		for (const JUtf8Byte* n :
			{
				"CBTestTree",
				"CBTree",
				"CBCtagsUser"
			})
			{
			if (n == c->GetFullName())
				{
				found++;
				break;
				}
			}
		}
	JAssertEqual(3, found);
}

JTEST(Namespace)
{
	JArray<JFAID_t> deadFileList;

	TestCTree tree;
	tree.PrepareForUpdate(false);
	tree.ParseFile(JString("./data/tree/c/namespace.h", false), 1);
	tree.UpdateFinished(deadFileList);

	const TestCTree& constTree     = tree;
	const JPtrArray<CBClass>& list = constTree.GetClasses();
	JAssertEqual(6, list.GetElementCount());

	JSize found = 0;
	for (CBClass* c : list)
		{
		if (c->GetFullName() == "Test1::Test2::Bar" ||
			c->GetFullName() == "Test1::Test2::Zot")
			{
			JAssertTrue(c->HasParents());
			JAssertEqual(1, c->GetParentCount());
			}
		else if (c->GetFullName() == "Test1::Test2::Baz")
			{
			JAssertTrue(c->HasParents());
			JAssertEqual(2, c->GetParentCount());
			}
		else if (c->GetFullName() == "Shug")
			{
			JAssertEqual(CBClass::kGhostType, c->GetDeclareType());
			}

		for (const JUtf8Byte* n :
			{
				"Test1::Foo",
				"Test1::Test2::Bar",
				"Test1::Test2::Bar::Zod",
				"Test1::Test2::Zot",
				"Test1::Test2::Baz",
				"Shug"
			})
			{
			if (n == c->GetFullName())
				{
				found++;
				break;
				}
			}
		}
	JAssertEqual(6, found);
}

JTEST(Template)
{
	JArray<JFAID_t> deadFileList;

	TestCTree tree;
	tree.PrepareForUpdate(false);
	tree.ParseFile(JString("./data/tree/c/template.h", false), 1);
	tree.UpdateFinished(deadFileList);

	const TestCTree& constTree     = tree;
	const JPtrArray<CBClass>& list = constTree.GetClasses();
	JAssertEqual(1, list.GetElementCount());

	CBClass* c = list.GetFirstElement();
	JAssertTrue(c->IsTemplate());
	JAssertStringsEqual("Foo", c->GetName());
}

bool
CBInUpdateThread()
{
	return true;
}
