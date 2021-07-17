/******************************************************************************
 CBDiffEditor.h

	Copyright © 2009 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBDiffEditor
#define _H_CBDiffEditor

#include "CBTextEditor.h"

class CBDiffDocument;

class CBDiffEditor : public CBTextEditor
{
public:

	CBDiffEditor(CBTextDocument* document, const JString& fileName,
				 JXMenuBar* menuBar, CBTELineIndexInput* lineInput,
				 CBTEColIndexInput* colInput,
				 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~CBDiffEditor();

	void	ReadDiff(std::istream& input,
					 const JFontStyle& removeStyle, const JFontStyle& insertStyle);

	void	ShowFirstDiff();
	void	ShowPrevDiff();
	void	ShowNextDiff();

private:

	CBDiffDocument*	itsDiffDoc;
	JFontStyle		itsRemoveStyle;
	JFontStyle		itsInsertStyle;

private:

	JUtf8Byte	ReadCmd(std::istream& input, JIndexRange* origRange, JIndexRange* newRange) const;
	JIndexRange	ReadRange(std::istream& input) const;
	void		IgnoreOrigText(std::istream& input, const JUtf8Byte cmd) const;
	JString		ReadNewText(std::istream& input, const JUtf8Byte cmd, JSize* lineCount) const;

	void	SelectDiff(const JCharacterRange& removeRange, const JCharacterRange& insertRange,
					   const bool preferRemove, const bool hadSelection,
					   const JStyledText::TextIndex& origIndex,
					   const JStyledText::TextRange& origRange);

	// not allowed

	CBDiffEditor(const CBDiffEditor& source);
	const CBDiffEditor& operator=(const CBDiffEditor& source);
};

#endif
