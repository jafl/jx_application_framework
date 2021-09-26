/******************************************************************************
 ExprEditor.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_ExprEditor
#define _H_ExprEditor

#include <jx-af/jexpr/JExprEditor.h>

class ExprEditor : public JExprEditor
{
public:

	ExprEditor(const JVariableList* varList, JFontManager* fontManager);

	virtual ~ExprEditor();

	void	Activate();
	void	CheckCmdStatus(const JArray<bool>& expected) const;

	void	HandleKeyPress(const JUtf8Character& c);

	void	HandleMouseDown(const JPoint& pt, const bool extend);
	void	HandleMouseDrag(const JPoint& pt);
	void	HandleMouseUp(const JPoint& pt);

protected:

	virtual void	EIPRefresh() override;
	virtual void	EIPRedraw() override;
	virtual void	EIPBoundsChanged() override;
	virtual bool	EIPScrollToRect(const JRect& r) override;
	virtual bool	EIPScrollForDrag(const JPoint& pt) override;
	virtual void	EIPAdjustNeedTab(const bool needTab) override;

	virtual void	EIPClipboardChanged() override;
	virtual bool	EIPOwnsClipboard() override;
	virtual bool	EIPGetExternalClipboard(JString* text) override;
};

#endif
