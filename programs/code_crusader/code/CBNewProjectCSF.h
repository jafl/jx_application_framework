/******************************************************************************
 CBNewProjectCSF.h

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBNewProjectCSF
#define _H_CBNewProjectCSF

#include <JXChooseSaveFile.h>
#include <JPrefObject.h>
#include "CBBuildManager.h"		// need defn of MakefileMethod

class CBNewProjectSaveFileDialog;

class CBNewProjectCSF : public JXChooseSaveFile
{
public:

	CBNewProjectCSF();

	virtual ~CBNewProjectCSF();

	bool						GetProjectTemplate(JString* fullName) const;
	CBBuildManager::MakefileMethod	GetMakefileMethod() const;

protected:

	virtual JXSaveFileDialog*
	CreateSaveFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						 const JString& fileFilter, const JString& origName,
						 const JString& prompt, const JString& message) override;

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBNewProjectSaveFileDialog*		itsSaveDialog;
	JString							itsProjectTemplate;
	CBBuildManager::MakefileMethod	itsMakefileMethod;

private:

	// not allowed

	CBNewProjectCSF(const CBNewProjectCSF& source);
	const CBNewProjectCSF& operator=(const CBNewProjectCSF& source);
};


/******************************************************************************
 GetProjectTemplate

 ******************************************************************************/

inline bool
CBNewProjectCSF::GetProjectTemplate
	(
	JString* fullName
	)
	const
{
	*fullName = itsProjectTemplate;
	return !fullName->IsEmpty();
}

/******************************************************************************
 GetMakefileMethod

 ******************************************************************************/

inline CBBuildManager::MakefileMethod
CBNewProjectCSF::GetMakefileMethod()
	const
{
	return itsMakefileMethod;
}

#endif
