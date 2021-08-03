/******************************************************************************
 CBCRMRuleTable.h

	Interface for the CBCRMRuleTable class

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBCRMRuleTable
#define _H_CBCRMRuleTable

#include <JXStringTable.h>
#include <JStyledText.h>		// need definition of CRMRuleList

class JXTextButton;
class JXColHeaderWidget;
class CBEditCRMDialog;
class CBListCSF;

class CBCRMRuleTable : public JXStringTable
{
public:

	enum
	{
		kFirstColumn   = 1,
		kRestColumn    = 2,
		kReplaceColumn = 3
	};

public:

	CBCRMRuleTable(CBEditCRMDialog* dialog,
				   JXTextButton* addRowButton, JXTextButton* removeRowButton,
				   JXTextButton* loadButton, JXTextButton* saveButton,
				   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	virtual ~CBCRMRuleTable();

	virtual void	Activate() override;
	virtual void	Deactivate() override;

	void	GetData(JStyledText::CRMRuleList* list) const;
	void	SetData(const JStyledText::CRMRuleList& list);
	void	ClearData();

	void	ReadGeometry(std::istream& input);
	void	WriteGeometry(std::ostream& output) const;
	void	SetColTitles(JXColHeaderWidget* widget) const;

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual JXInputField*
		CreateStringTableInput(const JPoint& cell, JXContainer* enclosure,
							   const HSizingOption hSizing, const VSizingOption vSizing,
							   const JCoordinate x, const JCoordinate y,
							   const JCoordinate w, const JCoordinate h) override;

	virtual void	PrepareDeleteXInputField() override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBEditCRMDialog*	itsDialog;
	JXTextButton*		itsAddRowButton;
	JXTextButton*		itsRemoveRowButton;

	CBListCSF*		itsCSF;
	JXTextButton*	itsLoadButton;
	JXTextButton*	itsSaveButton;

	// for testing input fields

	JRegex*			itsFirstRegex;
	JRegex*			itsRestRegex;
	JInterpolate*	itsReplaceInterpolator;

private:

	void	AddRow();
	void	RemoveRow();

	void	LoadRules();
	void	SaveRules() const;

	void	ReadData(const JString& fileName, const bool replace);
	void	WriteData(const JString& fileName) const;

	// not allowed

	CBCRMRuleTable(const CBCRMRuleTable& source);
	const CBCRMRuleTable& operator=(const CBCRMRuleTable& source);
};

#endif
