/******************************************************************************
 TestExprEditor.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestExprEditor
#define _H_TestExprEditor

#include <JExprEditor.h>

class TestExprEditor : public JExprEditor
{
public:

	TestExprEditor(JFontManager* fontManager, const JVariableList* varList);

	virtual ~TestExprEditor();

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

	TestExprEditor(const TestExprEditor& source);
	const TestExprEditor& operator=(const TestExprEditor& source);
};

#endif
