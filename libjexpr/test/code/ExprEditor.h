/******************************************************************************
 ExprEditor.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_ExprEditor
#define _H_ExprEditor

#include <JExprEditor.h>
#include <JRunArray.h>

class ExprEditor : public JExprEditor
{
public:

	ExprEditor(const JVariableList* varList, JFontManager* fontManager);

	virtual ~ExprEditor();

	void	Activate();
	void	CheckCmdStatus(const JRunArray<JBoolean>& expected) const;

	void	HandleKeyPress(const JUtf8Character& c);

	void	HandleMouseDown(const JPoint& pt, const JBoolean extend);
	void	HandleMouseDrag(const JPoint& pt);
	void	HandleMouseUp(const JPoint& pt);

protected:

	virtual void		EIPRefresh() override;
	virtual void		EIPRedraw() override;
	virtual void		EIPBoundsChanged() override;
	virtual JBoolean	EIPScrollToRect(const JRect& r) override;
	virtual JBoolean	EIPScrollForDrag(const JPoint& pt) override;
	virtual void		EIPAdjustNeedTab(const JBoolean needTab) override;

	virtual void		EIPClipboardChanged() override;
	virtual JBoolean	EIPOwnsClipboard() override;
	virtual JBoolean	EIPGetExternalClipboard(JString* text) override;

private:

	// not allowed

	ExprEditor(const ExprEditor& source);
	const ExprEditor& operator=(const ExprEditor& source);
};

#endif
