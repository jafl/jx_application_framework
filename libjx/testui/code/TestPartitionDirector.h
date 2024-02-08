/******************************************************************************
 TestPartitionDirector.h

	Interface for the TestPartitionDirector class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestPartitionDirector
#define _H_TestPartitionDirector

#include <jx-af/jx/JXWindowDirector.h>

class JXMenuBar;
class JXTextMenu;
class JXPartition;
class JXHorizPartition;
class JXVertPartition;

class TestPartitionDirector : public JXWindowDirector
{
public:

	TestPartitionDirector(JXDirector* supervisor);

	~TestPartitionDirector() override;

	JXHorizPartition*	GetHorizPartition();
	void				InsertHorizCompartment(const JIndex index);
	void				DeleteHorizCompartment(const JIndex index);

	JXVertPartition*	GetVertPartition();
	void				InsertVertCompartment(const JIndex index);
	void				DeleteVertCompartment(const JIndex index);

private:

	// owned by the menu bar

	JXTextMenu*	itsHorizMenu;
	JXTextMenu*	itsVertMenu;

// begin JXLayout

	JXMenuBar*        itsMenuBar;
	JXHorizPartition* itsHorizPartition;
	JXVertPartition*  itsVertPartition;

// end JXLayout

private:

	void	BuildWindow();
	void	AdjustMinWindowSize();

	void	UpdateHorizMenu();
	void	HandleHorizMenu(const JIndex item);

	void	UpdateVertMenu();
	void	HandleVertMenu(const JIndex item);

	void	CreateTestWidget(JXPartition* partition, const JIndex index);
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
