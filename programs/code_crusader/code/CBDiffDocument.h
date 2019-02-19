/******************************************************************************
 CBDiffDocument.h

	Copyright (C) 1999 by John Lindal.

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

	static JError	CreatePlain(const JString& fullName, const JString& cmd,
								const JFontStyle& defStyle,
								const JString& name1, const JFontStyle& removeStyle,
								const JString& name2, const JFontStyle& insertStyle,
								const JBoolean silent = kJFalse,
								CBDiffDocument* origDoc = nullptr);
	static JError	CreateCVS(const JString& fullName, const JString& getCmd,
							  const JString& diffCmd, const JFontStyle& defStyle,
							  const JString& name1, const JFontStyle& removeStyle,
							  const JString& name2, const JFontStyle& insertStyle,
							  const JBoolean silent = kJFalse,
							  CBDiffDocument* origDoc = nullptr);
	static JError	CreateSVN(const JString& fullName, const JString& getCmd,
							  const JString& diffCmd, const JFontStyle& defStyle,
							  const JString& name1, const JFontStyle& removeStyle,
							  const JString& name2, const JFontStyle& insertStyle,
							  const JBoolean silent = kJFalse,
							  CBDiffDocument* origDoc = nullptr);
	static JError	CreateGit(const JString& fullName,
							  const JString& get1Cmd, const JString& get2Cmd,
							  const JString& diffCmd, const JFontStyle& defStyle,
							  const JString& name1, const JFontStyle& removeStyle,
							  const JString& name2, const JFontStyle& insertStyle,
							  const JBoolean silent = kJFalse,
							  CBDiffDocument* origDoc = nullptr);

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

	CBDiffDocument(const Type type, const JString& fullName,
				   const JString& getCmd,
				   const JString& diffCmd, const JFontStyle& defStyle,
				   const JString& name1, const JFontStyle& removeStyle,
				   const JString& name2, const JFontStyle& insertStyle);

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
	static JError	FillOutputFile(const JString& path, const JString& cmd,
								   JString* resultFullName);

	void		Init(const JString& fullName);

	void		ReadDiff(std::istream& input);
	JUtf8Byte	ReadCmd(std::istream& input, JIndexRange* origRange, JIndexRange* newRange) const;
	JIndexRange	ReadRange(std::istream& input) const;
	void		IgnoreOrigText(std::istream& input, const JUtf8Byte cmd) const;
	JString		ReadNewText(std::istream& input, const JUtf8Byte cmd, JSize* lineCount) const;

	void	UpdateDiffMenu();
	void	HandleDiffMenu(const JIndex index);

	void	ShowFirstDiff();
	void	ShowPrevDiff();
	void	ShowNextDiff();
	void	SelectDiff(const JIndexRange& removeRange, const JIndexRange& insertRange,
					   const JBoolean preferRemove, const JBoolean hadSelection,
					   const JIndex origIndex, const JIndexRange& origRange);

	// not allowed

	CBDiffDocument(const CBDiffDocument& source);
	const CBDiffDocument& operator=(const CBDiffDocument& source);

public:

	// JError classes

	class DiffFailed : public JError
		{
		public:

			DiffFailed(const JString& s, const JBoolean isFileName);
			DiffFailed(const JError& err);

		private:

			void	SetMessage(const JString& msg);
		};
};

#endif
