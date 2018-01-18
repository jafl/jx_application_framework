/******************************************************************************
 CBDiffDocument.h

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBDiffDocument
#define _H_CBDiffDocument

#include "CBTextDocument.h"
#include "CBDiffFileDialog.h"	// need defn of DiffType
#include <JError.h>

class JXTextMenu;
class JXTextButton;

class CBDiffDocument : public CBTextDocument
{
public:

	static const JCharacter* theLocalSVNGetMarker;

public:

	static JError	CreatePlain(const JCharacter* fullName, const JCharacter* cmd,
								const JFontStyle& defStyle,
								const JCharacter* name1, const JFontStyle& removeStyle,
								const JCharacter* name2, const JFontStyle& insertStyle,
								const JBoolean silent = kJFalse,
								CBDiffDocument* origDoc = NULL);
	static JError	CreateCVS(const JCharacter* fullName, const JCharacter* getCmd,
							  const JCharacter* diffCmd, const JFontStyle& defStyle,
							  const JCharacter* name1, const JFontStyle& removeStyle,
							  const JCharacter* name2, const JFontStyle& insertStyle,
							  const JBoolean silent = kJFalse,
							  CBDiffDocument* origDoc = NULL);
	static JError	CreateSVN(const JCharacter* fullName, const JCharacter* getCmd,
							  const JCharacter* diffCmd, const JFontStyle& defStyle,
							  const JCharacter* name1, const JFontStyle& removeStyle,
							  const JCharacter* name2, const JFontStyle& insertStyle,
							  const JBoolean silent = kJFalse,
							  CBDiffDocument* origDoc = NULL);
	static JError	CreateGit(const JCharacter* fullName,
							  const JCharacter* get1Cmd, const JCharacter* get2Cmd,
							  const JCharacter* diffCmd, const JFontStyle& defStyle,
							  const JCharacter* name1, const JFontStyle& removeStyle,
							  const JCharacter* name2, const JFontStyle& insertStyle,
							  const JBoolean silent = kJFalse,
							  CBDiffDocument* origDoc = NULL);

	virtual ~CBDiffDocument();

protected:

	enum Type
	{
		kPlainType,
		kCVSType,
		kSVNType,
		kGitType
	};

protected:

	CBDiffDocument(const Type type, const JCharacter* fullName,
				   const JCharacter* getCmd,
				   const JCharacter* diffCmd, const JFontStyle& defStyle,
				   const JCharacter* name1, const JFontStyle& removeStyle,
				   const JCharacter* name2, const JFontStyle& insertStyle);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const Type		itsType;
	const JString	itsFullName;
	const JString	itsGetCmd;
	const JString	itsDiffCmd;
	const JString	itsName1;
	const JString	itsName2;

	const JFontStyle	itsDefaultStyle;
	const JFontStyle	itsRemoveStyle;
	const JFontStyle	itsInsertStyle;

	JXTextMenu*		itsDiffMenu;
	JXTextButton*	itsDiffButton;

private:

	static JError	CreateOutputFiles(JString* tempFileName, int* tempFileFD,
									  JString* errFileName, int* errFileFD);
	static JError	FillOutputFile(const JCharacter* path, const JCharacter* cmd,
								   JString* resultFullName);

	void		Init(const JCharacter* fullName);

	void		ReadDiff(std::istream& input);
	JCharacter	ReadCmd(std::istream& input, JIndexRange* origRange, JIndexRange* newRange) const;
	JIndexRange	ReadRange(std::istream& input) const;
	void		IgnoreOrigText(std::istream& input, const JCharacter cmd) const;
	JString		ReadNewText(std::istream& input, const JCharacter cmd, JSize* lineCount) const;

	void	UpdateDiffMenu();
	void	HandleDiffMenu(const JIndex index);

	void	ShowFirstDiff();
	void	ShowPrevDiff();
	void	ShowNextDiff();
	void	SelectDiff(const JIndexRange& removeRange, const JIndexRange& insertRange,
					   const JBoolean preferRemove, const JBoolean hadSelection,
					   const JIndex origIndex, const JIndexRange& origRange);

	class MatchStyle : public JTextEditor::FontMatch
	{
	public:

		MatchStyle(const JFontStyle& style);
		virtual ~MatchStyle();
		virtual JBoolean	Match(const JFont&) const;

	private:

		const JFontStyle	itsStyle;
	};

	// not allowed

	CBDiffDocument(const CBDiffDocument& source);
	const CBDiffDocument& operator=(const CBDiffDocument& source);

public:

	// JError classes

	class DiffFailed : public JError
		{
		public:

			DiffFailed(const JCharacter* s, const JBoolean isFileName);
			DiffFailed(const JError& err);

		private:

			void	SetMessage(const JCharacter* msg);
		};
};

#endif
