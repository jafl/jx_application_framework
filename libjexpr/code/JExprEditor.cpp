/******************************************************************************
 JExprEditor.cpp

	Abstract base class to allow user interaction with a rendered JFunction.

	Only public and protected functions are required to call SaveStateForUndo(),
	and only if the action to be performed changes the function itself.
	Private functions must -not- call SaveStateForUndo(), because several
	manipulations may be required to perform one user command, and only the user
	command as a whole is undoable.  (Otherwise, the user may get confused.)

	Because of this convention, public functions that affect undo should only
	be a public interface to a private function.  The public function calls
	SaveStateForUndo() and then calls the private function.  The private function
	does all the work, but doesn't modify the undo information.  This allows other
	private functions to use the routine (private version) without modifying the
	undo information.

	Derived classes must implement the following routines:

		EIPRefresh
			Put an update event in the queue to redraw the function eventually.

		EIPRedraw
			Redraw the function immediately.

		EIPBoundsChanged
			Adjust the scroll bars to fit the size of the current function.

		EIPScrollToRect
			Scroll the function to make the given rectangle visible.  Return true
			if scrolling was necessary.

		EIPScrollForDrag
			Scroll the function to make the given point visible.  Return true
			if scrolling was necessary.

		EIPAdjustNeedTab
			If the OS prefers to use TAB for other purposes (like shifting focus),
			this routine should notify the OS whether or not it can use TAB.
			Since it is -very- much easier for the derived class to call this after
			menu selections and key presses, we require that it do it instead of
			us.  We do it only in those cases when the derived class would never
			hear of the change.

		EIPClipboardChanged
			Do whatever is appropriate to update the system clipboard
			after a Copy or Cut operation.

		EIPOwnsClipboard
			Return true if we should paste from our internal clipboard.

		EIPGetExternalClipboard
			Return true if there is something pasteable on the system clipboard.

	Derived classes can override the following routines:

		EIPExprChanged
			Override to set or clear whatever flags are needed to keep
			track of whether or not the current expression has been saved.

	BASE CLASS = JExprRenderer, virtual JBroadcaster

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#include "JExprEditor.h"
#include "JExprRectList.h"
#include "JVariableList.h"
#include "JUserInputFunction.h"

#include "JExprParser.h"
#include "jFunctionUtil.h"
#include "JFunctionWithVar.h"
#include "JUnaryFunction.h"
#include "JBinaryFunction.h"
#include "JNaryFunction.h"
#include "JSummation.h"
#include "JNegation.h"
#include "JProduct.h"
#include "JDivision.h"
#include "JExponent.h"
#include "JParallel.h"
#include "JLogB.h"
#include "JVariableValue.h"
#include "JConstantValue.h"

#include <JEPSPrinter.h>
#include <JFontManager.h>
#include <JColorManager.h>
#include <JMinMax.h>
#include <jASCIIConstants.h>
#include <jMath.h>
#include <jGlobals.h>
#include <jAssert.h>

// JBroadcaster message types

const JUtf8Byte* JExprEditor::kExprChanged = "ExprChanged::JExprEditor";

/******************************************************************************
 Constructor

	Derived class must call ClearFunction().

 ******************************************************************************/

JExprEditor::JExprEditor
	(
	const JVariableList*	varList,
	JFontManager*			fontManager
	)
	:
	JExprRenderer(),
	itsFontManager(fontManager),
	itsTextColor(JColorManager::GetBlackColor()),
	itsSelectionColor(JColorManager::GetDefaultSelectionColor()),
	itsDefaultStyle(itsTextColor)
{
	itsVarList    = varList;
	itsFunction   = nullptr;
	itsRectList   = nullptr;
	itsSelection  = 0;
	itsActiveFlag = false;
	itsActiveUIF  = nullptr;

	itsUndoFunction  = nullptr;
	itsUndoSelection = 0;

	itsFunctionClip = nullptr;

	itsPainter = nullptr;

	itsDragType = kInvalidDrag;

	ListenTo(itsVarList);

	// We can't call ClearFunction() here because the _vtbl isn't fully
	// constructed.
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JExprEditor::~JExprEditor()
{
	assert( itsPainter == nullptr );

	jdelete itsFunction;
	jdelete itsRectList;

	jdelete itsUndoFunction;
	jdelete itsFunctionClip;
}

/******************************************************************************
 SetFunction

	We take ownership of the function.

 ******************************************************************************/

void
JExprEditor::SetFunction
	(
	const JVariableList*	varList,
	JFunction*				f
	)
{
	assert( f != nullptr );

	jdelete itsUndoFunction;
	itsUndoFunction  = nullptr;
	itsUndoSelection = 0;

	if (itsVarList != varList)
	{
		StopListening(itsVarList);
		itsVarList = varList;
		ListenTo(itsVarList);
	}

	jdelete itsFunction;
	itsFunction  = f;
	itsActiveUIF = nullptr;
	Render();
}

/******************************************************************************
 ClearFunction

 ******************************************************************************/

void
JExprEditor::ClearFunction()
{
	jdelete itsUndoFunction;
	itsUndoFunction  = nullptr;
	itsUndoSelection = 0;

	JUserInputFunction* newUIF = PrivateClearFunction();
	ActivateUIF(newUIF);
}

/******************************************************************************
 PrivateClearFunction (private)

 ******************************************************************************/

JUserInputFunction*
JExprEditor::PrivateClearFunction()
{
	jdelete itsFunction;
	auto* newUIF = jnew JUserInputFunction(this);
	assert( newUIF != nullptr );
	itsFunction = newUIF;

	itsActiveUIF = nullptr;
	Render();
	return newUIF;
}

/******************************************************************************
 BuildStyledText (virtual)

 ******************************************************************************/

JStyledText*
JExprEditor::BuildStyledText()
{
	auto* text = jnew JStyledText(false, false);
	assert( text != nullptr );
	return text;
}

/******************************************************************************
 ContainsUIF

	Returns true if we contain any JUserInputFunctions.

 ******************************************************************************/

bool
JExprEditor::ContainsUIF()
	const
{
	const JSize rectCount = itsRectList->GetElementCount();
	for (JIndex i=1; i<=rectCount; i++)
	{
		const JFunction* f = itsRectList->GetFunction(i);
		if (dynamic_cast<const JUserInputFunction*>(f) != nullptr)
		{
			return true;
		}
	}

	// falling through means there aren't any

	return false;
}

/******************************************************************************
 EIPDeactivate (protected)

	If the current UIF is invalid, we simply clear it.  This is because
	EndEditing() should already have been called.  If it wasn't, then the
	intent is to discard the function.

 ******************************************************************************/

void
JExprEditor::EIPDeactivate()
{
	if (itsActiveUIF != nullptr)
	{
		itsActiveUIF->Clear();
		itsActiveUIF = nullptr;
		Render();
	}
	else
	{
		ClearSelection();
	}

	itsActiveFlag = false;
}

/******************************************************************************
 EndEditing

	Parse the active JUserInputFunction and replace it.
	Returns true if successful.

 ******************************************************************************/

bool
JExprEditor::EndEditing()
{
	if (itsActiveUIF == nullptr)
	{
		return true;
	}
	else if (itsActiveUIF->IsEmpty())
	{
		itsActiveUIF->Deactivate();
		itsActiveUIF = nullptr;
		EIPRefresh();
		return true;
	}

	JFunction* newF = nullptr;
	JUserInputFunction* newUIF = nullptr;
	bool needRender;
	if (itsActiveUIF->Parse(JUtf8Character(' '), &newF, &newUIF, &needRender))
	{
		ReplaceFunction(itsActiveUIF, newF);
		itsActiveUIF = nullptr;
		Render();
		return true;
	}
	else if (needRender)
	{
		JUserInputFunction* savedUIF = itsActiveUIF;
		itsActiveUIF = nullptr;
		Render();
		itsActiveUIF = savedUIF;
		return false;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 CancelEditing

	Clear the active JUserInputFunction and deactivate it.

 ******************************************************************************/

void
JExprEditor::CancelEditing()
{
	if (itsActiveUIF != nullptr)
	{
		itsActiveUIF->Clear();
		itsActiveUIF->Deactivate();
		itsActiveUIF = nullptr;
		Render();	// text of UIF may have changed
	}
}

/******************************************************************************
 Undo

 ******************************************************************************/

void
JExprEditor::Undo()
{
	if (itsUndoFunction == nullptr)
	{
		return;
	}

	JGetUserNotification()->SetSilent(true);
	const bool currFnOK = EndEditing();
	JGetUserNotification()->SetSilent(false);

	if (currFnOK)
	{
		JFunction* savedF = itsFunction;
		itsFunction       = itsUndoFunction;
		itsUndoFunction   = savedF;

		JIndex savedSel  = itsSelection;
		itsSelection     = itsUndoSelection;
		itsUndoSelection = savedSel;

		// The issue isn't under the rug if we re-save itsSelection.
		// (Render() clears itsSelection)

		savedSel = itsSelection;
		Render();
		SetSelection(savedSel);
	}
	else
	{
		itsActiveUIF = nullptr;	// kill editing

		itsFunction     = itsUndoFunction;
		itsUndoFunction = nullptr;

		itsSelection     = itsUndoSelection;
		itsUndoSelection = 0;

		JIndex savedSel = itsSelection;
		Render();
		SetSelection(savedSel);
	}
}

/******************************************************************************
 SaveStateForUndo (private)

 ******************************************************************************/

void
JExprEditor::SaveStateForUndo()
{
	assert( itsFunction != nullptr );

	jdelete itsUndoFunction;
	itsUndoFunction  = itsFunction->Copy();
	itsUndoSelection = itsSelection;

	Broadcast(ExprChanged());
}

/******************************************************************************
 EIPExprChanged (virtual protected)

	Derived classes can override to set or clear "expr saved" flags.

 ******************************************************************************/

void
JExprEditor::EIPExprChanged()
{
}

/******************************************************************************
 Cut

 ******************************************************************************/

void
JExprEditor::Cut()
{
	JFunction* f;
	if (Cut(&f))
	{
		jdelete itsFunctionClip;
		itsFunctionClip = f;
		EIPClipboardChanged();
	}
}

/******************************************************************************
 Cut

	Returns true if there was anything to cut.
	If function returns false, *f is not modified.

 ******************************************************************************/

bool
JExprEditor::Cut
	(
	JFunction** f
	)
{
	if (Copy(f))
	{
		DeleteSelection();
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 Copy

 ******************************************************************************/

void
JExprEditor::Copy()
{
	JFunction* f;
	if (Copy(&f))
	{
		jdelete itsFunctionClip;
		itsFunctionClip = f;
		EIPClipboardChanged();
	}
}

/******************************************************************************
 Copy

	Returns true if there was anything to copy.
	If function returns false, *f is not modified.

 ******************************************************************************/

bool
JExprEditor::Copy
	(
	JFunction** f
	)
	const
{
	const JFunction* selF;
	if (GetConstSelectedFunction(&selF))
	{
		*f = selF->Copy();
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 Paste

 ******************************************************************************/

void
JExprEditor::Paste()
{
	JUserNotification* un = JGetUserNotification();

	JString text;
	PasteResult result = kPasteOK;
	if (itsFunctionClip != nullptr && EIPOwnsClipboard())
	{
		result = Paste(*itsFunctionClip);
	}
	else if (EIPGetExternalClipboard(&text))
	{
		un->SetSilent(true);
		result = Paste(text);
		un->SetSilent(false);

		if (result == kParseError)
		{
			const JUtf8Byte* map[] =
		{
				"name", text.GetBytes()
		};
			const JString msg = JGetString("ParseError::JExprEditor", map, sizeof(map));
			un->ReportError(msg);
		}
	}

	if (result == kNowhereToPaste)
	{
		un->ReportError(JGetString("NowhereToPaste::JExprEditor"));
	}
}

/******************************************************************************
 Paste

	Returns true if expr parsed successfully.

	We don't call Paste(f) because that would require an extra copy operation,
	which can be expensive for JFuntions.

 ******************************************************************************/

JExprEditor::PasteResult
JExprEditor::Paste
	(
	const JString& expr
	)
{
	JFunction* selF;
	if (GetSelectedFunction(&selF))
	{
		JExprParser p(this);
		JFunction* newF;
		if (p.Parse(expr, &newF))
		{
			SaveStateForUndo();
			ReplaceFunction(selF, newF);
			Render();
			SelectFunction(newF);
			return kPasteOK;
		}
		else
		{
			return kParseError;
		}
	}
	else if (itsActiveUIF != nullptr && itsActiveUIF->IsEmpty())
	{
		JExprParser p(this);
		JFunction* newF;
		if (p.Parse(expr, &newF))
		{
			SaveStateForUndo();
			ReplaceFunction(itsActiveUIF, newF);
			itsActiveUIF = nullptr;
			Render();
			SelectFunction(newF);
			return kPasteOK;
		}
		else
		{
			return kParseError;
		}
	}
	else
	{
		return kNowhereToPaste;
	}
}

/******************************************************************************
 Paste

 ******************************************************************************/

JExprEditor::PasteResult
JExprEditor::Paste
	(
	const JFunction& f
	)
{
	JFunction* selF;
	if (GetSelectedFunction(&selF))
	{
		JFunction* newF = f.Copy();
		SaveStateForUndo();
		ReplaceFunction(selF, newF);
		Render();
		SelectFunction(newF);
		return kPasteOK;
	}
	else if (itsActiveUIF != nullptr && itsActiveUIF->IsEmpty())
	{
		JFunction* newF = f.Copy();
		SaveStateForUndo();
		ReplaceFunction(itsActiveUIF, newF);
		itsActiveUIF = nullptr;
		Render();
		SelectFunction(newF);
		return kPasteOK;
	}
	else
	{
		return kNowhereToPaste;
	}
}

/******************************************************************************
 DeleteSelection

 ******************************************************************************/

void
JExprEditor::DeleteSelection()
{
	if (HasSelection())
	{
		SaveStateForUndo();
		DeleteFunction( itsRectList->GetFunction(itsSelection) );
		Render();
	}
}

/******************************************************************************
 SelectAll

 ******************************************************************************/

void
JExprEditor::SelectAll()
{
	SelectFunction(itsFunction);
}

/******************************************************************************
 EvaluateSelection

 ******************************************************************************/

bool
JExprEditor::EvaluateSelection
	(
	JFloat* value
	)
	const
{
	const JFunction* f;
	if (GetConstSelectedFunction(&f))
	{
		return f->Evaluate(value);
	}
	else
	{
		return itsFunction->Evaluate(value);
	}
}

/******************************************************************************
 NegateSelection

 ******************************************************************************/

void
JExprEditor::NegateSelection()
{
	JFunction* f = itsActiveUIF;
	if (f != nullptr || GetSelectedFunction(&f))
	{
		SaveStateForUndo();
		JFunction* arg = f->Copy();
		auto* neg = jnew JNegation(arg);
		assert( neg != nullptr );
		itsActiveUIF = nullptr;
		ReplaceFunction(f, neg);
		Render();
		SelectFunction(arg);
	}
}

/******************************************************************************
 ApplyFunctionToSelection

 ******************************************************************************/

void
JExprEditor::ApplyFunctionToSelection
	(
	const JString& fnName
	)
{
	JFunction* f = itsActiveUIF;
	if (f != nullptr || GetSelectedFunction(&f))
	{
		JFunction* newF;
		JFunction* newArg;
		JUserInputFunction* newUIF;
		if (ApplyFunction(fnName, *f, &newF, &newArg, &newUIF))
		{
			SaveStateForUndo();
			itsActiveUIF = nullptr;
			ReplaceFunction(f, newF);
			Render();
			if (newUIF != nullptr)
			{
				ActivateUIF(newUIF);
			}
			else
			{
				SelectFunction(newArg);
			}
		}
	}
}

/******************************************************************************
 ApplyFunction

	Apply the specified function to the given JFunction.  Returns the
	resulting function, the copy of the argument that was passed in,
	and the first new UIF, if successful.

 ******************************************************************************/

bool
JExprEditor::ApplyFunction
	(
	const JString&			fnName,
	const JFunction&		origF,

	JFunction**				newF,
	JFunction**				newArg,
	JUserInputFunction**	newUIF
	)
{
	*newF   = nullptr;
	*newUIF = nullptr;

	JSize argCount = 0;
	if (!JGetArgCount(fnName, &argCount))
	{
		const JUtf8Byte* map[] =
	{
			"name", fnName.GetBytes()
	};
		const JString msg = JGetString("UnknownFunction::JExprEditor", map, sizeof(map));
		JGetUserNotification()->ReportError(msg);
		return false;
	}

	JString buffer(fnName);
	buffer += "(";
	for (JIndex i=1; i<argCount; i++)
	{
		buffer += "1,";
	}
	buffer += "1)";

	const JSize origArgCount = argCount;

	JExprParser p(this);
	const bool ok = p.Parse(buffer, newF);
	if (ok)
	{
		*newArg = origF.Copy();

		const bool isLogB = dynamic_cast<JLogB*>(*newF) != nullptr;

		auto* fwa = dynamic_cast<JFunctionWithArgs*>(*newF);
		assert( fwa != nullptr );
		argCount = fwa->GetArgCount();
		for (JIndex i=1; i<=argCount; i++)
		{
			if (i == 2 && isLogB)
			{
				fwa->SetArg(2, *newArg);
			}
			else if (i == 1 && !isLogB)
			{
				fwa->SetArg(1, *newArg);
			}
			else if (!isLogB || origArgCount > 1)
			{
				auto* uif = jnew JUserInputFunction(this);
				assert( uif != nullptr );
				fwa->SetArg(i, uif);
				if (*newUIF == nullptr)
				{
					*newUIF = uif;
				}
			}
		}
	}
	return ok;
}

/******************************************************************************
 AddArgument

 ******************************************************************************/

void
JExprEditor::AddArgument()
{
	JFunction* f;
	if (GetSelectedFunction(&f))
	{
		auto* naryF = dynamic_cast<JNaryFunction*>(f);
		if (naryF != nullptr)
		{
			SaveStateForUndo();
			auto* uif = jnew JUserInputFunction(this);
			assert( uif != nullptr );
			naryF->AppendArg(uif);
			Render();
			ActivateUIF(uif);
		}
	}
}

/******************************************************************************
 MoveArgument

	delta specifies how many slots to shift the selected argument by.

	delta<0 => shift left
	delta>0 => shift right

 ******************************************************************************/

void
JExprEditor::MoveArgument
	(
	const JInteger delta
	)
{
	JFunction* f;
	JFunction* parentF;
	if (delta != 0 && GetNegAdjSelFunction(&f, &parentF))
	{
		auto* naryParentF = dynamic_cast<JNaryFunction*>(parentF);
		if (naryParentF != nullptr)
		{
			JIndex argIndex;
			const bool found = naryParentF->FindArg(f, &argIndex);
			assert( found );
			JIndex newIndex = argIndex + delta;
			if (naryParentF->ArgIndexValid(newIndex))
			{
				SaveStateForUndo();
				naryParentF->MoveArgToIndex(argIndex, newIndex);
				itsActiveUIF = nullptr;
				Render();
				SelectFunction(f);
			}
		}
	}
}

/******************************************************************************
 GroupArguments

	delta specifies how many arguments to group together with the selected one.

	delta<0 => group selected arg with -delta-1 args to the left
	delta>0 => group selected arg with  delta-1 args to the right

 ******************************************************************************/

void
JExprEditor::GroupArguments
	(
	const JInteger delta
	)
{
	JFunction* f;
	JFunction* parentF;
	if (delta == 0 || !GetNegAdjSelFunction(&f, &parentF))
	{
		return;
	}

	auto* naryParentF = dynamic_cast<JNaryOperator*>(parentF);
	if (naryParentF == nullptr)
	{
		return;
	}

	JIndex argIndex;
	const bool found = naryParentF->FindArg(f, &argIndex);
	assert( found );
	const JIndex firstArg = JMin(argIndex, argIndex+delta);
	const JIndex lastArg  = JMax(argIndex, argIndex+delta);

	const JSize parentArgCount = naryParentF->GetArgCount();

	const bool sameType =
		typeid(*parentF) == typeid(*f);
	const bool argsInRange =
		1 <= firstArg && lastArg <= parentArgCount;
	const bool groupAll =
		lastArg - firstArg + 1 == parentArgCount;

	auto* neg = dynamic_cast<JUnaryFunction*>(f);
	JFunction* negArg   = neg != nullptr ? neg->GetArg() : nullptr;
	const bool extendNegation =
		typeid(*parentF) == typeid(JSummation) &&
			  neg != nullptr && typeid(*neg) == typeid(JNegation) &&
			  typeid(*negArg) == typeid(JSummation);

	// handle special case w+x-(y+z)
	// (if gobbles last argument, remove parentF entirely)

	if (extendNegation && argsInRange)
	{
		SaveStateForUndo();
		auto* naryF = dynamic_cast<JNaryOperator*>(neg->GetArg());
		assert( naryF != nullptr );
		if (delta > 0)
		{
			for (JIndex i=1; i <= (JSize) delta; i++)
			{
				naryF->AppendArg(Negate(*(naryParentF->GetArg(argIndex+1))));
				naryParentF->DeleteArg(argIndex+1);
			}
		}
		else	// from above, know that delta != 0
		{
			for (JIndex i=1; i <= (JSize) -delta; i++)
			{
				naryF->PrependArg(Negate(*(naryParentF->GetArg(argIndex-i))));
				naryParentF->DeleteArg(argIndex-i);
			}
		}
		itsActiveUIF = nullptr;
		if (groupAll)
		{
			f = f->Copy();
			ReplaceFunction(parentF, f);
		}
		Render();
		SelectFunction(f);
	}

	// create a new JNaryOperator to contain the group

	else if (!sameType && argsInRange && !groupAll)
	{
		JIndex i;
		SaveStateForUndo();

		// Copy the parent function and delete the args that
		// will not be in the group.

		JFunction* newF      = parentF->Copy();
		auto* group = dynamic_cast<JNaryOperator*>(newF);
		assert( group != nullptr );
		while (group->GetArgCount() > lastArg)
		{
			group->DeleteArg(lastArg+1);
		}
		for (i=1; i<firstArg; i++)
		{
			group->DeleteArg(1);
		}

		// Replace the original args from the parent function
		// with the new group.

		for (i=1; i<=lastArg-firstArg+1; i++)
		{
			naryParentF->DeleteArg(firstArg);
		}
		naryParentF->InsertArg(firstArg, group);

		// show the result

		itsActiveUIF = nullptr;
		Render();
		SelectFunction(group);
	}

	// add the arguments to the existing group

	else if (sameType && argsInRange && !groupAll)
	{
		SaveStateForUndo();
		auto* naryF = dynamic_cast<JNaryOperator*>(f);
		assert( naryF != nullptr );
		if (delta > 0)
		{
			for (JIndex i=1; i <= (JSize) delta; i++)
			{
				naryF->AppendArg((naryParentF->GetArg(argIndex+1))->Copy());
				naryParentF->DeleteArg(argIndex+1);
			}
		}
		else	// from above, know that delta != 0
		{
			for (JIndex i=1; i <= (JSize) -delta; i++)
			{
				naryF->PrependArg((naryParentF->GetArg(argIndex-i))->Copy());
				naryParentF->DeleteArg(argIndex-i);
			}
		}
		itsActiveUIF = nullptr;
		Render();
		SelectFunction(f);
	}

	// group all arguments => just remove existing group

	else if (sameType && argsInRange && groupAll)
	{
		UngroupArguments();
	}
}

/******************************************************************************
 UngroupArguments

	Absorb the grouped arguments into the parent function.

 ******************************************************************************/

void
JExprEditor::UngroupArguments()
{
	JFunction* f;
	JFunction* parentF;
	if (GetNegAdjSelFunction(&f, &parentF))
	{
		auto* naryF       = dynamic_cast<JNaryOperator*>(f);
		auto* naryParentF = dynamic_cast<JNaryOperator*>(parentF);
		if (naryF != nullptr && naryParentF != nullptr &&
			typeid(*naryF) == typeid(*naryParentF))
		{
			SaveStateForUndo();

			JIndex argIndex;
			const bool found = naryParentF->FindArg(naryF, &argIndex);
			assert( found );

			const JSize fArgCount = naryF->GetArgCount();
			for (JIndex i=1; i<=fArgCount; i++)
			{
				naryParentF->InsertArg(argIndex+i, (naryF->GetArg(i))->Copy());
			}
			naryParentF->DeleteArg(argIndex);

			itsActiveUIF = nullptr;
			Render();
			SelectFunction(naryParentF);
		}

		// handle special case w+x-(y+z)

		else if (naryParentF != nullptr &&
				 typeid(*naryParentF) == typeid(JSummation) &&
				 typeid(*f)           == typeid(JNegation))
		{
			const auto* neg = dynamic_cast<const JUnaryFunction*>(f);
			assert( neg != nullptr );
			const auto* naryF = dynamic_cast<const JNaryOperator*>(neg->GetArg());
			if (naryF != nullptr && typeid(*naryF) == typeid(JSummation))
			{
				SaveStateForUndo();

				JIndex argIndex;
				const bool found = naryParentF->FindArg(f, &argIndex);
				assert( found );

				const JSize fArgCount = naryF->GetArgCount();
				for (JIndex i=1; i<=fArgCount; i++)
				{
					naryParentF->InsertArg(argIndex+i, Negate(*(naryF->GetArg(i))));
				}
				naryParentF->DeleteArg(argIndex);

				itsActiveUIF = nullptr;
				Render();
				SelectFunction(naryParentF);
			}
		}
	}
}

/******************************************************************************
 Negate (private)

	Returns a copy of the negation of the given function.  If the function is
	already a negation, this is simply removed.

 ******************************************************************************/

JFunction*
JExprEditor::Negate
	(
	const JFunction& f
	)
	const
{
	const auto* neg = dynamic_cast<const JUnaryFunction*>(&f);
	if (neg != nullptr)
	{
		return (neg->GetArg())->Copy();
	}
	else
	{
		JFunction* neg = jnew JNegation(f.Copy());
		assert( neg != nullptr );
		return neg;
	}
}

/******************************************************************************
 HasSelection

 ******************************************************************************/

bool
JExprEditor::HasSelection()
	const
{
	return itsRectList != nullptr && itsRectList->SelectionValid(itsSelection);
}

/******************************************************************************
 GetSelection (protected)

 ******************************************************************************/

bool
JExprEditor::GetSelection
	(
	JIndex* selection
	)
	const
{
	*selection = itsSelection;
	return HasSelection();
}

/******************************************************************************
 GetSelectionRect (protected)

 ******************************************************************************/

bool
JExprEditor::GetSelectionRect
	(
	JRect* selRect
	)
	const
{
	if (HasSelection())
	{
		*selRect = itsRectList->GetRect(itsSelection);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 GetSelectedFunction

 ******************************************************************************/

// protected

bool
JExprEditor::GetSelectedFunction
	(
	JFunction** f
	)
{
	if (HasSelection())
	{
		*f = itsRectList->GetFunction(itsSelection);
		return true;
	}
	else
	{
		return false;
	}
}

// public

bool
JExprEditor::GetConstSelectedFunction	// to avoid "ambiguous reference" errors
	(
	const JFunction** f
	)
	const
{
	if (HasSelection())
	{
		*f = itsRectList->GetFunction(itsSelection);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 GetNegAdjSelFunction (private)

	If possible, returns the selected function and its parent.
	If the selected function is y in the case of "x-y", then it returns
	"-y" as the function.  This is the entire reason for the function.
	It returns the "negation adjusted" selected function.

	This treats an active UIF as if it were selected.

	Returns false if the selected function has no parent.

 ******************************************************************************/

bool
JExprEditor::GetNegAdjSelFunction
	(
	JFunction** selF,
	JFunction** parentF
	)
	const
{
	*selF = itsActiveUIF;
	if ((*selF != nullptr ||
		 GetConstSelectedFunction(const_cast<const JFunction**>(selF))) &&
		*selF != itsFunction)
	{
		bool hasParent = (**selF).GetParent(parentF);
		assert( hasParent );

		JFunction* grandparentF = nullptr;
		if (*parentF != itsFunction)
		{
			hasParent = (**parentF).GetParent(&grandparentF);
			assert( hasParent );
		}

		if (typeid(**parentF) == typeid(JNegation) &&
			grandparentF != nullptr && typeid(*grandparentF) == typeid(JSummation))
		{
			*selF    = *parentF;
			*parentF = grandparentF;
		}

		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 SetSelection (protected)

 ******************************************************************************/

void
JExprEditor::SetSelection
	(
	const JIndex index
	)
{
	if (itsRectList != nullptr &&
		index != itsSelection && itsRectList->SelectionValid(index))
	{
		itsSelection = index;
		if (!EIPScrollToRect(itsRectList->GetRect(itsSelection)))
		{
			EIPRedraw();
		}
	}
	else if (itsRectList != nullptr &&
			 index == 0 && itsSelection != 0)
	{
		itsSelection = 0;
		EIPRedraw();
	}
}

/******************************************************************************
 ClearSelection

 ******************************************************************************/

void
JExprEditor::ClearSelection()
{
	if (itsRectList != nullptr &&
		itsRectList->SelectionValid(itsSelection))
	{
		itsSelection = 0;
		EIPRedraw();
	}
}

/******************************************************************************
 SelectFunction

	If we can't select the specified function, we try each of its arguments.
	We can always select terminal nodes, so we won't get stuck.

	If f is a JUIF, we activate it instead.

	We need this mainly because JSummation hides JNegations.

 ******************************************************************************/

void
JExprEditor::SelectFunction
	(
	const JFunction* f
	)
{
	const bool wasItsFunction = f == itsFunction;
	if (itsRectList != nullptr && EndEditing())
	{
		if (wasItsFunction)		// in case itsFunction was single, active UIF
		{
			f = itsFunction;
		}

		JIndex fIndex;
		bool found = itsRectList->FindFunction(f, &fIndex);
		while (!found)
		{
			const auto* fwa = dynamic_cast<const JFunctionWithArgs*>(f);
			assert( fwa != nullptr && fwa->GetArgCount() > 0 );
			f     = fwa->GetArg(1);
			found = itsRectList->FindFunction(f, &fIndex);
		}

		const auto* uif = dynamic_cast<const JUserInputFunction*>(f);
		if (uif != nullptr)
		{
			ActivateUIF(const_cast<JUserInputFunction*>(uif));
		}
		else
		{
			SetSelection(fIndex);
		}
	}
}

/******************************************************************************
 Set font

	Set the current font of the active UIF.

 ******************************************************************************/

void
JExprEditor::SetNormalFont()
{
	if (itsActiveUIF != nullptr)
	{
		itsActiveUIF->SetGreek(false);
	}
}

void
JExprEditor::SetGreekFont()
{
	if (itsActiveUIF != nullptr)
	{
		itsActiveUIF->SetGreek(true);
	}
}

/******************************************************************************
 GetCmdStatus (protected)

	Returns an array indicating which commands can be performed in the
	current state.

	evalStr can be nullptr.

 ******************************************************************************/

JArray<bool>
JExprEditor::GetCmdStatus
	(
	JString* evalStr
	)
	const
{
	JArray<bool> flags(kCmdCount);
	for (JIndex i=1; i<=kCmdCount; i++)
	{
		flags.AppendElement(false);
	}

	if (itsFunction == nullptr || itsRectList == nullptr || !itsActiveFlag)
	{
		return flags;
	}

	flags.SetElement(kEvaluateSelCmd, CanDisplaySelectionValue());
	flags.SetElement(kSelectAllCmd, true);
	flags.SetElement(kPrintEPSCmd, true);

	const bool hasSelection = itsRectList->SelectionValid(itsSelection);

	if (itsUndoFunction != nullptr)
	{
		flags.SetElement(kUndoCmd, true);
	}
	if (hasSelection || (itsActiveUIF != nullptr && itsActiveUIF->IsEmpty()))
	{
		flags.SetElement(kPasteCmd, true);
	}

	if (evalStr != nullptr)
	{
		*evalStr = JGetString(hasSelection ? "EvalSelectionCmd::JExprEditor" : "EvalFnCmd::JExprEditor");
	}

	if (hasSelection)
	{
		flags.SetElement(kCutCmd, true);
		flags.SetElement(kCopyCmd, true);
		flags.SetElement(kDeleteSelCmd, true);

		flags.SetElement(kNegateSelCmd, true);
		flags.SetElement(kApplyFnToSelCmd, true);

		JFunction* selF = itsRectList->GetFunction(itsSelection);
		if (dynamic_cast<JNaryFunction*>(selF) != nullptr)
		{
			flags.SetElement(kAddArgCmd, true);
		}
	}

	if (itsActiveUIF != nullptr)
	{
		flags.SetElement(kNegateSelCmd, true);
		flags.SetElement(kApplyFnToSelCmd, true);

		if (itsActiveUIF->IsGreek())
		{
			flags.SetElement(kSetGreekFontCmd, true);
		}
		else
		{
			flags.SetElement(kSetNormalFontCmd, true);
		}
	}

	JFunction *selF, *parentF;
	if (GetNegAdjSelFunction(&selF, &parentF))
	{
		auto* naryParentF = dynamic_cast<JNaryFunction*>(parentF);
		if (naryParentF != nullptr)
		{
			const JSize parentArgCount = naryParentF->GetArgCount();
			JIndex argIndex;
			const bool found = naryParentF->FindArg(selF, &argIndex);
			assert( found );
			if (argIndex > 1)
			{
				flags.SetElement(kMoveArgLeftCmd, true);
			}
			if (argIndex < parentArgCount)
			{
				flags.SetElement(kMoveArgRightCmd, true);
			}

			if (dynamic_cast<JNaryOperator*>(naryParentF) != nullptr)
			{
				if (argIndex > 1)
				{
					flags.SetElement(kGroupLeftCmd, true);
				}
				if (argIndex < parentArgCount)
				{
					flags.SetElement(kGroupRightCmd, true);
				}
				if (typeid(*selF) == typeid(*naryParentF))
				{
					flags.SetElement(kUngroupCmd, true);
				}
				else if (typeid(*naryParentF) == typeid(JSummation) &&
						 typeid(*selF)        == typeid(JNegation))
				{
					const auto* neg = dynamic_cast<const JUnaryFunction*>(selF);
					assert( neg != nullptr );
					const JFunction* negArg = neg->GetArg();
					if (typeid(*negArg) == typeid(JSummation))
					{
						flags.SetElement(kUngroupCmd, true);
					}
				}
			}
		}
	}

	return flags;
}

/******************************************************************************
 CanDisplaySelectionValue (virtual protected)

 ******************************************************************************/

bool
JExprEditor::CanDisplaySelectionValue()
	const
{
	return false;
}

/******************************************************************************
 DisplaySelectionValue (virtual protected)

 ******************************************************************************/

void
JExprEditor::DisplaySelectionValue()
	const
{
}

/******************************************************************************
 Print

	EPS

 ******************************************************************************/

void
JExprEditor::Print
	(
	JEPSPrinter& p
	)
{
	const JRect bounds = GetPrintBounds();

	if (p.WantsPreview())
	{
		JPainter& p1 = p.GetPreviewPainter(bounds);
		EIPDraw(p1);
	}

	if (p.OpenDocument(bounds))
	{
		EIPDraw(p);
		p.CloseDocument();
	}
}

/******************************************************************************
 GetPrintBounds

 ******************************************************************************/

JRect
JExprEditor::GetPrintBounds()
	const
{
	return itsRectList->GetBoundsRect();
}

/******************************************************************************
 DrawForPrint

	Anywhere on the page

	Not called Print() because it would conflict with other prototypes.

 ******************************************************************************/

void
JExprEditor::DrawForPrint
	(
	JPainter&		p,
	const JPoint&	topLeft
	)
{
	p.ShiftOrigin(topLeft);
	EIPDraw(p);
	p.ShiftOrigin(-topLeft);
}

/******************************************************************************
 EIPDraw (protected)

 ******************************************************************************/

void
JExprEditor::EIPDraw
	(
	JPainter& p
	)
{
	assert( itsPainter == nullptr );
	itsPainter = &p;

	// hilight selection

	JRect selRect;
	if (GetSelectionRect(&selRect))
	{
		p.SetPenColor(itsSelectionColor);
		p.SetFilling(true);
		p.Rect(selRect);
		p.SetFilling(false);
		p.SetPenColor(itsTextColor);
	}

	// draw the function

	itsFunction->Render(*this, *itsRectList);

	// clean up

	itsPainter = nullptr;
}

/******************************************************************************
 EIPHandleMouseDown (protected)

 ******************************************************************************/

void
JExprEditor::EIPHandleMouseDown
	(
	const JPoint&	currPt,
	const bool	extend
	)
{
	itsDragType = kInvalidDrag;
	if (!itsActiveFlag)
	{
		return;
	}

	// If the user clicked on the active UIF, let it handle the click.

	if (itsRectList != nullptr && MouseOnActiveUIF(currPt))
	{
		itsDragType = kSendToUIF;
		const bool redraw =
			itsActiveUIF->HandleMouseDown(currPt, extend, *itsRectList, *this);
		if (redraw)
		{
			EIPRedraw();
		}
	}

	// The click is for us.  We let the user select something.

	else if (itsRectList != nullptr)
	{
		itsDragType = kSelectExpr;
		if (itsActiveUIF != nullptr && !EndEditing())
		{
			itsDragType = kInvalidDrag;
			return;
		}

		itsPrevSelectedFunction = nullptr;
		const bool hasSelection = itsRectList->SelectionValid(itsSelection);
		if (hasSelection)
		{
			itsPrevSelectedFunction = itsRectList->GetFunction(itsSelection);
		}

		if (extend && hasSelection)
		{
			const JRect origSelRect = itsRectList->GetRect(itsSelection);
			itsStartPt.x = origSelRect.xcenter();
			itsStartPt.y = origSelRect.ycenter();
		}
		else
		{
			itsStartPt = currPt;
		}

		const JIndex newSelection = itsRectList->GetSelection(itsStartPt, currPt);
		SetSelection(newSelection);
	}
}

/******************************************************************************
 EIPHandleMouseDrag (protected)

 ******************************************************************************/

void
JExprEditor::EIPHandleMouseDrag
	(
	const JPoint& currPt
	)
{
	if (itsRectList != nullptr && itsDragType == kSelectExpr)
	{
		EIPScrollForDrag(currPt);

		const JIndex newSelection = itsRectList->GetSelection(itsStartPt, currPt);
		if (newSelection != itsSelection)
		{
			itsSelection = newSelection;
			EIPRedraw();
		}
	}
	else if (itsRectList != nullptr && itsDragType == kSendToUIF)
	{
		const bool redraw =
			itsActiveUIF->HandleMouseDrag(currPt, *itsRectList, *this);
		if (redraw)
		{
			EIPRedraw();
		}
	}
}

/******************************************************************************
 EIPHandleMouseUp (protected)

	Activate a JUserInputFunction if only it was selected.
	Allow editing of a number if it was clicked on a second time.

 ******************************************************************************/

void
JExprEditor::EIPHandleMouseUp()
{
	if (itsRectList != nullptr && itsDragType == kSelectExpr &&
		itsRectList->SelectionValid(itsSelection))
	{
		assert( itsActiveUIF == nullptr );

		JFunction* selectedF = itsRectList->GetFunction(itsSelection);
		const std::type_info& selectedFType = typeid(*selectedF);
		if (selectedFType == typeid(JUserInputFunction))
		{
			auto* uif = dynamic_cast<JUserInputFunction*>(selectedF);
			assert( uif != nullptr );
			ActivateUIF(uif);
			assert( itsActiveUIF != nullptr );
			itsActiveUIF->HandleMouseDown(itsStartPt, false, *itsRectList, *this);
			itsActiveUIF->HandleMouseUp();
			EIPRefresh();
		}
		else if ((selectedFType == typeid(JConstantValue) ||
				  selectedFType == typeid(JVariableValue)) &&
				 selectedF == itsPrevSelectedFunction)
		{
			SaveStateForUndo();
			JString s;
			if (selectedFType == typeid(JConstantValue))
			{
				auto* constVal = dynamic_cast<JConstantValue*>(selectedF);
				assert( constVal != nullptr );
				s = JString(constVal->GetValue());
			}
			else
			{
				assert( selectedFType == typeid(JVariableValue) );
				auto* varVal = dynamic_cast<JFunctionWithVar*>(selectedF);
				assert( varVal != nullptr );
				s = itsVarList->GetVariableName(varVal->GetVariableIndex());
			}
			auto* newUIF = jnew JUserInputFunction(this, s);
			assert( newUIF != nullptr );
			ReplaceFunction(selectedF, newUIF);
			Render();
			ActivateUIF(newUIF);
			assert( itsActiveUIF != nullptr );
			itsActiveUIF->HandleMouseDown(itsStartPt, false, *itsRectList, *this);
			itsActiveUIF->HandleMouseUp();
			EIPRefresh();
		}
	}
	else if (itsDragType == kSendToUIF)
	{
		const bool redraw = itsActiveUIF->HandleMouseUp();
		if (redraw)
		{
			EIPRefresh();
		}
	}

	itsDragType = kInvalidDrag;
}

/******************************************************************************
 MouseOnActiveUIF (protected)

 ******************************************************************************/

bool
JExprEditor::MouseOnActiveUIF
	(
	const JPoint& pt
	)
	const
{
	JIndex uifIndex;
	return itsRectList != nullptr && itsActiveUIF != nullptr &&
				itsRectList->FindFunction(itsActiveUIF, &uifIndex) &&
				itsRectList->GetSelection(pt, pt) == uifIndex;
}

/******************************************************************************
 EIPHandleKeyPress (protected)

 ******************************************************************************/

void
JExprEditor::EIPHandleKeyPress
	(
	const JUtf8Character& key
	)
{
	if (itsFunction == nullptr || itsRectList == nullptr || !itsActiveFlag)
	{
		return;
	}
	else if (key == '\t')
	{
		ActivateNextUIF();
		return;
	}

	const bool selectionValid = itsRectList->SelectionValid(itsSelection);
	if (!selectionValid && itsActiveUIF == nullptr)
	{
		return;
	}
	assert( ( selectionValid && itsActiveUIF == nullptr) ||
			(!selectionValid && itsActiveUIF != nullptr) );

	const bool isOperatorKey =
		key == '+' || (key == '-' && (selectionValid || !itsActiveUIF->IsEmpty())) ||
			key == '*' || key == '/' || key == '^' || key == '|' ||
			(key == ',' && CanApplyCommaOperator());

	if (selectionValid && (key == ' ' || key == ')'))
	{
		SetSelection(itsRectList->GetParent(itsSelection));
	}

	else if (selectionValid && key == kJDeleteKey)
	{
		SaveStateForUndo();

		// replace selection with JUserInputFunction

		auto* newUIF = jnew JUserInputFunction(this);
		assert( newUIF != nullptr );
		JFunction* selectedF = itsRectList->GetFunction(itsSelection);
		ReplaceFunction(selectedF, newUIF);
		Render();
		ActivateUIF(newUIF);
	}

	else if (selectionValid && key.IsPrint())
	{
		SaveStateForUndo();

		JFunction* selectedF = itsRectList->GetFunction(itsSelection);
		if (isOperatorKey)
		{
			ApplyOperatorKey(key, selectedF);
		}
		else
		{
			// replace selection with JUserInputFunction

			auto* newUIF = jnew JUserInputFunction(this);
			assert( newUIF != nullptr );
			newUIF->Activate();
			bool needParse, needRender;
			newUIF->HandleKeyPress(key, &needParse, &needRender);
			assert( !needParse );	// it was just created
			newUIF->Deactivate();

			ReplaceFunction(selectedF, newUIF);
			Render();
			ActivateUIF(newUIF);
		}
	}

	else if (itsActiveUIF != nullptr && key == kJEscapeKey)
	{
		if (!itsActiveUIF->IsEmpty())
		{
			itsActiveUIF->Clear();
			JUserInputFunction* savedUIF = itsActiveUIF;
			itsActiveUIF = nullptr;
			Render();
			itsActiveUIF = savedUIF;
		}
	}

	else if (itsActiveUIF != nullptr &&
			 (isOperatorKey ||
			  key == '\n' || key == '\r' || key == ' ' || key == ')'))
	{
		JFunction* arg1 = nullptr;
		JUserInputFunction* newUIF = nullptr;
		bool needRender;
		if (itsActiveUIF->Parse(key, &arg1, &newUIF, &needRender))
		{
			assert( newUIF == nullptr );
			ReplaceFunction(itsActiveUIF, arg1);
			itsActiveUIF = nullptr;
			if (isOperatorKey)
			{
				SaveStateForUndo();
				ApplyOperatorKey(key, arg1);
			}
			else
			{
				assert( key == '\n' || key == '\r' || key == ' ' || key == ')' );

				// select the new function

				Render();
				SelectFunction(arg1);
			}
		}
		else if (needRender)
		{
			JUserInputFunction* savedUIF = itsActiveUIF;
			itsActiveUIF = nullptr;
			Render();
			itsActiveUIF = savedUIF;
		}
	}

	else if (itsActiveUIF != nullptr && itsActiveUIF != itsFunction &&
			 itsActiveUIF->IsEmpty() && key == kJDeleteKey)
	{
		SaveStateForUndo();

		JFunction* fToSelect = DeleteFunction(itsActiveUIF);
		itsActiveUIF = nullptr;
		Render();
		SelectFunction(fToSelect);
	}

	else if (itsActiveUIF != nullptr)
	{
		if (itsUndoFunction == nullptr)
		{
			SaveStateForUndo();
		}

		bool needParse, needRender;
		const bool changed = itsActiveUIF->HandleKeyPress(key, &needParse, &needRender);

		JFunction* newF = nullptr;
		JUserInputFunction* newUIF = nullptr;
		if (needParse && itsActiveUIF->Parse(key, &newF, &newUIF, &needRender))
		{
			ReplaceFunction(itsActiveUIF, newF);
			itsActiveUIF = nullptr;
			Render();
			ActivateUIF(newUIF);
		}
		else if (needRender)
		{
			JUserInputFunction* savedUIF = itsActiveUIF;
			itsActiveUIF = nullptr;
			Render();
			itsActiveUIF = savedUIF;
		}
		else if (changed)
		{
			EIPRefresh();
		}
	}
}

/******************************************************************************
 ApplyOperatorKey (private)

	Given the operator key, modify the target function appropriately.

 ******************************************************************************/

void
JExprEditor::ApplyOperatorKey
	(
	const JUtf8Character&	key,
	JFunction*				targetF
	)
{
	const std::type_info& targetType = typeid(*targetF);

	JFunction* parentF = nullptr;
	const std::type_info* parentType = & typeid(JVariableValue);
	if (targetF != itsFunction)
	{
		const bool hasParent = targetF->GetParent(&parentF);
		assert( hasParent );

		parentType = & typeid(*parentF);
	}

	auto* newUIF = jnew JUserInputFunction(this);
	assert( newUIF != nullptr );

	JFunction* newArg = newUIF;
	if (key == '-')
	{
		newArg = jnew JNegation(newUIF);
		assert( newArg != nullptr );
	}

	JFunction* newF = nullptr;

	if (((key == '+' || key == '-') && targetType == typeid(JSummation)) ||
		(key == '*' && targetType == typeid(JProduct)) ||
		(key == '|' && targetType == typeid(JParallel)))
	{
		auto* naryOp = dynamic_cast<JNaryOperator*>(targetF);
		assert( naryOp != nullptr );
		naryOp->SetArg(naryOp->GetArgCount()+1, newArg);
	}
	else if (parentF != nullptr &&
			(((key == '+' || key == '-') && *parentType == typeid(JSummation)) ||
			 (key == '*' && *parentType == typeid(JProduct)) ||
			 (key == '|' && *parentType == typeid(JParallel))))
	{
		auto* naryOp = dynamic_cast<JNaryOperator*>(parentF);
		assert( naryOp != nullptr );
		JIndex selFIndex;
		const bool found = naryOp->FindArg(targetF, &selFIndex);
		assert( found );
		naryOp->InsertArg(selFIndex+1, newArg);
	}
	else if (key == '+' || key == '-' || key == '*' || key == '|')
	{
		JNaryOperator* newOp = nullptr;
		if (key == '+' || key == '-')
		{
			newOp = jnew JSummation;
		}
		else if (key == '*')
		{
			newOp = jnew JProduct;
		}
		else
		{
			assert( key == '|' );
			newOp = jnew JParallel;
		}
		assert( newOp != nullptr );
		newOp->SetArg(1, targetF->Copy());
		newOp->SetArg(2, newArg);
		newF = newOp;
	}

	else if (key == '/')
	{
		newF = jnew JDivision(targetF->Copy(), newArg);
		assert( newF != nullptr );
	}

	else if (key == '^')
	{
		newF = jnew JExponent(targetF->Copy(), newArg);
		assert( newF != nullptr );
	}

	else if (key == ',')
	{
		JNaryFunction* commaTargetF;
		JIndex newArgIndex;
		const bool found = GetCommaTarget(targetF, &commaTargetF, &newArgIndex);
		assert( found );
		commaTargetF->InsertArg(newArgIndex, newArg);
	}

	else
	{
		assert( 0 );	// this is an error and should never happen
	}

	if (newF != nullptr)
	{
		ReplaceFunction(targetF, newF);
	}
	Render();
	ActivateUIF(newUIF);
}

/******************************************************************************
 CanApplyCommaOperator (private)

	Returns true if it is possible to apply the comma operator to the
	current selection or active UIF.

 ******************************************************************************/

bool
JExprEditor::CanApplyCommaOperator()
{
	JFunction* startF;
	if (!GetSelectedFunction(&startF))
	{
		startF = itsActiveUIF;
	}

	JNaryFunction* targetF;
	JIndex newArgIndex;
	return startF != nullptr && GetCommaTarget(startF, &targetF, &newArgIndex);
}

/******************************************************************************
 GetCommaTarget (private)

	Search up the tree for a function to apply the comma operator to.
	We only apply commas to JNaryFunctions, not JNaryOperators.

 ******************************************************************************/

bool
JExprEditor::GetCommaTarget
	(
	JFunction*		startF,
	JNaryFunction**	targetF,
	JIndex*			newArgIndex
	)
{
	if (startF == itsFunction)
	{
		return false;
	}

	JFunction* currF = startF;
	JFunction* parentF = nullptr;
	while (currF != itsFunction)
	{
		const bool hasParent = currF->GetParent(&parentF);
		assert( hasParent );

		auto* naryParentF = dynamic_cast<JNaryFunction*>(parentF);
		if (naryParentF != nullptr && dynamic_cast<JNaryOperator*>(parentF) == nullptr)
		{
			*targetF             = naryParentF;
			const bool found = naryParentF->FindArg(currF, newArgIndex);
			assert( found );
			(*newArgIndex)++;	// insert after current arg
			return true;
		}
		currF = parentF;
	}

	// falling through means that we can't find an nary function

	return false;
}

/******************************************************************************
 ActivateUIF (private)

 ******************************************************************************/

void
JExprEditor::ActivateUIF
	(
	JUserInputFunction* uif
	)
{
	if (itsActiveFlag && uif != nullptr && uif != itsActiveUIF && EndEditing())
	{
		itsActiveUIF = uif;
		uif->Activate();
		itsSelection = 0;

		JIndex uifIndex;
		const bool found = itsRectList->FindFunction(uif, &uifIndex);
		assert( found );
		if (!EIPScrollToRect(itsRectList->GetRect(uifIndex)))
		{
			EIPRefresh();
		}
	}
}

/******************************************************************************
 ActivateNextUIF

 ******************************************************************************/

void
JExprEditor::ActivateNextUIF()
{
	JUserInputFunction* nextUIF = FindNextUIF(itsActiveUIF);
	if (nextUIF != itsActiveUIF)
	{
		ActivateUIF(nextUIF);
	}
	else if (itsActiveUIF != nullptr && !itsActiveUIF->IsEmpty())
	{
		EndEditing();
	}
	// if itsActiveUIF is empty, leave it active
}

/******************************************************************************
 DeleteFunction (private)

	Returns the sensible selection after deleting the target function.

 ******************************************************************************/

JFunction*
JExprEditor::DeleteFunction
	(
	JFunction* targetF
	)
{
	if (targetF == itsFunction)
	{
		PrivateClearFunction();
		return itsFunction;
	}

	JFunction* parentF;
	bool hasParent = targetF->GetParent(&parentF);
	assert( hasParent );

	auto* parentfwa = dynamic_cast<JFunctionWithArgs*>(parentF);
	if (parentfwa == nullptr || parentfwa->GetArgCount() == 1)
	{
		JFunction* currentF = parentF;
		while (true)
		{
			if (currentF == itsFunction)
			{
				PrivateClearFunction();
				return itsFunction;
			}
			JFunction* ancestorF;
			hasParent = currentF->GetParent(&ancestorF);
			assert( hasParent );

			parentfwa = dynamic_cast<JFunctionWithArgs*>(ancestorF);
			if (parentfwa != nullptr && parentfwa->GetArgCount() > 1)
			{
				targetF = currentF;
				parentF = ancestorF;
				break;
			}
			else
			{
				currentF = ancestorF;
			}
		}
	}

	JFunction* fToSelect = nullptr;

	const JSize parentArgCount = parentfwa->GetArgCount();
	if (parentArgCount == 2)
	{
		JFunction* arg1 = parentfwa->GetArg(1);
		JFunction* arg2 = parentfwa->GetArg(2);
		JFunction* savedF = nullptr;
		if (arg1 == targetF)
		{
			savedF = arg2->Copy();
		}
		else
		{
			assert( arg2 == targetF );
			savedF = arg1->Copy();
		}
		ReplaceFunction(parentF, savedF);
		fToSelect = savedF;
	}
	else if (parentArgCount > 2)
	{
		auto* naryF = dynamic_cast<JNaryFunction*>(parentF);
		assert( naryF != nullptr );
		const bool ok = naryF->DeleteArg(targetF);
		assert( ok );
		fToSelect = naryF;
	}

	return fToSelect;
}

/******************************************************************************
 ReplaceFunction (private)

 ******************************************************************************/

void
JExprEditor::ReplaceFunction
	(
	JFunction* origF,
	JFunction* newF
	)
{
	if (origF == itsFunction)
	{
		jdelete itsFunction;
		itsFunction = newF;
	}
	else
	{
		JFunction* parentF;
		const bool hasParent = origF->GetParent(&parentF);
		assert( hasParent );

		auto* fwa = dynamic_cast<JFunctionWithArgs*>(parentF);
		auto* fwv  = dynamic_cast<JFunctionWithVar*>(parentF);
		if (fwa != nullptr)
		{
			const bool ok = fwa->ReplaceArg(origF, newF);
			assert( ok );
		}
		else
		{
			assert( fwv != nullptr );
			assert( fwv->GetArrayIndex() == origF );
			fwv->SetArrayIndex(newF);
		}
	}
}

/******************************************************************************
 FindNextUIF (private)

 ******************************************************************************/

JUserInputFunction*
JExprEditor::FindNextUIF
	(
	JUserInputFunction* currUIF
	)
	const
{
	JIndex currentIndex = 0;
	if (currUIF != nullptr)
	{
		const bool found = itsRectList->FindFunction(currUIF, &currentIndex);
		assert( found );
	}

	const JSize rectCount = itsRectList->GetElementCount();

	for (JIndex i=currentIndex+1; i<=rectCount; i++)
	{
		const JFunction* f = itsRectList->GetFunction(i);
		const auto* uif = dynamic_cast<const JUserInputFunction*>(f);
		if (uif != nullptr)
		{
			return const_cast<JUserInputFunction*>(uif);
		}
	}

	for (JIndex i=1; i<=currentIndex; i++)
	{
		const JFunction* f = itsRectList->GetFunction(i);
		const auto* uif = dynamic_cast<const JUserInputFunction*>(f);
		if (uif != nullptr)
		{
			return const_cast<JUserInputFunction*>(uif);
		}
	}

	return nullptr;
}

/******************************************************************************
 Render (private)

 ******************************************************************************/

void
JExprEditor::Render()
{
	assert( itsFunction != nullptr );
	assert( itsActiveUIF == nullptr );		// forces caller to think about this issue

	itsSelection = 0;

	jdelete itsRectList;
	itsRectList = jnew JExprRectList;
	assert( itsRectList != nullptr );

	// calculate the locations of everything

	JPoint upperLeft(0,0);
	itsFunction->Layout(*this, upperLeft, GetInitialFontSize(), itsRectList);

	// tell whether or not we now need the tab key

	EIPAdjustNeedTab(ContainsUIF());

	// adjust bounds

	EIPBoundsChanged();

	// redraw the expression

	EIPRefresh();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JExprEditor::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == const_cast<JVariableList*>(itsVarList) &&
		message.Is(JVariableList::kVarNameChanged))
	{
		JIndex savedSel = itsSelection;
		JUserInputFunction* savedUIF = itsActiveUIF;
		itsActiveUIF = nullptr;
		Render();
		itsActiveUIF = savedUIF;
		SetSelection(savedSel);
	}
	else
	{
		JBroadcaster::Receive(sender, message);
	}
}

/******************************************************************************
 ExprRenderer routines

	Not inline because they are virtual

	We don't check for itsPainter!=nullptr because it should never happen and
	would waste a lot of time.

 ******************************************************************************/

/******************************************************************************
 Font size

 ******************************************************************************/

JSize
JExprEditor::GetInitialFontSize()
	const
{
	return JFontManager::GetDefaultFontSize();
}

JSize
JExprEditor::GetSuperSubFontSize
	(
	const JSize baseFontSize
	)
	const
{
	const JSize size = JFontManager::GetDefaultFontSize();
	if (baseFontSize == size)
	{
		return size-2;
	}
	else
	{
		return size-4;
	}
}

/******************************************************************************
 GetLineHeight

 ******************************************************************************/

JSize
JExprEditor::GetLineHeight
	(
	const JSize fontSize
	)
	const
{
	const JFont font = JFontManager::GetFont(JFontManager::GetDefaultFontName(), fontSize, itsDefaultStyle);
	return font.GetLineHeight(itsFontManager);
}

/******************************************************************************
 Strings

 ******************************************************************************/

JSize
JExprEditor::GetSpaceWidth
	(
	const JSize fontSize
	)
	const
{
	return GetStringWidth(fontSize, JString(" ", JString::kNoCopy));
}

JSize
JExprEditor::GetStringWidth
	(
	const JSize		fontSize,
	const JString&	str
	)
	const
{
	const JFont font = itsFontManager->GetFont(JFontManager::GetDefaultFontName(), fontSize, itsDefaultStyle);
	return font.GetStringWidth(itsFontManager, str);
}

void
JExprEditor::DrawString
	(
	const JCoordinate	left,
	const JCoordinate	midline,
	const JSize			fontSize,
	const JString&		str
	)
	const
{
	const JCoordinate h = GetLineHeight(fontSize);
	JCoordinate y = midline - h/2;
	itsPainter->SetFont(itsFontManager->GetFont(JFontManager::GetDefaultFontName(), fontSize, itsDefaultStyle));
	itsPainter->String(left,y, str);
}

/******************************************************************************
 Horizontal bar

 ******************************************************************************/

JSize
JExprEditor::GetHorizBarHeight()
	const
{
	return 3;
}

void
JExprEditor::DrawHorizBar
	(
	const JCoordinate	left,
	const JCoordinate	v,
	const JSize			length
	)
	const
{
	const JCoordinate y = v+1;
	itsPainter->Line(left,y, left+length-1,y);
}

/******************************************************************************
 Vertical bar

 ******************************************************************************/

JSize
JExprEditor::GetVertBarWidth()
	const
{
	return 3;
}

void
JExprEditor::DrawVertBar
	(
	const JCoordinate	h,
	const JCoordinate	top,
	const JSize			length
	)
	const
{
	const JCoordinate x = h+1;
	itsPainter->Line(x,top, x,top+length-1);
}

/******************************************************************************
 GetSuperscriptHeight

 ******************************************************************************/

JSize
JExprEditor::GetSuperscriptHeight
	(
	const JCoordinate baseHeight
	)
	const
{
	return JRound(baseHeight*2.0/3.0);
}

/******************************************************************************
 GetSubscriptDepth

 ******************************************************************************/

JSize
JExprEditor::GetSubscriptDepth
	(
	const JCoordinate baseHeight
	)
	const
{
	return JRound(baseHeight/2.0);
}

/******************************************************************************
 Parentheses

 ******************************************************************************/

const long kDegParenAngle = 30;
const JFloat kParenAngle  = kDegParenAngle * kJDegToRad;

JSize
JExprEditor::GetParenthesisWidth
	(
	const JCoordinate argHeight
	)
	const
{
	return 2+JRound(0.5 + (argHeight * (1.0 - cos(kParenAngle)))/(2.0 * sin(kParenAngle)));
}

void
JExprEditor::DrawParentheses
	(
	const JRect& argRect
	)
	const
{
	const JFloat r = argRect.height()/(2.0*sin(kParenAngle));
	const JCoordinate r1  = JRound(r);
	const JCoordinate dxc = JRound(r*cos(kParenAngle));
	const JCoordinate xcl = argRect.left  + dxc - 2;
	const JCoordinate xcr = argRect.right - dxc + 1;
	const JCoordinate yt  = argRect.ycenter() - r1;

	itsPainter->Arc(xcl-r1, yt, 2*r1, 2*r1, 180-kDegParenAngle,  2*kDegParenAngle);
	itsPainter->Arc(xcr-r1, yt, 2*r1, 2*r1,     kDegParenAngle, -2*kDegParenAngle);
}

/******************************************************************************
 Square brackets

 ******************************************************************************/

JSize
JExprEditor::GetSquareBracketWidth
	(
	const JCoordinate argHeight
	)
	const
{
	return 3+((argHeight-1)/10);
}

void
JExprEditor::DrawSquareBrackets
	(
	const JRect& argRect
	)
	const
{
	const JCoordinate h = argRect.height();
	const JCoordinate w = GetSquareBracketWidth(h)-2;

	itsPainter->SetPenLocation(argRect.left-2, argRect.top);
	itsPainter->DeltaLine(-w,0);
	itsPainter->DeltaLine( 0,h-1);
	itsPainter->DeltaLine( w,0);

	itsPainter->SetPenLocation(argRect.right+1, argRect.top);
	itsPainter->DeltaLine( w,0);
	itsPainter->DeltaLine( 0,h-1);
	itsPainter->DeltaLine(-w,0);
}

/******************************************************************************
 Square root

	tan(60) = sqrt(3)

 ******************************************************************************/

JSize
JExprEditor::GetSquareRootLeadingWidth
	(
	const JCoordinate argHeight
	)
	const
{
	return 1+JRound(2.0*argHeight/(4.0*sqrt(3.0)));
}

JSize
JExprEditor::GetSquareRootTrailingWidth
	(
	const JCoordinate argHeight
	)
	const
{
	return 3;
}

JSize
JExprEditor::GetSquareRootExtraHeight()
	const
{
	return 4;
}

void
JExprEditor::DrawSquareRoot
	(
	const JRect& enclosure
	)
	const
{
	const JCoordinate x = enclosure.left;
	const JCoordinate y = enclosure.top + JRound(3.0*enclosure.height()/4.0);
	const JCoordinate w = JRound((enclosure.height()-3)/(4.0*sqrt(3.0)));

	itsPainter->SetPenLocation(x,y);
	itsPainter->DeltaLine(w,enclosure.bottom-1-y);
	itsPainter->DeltaLine(w,-enclosure.height()+3);
	itsPainter->DeltaLine(enclosure.width()-2*w-1,0);
	itsPainter->DeltaLine(0,JRound(enclosure.height()/8.0));
}
