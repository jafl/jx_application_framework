/******************************************************************************
 CBNewProjectSaveFileDialog.h

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBNewProjectSaveFileDialog
#define _H_CBNewProjectSaveFileDialog

#include <JXSaveFileDialog.h>
#include "CBBuildManager.h"		// need defn of MakefileMethod

class JXRadioGroup;
class JXTextMenu;

class CBNewProjectSaveFileDialog : public JXSaveFileDialog
{
public:

	static CBNewProjectSaveFileDialog*
		Create(JXDirector* supervisor, JDirInfo* dirInfo,
			   const JCharacter* fileFilter, const JCharacter* templateFile,
			   const CBBuildManager::MakefileMethod method,
			   const JCharacter* origName, const JCharacter* prompt,
			   const JCharacter* message = NULL);

	virtual ~CBNewProjectSaveFileDialog();

	JBoolean						GetProjectTemplate(JString* fullName) const;
	CBBuildManager::MakefileMethod	GetMakefileMethod() const;

protected:

	CBNewProjectSaveFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
							   const JCharacter* fileFilter,
							   const CBBuildManager::MakefileMethod method);

	virtual JBoolean	OKToDeactivate();
	virtual void		Receive(JBroadcaster* sender, const Message& message);

private:

	JIndex							itsTemplateIndex;
	CBBuildManager::MakefileMethod	itsMakefileMethod;

// begin JXLayout

	JXRadioGroup* itsMethodRG;
	JXTextMenu*   itsTemplateMenu;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* origName, const JCharacter* prompt,
						const JCharacter* message = NULL);
	void	UpdateMakefileMethod();

	void	BuildTemplateMenu(const JCharacter* templateFile);
	void	BuildTemplateMenuItems(const JCharacter* path, const JBoolean isUserPath,
								   JPtrArray<JString>* menuText,
								   const JCharacter* templateFile,
								   JString** menuTextStr) const;

	JBoolean	OKToReplaceFile(const JCharacter* fullName,
								const JCharacter* programName);

	// not allowed

	CBNewProjectSaveFileDialog(const CBNewProjectSaveFileDialog& source);
	const CBNewProjectSaveFileDialog& operator=(const CBNewProjectSaveFileDialog& source);
};

#endif
