/******************************************************************************
 GMailboxChooseSaveFile.h

	Interface for the GMailboxChooseSaveFile class.

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_GMailboxChooseSaveFile
#define _H_GMailboxChooseSaveFile

#include <JXChooseSaveFile.h>


class GMailboxChooseSaveFile : public JXChooseSaveFile
{
public:

	GMailboxChooseSaveFile();

	virtual ~GMailboxChooseSaveFile();

protected:

	virtual void	SetChooseFileFilters(JDirInfo* dirInfo);

private:

	// not allowed

	GMailboxChooseSaveFile(const GMailboxChooseSaveFile& source);
	const GMailboxChooseSaveFile& operator=(const GMailboxChooseSaveFile& source);
};
#endif
