/******************************************************************************
 CMSourceText.h

	Interface for the CMSourceText class

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_CMSourceText
#define _H_CMSourceText

#include "CMTextDisplayBase.h"
#include "CBTextFileType.h"
#include <JXStyledText.h>
#include <JSTStyler.h>

class CMSourceDirector;
class CMCommandDirector;
class CBStylerBase;

class CMSourceText : public CMTextDisplayBase
{
public:

	CMSourceText(CMSourceDirector* srcDir, CMCommandDirector* cmdDir,
					JXMenuBar* menuBar,
					JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					const HSizingOption hSizing, const VSizingOption vSizing,
					const JCoordinate x, const JCoordinate y,
					const JCoordinate w, const JCoordinate h);

	virtual ~CMSourceText();

	void	SetFileType(const CBTextFileType type);

	virtual void	HandleKeyPress(const JUtf8Character& c, const int keySym,
								   const JXKeyModifiers& modifiers) override;

protected:

	virtual void	BoundsResized(const JCoordinate dw, const JCoordinate dh) override;
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

protected:

	class StyledText : public JXStyledText
	{
		public:

		StyledText(CBStylerBase* styler, JFontManager* fontManager)
			:
			JXStyledText(kJFalse, kJFalse, fontManager),
			itsStyler(styler),
			itsTokenStartList(JSTStyler::NewTokenStartList())
		{ };

		virtual ~StyledText();

		protected:

		virtual void	AdjustStylesBeforeBroadcast(const JString& text,
													JRunArray<JFont>* styles,
													TextRange* recalcRange,
													TextRange* redrawRange,
													const JBoolean deletion);

		private:

		CBStylerBase*					itsStyler;
		JArray<JSTStyler::TokenData>*	itsTokenStartList;	// nullptr if styling is turned off
	};

private:

	CMSourceDirector*	itsSrcDir;
	CMCommandDirector*	itsCmdDir;
	CBStylerBase*		itsStyler;
	JIndex				itsFirstSearchMenuItem;	// index of first item added to Search menu
	JSize				itsLastClickCount;

private:

	void		UpdateCustomSearchMenuItems();
	JBoolean	HandleCustomSearchMenuItems(const JIndex index);

	// not allowed

	CMSourceText(const CMSourceText& source);
	const CMSourceText& operator=(const CMSourceText& source);

};

#endif
