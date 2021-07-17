/******************************************************************************
 CBCompileDocument.h

	Interface for the CBCompileDocument class

	Copyright © 1997 by John Lindal.

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
								  const JString& windowTitle,
								  const JString& dontCloseMsg,
								  const JString& execDir,
								  const JString& execCmd,
								  const bool showPID) override;

	virtual void	OpenPrevListItem() override;
	virtual void	OpenNextListItem() override;

	virtual void	ConvertSelectionToFullPath(JString* fileName) const override;

	static bool	WillDoubleSpace();
	static void		ShouldDoubleSpace(const bool ds);

protected:

	virtual void		AppendText(const JString& text) override;
	virtual bool	ProcessFinished(const JProcess::Finished& info) override;
	virtual bool	NeedsFormattedData() const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBProjectDocument*	itsProjDoc;			// nullptr is allowed
	JString				itsPrevLine;
	bool			itsHasErrorsFlag;	// true => found errors
	JXTextMenu*			itsErrorMenu;		// not owned

	static bool	theDoubleSpaceFlag;

private:

	void	UpdateErrorMenu();
	void	HandleErrorMenu(const JIndex index);

	void		ShowFirstError();
	bool	ShowPrevError();
	bool	ShowNextError();

	// not allowed

	CBCompileDocument(const CBCompileDocument& source);
	const CBCompileDocument& operator=(const CBCompileDocument& source);
};


/******************************************************************************
 Double spaced output (static)

 ******************************************************************************/

inline bool
CBCompileDocument::WillDoubleSpace()
{
	return theDoubleSpaceFlag;
}

inline void
CBCompileDocument::ShouldDoubleSpace
	(
	const bool ds
	)
{
	theDoubleSpaceFlag = ds;
}

#endif
