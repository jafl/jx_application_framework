/******************************************************************************
 SCDirectorBase.h

	Interface for the SCDirectorBase class

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCDirectorBase
#define _H_SCDirectorBase

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>

class SCCircuit;
class SCCircuitVarList;

class SCCircuitDocument;

class SCDirectorBase : public JXWindowDirector
{
public:

	SCDirectorBase(SCCircuitDocument* supervisor);
	SCDirectorBase(istream& input, const JFileVersion vers,
				   SCCircuitDocument* supervisor);

	virtual ~SCDirectorBase();

	SCCircuitDocument*		GetCircuitDocument() const;
	const SCCircuit*		GetCircuit() const;
	const SCCircuitVarList*	GetVarList() const;

	static SCDirectorBase*	StreamIn(istream& input, const JFileVersion vers,
									 SCCircuitDocument* supervisor);
	virtual void			StreamOut(ostream& output) const = 0;

protected:

	// Do not change these values once they are assigned
	// because they are stored in files

	enum Type
	{
		kXferFnType   = 0,
		kZinType      = 1,
		kEETType      = 2,
		kFeedbackType = 3,
		kNDIType      = 4,
		kScratchType  = 5
	};

	friend istream& operator>>(istream& input, Type& type);
	friend ostream& operator<<(ostream& output, const Type type);

private:

	SCCircuitDocument*	itsDoc;		// owns us

private:

	void	SCDirectorBaseX(SCCircuitDocument* doc);

	// not allowed

	SCDirectorBase(const SCDirectorBase& source);
	const SCDirectorBase& operator=(const SCDirectorBase& source);
};


/******************************************************************************
 GetCircuitDocument

 ******************************************************************************/

inline SCCircuitDocument*
SCDirectorBase::GetCircuitDocument()
	const
{
	return itsDoc;
}

#endif
