/******************************************************************************
 CBCompileDocument.h

	Interface for the CBCompileDocument class

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBCompileDocument
#define _H_CBCompileDocument

#include "CBExecOutputDocument.h"

class CBProjectDocument;

class CBCompileDocument : public CBExecOutputDocument
{
public:

	CBCompileDocument(CBProjectDocument* projDoc);

	virtual ~CBCompileDocument();

	virtual void	SetConnection(JProcess* p, const int inFD, const int outFD,
								  const JCharacter* windowTitle,
								  const JCharacter* dontCloseMsg,
								  const JCharacter* execDir,
								  const JCharacter* execCmd,
								  const JBoolean showPID);

	virtual void	OpenPrevListItem();
	virtual void	OpenNextListItem();

	virtual void	ConvertSelectionToFullPath(JString* fileName) const;

	static JBoolean	WillDoubleSpace();
	static void		ShouldDoubleSpace(const JBoolean ds);

protected:

	virtual void		AppendText(const JString& text);
	virtual JBoolean	ProcessFinished(const JProcess::Finished& info);
	virtual JBoolean	NeedsFormattedData() const;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBProjectDocument*	itsProjDoc;			// nullptr is allowed
	JString				itsPrevLine;
	JBoolean			itsHasErrorsFlag;	// kJTrue => found errors
	JXTextMenu*			itsErrorMenu;		// not owned

	static JBoolean	theDoubleSpaceFlag;

private:

	void	UpdateErrorMenu();
	void	HandleErrorMenu(const JIndex index);

	void		ShowFirstError();
	JBoolean	ShowPrevError();
	JBoolean	ShowNextError();

	JFont	GetErrorFont() const;

	class MatchErrorStyle : public JTextEditor::FontMatch
	{
	public:

		virtual ~MatchErrorStyle();
		virtual JBoolean	Match(const JFont&) const;
	};

	// not allowed

	CBCompileDocument(const CBCompileDocument& source);
	const CBCompileDocument& operator=(const CBCompileDocument& source);
};


/******************************************************************************
 Double spaced output (static)

 ******************************************************************************/

inline JBoolean
CBCompileDocument::WillDoubleSpace()
{
	return theDoubleSpaceFlag;
}

inline void
CBCompileDocument::ShouldDoubleSpace
	(
	const JBoolean ds
	)
{
	theDoubleSpaceFlag = ds;
}

#endif
