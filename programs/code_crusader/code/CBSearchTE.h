/******************************************************************************
 CBSearchTE.h

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBSearchTE
#define _H_CBSearchTE

#include <JTextEditor.h>
#include "CBSearchDocument.h"	// need definition of RecordLink

class JFontManager;
class JColormap;

class CBSearchTE : public JTextEditor
{
public:

	static const JCharacter kNewMatchLine;
	static const JCharacter kRepeatMatchLine;
	static const JCharacter kIncrementProgress;
	static const JCharacter kError;

public:

	CBSearchTE();

	virtual ~CBSearchTE();

	void	SearchFiles(const JPtrArray<JString>& fileList,
						const JPtrArray<JString>& nameList,
						const JBoolean onlyListFiles,
						const JBoolean listFilesWithoutMatch,
						ostream& output);

	static void	SetProtocol(CBSearchDocument::RecordLink* link);

	virtual JBoolean	TEHasSearchText() const;

protected:

	virtual void		TERefresh();
	virtual void		TERefreshRect(const JRect& rect);
	virtual void		TERedraw();
	virtual void		TESetGUIBounds(const JCoordinate w, const JCoordinate h,
									   const JCoordinate changeY);
	virtual JBoolean	TEWidthIsBeyondDisplayCapacity(const JSize width) const;
	virtual JBoolean	TEScrollToRect(const JRect& rect,
									   const JBoolean centerInDisplay);
	virtual JBoolean	TEScrollForDrag(const JPoint& pt);
	virtual JBoolean	TEScrollForDND(const JPoint& pt);
	virtual void		TESetVertScrollStep(const JCoordinate vStep);
	virtual void		TEClipboardChanged();
	virtual JBoolean	TEOwnsClipboard() const;
	virtual JBoolean	TEGetExternalClipboard(JString* text, JRunArray<JFont>* style) const;
	virtual void		TEDisplayBusyCursor() const;
	virtual JBoolean	TEBeginDND();
	virtual void		TEPasteDropData();
	virtual void		TECaretShouldBlink(const JBoolean blink);

private:

	void	SearchFile(const JString& fileName, const JString& printName,
					   const JBoolean onlyListFiles, const JBoolean listFilesWithoutMatch,
					   ostream& output,
					   const JString& searchStr, const JBoolean isRegex,
					   const JBoolean caseSensitive, const JBoolean entireWord,
					   const JRegex& regex);

	JBoolean	IsKnownBinaryFile(const JString& fileName) const;

	// not allowed

	CBSearchTE(const CBSearchTE& source);
	const CBSearchTE& operator=(const CBSearchTE& source);
};

#endif
