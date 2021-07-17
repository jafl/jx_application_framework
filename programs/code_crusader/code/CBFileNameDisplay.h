/******************************************************************************
 CBFileNameDisplay.h

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBFileNameDisplay
#define _H_CBFileNameDisplay

#include <JXFileInput.h>

class CBTextDocument;
class CBFileDragSource;

class CBFileNameDisplay : public JXFileInput
{
public:

	CBFileNameDisplay(CBTextDocument* doc, CBFileDragSource* dragSource,
					  JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~CBFileNameDisplay();

	void	SetTE(JXTEBase* te);
	void	DiskCopyIsModified(const bool mod);

	virtual void		HandleKeyPress(const JUtf8Character& c, const int keySym,
									   const JXKeyModifiers& modifiers) override;
	virtual bool	InputValid() override;

protected:

	class StyledText : public JXFileInput::StyledText
	{
		public:

		StyledText(CBFileNameDisplay* field, JFontManager* fontManager)
			:
			JXFileInput::StyledText(field, fontManager),
			itsCBField(field)
		{ };

		protected:

		virtual void	AdjustStylesBeforeBroadcast(
							const JString& text, JRunArray<JFont>* styles,
							JStyledText::TextRange* recalcRange,
							JStyledText::TextRange* redrawRange,
							const bool deletion) override;

		private:

		CBFileNameDisplay*	itsCBField;
	};

protected:

	virtual void	HandleFocusEvent() override;
	virtual void	HandleUnfocusEvent() override;

private:

	enum Action
	{
		kCancel,
		kSaveAs,
		kRename
	};

private:

	JXTEBase*	itsTE;					// not owned
	JString		itsOrigFile;
	Action		itsUnfocusAction;

	CBTextDocument*		itsDoc;			// not owned
	CBFileDragSource*	itsDragSource;	// not owned
	bool			itsDiskModFlag;
	bool			itsCBHasFocusFlag;

private:

	void	UpdateDisplay(const bool hasFocus);

	// not allowed

	CBFileNameDisplay(const CBFileNameDisplay& source);
	const CBFileNameDisplay& operator=(const CBFileNameDisplay& source);
};

#endif
