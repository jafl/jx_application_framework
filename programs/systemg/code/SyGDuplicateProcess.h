/******************************************************************************
 SyGDuplicateProcess.h

	Copyright (C) 2000 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SyGDuplicateProcess
#define _H_SyGDuplicateProcess

#include <JPtrArray-JString.h>

class JSimpleProcess;
class SyGFileTreeTable;
class SyGFileTreeNode;

class SyGDuplicateProcess : virtual public JBroadcaster
{
public:

	static void	Duplicate(SyGFileTreeTable* table,
						  const JPtrArray<SyGFileTreeNode>& nodeList);

	virtual ~SyGDuplicateProcess();

protected:

	SyGDuplicateProcess(SyGFileTreeTable* table,
						const JPtrArray<SyGFileTreeNode>& nodeList);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	SyGFileTreeTable*			itsTable;			// not owned; can be NULL
	JPtrArray<SyGFileTreeNode>	itsNodeList;		// contents not owned
	JPtrArray<JString>			itsFullNameList;	// store strings so don't need table
	JSimpleProcess*				itsProcess;
	JString						itsCurrentName;
	const JBoolean				itsShouldEditFlag;	// BeginEditing() after process finishes

private:

	void	ProcessNextFile();

	// not allowed

	SyGDuplicateProcess(const SyGDuplicateProcess& source);
	const SyGDuplicateProcess& operator=(const SyGDuplicateProcess& source);
};

#endif
