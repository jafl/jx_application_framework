/******************************************************************************
 JExprLibVersion.h

	Defines current version of JExpr library

	Copyright (C) 1996-2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_JExprLibVersion
#define _H_JExprLibVersion

// This has to be a #define so it can be used for conditional compilation.

#define CURRENT_JEXPR_MAJOR_VERSION	2
#define CURRENT_JEXPR_MINOR_VERSION	0
#define CURRENT_JEXPR_PATCH_VERSION	0

// This is mainly provided so programmers can see the official version number.

static const char* kCurrentJExprLibVersionStr = "2.0.0";

// version 2.0.0:
//	Supports utf-8
//	*** Removed JDecision and all related classes.
//	*** Replaced recursive descent parser with flex/bison parser.
//	*** Replaced CastTo*() with dynamic_cast.

// version 1.1.9:
//	Renamed from JParser to JExpr.
//	JProduct:
//		Draws dot between terms.  This increases the readability, especially
//			when multiplying raw numbers.
//	JExprRenderer:
//		Added GetMultiplicationString().
//		Fixed bug so right parens are drawn correctly when printing.
//	JExprEditor:
//		Added GetPrintBounds() and DrawForPrint() so you can print an
//			expression as part of a page layout.
//	JXExprEditor:
//		Defined GetMultiplicationString() to return a vertically centered dot.
//		Removed menuStyle argument from constructor since it is now a
//			global value stored by JXMenu.
//	JXExprEditorSet:
//		Removed menuStyle argument from constructor since it is now a
//			global value stored by JXMenu.
//	JXExprInput:
//		Removed menuStyle argument from CreateFontMenu() since it is now a
//			global value stored by JXMenu.

// version 1.1.5:
//	JExprEditor:
//		Added CancelEditing().
//		Fixed SelectFunction() so it works if itsFunction is a single, active UIF.
//		Fixed ActivateUIF() so it works if the specified UIF is already active.

// version 1.1.3:
//	Fixed bug in JUserInputFunction so E can be entered as a hexadecimal digit.

// version 1.1.2:
//	Updated to work with JX 1.1.20.

// version 1.1.1:
//	*** All trig functions take radians instead of degrees.
//	Changed all global constant names to begin with kJ.
//	JXExprEvalDirector:
//		Positions itself near the bottom left corner of the window that creates it.
//	JXExprEditor:
//		Changed Meta-E shortcut to Meta-=.
//		Promoted EvaluateSelection() to public virtual so derived classes can
//			call it and override it.
//	JUserInputFunction:
//		Added Clear().
//	JExprEditor:
//		EIPDeactivate() no longer returns JBoolean.
//		Escape clears the active JUserInputFunction.
//		NegateSelection(), ApplyFunctionToSelection(), MoveArgument(),
//			Group/UngroupArguments() now work on active UIF.
//		Group/UngroupArguments() now works on w+x-(y+z).
//		Added access to the Edit menu.  It now uses actions so the menu
//			can be shared with other types of widgets.
//	Renamed JXVarNameInput to JXExprInput.
//		Added ability to create and use Font menu.
//	JVariableList:
//		Added support for moving and removing variables.  Derived classes that
//			use this functionality must support the new JBroadcaster messages.
//		Added VariableUserCreated() and VariableUserDeleted().  These must be
//			called in the constructors and destructors respectively of all
//			JFunctions and JDecisions that store indicies into the JVariableList.
//	JFunction, JDecision:
//		Added virtual functions UsesVariable(), etc. that must be overridden
//			by all derived classes that store indicies into the JVariableList.
//	Renamed:
//		JArcsin  -> JArcSine
//		JArccos  -> JArcCosine
//		JArctan  -> JArcTangent
//		JArctan2 -> JArcTangent2
//	JNaryFunction:
//		Added PrependArg() and ArgIndexValid().  (what took me so long?)
//	JArcTangent2:
//		Implemented Evaluate(JComplex*).

// version 1.1.0:
//	Initial release.  Minor version of 1 indicates that the API is not
//		guaranteed to be stable.
//	JXExprEditor:
//		Added shortcut information to "Apply function" submenu.

// version 1.0.3:
//	JVariableList:
//		Added pure virtual SetNumericValue() and SetDiscreteValue().

// version 1.0.2:
//	All functions that used to take "const JColormap*" now take "JColormap*".

// version 1.0.1:
//	Colors are no longer available as global constants.
//		Classes derived from JExprEditor must provide a JColormap object.
//		*** Windows containing JXExprEditors must use the display's colormap.
//	Moved global variable gGetCurrFontMgr behind function in jGlobals.h
//	All string constants and other parser data in jParserData.cc is now
//		behind a function interface defined in jParserData.h

// version 1.0.0:
//	Renamed all utility modules to standardize capitalization:
//		jParseDecision, jParseFunction, jParserData, jParseUtil, jExprUIUtil
//	Parser now recognizes i,j,I as sqrt(-1).
//		*** You should increment all file version numbers so old versions of
//			programs will not attempt to parse these new values.
//	JVariableList:
//		Added pure virtual function GetNumericValue(..., JComplex*).
//	JFunction:
//		Added Evaluate(JComplex*).
//	Unable to implement Evaluate(JComplex*) for JArctan2
//	JSine, JCosine, JTangent:
//		For real numbers, argument is in degrees.
//		For complex numbers, argument is in radians?
//	JMinFunc,JMaxFunc,JSign:
//		Since one cannot compare complex numbers, these use the real parts.
//	JDecision calls Evaluate(JFloat*) for <,<=,>,>=.
//	Renamed classes:
//		JSign     -> JAlgSign
//		JRound    -> JRoundToInt
//		JTruncate -> JTruncateToInt
//	New classes:
//		JHypSine, JHypCosine, JHypTangent,
//		JRealPart, JImagPart, JPhaseAngle, JConjugate, JRotateComplex
//	JExprEditor:
//		Created Print(JEPSPrinter&) and added kPrintEPSCmd to CmdIndex.
//		Changed prototype for GetCmdStatus().
//		When it gets focus, it activates the first input field automatically.
//	JXExprEditor:
//		Added "Print to EPS..." to Math menu.
//		It is now ok to unfocus an expression that has empty UIF's in it.

//	(Sorry, I started this after making a lot of changes, so it's not complete.)
//	Renamed JExprInputPane -> JExprEditor, JXExprWidget -> JXExprEditor,
//		JXExprSet -> JXExprEditorSet.
//	JExprEditor handles all JExprRenderer functions via JPainter object.
//		Derived class provides JColorIndex values.
//	JExprEditor stores local clipboard information and asks derived class
//		to get global clipboard information, if different.
//	JExprEditor allows greek characters.  Single back quote (`) indicates
//		that next character is greek.  (e.g. `a -> alpha)
//	JUserInputFunction derives from JTextEditor to support greek characters.
//		New JExprEditor menu lets user switch font while typing.
//	Added JExprEditor::CmdIndex::kSeparatorCmd to be used if
//		GUI menus consider a separator to be a separate menu item.
//	JFontManager is no longer a global object.
//		Classes derived from JExprEditor must provide a JFontManager object.
//		Programs must provide the global variable JGetCurrFontMgr* gGetCurrFontMgr.
//	JExprEditor::Paste() now returns JExprEditor::PasteResult.

#endif
