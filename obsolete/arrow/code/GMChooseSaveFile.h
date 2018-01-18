/******************************************************************************
 GMChooseSaveFile.h

	Interface for the GMChooseSaveFile class.

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_GMChooseSaveFile
#define _H_GMChooseSaveFile

#include <JXChooseSaveFile.h>

class GMSaveFileDialog;
class GMChooseFileDialog;

class GMChooseSaveFile : public JXChooseSaveFile
{

public:

	GMChooseSaveFile();

	virtual ~GMChooseSaveFile();

	void		ShouldSaveHeaders(const JBoolean save);
	JBoolean	IsSavingHeaders() const;

	void	ReadGMSetup(std::istream& input);
	void	WriteGMSetup(std::ostream& output) const;

protected:

	virtual JXSaveFileDialog*
	CreateSaveFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						 const JCharacter* fileFilter, const JCharacter* origName,
						 const JCharacter* prompt, const JCharacter* message);

	virtual JXChooseFileDialog*
	CreateChooseFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						   const JCharacter* fileFilter, const JCharacter* origName,
						   const JBoolean allowSelectMultiple, const JCharacter* message);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	GMSaveFileDialog*	itsSaveDialog;
	GMChooseFileDialog*	itsChooseDialog;
	JBoolean			itsSaveHeaders;

private:

	// not allowed

	GMChooseSaveFile(const GMChooseSaveFile& source);
	const GMChooseSaveFile& operator=(const GMChooseSaveFile& source);
};

#endif
