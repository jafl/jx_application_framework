/******************************************************************************
 CBSearchDocument.h

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBSearchDocument
#define _H_CBSearchDocument

#include "CBExecOutputDocument.h"

class JXTextMenu;
class JXProgressIndicator;
class CBSearchTE;

class CBSearchDocument : public CBExecOutputDocument
{
public:

	static JError	Create(const JPtrArray<JString>& fileList,
						   const JPtrArray<JString>& nameList,
						   const JRegex& searchRegex,
						   const JBoolean onlyListFiles,
						   const JBoolean listFilesWithoutMatch);

	static JError	Create(const JPtrArray<JString>& fileList,
						   const JPtrArray<JString>& nameList,
						   const JRegex& searchRegex,
						   const JString& replaceStr);

	virtual ~CBSearchDocument();

	virtual void	OpenPrevListItem() override;
	virtual void	OpenNextListItem() override;

	virtual void	ConvertSelectionToFullPath(JString* fileName) const override;

protected:

	CBSearchDocument(const JBoolean isReplace, const JBoolean onlyListFiles,
					 const JSize fileCount,
					 JProcess* p, const int fd,
					 const JString& windowTitle);

	virtual void		PlaceCmdLineWidgets() override;
	virtual void		AppendText(const JString& text) override;
	virtual JBoolean	ProcessFinished(const JProcess::Finished& info) override;
	virtual JBoolean	NeedsFormattedData() const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const JBoolean			itsIsReplaceFlag;
	const JBoolean			itsOnlyListFilesFlag;
	JBoolean				itsFoundFlag;			// kJTrue => something matched
	JStyledText::TextIndex	itsPrevQuoteIndex;		// start of previous quote from file

	JXTextMenu*				itsMatchMenu;
	JXProgressIndicator*	itsIndicator;	// nullptr after ProcessFinished()
	CBSearchTE*				itsReplaceTE;	// nullptr unless replacing

private:

	void	ReplaceAll(const JString& fileName);

	void	UpdateMatchMenu();
	void	HandleMatchMenu(const JIndex index);

	void		ShowFirstMatch();
	JBoolean	ShowPrevMatch();
	JBoolean	ShowNextMatch();

	JFontStyle	GetFileNameStyle() const;
	JFontStyle	GetMatchStyle() const;
	JFontStyle	GetErrorStyle() const;

	// not allowed

	CBSearchDocument(const CBSearchDocument& source);
	const CBSearchDocument& operator=(const CBSearchDocument& source);
};

#endif
