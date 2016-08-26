/******************************************************************************
 SCIdepSource.h

	Interface for SCIdepSource class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCIdepSource
#define _H_SCIdepSource

#include "SCDepSource.h"

class SCIdepSource : public SCDepSource
{
public:

	SCIdepSource(const SCComponentType type, SCCircuit* theCircuit, const JCharacter* name,
				 const JIndex posNode, const JIndex negNode,
				 const JIndex depCompIndex, const JFloat coeff);
	SCIdepSource(const SCIdepSource& source, SCCircuit* theCircuit,
				 const JArray<JIndex>* nodeMap, const JString& coeffSymbol);

	// construct from private file

	SCIdepSource(istream& input, const JFileVersion vers, const SCComponentType type,
				 SCCircuit* theCircuit);

	// construct from netlist

	SCIdepSource(istream& input, const SCComponentType type, SCCircuit* theCircuit);

	virtual ~SCIdepSource();

	JBoolean	GetDepCompIndex(JIndex* compIndex) const;
	void		SetDepCompIndex(const JIndex compIndex);

	virtual void		GetAuxEquations(JPtrArray<JString>* lhsList,
										JPtrArray<JString>* rhsList,
										JPtrArray<JString>* varList) const;
	virtual JBoolean	RequiresComponent(const JIndex index) const;
	virtual void		StreamOut(ostream& output) const;
	virtual void		PrintToNetlist(ostream& output) const;

	// provides safe downcasting

	virtual SCIdepSource*		CastToSCIdepSource();
	virtual const SCIdepSource*	CastToSCIdepSource() const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JIndex	itsDepCompIndex;	// index of controlling component (0 => off)

private:

	void	SCIdepSourceX();

	// not allowed

	SCIdepSource(const SCIdepSource& source);
	const SCIdepSource& operator=(const SCIdepSource& source);
};

#endif
