/******************************************************************************
 SCLinearComp.h

	Interface for SCLinearComp class.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCLinearComp
#define _H_SCLinearComp

#include "SCComponent.h"

class SCCircuitVarList;

class SCLinearComp : public SCComponent
{
public:

	SCLinearComp(const SCComponentType type, SCCircuit* theCircuit, const JCharacter* name,
				 const JIndex posNode, const JIndex negNode);
	SCLinearComp(const SCLinearComp& source, SCCircuit* theCircuit,
				 const JArray<JIndex>* nodeMap);

	// construct from private file

	SCLinearComp(std::istream& input, const JFileVersion vers, const SCComponentType type,
				 SCCircuit* theCircuit);

	// construct from netlist

	SCLinearComp(std::istream& input, const SCComponentType type, SCCircuit* theCircuit);

	virtual ~SCLinearComp();

	const JString&	GetValueSymbol() const;
	void			SetValueSymbol(const JCharacter* symbol);
	const JString&	GetCurrentSymbol() const;
	virtual void	GetAuxEquations(JPtrArray<JString>* lhsList, JPtrArray<JString>* rhsList,
									JPtrArray<JString>* varList) const;
	virtual void	GetParserVariables(SCCircuitVarList* varList) const;

	JIndex	GetPositiveNode() const;
	void	SetPositiveNode(const JIndex node);
	JIndex	GetNegativeNode() const;
	void	SetNegativeNode(const JIndex node);

	virtual JSize	GetTerminalCount() const;
	virtual JIndex	GetTerminal(const JIndex index) const;
	virtual void	SetTerminal(const JIndex index, const JIndex node);
	virtual void	StreamOut(std::ostream& output) const;
	virtual void	PrintToNetlist(std::ostream& output) const;

	virtual JBoolean	RequiresNode(const JIndex index) const;

	// provides safe downcasting

	virtual SCLinearComp*		CastToSCLinearComp();
	virtual const SCLinearComp*	CastToSCLinearComp() const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JString*	itsValueSymbol;		// value for use in auxilliary equations
	JString*	itsCurrentSymbol;	// current for use in KCL equations
	JIndex		itsPosNode;			// positive node of device (current in)
	JIndex		itsNegNode;			// negative node of device (current out)

private:

	void	SCLinearCompX();

	// not allowed

	SCLinearComp(const SCLinearComp& source);
	const SCLinearComp& operator=(const SCLinearComp& source);
};

/******************************************************************************
 GetValueSymbol

 ******************************************************************************/

inline const JString&
SCLinearComp::GetValueSymbol()
	const
{
	return *itsValueSymbol;
}

/******************************************************************************
 GetCurrentSymbol

 ******************************************************************************/

inline const JString&
SCLinearComp::GetCurrentSymbol()
	const
{
	return *itsCurrentSymbol;
}

/******************************************************************************
 GetPositiveNode

 ******************************************************************************/

inline JIndex
SCLinearComp::GetPositiveNode()
	const
{
	return itsPosNode;
}

/******************************************************************************
 SetPositiveNode

 ******************************************************************************/

inline void
SCLinearComp::SetPositiveNode
	(
	const JIndex node
	)
{
	itsPosNode = node;
}

/******************************************************************************
 GetNegativeNode

 ******************************************************************************/

inline JIndex
SCLinearComp::GetNegativeNode()
	const
{
	return itsNegNode;
}

/******************************************************************************
 SetNegativeNode

 ******************************************************************************/

inline void
SCLinearComp::SetNegativeNode
	(
	const JIndex node
	)
{
	itsNegNode = node;
}

#endif
