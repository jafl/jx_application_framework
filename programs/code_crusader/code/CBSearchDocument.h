/******************************************************************************
 CBSearchDocument.h

	Copyright (C) 1998 by John Lindal.

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
						   const JCharacter* searchStr,
						   const JBoolean onlyListFiles,
						   const JBoolean listFilesWithoutMatch);

	static JError	Create(const JPtrArray<JString>& fileList,
						   const JPtrArray<JString>& nameList,
						   const JCharacter* searchStr,
						   const JCharacter* replaceStr);

	virtual ~CBSearchDocument();

	virtual void	OpenPrevListItem();
	virtual void	OpenNextListItem();

	virtual void	ConvertSelectionToFullPath(JString* fileName) const;

protected:

	CBSearchDocument(const JBoolean isReplace, const JBoolean onlyListFiles,
					 const JSize fileCount,
					 JProcess* p, const int fd,
					 const JCharacter* windowTitle);

	virtual void		PlaceCmdLineWidgets();
	virtual void		AppendText(const JString& text);
	virtual JBoolean	ProcessFinished(const JProcess::Finished& info);
	virtual JBoolean	NeedsFormattedData() const;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const JBoolean	itsIsReplaceFlag;
	const JBoolean	itsOnlyListFilesFlag;
	JBoolean		itsFoundFlag;			// kJTrue => something matched
	JSize			itsPrevQuoteOffset;		// start of previous quote from file

	JXTextMenu*				itsMatchMenu;
	JXProgressIndicator*	itsIndicator;	// NULL after ProcessFinished()
	CBSearchTE*				itsReplaceTE;	// NULL unless replacing

private:

	void	ReplaceAll(const JCharacter* fileName);

	void	UpdateMatchMenu();
	void	HandleMatchMenu(const JIndex index);

	void		ShowFirstMatch();
	JBoolean	ShowPrevMatch();
	JBoolean	ShowNextMatch();

	JFontStyle	GetFileNameStyle() const;
	JFontStyle	GetMatchStyle() const;
	JFontStyle	GetErrorStyle() const;

	class MatchFileNameStyle : public JTextEditor::FontMatch
	{
	public:

		virtual ~MatchFileNameStyle();
		virtual JBoolean	Match(const JFont&) const;
	};

	// not allowed

	CBSearchDocument(const CBSearchDocument& source);
	const CBSearchDocument& operator=(const CBSearchDocument& source);
};

#endif
