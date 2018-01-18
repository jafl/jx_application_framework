/******************************************************************************
 CBDiffFileDialog.h

	Interface for the CBDiffFileDialog class

	Copyright (C) 1999 by John Lindal. All rights reserved.

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

	void	SetFile1(const JCharacter* fullName);
	void	SetFile2(const JCharacter* fullName);
	void	SetCVSFile(const JCharacter* fullName);
	void	SetSVNFile(const JCharacter* fullName);
	void	SetGitFile(const JCharacter* fullName);

	void	ViewDiffs(const JCharacter* fullName1, const JCharacter* fullName2,
					  const JBoolean silent = kJFalse);
	void	ViewCVSDiffs(const JCharacter* fileName,
						 const JBoolean silent = kJFalse);
	void	ViewCVSDiffs(const JCharacter* fullName,
						 const JCharacter* rev1, const JCharacter* rev2,
						 const JBoolean silent = kJFalse);
	void	ViewSVNDiffs(const JCharacter* fileName,
						 const JBoolean silent = kJFalse);
	void	ViewSVNDiffs(const JCharacter* fullName,
						 const JCharacter* rev1, const JCharacter* rev2,
						 const JBoolean silent = kJFalse);
	void	ViewGitDiffs(const JCharacter* fileName,
						 const JBoolean silent = kJFalse);
	void	ViewGitDiffs(const JCharacter* fullName,
						 const JCharacter* rev1, const JCharacter* rev2,
						 const JBoolean silent = kJFalse);

	const JCharacter*	GetSmartDiffItemText(const JBoolean onDisk,
											 const JCharacter* fullName,
											 JBoolean* enable) const;
	void				ViewDiffs(const JBoolean onDisk,
								  const JCharacter* fullName,
								  const JBoolean silent = kJFalse);

protected:

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

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
	JBoolean	ViewDiffs();

	JBoolean	CheckFile(JXFileInput* widget) const;
	void		ChooseFile(JXFileInput* widget);
	void		ChoosePath(JXFileInput* widget);

	JString		BuildDiffCmd();
	void		DiffDirectory(const JCharacter* fullName, const JCharacter* diffCmd,
							  JXCheckbox* summaryCB, const JCharacter* summaryArgs);

	JBoolean	CheckVCSFileOrPath(JXFileInput* widget, const JBoolean reportError,
								   JString* fullName) const;
	void		UpdateVCSRevMenu(JXTextMenu* menu, const JIndex cmd);
	void		HandleCVSRevMenu(JXTextMenu* menu, const JIndex index,
								 JIndex* cmd, JXInputField* input);
	JBoolean	BuildCVSDiffCmd(const JCharacter* fullName,
								const JIndex rev1Cmd, const JCharacter* rev1,
								const JIndex rev2Cmd, const JCharacter* rev2,
								JString* getCmd, JString* diffCmd,
								JString* name1, JString* name2,
								const JBoolean silent);

	JBoolean	GetCurrentCVSRevision(const JCharacter* fullName, JString* rev);
	JBoolean	GetPreviousCVSRevision(const JCharacter* fullName, JString* rev);

	JBoolean	CheckSVNFileOrPath(JXFileInput* widget, const JBoolean reportError,
								   JString* fullName) const;
	void		HandleSVNRevMenu(JXTextMenu* menu, const JIndex index,
								 JIndex* cmd, JXInputField* input);
	JBoolean	BuildSVNDiffCmd(const JCharacter* fullName,
								const JIndex rev1Cmd, const JCharacter* rev1,
								const JIndex rev2Cmd, const JCharacter* rev2,
								JString* getCmd, JString* diffCmd,
								JString* name1, JString* name2,
								const JBoolean silent,
								const JBoolean forDirectory = kJFalse);
	JBoolean	GetPreviousSVNRevision(const JCharacter* fullName, JString* rev);
	JBoolean	BuildSVNRepositoryPath(JString* fullName, const JIndex cmd,
									   const JCharacter* rev, JString* name,
									   const JBoolean silent);

	void		HandleGitRevMenu(JXTextMenu* menu, const JIndex index,
								 JIndex* cmd, JXInputField* input);
	JBoolean	BuildGitDiffCmd(const JCharacter* fullName,
								const JIndex rev1Cmd, const JCharacter* rev1,
								const JIndex rev2Cmd, const JCharacter* rev2,
								JString* get1Cmd, JString* get2Cmd, JString* diffCmd,
								JString* name1, JString* name2,
								const JBoolean silent);
	JBoolean	BuildGitDiffDirectoryCmd(const JCharacter* path,
										 const JIndex rev1Cmd, const JCharacter* rev1,
										 const JIndex rev2Cmd, const JCharacter* rev2,
										 JString* diffCmd);
	JBoolean	GetCurrentGitRevision(const JCharacter* fullName, JString* rev);
	JBoolean	GetPreviousGitRevision(const JCharacter* fullName, JString* rev);
	JBoolean	GetLatestGitRevisions(const JCharacter* fullName,
									  JString* rev1, JString* rev2);
	JBoolean	GetBestCommonGitAncestor(const JCharacter* path,
										 JString* rev1, const JCharacter* rev2);

	JString	GetSmartDiffInfo(const JCharacter* origFileName,
							 JBoolean* isSafetySave, JBoolean* isBackup) const;

	void	UpdateBasePath();

	// not allowed

	CBDiffFileDialog(const CBDiffFileDialog& source);
	const CBDiffFileDialog& operator=(const CBDiffFileDialog& source);
};

#endif
