/******************************************************************************
 CBSubprojectConfigDialog.h

	Copyright (C) 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBSubprojectConfigDialog
#define _H_CBSubprojectConfigDialog

#include <JXDialogDirector.h>

class JString;
class JXTextButton;
class JXTextCheckbox;
class CBProjectDocument;
class CBProjectFileInput;
class CBRelPathCSF;

class CBSubprojectConfigDialog : public JXDialogDirector
{
public:

	CBSubprojectConfigDialog(CBProjectDocument* supervisor,
							 const JBoolean includeInDepList,
							 const JCharacter* subProjName,
							 const JBoolean shouldBuild,
							 CBRelPathCSF* csf);

	virtual ~CBSubprojectConfigDialog();

	void	GetConfig(JBoolean* includeInDepList,
					  JString* subProjName, JBoolean* shouldBuild) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBRelPathCSF*	itsCSF;			// not owned

// begin JXLayout

	JXTextCheckbox*     itsShouldBuildCB;
	CBProjectFileInput* itsSubProjName;
	JXTextButton*       itsChooseFileButton;
	JXTextCheckbox*     itsIncludeInDepListCB;

// end JXLayout

private:

	void	BuildWindow(CBProjectDocument* supervisor,
						const JBoolean includeInDepList,
						const JCharacter* subProjName,
						const JBoolean shouldBuild,
						const JCharacter* basePath);
	void	UpdateDisplay();

	void	ChooseProjectFile();

	// not allowed

	CBSubprojectConfigDialog(const CBSubprojectConfigDialog& source);
	const CBSubprojectConfigDialog& operator=(const CBSubprojectConfigDialog& source);
};

#endif
