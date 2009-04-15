/******************************************************************************
 TestPartitionDirector.h

	Interface for the TestPartitionDirector class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestPartitionDirector
#define _H_TestPartitionDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

class SetElasticDialog;

class JXTextMenu;
class JXPartition;
class JXHorizPartition;
class JXVertPartition;

class TestPartitionDirector : public JXWindowDirector
{
public:

	TestPartitionDirector(JXDirector* supervisor);

	virtual ~TestPartitionDirector();

	JXHorizPartition*	GetHorizPartition();
	void				InsertHorizCompartment(const JIndex index);
	void				DeleteHorizCompartment(const JIndex index);

	JXVertPartition*	GetVertPartition();
	void				InsertVertCompartment(const JIndex index);
	void				DeleteVertCompartment(const JIndex index);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	enum ElasticType
	{
		kHorizElastic,
		kVertElastic
	};

private:

	JXVertPartition* itsVertPartition;	// owned by the horiz partition

	SetElasticDialog*	itsSetElasticDialog;
	ElasticType			itsSetElasticType;

	// owned by the menu bar

	JXTextMenu*	itsHorizMenu;
	JXTextMenu*	itsVertMenu;

// begin JXLayout

    JXHorizPartition* itsHorizPartition;

// end JXLayout

private:

	void	BuildWindow();
	void	AdjustMinWindowSize();

	void	UpdateHorizMenu();
	void	HandleHorizMenu(const JIndex item);

	void	UpdateVertMenu();
	void	HandleVertMenu(const JIndex item);

	void	CreateTestWidget(JXPartition* partition, const JIndex index);

	// not allowed

	TestPartitionDirector(const TestPartitionDirector& source);
	const TestPartitionDirector& operator=(const TestPartitionDirector& source);
};

/******************************************************************************
 Get partition

 ******************************************************************************/

inline JXHorizPartition*
TestPartitionDirector::GetHorizPartition()
{
	return itsHorizPartition;
}

inline JXVertPartition*
TestPartitionDirector::GetVertPartition()
{
	return itsVertPartition;
}

#endif
