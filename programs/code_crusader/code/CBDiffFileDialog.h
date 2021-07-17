/******************************************************************************
 CBDiffFileDialog.h

	Interface for the CBDiffFileDialog class

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBDiffFileDialog
#define _H_CBDiffFileDialog

#include <JXWindowDirector.h>
#include <JPrefObject.h>

class JXTextButton;
class JXCheckbox;
class JXTextCheckbox;
class JXTextMenu;
class JXInputField;
class JXFileInput;
class JXTabGroup;
class CBDiffStyleMenu;
class CBSVNFileInput;

class CBDiffFileDialog : public JXWindowDirector, public JPrefObject
{
public:

	CBDiffFileDialog(JXDirector* supervisor);

	virtual ~CBDiffFileDialog();

	void	SetFile1(const JString& fullName);
	void	SetFile2(const JString& fullName);
	void	SetCVSFile(const JString& fullName);
	void	SetSVNFile(const JString& fullName);
	void	SetGitFile(const JString& fullName);

	void	ViewDiffs(const JString& fullName1, const JString& fullName2,
					  const bool silent = false);
	void	ViewVCSDiffs(const JString& fullName,
						 const bool silent = false);
	void	ViewCVSDiffs(const JString& fullName,
						 const JString& rev1, const JString& rev2,
						 const bool silent = false);
	void	ViewSVNDiffs(const JString& fullName,
						 const JString& rev1, const JString& rev2,
						 const bool silent = false);
	void	ViewGitDiffs(const JString& fullName,
						 const JString& rev1, const JString& rev2,
						 const bool silent = false);

	const JString&	GetSmartDiffItemText(const bool onDisk,
										 const JString& fullName,
										 bool* enable) const;
	void			ViewDiffs(const bool onDisk,
							  const JString& fullName,
							  const bool silent = false);

protected:

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	enum
	{
		kPlainDiffTabIndex = 1,
		kCVSDiffTabIndex,
		kSVNDiffTabIndex,
		kGitDiffTabIndex,

		kTabCount = 4
	};

private:

	JIndex				itsTabIndex;
	CBDiffStyleMenu*	itsStyleMenu[ kTabCount ][2];
	JIndex				itsCVSRev1Cmd;
	JIndex				itsCVSRev2Cmd;
	JIndex				itsSVNRev1Cmd;
	JIndex				itsSVNRev2Cmd;
	JIndex				itsGitRev1Cmd;
	JIndex				itsGitRev2Cmd;

// begin JXLayout

	JXTextButton*    itsCloseButton;
	JXTextButton*    itsViewButton;
	JXTextButton*    itsHelpButton;
	CBDiffStyleMenu* itsCommonStyleMenu;
	JXTextCheckbox*  itsStayOpenCB;
	JXTextCheckbox*  itsIgnoreSpaceChangeCB;
	JXTextCheckbox*  itsIgnoreBlankLinesCB;
	JXTabGroup*      itsTabGroup;

// end JXLayout

// begin plainLayout

	CBDiffStyleMenu* itsPlainOnly1StyleMenu;
	CBDiffStyleMenu* itsPlainOnly2StyleMenu;
	JXFileInput*     itsPlainFile1Input;
	JXTextButton*    itsPlainChoose1Button;
	JXFileInput*     itsPlainFile2Input;
	JXTextButton*    itsPlainChoose2Button;

// end plainLayout

// begin cvsLayout

	JXFileInput*     itsCVSFileInput;
	CBDiffStyleMenu* itsCVSOnly1StyleMenu;
	CBDiffStyleMenu* itsCVSOnly2StyleMenu;
	JXTextButton*    itsCVSChooseButton;
	JXInputField*    itsCVSRev1Input;
	JXTextMenu*      itsCVSRev1Menu;
	JXInputField*    itsCVSRev2Input;
	JXTextMenu*      itsCVSRev2Menu;
	JXTextCheckbox*  itsCVSSummaryCB;

// end cvsLayout

// begin svnLayout

	CBSVNFileInput*  itsSVNFileInput;
	CBDiffStyleMenu* itsSVNOnly1StyleMenu;
	CBDiffStyleMenu* itsSVNOnly2StyleMenu;
	JXTextButton*    itsSVNChooseButton;
	JXInputField*    itsSVNRev1Input;
	JXTextMenu*      itsSVNRev1Menu;
	JXInputField*    itsSVNRev2Input;
	JXTextMenu*      itsSVNRev2Menu;
	JXTextCheckbox*  itsSVNSummaryCB;

// end svnLayout

// begin gitLayout

	JXFileInput*     itsGitFileInput;
	CBDiffStyleMenu* itsGitOnly1StyleMenu;
	CBDiffStyleMenu* itsGitOnly2StyleMenu;
	JXTextButton*    itsGitChooseButton;
	JXInputField*    itsGitRev1Input;
	JXTextMenu*      itsGitRev1Menu;
	JXInputField*    itsGitRev2Input;
	JXTextMenu*      itsGitRev2Menu;
	JXTextCheckbox*  itsGitSummaryCB;

// end gitLayout

private:

	void		BuildWindow();
	void		UpdateDisplay();
	bool	ViewDiffs();

	bool	CheckFile(JXFileInput* widget) const;
	void		ChooseFile(JXFileInput* widget);
	void		ChoosePath(JXFileInput* widget);

	JString		BuildDiffCmd();
	void		DiffDirectory(const JString& fullName, const JString& diffCmd,
							  JXCheckbox* summaryCB, const JString& summaryArgs);

	bool	CheckVCSFileOrPath(JXFileInput* widget, const bool reportError,
								   JString* fullName) const;
	void		UpdateVCSRevMenu(JXTextMenu* menu, const JIndex cmd);

	void		ViewCVSDiffs(const JString& fullName, const bool silent);
	void		HandleCVSRevMenu(JXTextMenu* menu, const JIndex index,
								 JIndex* cmd, JXInputField* input);
	bool	BuildCVSDiffCmd(const JString& fullName,
								const JIndex rev1Cmd, const JString& rev1,
								const JIndex rev2Cmd, const JString& rev2,
								JString* getCmd, JString* diffCmd,
								JString* name1, JString* name2,
								const bool silent);

	bool	GetCurrentCVSRevision(const JString& fullName, JString* rev);
	bool	GetPreviousCVSRevision(const JString& fullName, JString* rev);

	void		ViewSVNDiffs(const JString& fullName, const bool silent);
	bool	CheckSVNFileOrPath(JXFileInput* widget, const bool reportError,
								   JString* fullName) const;
	void		HandleSVNRevMenu(JXTextMenu* menu, const JIndex index,
								 JIndex* cmd, JXInputField* input);
	bool	BuildSVNDiffCmd(const JString& fullName,
								const JIndex rev1Cmd, const JString& rev1,
								const JIndex rev2Cmd, const JString& rev2,
								JString* getCmd, JString* diffCmd,
								JString* name1, JString* name2,
								const bool silent,
								const bool forDirectory = false);
	bool	BuildSVNRepositoryPath(JString* fullName, const JIndex cmd,
									   const JString& rev, JString* name,
									   const bool silent);

	void		ViewGitDiffs(const JString& fullName, const bool silent);
	void		HandleGitRevMenu(JXTextMenu* menu, const JIndex index,
								 JIndex* cmd, JXInputField* input);
	bool	BuildGitDiffCmd(const JString& fullName,
								const JIndex rev1Cmd, const JString& rev1,
								const JIndex rev2Cmd, const JString& rev2,
								JString* get1Cmd, JString* get2Cmd, JString* diffCmd,
								JString* name1, JString* name2,
								const bool silent);
	bool	BuildGitDiffDirectoryCmd(const JString& path,
										 const JIndex rev1Cmd, const JString& rev1,
										 const JIndex rev2Cmd, const JString& rev2,
										 JString* diffCmd);
	bool	GetCurrentGitRevision(const JString& fullName, JString* rev);
	bool	GetPreviousGitRevision(const JString& fullName, JString* rev);
	bool	GetLatestGitRevisions(const JString& fullName,
									  JString* rev1, JString* rev2);
	bool	GetBestCommonGitAncestor(const JString& path,
										 JString* rev1, const JUtf8Byte* rev2);

	JString	GetSmartDiffInfo(const JString& origFileName,
							 bool* isSafetySave, bool* isBackup) const;

	void	UpdateBasePath();

	// not allowed

	CBDiffFileDialog(const CBDiffFileDialog& source);
	const CBDiffFileDialog& operator=(const CBDiffFileDialog& source);
};

#endif
