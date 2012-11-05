/******************************************************************************
 CMArray2DCommand.h

	Copyright � 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CMArray2DCommand
#define _H_CMArray2DCommand

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "CMCommand.h"
#include <JPoint.h>
#include <JIntRange.h>

class JStringTableData;
class JXStringTable;
class CMArray2DDir;

class CMArray2DCommand : public CMCommand
{
public:

	enum UpdateType
	{
		kRow,
		kCol
	};

public:

	CMArray2DCommand(CMArray2DDir* dir,
					 JXStringTable* table, JStringTableData* data);

	virtual	~CMArray2DCommand();

	virtual void	PrepareToSend(const UpdateType type, const JIndex index,
								  const JInteger arrayIndex);

	CMArray2DDir*		GetDirector();
	JXStringTable*		GetTable();
	JStringTableData*	GetData();
	UpdateType			GetType() const;

protected:

	virtual void	HandleFailure();

	void		HandleFailure(const JIndex startIndex, const JCharacter* value);
	JPoint		SetCell(const JIndex i) const;
	JBoolean	ItsIndexValid() const;

private:

	CMArray2DDir*		itsDirector;
	JXStringTable*		itsTable;
	JStringTableData*	itsData;		// not owned
	UpdateType			itsType;
	JIndex				itsIndex;		// row or column to update
	JInteger			itsArrayIndex;	// i or j value being updated

private:

	// not allowed

	CMArray2DCommand(const CMArray2DCommand& source);
	const CMArray2DCommand& operator=(const CMArray2DCommand& source);
};


/******************************************************************************
 GetDirector

 ******************************************************************************/

inline CMArray2DDir*
CMArray2DCommand::GetDirector()
{
	return itsDirector;
}

/******************************************************************************
 GetTable

 ******************************************************************************/

inline JXStringTable*
CMArray2DCommand::GetTable()
{
	return itsTable;
}

/******************************************************************************
 GetData

 ******************************************************************************/

inline JStringTableData*
CMArray2DCommand::GetData()
{
	return itsData;
}

/******************************************************************************
 GetType

 ******************************************************************************/

inline CMArray2DCommand::UpdateType
CMArray2DCommand::GetType()
	const
{
	return itsType;
}

#endif
