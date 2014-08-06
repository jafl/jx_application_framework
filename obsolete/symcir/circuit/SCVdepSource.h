/******************************************************************************
 SCVdepSource.h

	Interface for SCVdepSource class.

	Copyright � 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCVdepSource
#define _H_SCVdepSource

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "SCDepSource.h"

class SCVdepSource : public SCDepSource
{
public:

	SCVdepSource(const SCComponentType type, SCCircuit* theCircuit, const JCharacter* name,
				 const JIndex posNode, const JIndex negNode,
				 const JIndex posDepNode, const JIndex negDepNode,
				 const JFloat coeff);
	SCVdepSource(const SCVdepSource& source, SCCircuit* theCircuit,
				 const JArray<JIndex>* nodeMap, const JString& coeffSymbol);

	// construct from private file

	SCVdepSource(istream& input, const JFileVersion vers, const SCComponentType type,
				 SCCircuit* theCircuit);

	// construct from netlist

	SCVdepSource(istream& input, const SCComponentType type, SCCircuit* theCircuit);

	virtual ~SCVdepSource();

	JIndex	GetPosDepNode() const;
	void	SetPosDepNode(const JIndex node);
	JIndex	GetNegDepNode() const;
	void	SetNegDepNode(const JIndex node);

	virtual void		GetAuxEquations(JPtrArray<JString>* lhsList,
										JPtrArray<JString>* rhsList,
										JPtrArray<JString>* varList) const;
	virtual JBoolean	RequiresNode(const JIndex index) const;
	virtual void		StreamOut(ostream& output) const;
	virtual void		PrintToNetlist(ostream& output) const;

	// provides safe downcasting

	virtual SCVdepSource*		CastToSCVdepSource();
	virtual const SCVdepSource*	CastToSCVdepSource() const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JIndex	itsPosDepNode;		// positive node of controlling voltage
	JIndex	itsNegDepNode;		// negative node of controlling voltage

private:

	void	SCVdepSourceX();

	// not allowed

	SCVdepSource(const SCVdepSource& source);
	const SCVdepSource& operator=(const SCVdepSource& source);
};

/******************************************************************************
 GetPosDepNode

 ******************************************************************************/

inline JIndex
SCVdepSource::GetPosDepNode()
	const
{
	return itsPosDepNode;
}

/******************************************************************************
 SetPosDepNode

 ******************************************************************************/

inline void
SCVdepSource::SetPosDepNode
	(
	const JIndex node
	)
{
	itsPosDepNode = node;
}

/******************************************************************************
 GetNegDepNode

 ******************************************************************************/

inline JIndex
SCVdepSource::GetNegDepNode()
	const
{
	return itsNegDepNode;
}

/******************************************************************************
 SetNegDepNode

 ******************************************************************************/

inline void
SCVdepSource::SetNegDepNode
	(
	const JIndex node
	)
{
	itsNegDepNode = node;
}

#endif
