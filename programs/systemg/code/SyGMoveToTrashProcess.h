/******************************************************************************
 SyGMoveToTrashProcess.h

	Copyright © 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SyGMoveToTrashProcess
#define _H_SyGMoveToTrashProcess

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPtrArray-JString.h>

class JSimpleProcess;
class SyGFileTreeTable;
class SyGFileTreeNode;

class SyGMoveToTrashProcess : virtual public JBroadcaster
{
public:

	static void	Move(SyGFileTreeTable* table, JPtrArray<JString>* fullNameList);

	virtual ~SyGMoveToTrashProcess();

protected:

	SyGMoveToTrashProcess(SyGFileTreeTable* table, JPtrArray<JString>* fullNameList,
						  const JCharacter* trashDir);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	SyGFileTreeTable*	itsTable;			// not owned; can be NULL
	JPtrArray<JString>*	itsFullNameList;	// store strings so don't need table
	JString				itsTrashDir;
	JSimpleProcess*		itsProcess;

private:

	void	ProcessNextFile();

	// not allowed

	SyGMoveToTrashProcess(const SyGMoveToTrashProcess& source);
	const SyGMoveToTrashProcess& operator=(const SyGMoveToTrashProcess& source);
};

#endif
