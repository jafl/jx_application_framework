/******************************************************************************
 SCDirectorBase.h

	Interface for the SCDirectorBase class

	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCDirectorBase
#define _H_SCDirectorBase

#include <JXWindowDirector.h>

class SCCircuit;
class SCCircuitVarList;

class SCCircuitDocument;

class SCDirectorBase : public JXWindowDirector
{
public:

	SCDirectorBase(SCCircuitDocument* supervisor);
	SCDirectorBase(std::istream& input, const JFileVersion vers,
				   SCCircuitDocument* supervisor);

	virtual ~SCDirectorBase();

	SCCircuitDocument*		GetCircuitDocument() const;
	const SCCircuit*		GetCircuit() const;
	const SCCircuitVarList*	GetVarList() const;

	static SCDirectorBase*	StreamIn(std::istream& input, const JFileVersion vers,
									 SCCircuitDocument* supervisor);
	virtual void			StreamOut(std::ostream& output) const = 0;

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

	friend std::istream& operator>>(std::istream& input, Type& type);
	friend std::ostream& operator<<(std::ostream& output, const Type type);

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
