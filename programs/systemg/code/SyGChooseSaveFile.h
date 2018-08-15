/******************************************************************************
 SyGChooseSaveFile.h

	Interface for the SyGChooseSaveFile class.

	Copyright (C) 1999 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_SyGChooseSaveFile
#define _H_SyGChooseSaveFile

#include <JXChooseSaveFile.h>

class SyGChoosePathDialog;

class SyGChooseSaveFile : public JXChooseSaveFile
{

public:

	SyGChooseSaveFile(JPrefsManager* prefsMgr, const JPrefID& id);

	virtual ~SyGChooseSaveFile();

	void		ShouldOpenInNewWindow(const JBoolean newWindow);
	JBoolean	IsOpeningInNewWindow() const;

	void		SGReadSetup(std::istream& input);
	void		SGWriteSetup(std::ostream& output) const;

protected:

//	virtual JXSaveFileDialog*
//	CreateSaveFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
//						 const JString& fileFilter, const JString& origName,
//						 const JString& prompt, const JString& message) override;

//	virtual JXChooseFileDialog*
//	CreateChooseFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
//						   const JString& fileFilter, const JString& origName,
//						   const JBoolean allowSelectMultiple, const JString& message) override;

	virtual JXChoosePathDialog*
	CreateChoosePathDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						   const JString& fileFilter,
						   const JBoolean selectOnlyWritable,
						   const JString& message) override;

	virtual void	ReadPrefs(std::istream& input) override;
	virtual void	WritePrefs(std::ostream& output) const override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	SyGChoosePathDialog*	itsPathDialog;
	JBoolean				itsOpenInNewWindow;

private:

	// not allowed

	SyGChooseSaveFile(const SyGChooseSaveFile& source);
	const SyGChooseSaveFile& operator=(const SyGChooseSaveFile& source);
};

#endif
