/******************************************************************************
 SCDepSource.h

	Interface for SCDepSource class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCDepSource
#define _H_SCDepSource

#include "SCLinearComp.h"

class SCDepSource : public SCLinearComp
{
public:

	SCDepSource(const SCComponentType type, SCCircuit* theCircuit, const JCharacter* name,
				const JIndex posNode, const JIndex negNode, const JFloat coeff);
	SCDepSource(const SCDepSource& source, SCCircuit* theCircuit,
				const JArray<JIndex>* nodeMap, const JString& coeffSymbol);

	// construct from private file

	SCDepSource(istream& input, const JFileVersion vers, const SCComponentType type,
				SCCircuit* theCircuit);

	// construct from netlist

	SCDepSource(istream& input, const SCComponentType type, SCCircuit* theCircuit);

	virtual ~SCDepSource();

	const JString&	GetCoeffSymbol() const;
	JFloat			GetCoeffValue() const;
	void			SetCoeffValue(const JFloat coeff);

	virtual void	StreamOut(ostream& output) const;
	virtual void	GetParserVariables(SCCircuitVarList* varList) const;

	// provides safe downcasting

	virtual SCDepSource*		CastToSCDepSource();
	virtual const SCDepSource*	CastToSCDepSource() const;

protected:

	void	SetCoeffSymbol(const JCharacter* symbol);

private:

	JString*	itsCoeffSymbol;		// gain coefficient
	JFloat		itsCoeffValue;

private:

	void	SCDepSourceX();

	// not allowed

	SCDepSource(const SCDepSource& source);
	const SCDepSource& operator=(const SCDepSource& source);
};

/******************************************************************************
 GetCoeffSymbol

 ******************************************************************************/

inline const JString&
SCDepSource::GetCoeffSymbol()
	const
{
	return *itsCoeffSymbol;
}

/******************************************************************************
 GetCoeffValue

 ******************************************************************************/

inline JFloat
SCDepSource::GetCoeffValue()
	const
{
	return itsCoeffValue;
}

/******************************************************************************
 SetCoeffValue

 ******************************************************************************/

inline void
SCDepSource::SetCoeffValue
	(
	const JFloat coeff
	)
{
	itsCoeffValue = coeff;
}

#endif
