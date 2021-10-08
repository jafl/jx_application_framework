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

	~ExprEditor();

	void	Activate();
	void	CheckCmdStatus(const JArray<bool>& expected) const;

	void	HandleKeyPress(const JUtf8Character& c);

	void	HandleMouseDown(const JPoint& pt, const bool extend);
	void	HandleMouseDrag(const JPoint& pt);
	void	HandleMouseUp(const JPoint& pt);

protected:

	void	EIPRefresh() override;
	void	EIPRedraw() override;
	void	EIPBoundsChanged() override;
	bool	EIPScrollToRect(const JRect& r) override;
	bool	EIPScrollForDrag(const JPoint& pt) override;
	void	EIPAdjustNeedTab(const bool needTab) override;

	void	EIPClipboardChanged() override;
	bool	EIPOwnsClipboard() override;
	bool	EIPGetExternalClipboard(JString* text) override;
};

#endif
