/******************************************************************************
 test_JExprEditor.cpp

	Tests for JExprEditor.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include "ExprEditor.h"
#include "TestVarList.h"
#include "TestFontManager.h"
#include <JExprParser.h>
#include <JSummation.h>
#include <JConstantValue.h>
#include <fstream>
#include <typeinfo>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(rtti)
{
	JFunction* c1 = jnew JConstantValue(3.5);
	assert( c1 != nullptr );

	JFunction* c2 = jnew JConstantValue(-2);
	assert( c2 != nullptr );

	auto* sum = jnew JSummation;
	assert( sum != nullptr );
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
	assert( f != nullptr );
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
		status.AppendElement(false);
	}

	TestFontManager fontMgr;
	TestVarList varList;
	ExprEditor e(&varList, &fontMgr);
	e.Activate();

	status.SetElement(JExprEditor::kPasteCmd, true);
	status.SetElement(JExprEditor::kSelectAllCmd, true);
	status.SetElement(JExprEditor::kPrintEPSCmd, true);
	status.SetElement(JExprEditor::kNegateSelCmd, true);
	status.SetElement(JExprEditor::kApplyFnToSelCmd, true);

	status.SetElement(JExprEditor::kSetNormalFontCmd, true);

	std::cout << "GetCmdStatus::basic" << std::endl;
	e.CheckCmdStatus(status);

	JFunction* f = jnew JConstantValue(3.5);
	assert( f != nullptr );
	e.SetFunction(&varList, f);
	e.SelectAll();

	status.SetElement(JExprEditor::kCutCmd, true);
	status.SetElement(JExprEditor::kCopyCmd, true);
	status.SetElement(JExprEditor::kDeleteSelCmd, true);
	status.SetElement(JExprEditor::kSetNormalFontCmd, false);

	std::cout << "GetCmdStatus::has selection" << std::endl;
	e.CheckCmdStatus(status);

	e.DeleteSelection();
	e.Activate();

	status.SetElement(JExprEditor::kUndoCmd, true);
	status.SetElement(JExprEditor::kCutCmd, false);
	status.SetElement(JExprEditor::kCopyCmd, false);
	status.SetElement(JExprEditor::kDeleteSelCmd, false);
	status.SetElement(JExprEditor::kSetNormalFontCmd, true);

	std::cout << "GetCmdStatus::has undo" << std::endl;
	e.CheckCmdStatus(status);

	e.SetGreekFont();

	status.SetElement(JExprEditor::kSetNormalFontCmd, false);
	status.SetElement(JExprEditor::kSetGreekFontCmd, true);

	std::cout << "GetCmdStatus::greek font" << std::endl;
	e.CheckCmdStatus(status);

	e.ApplyFunctionToSelection(JString("max", JString::kNoCopy));
	e.SelectAll();

	status.SetElement(JExprEditor::kCutCmd, true);
	status.SetElement(JExprEditor::kCopyCmd, true);
	status.SetElement(JExprEditor::kDeleteSelCmd, true);
	status.SetElement(JExprEditor::kAddArgCmd, true);
	status.SetElement(JExprEditor::kSetGreekFontCmd, false);

	std::cout << "GetCmdStatus::add arg" << std::endl;
	e.CheckCmdStatus(status);

	e.AddArgument();
	e.ActivateNextUIF();

	status.SetElement(JExprEditor::kCutCmd, false);
	status.SetElement(JExprEditor::kCopyCmd, false);
	status.SetElement(JExprEditor::kDeleteSelCmd, false);
	status.SetElement(JExprEditor::kAddArgCmd, false);
	status.SetElement(JExprEditor::kMoveArgRightCmd, true);
	status.SetElement(JExprEditor::kSetNormalFontCmd, true);
	status.SetElement(JExprEditor::kSetGreekFontCmd, false);

	std::cout << "GetCmdStatus::move arg" << std::endl;
	e.CheckCmdStatus(status);

	JExprParser p(&varList);
	JAssertTrue(p.Parse(JString("1+2", JString::kNoCopy), &f));

	e.SetFunction(&varList, f);
	e.SelectAll();
	e.AddArgument();

	status.SetElement(JExprEditor::kMoveArgLeftCmd, true);
	status.SetElement(JExprEditor::kMoveArgRightCmd, false);
	status.SetElement(JExprEditor::kGroupLeftCmd, true);

	std::cout << "GetCmdStatus::group arg" << std::endl;
	e.CheckCmdStatus(status);

	e.GroupArguments(-1);

	status.SetElement(JExprEditor::kCutCmd, true);
	status.SetElement(JExprEditor::kCopyCmd, true);
	status.SetElement(JExprEditor::kDeleteSelCmd, true);
	status.SetElement(JExprEditor::kAddArgCmd, true);
	status.SetElement(JExprEditor::kUngroupCmd, true);
	status.SetElement(JExprEditor::kSetNormalFontCmd, false);

	std::cout << "GetCmdStatus::ungroup arg" << std::endl;
	e.CheckCmdStatus(status);
}
