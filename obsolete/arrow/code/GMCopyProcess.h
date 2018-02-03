/******************************************************************************
 GMCopyProcess.h

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_GMCopyProcess
#define _H_GMCopyProcess

#include <JPtrArray-JString.h>

class JSimpleProcess;

class GMCopyProcess : virtual public JBroadcaster
{
public:

	static void	Copy(JPtrArray<JString>* srcNameList, const JCharacter* dest);
	static void	Move(JPtrArray<JString>* srcNameList, const JCharacter* dest);

	virtual ~GMCopyProcess();

protected:

	GMCopyProcess(JPtrArray<JString>* srcNameList, const JCharacter* dest,
				   const JBoolean isCopy);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JPtrArray<JString>*			itsSrcNameList;		// usually from DND data
	JSimpleProcess*				itsProcess;
	JString						itsDestPath;

private:

	static JBoolean	CleanSrcList(JPtrArray<JString>* srcNameList,
								 const JCharacter* dest);
	static JBoolean	ActionIsUseful(const JCharacter* srcName, const JCharacter* destPath,
								   JString* destName);
	static JBoolean	OKToReplace(const JCharacter* srcName, const JCharacter* destName,
								JBoolean* ask, JBoolean* first);

	// not allowed

	GMCopyProcess(const GMCopyProcess& source);
	const GMCopyProcess& operator=(const GMCopyProcess& source);
};

#endif
