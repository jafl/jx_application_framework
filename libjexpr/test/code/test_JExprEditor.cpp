/******************************************************************************
 test_JExprEditor.cpp

	Tests for JExprEditor.

	Written by John Lindal.

 *****************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include "ExprEditor.h"
#include "TestVarList.h"
#include "TestFontManager.h"
#include <jx-af/jexpr/JExprParser.h>
#include <jx-af/jexpr/JSummation.h>
#include <jx-af/jexpr/JConstantValue.h>
#include <fstream>
#include <typeinfo>
#include <jx-af/jcore/jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(rtti)
{
	JFunction* c1 = jnew JConstantValue(3.5);

	JFunction* c2 = jnew JConstantValue(-2);

	auto* sum = jnew JSummation;
	sum->AppendArg(c1);
	sum->AppendArg(c2);

	const std::type_info& i1 = typeid(*c1);
	const std::type_info& i2 = typeid(*c2);
	JAssertTrue(i1 == i2);

	JAssertTrue(i1 == typeid(JConstantValue));

	const std::type_info* i3 = & typeid(*sum);

	JAssertTrue(*i3 == typeid(JSummation));

	const JFunction* sum2 = sum;

	JAssertTrue(typeid(*sum2) == typeid(JSummation));
}

JTEST(Exercise)
{
	TestFontManager fontMgr;
	TestVarList varList;
	ExprEditor e(&varList, &fontMgr);

	e.ApplyFunctionToSelection(JString("sqrt", JString::kNoCopy));

	JAssertStringsEqual("sqrt(?)", e.GetFunction()->Print());

	JFunction* f = jnew JConstantValue(3.5);
	e.SetFunction(&varList, f);

	e.SelectAll();
	e.Copy();

	e.ApplyFunctionToSelection(JString("log", JString::kNoCopy));

	JAssertStringsEqual("log(?,3.5)", e.GetFunction()->Print());

	e.ClearSelection();
	e.ApplyFunctionToSelection(JString("arctan2", JString::kNoCopy));

	JAssertStringsEqual("log(arctan2(?,?),3.5)", e.GetFunction()->Print());

	e.SelectAll();
	e.Paste();

	JAssertStringsEqual("3.5", e.GetFunction()->Print());

	JFloat v;
	JAssertTrue(e.EvaluateSelection(&v));
	JAssertEqual(3.5, v);

	e.Undo();

	JAssertStringsEqual("log(arctan2(?,?),3.5)", e.GetFunction()->Print());
}

JTEST(GetCmdStatus)
{
	JArray<bool> status;
	for (JIndex i=1; i<=JExprEditor::kCmdCount; i++)
	{
		status.AppendItem(false);
	}

	TestFontManager fontMgr;
	TestVarList varList;
	ExprEditor e(&varList, &fontMgr);
	e.Activate();

	status.SetItem(JExprEditor::kPasteCmd, true);
	status.SetItem(JExprEditor::kSelectAllCmd, true);
	status.SetItem(JExprEditor::kPrintEPSCmd, true);
	status.SetItem(JExprEditor::kNegateSelCmd, true);
	status.SetItem(JExprEditor::kApplyFnToSelCmd, true);

	status.SetItem(JExprEditor::kSetNormalFontCmd, true);

	std::cout << "GetCmdStatus::basic" << std::endl;
	e.CheckCmdStatus(status);

	JFunction* f = jnew JConstantValue(3.5);
	e.SetFunction(&varList, f);
	e.SelectAll();

	status.SetItem(JExprEditor::kCutCmd, true);
	status.SetItem(JExprEditor::kCopyCmd, true);
	status.SetItem(JExprEditor::kDeleteSelCmd, true);
	status.SetItem(JExprEditor::kSetNormalFontCmd, false);

	std::cout << "GetCmdStatus::has selection" << std::endl;
	e.CheckCmdStatus(status);

	e.DeleteSelection();
	e.Activate();

	status.SetItem(JExprEditor::kUndoCmd, true);
	status.SetItem(JExprEditor::kCutCmd, false);
	status.SetItem(JExprEditor::kCopyCmd, false);
	status.SetItem(JExprEditor::kDeleteSelCmd, false);
	status.SetItem(JExprEditor::kSetNormalFontCmd, true);

	std::cout << "GetCmdStatus::has undo" << std::endl;
	e.CheckCmdStatus(status);

	e.SetGreekFont();

	status.SetItem(JExprEditor::kSetNormalFontCmd, false);
	status.SetItem(JExprEditor::kSetGreekFontCmd, true);

	std::cout << "GetCmdStatus::greek font" << std::endl;
	e.CheckCmdStatus(status);

	e.ApplyFunctionToSelection(JString("max", JString::kNoCopy));
	e.SelectAll();

	status.SetItem(JExprEditor::kCutCmd, true);
	status.SetItem(JExprEditor::kCopyCmd, true);
	status.SetItem(JExprEditor::kDeleteSelCmd, true);
	status.SetItem(JExprEditor::kAddArgCmd, true);
	status.SetItem(JExprEditor::kSetGreekFontCmd, false);

	std::cout << "GetCmdStatus::add arg" << std::endl;
	e.CheckCmdStatus(status);

	e.AddArgument();
	e.ActivateNextUIF();

	status.SetItem(JExprEditor::kCutCmd, false);
	status.SetItem(JExprEditor::kCopyCmd, false);
	status.SetItem(JExprEditor::kDeleteSelCmd, false);
	status.SetItem(JExprEditor::kAddArgCmd, false);
	status.SetItem(JExprEditor::kMoveArgRightCmd, true);
	status.SetItem(JExprEditor::kSetNormalFontCmd, true);
	status.SetItem(JExprEditor::kSetGreekFontCmd, false);

	std::cout << "GetCmdStatus::move arg" << std::endl;
	e.CheckCmdStatus(status);

	JExprParser p(&varList);
	JAssertTrue(p.Parse(JString("1+2", JString::kNoCopy), &f));

	e.SetFunction(&varList, f);
	e.SelectAll();
	e.AddArgument();

	status.SetItem(JExprEditor::kMoveArgLeftCmd, true);
	status.SetItem(JExprEditor::kMoveArgRightCmd, false);
	status.SetItem(JExprEditor::kGroupLeftCmd, true);

	std::cout << "GetCmdStatus::group arg" << std::endl;
	e.CheckCmdStatus(status);

	e.GroupArguments(-1);

	status.SetItem(JExprEditor::kCutCmd, true);
	status.SetItem(JExprEditor::kCopyCmd, true);
	status.SetItem(JExprEditor::kDeleteSelCmd, true);
	status.SetItem(JExprEditor::kAddArgCmd, true);
	status.SetItem(JExprEditor::kUngroupCmd, true);
	status.SetItem(JExprEditor::kSetNormalFontCmd, false);

	std::cout << "GetCmdStatus::ungroup arg" << std::endl;
	e.CheckCmdStatus(status);
}
