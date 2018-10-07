/******************************************************************************
 JExprEditor.h

	Interface for the JExprEditor class

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_JExprEditor
#define _H_JExprEditor

#include <JExprRenderer.h>
#include <JArray.h>
#include <JFontStyle.h>

class JString;
class JPainter;
class JEPSPrinter;
class JFontManager;
class JVariableList;
class JFunction;
class JNaryFunction;
class JFunctionWithArgs;
class JUserInputFunction;
class JExprNodeList;
class JExprRectList;
class JStyledText;

class JExprEditor : public JExprRenderer, virtual public JBroadcaster
{
public:

	enum CmdIndex
	{
		kSeparatorCmd = 1,	// always disabled, use if menu separator is separate item

		kUndoCmd,
		kCutCmd,
		kCopyCmd,
		kPasteCmd,
		kDeleteSelCmd,
		kSelectAllCmd,

		kPrintEPSCmd,

		kEvaluateSelCmd,
		kNegateSelCmd,
		kApplyFnToSelCmd,

		kAddArgCmd,
		kMoveArgLeftCmd,
		kMoveArgRightCmd,

		kGroupLeftCmd,
		kGroupRightCmd,
		kUngroupCmd,

		kSetNormalFontCmd,
		kSetGreekFontCmd,

		kCmdCount = kSetGreekFontCmd
	};

	enum PasteResult
	{
		kPasteOK,
		kParseError,
		kNowhereToPaste
	};

public:

	JExprEditor(const JVariableList* varList, JFontManager* fontManager);

	virtual ~JExprEditor();

	void					SetFunction(const JVariableList* varList, JFunction* f);
	void					ClearFunction();
	const JVariableList*	GetVariableList() const;
	const JFunction*		GetFunction() const;
	JBoolean				ContainsUIF() const;
	JBoolean				UIFIsActive() const;
	void					ActivateNextUIF();

	JBoolean	HasSelection() const;
	JBoolean	GetConstSelectedFunction(const JFunction** f) const;	
	void		ClearSelection();
	void		SelectFunction(const JFunction* f);

	JBoolean	EndEditing();
	void		CancelEditing();

	void		Undo();
	void		DeleteSelection();
	void		SelectAll();

	// our clipboard

	void	Cut();
	void	Copy();
	void	Paste();

	// other source

	JBoolean	Cut(JFunction** f);
	JBoolean	Copy(JFunction** f) const;
	PasteResult	Paste(const JString& expr);
	PasteResult	Paste(const JFunction& f);

	void		Print(JEPSPrinter& p);
	JRect		GetPrintBounds() const;
	void		DrawForPrint(JPainter& p, const JPoint& topLeft);

	JBoolean	EvaluateSelection(JFloat* value) const;
	void		NegateSelection();
	void		ApplyFunctionToSelection(const JString& fnName);
	void		AddArgument();
	void		MoveArgument(const JInteger delta);
	void		GroupArguments(const JInteger delta);
	void		UngroupArguments();

	void	SetNormalFont();
	void	SetGreekFont();

// JExprRenderer routines

	virtual JSize	GetInitialFontSize() const override;
	virtual JSize	GetSuperSubFontSize(const JSize baseFontSize) const override;
	virtual JSize	GetSpaceWidth(const JSize fontSize) const override;
	virtual JSize	GetStringWidth(const JSize fontSize, const JString& str) const override;
	virtual JSize	GetLineHeight(const JSize fontSize) const override;
	virtual void	DrawString(const JCoordinate left, const JCoordinate midline,
							   const JSize fontSize, const JString& str) const override;

	virtual JSize	GetHorizBarHeight() const override;
	virtual void	DrawHorizBar(const JCoordinate left, const JCoordinate v,
								 const JSize length) const override;

	virtual JSize	GetVertBarWidth() const override;
	virtual void	DrawVertBar(const JCoordinate h, const JCoordinate top,
								const JSize length) const override;

	virtual JSize	GetSuperscriptHeight(const JCoordinate baseHeight) const override;
	virtual JSize	GetSubscriptDepth(const JCoordinate baseHeight) const override;

	virtual JSize	GetParenthesisWidth(const JCoordinate argHeight) const override;
	virtual void	DrawParentheses(const JRect& argRect) const override;

	virtual JSize	GetSquareBracketWidth(const JCoordinate argHeight) const override;
	virtual void	DrawSquareBrackets(const JRect& argRect) const override;

	virtual JSize	GetSquareRootLeadingWidth(const JCoordinate argHeight) const override;
	virtual JSize	GetSquareRootTrailingWidth(const JCoordinate argHeight) const override;
	virtual JSize	GetSquareRootExtraHeight() const override;
	virtual void	DrawSquareRoot(const JRect& enclosure) const override;

	// for JUserInputFunction

	JFontManager*	GetFontManager() const;
	JPainter*		GetPainter() const;

	JBoolean	ApplyFunction(const JString& fnName, const JFunction& origF,
							  JFunction** newF, JFunction** newArg,
							  JUserInputFunction** newUIF);

	virtual JStyledText*	BuildStyledText();

protected:

	void	EIPActivate();
	void	EIPDeactivate();

	const JExprRectList*	GetRectList() const;	// ideally, we wouldn't need this
	JArray<JBoolean>		GetCmdStatus(JString* evalStr) const;

	JBoolean	GetSelection(JIndex* selection) const;
	JBoolean	GetSelectionRect(JRect* selRect) const;
	JBoolean	GetSelectedFunction(JFunction** f);
	void		SetSelection(const JIndex index);

	void		EIPDraw(JPainter& p);
	void		EIPHandleMouseDown(const JPoint& currPt,
								   const JBoolean extend);
	void		EIPHandleMouseDrag(const JPoint& currPt);
	void		EIPHandleMouseUp();
	JBoolean	MouseOnActiveUIF(const JPoint& pt) const;
	void		EIPHandleKeyPress(const JUtf8Character& key);

	virtual void		EIPRefresh() = 0;
	virtual void		EIPRedraw() = 0;
	virtual void		EIPBoundsChanged() = 0;
	virtual JBoolean	EIPScrollToRect(const JRect& r) = 0;
	virtual JBoolean	EIPScrollForDrag(const JPoint& pt) = 0;
	virtual void		EIPAdjustNeedTab(const JBoolean needTab) = 0;
	virtual void		EIPExprChanged();				// must call inherited
	virtual void		EIPClipboardChanged() = 0;
	virtual JBoolean	EIPOwnsClipboard() = 0;
	virtual JBoolean	EIPGetExternalClipboard(JString* text) = 0;

	JBoolean	GetClipboard(const JFunction** f) const;

	virtual JBoolean	CanDisplaySelectionValue() const;
	virtual void		DisplaySelectionValue() const;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	enum DragType
	{
		kInvalidDrag,
		kSelectExpr,
		kSendToUIF
	};

private:

	const JVariableList*	itsVarList;
	JFunction*				itsFunction;
	JExprRectList*			itsRectList;
	JIndex					itsSelection;
	JBoolean				itsActiveFlag;
	JUserInputFunction*		itsActiveUIF;	// can be nullptr; owned by itsFunction

	JFunction*	itsFunctionClip;

	JFontManager*	itsFontManager;

	// used for undo

	JFunction*	itsUndoFunction;
	JIndex		itsUndoSelection;

	// used while drawing

	JPainter*			itsPainter;
	const JColorID	itsTextColor;
	const JColorID	itsSelectionColor;
	const JFontStyle	itsDefaultStyle;

	// used during drag

	DragType			itsDragType;
	JPoint				itsStartPt;
	const JFunction*	itsPrevSelectedFunction;	// this is owned by itsFunction

private:

	void	SaveStateForUndo();

	void		ApplyOperatorKey(const JUtf8Character& key, JFunction* targetF);
	JBoolean	CanApplyCommaOperator();
	JBoolean	GetCommaTarget(JFunction* startF, JNaryFunction** targetF,
							   JIndex* newArgIndex);

	void	ActivateUIF(JUserInputFunction* uif);

	void		Render();
	JFunction*	DeleteFunction(JFunction* f);
	void		ReplaceFunction(JFunction* origF, JFunction* newF);

	JUserInputFunction*	PrivateClearFunction();
	JUserInputFunction*	FindNextUIF(JUserInputFunction* currUIF) const;

	JBoolean	GetNegAdjSelFunction(JFunction** selF, JFunction** parentF) const;
	JFunction*	Negate(const JFunction& f) const;

	// not allowed

	JExprEditor(const JExprEditor& source);
	const JExprEditor& operator=(const JExprEditor& source);

public:

	// JBroadcaster messages

	static const JUtf8Byte* kExprChanged;

	class ExprChanged : public JBroadcaster::Message
	{
	public:

		ExprChanged()
			:
			JBroadcaster::Message(kExprChanged)
			{ };
	};
};


/******************************************************************************
 EIPActivate (protected)

 ******************************************************************************/

inline void
JExprEditor::EIPActivate()
{
	itsActiveFlag = kJTrue;
	ActivateNextUIF();
}

/******************************************************************************
 GetVariableList

 ******************************************************************************/

inline const JVariableList*
JExprEditor::GetVariableList()
	const
{
	return itsVarList;
}

/******************************************************************************
 GetFunction

 ******************************************************************************/

inline const JFunction*
JExprEditor::GetFunction()
	const
{
	return itsFunction;
}

/******************************************************************************
 GetFontManager

 ******************************************************************************/

inline JFontManager*
JExprEditor::GetFontManager()
	const
{
	return itsFontManager;
}

/******************************************************************************
 GetPainter

 ******************************************************************************/

inline JPainter*
JExprEditor::GetPainter()
	const
{
	return itsPainter;
}

/******************************************************************************
 GetRectList (protected)

 ******************************************************************************/

inline const JExprRectList*
JExprEditor::GetRectList()
	const
{
	return itsRectList;
}

/******************************************************************************
 GetClipboard (protected)

	Returns kJTrue if there is anything on our clipboard.

 ******************************************************************************/

inline JBoolean
JExprEditor::GetClipboard
	(
	const JFunction** f
	)
	const
{
	*f = itsFunctionClip;
	return JConvertToBoolean( itsFunctionClip != nullptr );
}

/******************************************************************************
 UIFIsActive

 ******************************************************************************/

inline JBoolean
JExprEditor::UIFIsActive()
	const
{
	return JI2B( itsActiveUIF != nullptr );
}

#endif
