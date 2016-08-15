/******************************************************************************
 CBSearchDocument.h

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBSearchDocument
#define _H_CBSearchDocument

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CBExecOutputDocument.h"

class JXTextMenu;
class JXProgressIndicator;

class CBSearchDocument : public CBExecOutputDocument
{
public:

	static JError	Create(const JPtrArray<JString>& fileList,
						   const JPtrArray<JString>& nameList,
						   const JCharacter* searchStr,
						   const JBoolean onlyListFiles,
						   const JBoolean listFilesWithoutMatch);

	virtual ~CBSearchDocument();

	virtual void	OpenPrevListItem();
	virtual void	OpenNextListItem();

	virtual void	ConvertSelectionToFullPath(JString* fileName) const;

	static JError	GetFileListForReplace(const JPtrArray<JString>& fileList,
										  const JPtrArray<JString>& nameList,
										  JProcess** process,
										  RecordLink** link);
	static void		DeleteReplaceLink(RecordLink** link);

protected:

	CBSearchDocument(const JBoolean onlyListFiles, const JSize fileCount,
					 JProcess* p, const int fd,
					 const JCharacter* windowTitle,
					 const JCharacter* dontCloseMsg,
					 const JCharacter* execDir,
					 const JCharacter* execCmd);

	virtual void		AppendText(const JString& text);
	virtual JBoolean	ProcessFinished(const JProcess::Finished& info);
	virtual JBoolean	NeedsFormattedData() const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	const JBoolean	itsOnlyListFilesFlag;
	JBoolean		itsFoundFlag;			// kJTrue => something matched
	JSize			itsPrevQuoteOffset;		// start of previous quote from file

	JXTextMenu*				itsMatchMenu;
	JXProgressIndicator*	itsIndicator;	// NULL after ProcessFinished()

private:

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
