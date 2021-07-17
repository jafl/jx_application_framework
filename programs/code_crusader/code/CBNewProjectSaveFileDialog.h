/******************************************************************************
 CBNewProjectSaveFileDialog.h

	Copyright © 2000 by John Lindal.

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
			   const JString& fileFilter, const JString& templateFile,
			   const CBBuildManager::MakefileMethod method,
			   const JString& origName, const JString& prompt,
			   const JString& message = JString::empty);

	virtual ~CBNewProjectSaveFileDialog();

	bool						GetProjectTemplate(JString* fullName) const;
	CBBuildManager::MakefileMethod	GetMakefileMethod() const;

protected:

	CBNewProjectSaveFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
							   const JString& fileFilter,
							   const CBBuildManager::MakefileMethod method);

	virtual bool	OKToDeactivate() override;
	virtual void		Receive(JBroadcaster* sender, const Message& message) override;

private:

	JIndex							itsTemplateIndex;
	CBBuildManager::MakefileMethod	itsMakefileMethod;

// begin JXLayout

	JXRadioGroup* itsMethodRG;
	JXTextMenu*   itsTemplateMenu;

// end JXLayout

private:

	void	BuildWindow(const JString& origName, const JString& prompt,
						const JString& message = JString::empty);
	void	UpdateMakefileMethod();

	void	BuildTemplateMenu(const JString& templateFile);
	void	BuildTemplateMenuItems(const JString& path, const bool isUserPath,
								   JPtrArray<JString>* menuText,
								   const JString& templateFile,
								   JString** menuTextStr) const;

	bool	OKToReplaceFile(const JString& fullName,
								const JString& programName);

	// not allowed

	CBNewProjectSaveFileDialog(const CBNewProjectSaveFileDialog& source);
	const CBNewProjectSaveFileDialog& operator=(const CBNewProjectSaveFileDialog& source);
};

#endif
