/******************************************************************************
 CBSearchTE.h

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBSearchTE
#define _H_CBSearchTE

#include <JTextEditor.h>
#include "CBSearchDocument.h"	// need definition of RecordLink

class CBSearchTE : public JTextEditor
{
public:

	static const JUtf8Byte kNewMatchLine;
	static const JUtf8Byte kRepeatMatchLine;
	static const JUtf8Byte kIncrementProgress;
	static const JUtf8Byte kError;

public:

	CBSearchTE();

	virtual ~CBSearchTE();

	void	SearchFiles(const JPtrArray<JString>& fileList,
						const JPtrArray<JString>& nameList,
						const JBoolean onlyListFiles,
						const JBoolean listFilesWithoutMatch,
						std::ostream& output);

	JBoolean	ReplaceAllForward();

	static void	SetProtocol(CBSearchDocument::RecordLink* link);

	virtual JBoolean	TEHasSearchText() const override;

protected:

	virtual void		TERefresh() override;
	virtual void		TERefreshRect(const JRect& rect) override;
	virtual void		TERedraw() override;
	virtual void		TESetGUIBounds(const JCoordinate w, const JCoordinate h,
									   const JCoordinate changeY) override;
	virtual JBoolean	TEWidthIsBeyondDisplayCapacity(const JSize width) const override;
	virtual JBoolean	TEScrollToRect(const JRect& rect,
									   const JBoolean centerInDisplay) override;
	virtual JBoolean	TEScrollForDrag(const JPoint& pt) override;
	virtual JBoolean	TEScrollForDND(const JPoint& pt) override;
	virtual void		TESetVertScrollStep(const JCoordinate vStep) override;
	virtual void		TEUpdateClipboard(const JString& text, const JRunArray<JFont>& style) const override;
	virtual JBoolean	TEGetClipboard(JString* text, JRunArray<JFont>* style) const override;
	virtual JBoolean	TEBeginDND() override;
	virtual void		TEPasteDropData() override;
	virtual void		TECaretShouldBlink(const JBoolean blink) override;

private:

	void	SearchFile(const JString& fileName, const JString& printName,
					   const JBoolean onlyListFiles, const JBoolean listFilesWithoutMatch,
					   std::ostream& output,
					   const JRegex& searchRegex, const JBoolean	entireWord);

	JBoolean	IsKnownBinaryFile(const JString& fileName) const;

	// not allowed

	CBSearchTE(const CBSearchTE& source);
	const CBSearchTE& operator=(const CBSearchTE& source);
};

#endif
