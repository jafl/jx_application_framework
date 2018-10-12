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

	JSummation* sum = jnew JSummation;
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

	e.ApplyFunctionToSelection(JString("sqrt", kJFalse));

	JAssertStringsEqual("sqrt(?)", e.GetFunction()->Print());

	JFunction* f = jnew JConstantValue(3.5);
	assert( f != nullptr );
	e.SetFunction(&varList, f);

	e.SelectAll();
	e.Copy();

	e.ApplyFunctionToSelection(JString("log", kJFalse));

	JAssertStringsEqual("log(?,3.5)", e.GetFunction()->Print());

	e.ClearSelection();
	e.ApplyFunctionToSelection(JString("arctan2", kJFalse));

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
	JRunArray<JBoolean> status;
	status.AppendElements(kJFalse, JExprEditor::kCmdCount);

	TestFontManager fontMgr;
	TestVarList varList;
	ExprEditor e(&varList, &fontMgr);
	e.Activate();

	status.SetElement(JExprEditor::kPasteCmd, kJTrue);
	status.SetElement(JExprEditor::kSelectAllCmd, kJTrue);
	status.SetElement(JExprEditor::kPrintEPSCmd, kJTrue);
	status.SetElement(JExprEditor::kNegateSelCmd, kJTrue);
	status.SetElement(JExprEditor::kApplyFnToSelCmd, kJTrue);

	status.SetElement(JExprEditor::kSetNormalFontCmd, kJTrue);

	std::cout << "GetCmdStatus::basic" << std::endl;
	e.CheckCmdStatus(status);

	JFunction* f = jnew JConstantValue(3.5);
	assert( f != nullptr );
	e.SetFunction(&varList, f);
	e.SelectAll();

	status.SetElement(JExprEditor::kCutCmd, kJTrue);
	status.SetElement(JExprEditor::kCopyCmd, kJTrue);
	status.SetElement(JExprEditor::kDeleteSelCmd, kJTrue);
	status.SetElement(JExprEditor::kSetNormalFontCmd, kJFalse);

	std::cout << "GetCmdStatus::has selection" << std::endl;
	e.CheckCmdStatus(status);

	e.DeleteSelection();
	e.Activate();

	status.SetElement(JExprEditor::kUndoCmd, kJTrue);
	status.SetElement(JExprEditor::kCutCmd, kJFalse);
	status.SetElement(JExprEditor::kCopyCmd, kJFalse);
	status.SetElement(JExprEditor::kDeleteSelCmd, kJFalse);
	status.SetElement(JExprEditor::kSetNormalFontCmd, kJTrue);

	std::cout << "GetCmdStatus::has undo" << std::endl;
	e.CheckCmdStatus(status);

	e.SetGreekFont();

	status.SetElement(JExprEditor::kSetNormalFontCmd, kJFalse);
	status.SetElement(JExprEditor::kSetGreekFontCmd, kJTrue);

	std::cout << "GetCmdStatus::greek font" << std::endl;
	e.CheckCmdStatus(status);

	e.ApplyFunctionToSelection(JString("max", kJFalse));
	e.SelectAll();

	status.SetElement(JExprEditor::kCutCmd, kJTrue);
	status.SetElement(JExprEditor::kCopyCmd, kJTrue);
	status.SetElement(JExprEditor::kDeleteSelCmd, kJTrue);
	status.SetElement(JExprEditor::kAddArgCmd, kJTrue);
	status.SetElement(JExprEditor::kSetGreekFontCmd, kJFalse);

	std::cout << "GetCmdStatus::add arg" << std::endl;
	e.CheckCmdStatus(status);

	e.AddArgument();
	e.ActivateNextUIF();

	status.SetElement(JExprEditor::kCutCmd, kJFalse);
	status.SetElement(JExprEditor::kCopyCmd, kJFalse);
	status.SetElement(JExprEditor::kDeleteSelCmd, kJFalse);
	status.SetElement(JExprEditor::kAddArgCmd, kJFalse);
	status.SetElement(JExprEditor::kMoveArgRightCmd, kJTrue);
	status.SetElement(JExprEditor::kSetNormalFontCmd, kJTrue);
	status.SetElement(JExprEditor::kSetGreekFontCmd, kJFalse);

	std::cout << "GetCmdStatus::move arg" << std::endl;
	e.CheckCmdStatus(status);

	JExprParser p(&varList);
	JAssertTrue(p.Parse(JString("1+2", kJFalse), &f));

	e.SetFunction(&varList, f);
	e.SelectAll();
	e.AddArgument();

	status.SetElement(JExprEditor::kMoveArgLeftCmd, kJTrue);
	status.SetElement(JExprEditor::kMoveArgRightCmd, kJFalse);
	status.SetElement(JExprEditor::kGroupLeftCmd, kJTrue);

	std::cout << "GetCmdStatus::group arg" << std::endl;
	e.CheckCmdStatus(status);

	e.GroupArguments(-1);

	status.SetElement(JExprEditor::kCutCmd, kJTrue);
	status.SetElement(JExprEditor::kCopyCmd, kJTrue);
	status.SetElement(JExprEditor::kDeleteSelCmd, kJTrue);
	status.SetElement(JExprEditor::kAddArgCmd, kJTrue);
	status.SetElement(JExprEditor::kUngroupCmd, kJTrue);
	status.SetElement(JExprEditor::kSetNormalFontCmd, kJFalse);

	std::cout << "GetCmdStatus::ungroup arg" << std::endl;
	e.CheckCmdStatus(status);
}
