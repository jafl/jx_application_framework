/******************************************************************************
 CBNewProjectCSF.h

	Copyright © 2000 by John Lindal. All rights reserved.

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

	JBoolean						GetProjectTemplate(JString* fullName) const;
	CBBuildManager::MakefileMethod	GetMakefileMethod() const;

protected:

	virtual JXSaveFileDialog*
	CreateSaveFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						 const JCharacter* fileFilter, const JCharacter* origName,
						 const JCharacter* prompt, const JCharacter* message);

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);

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

inline JBoolean
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
