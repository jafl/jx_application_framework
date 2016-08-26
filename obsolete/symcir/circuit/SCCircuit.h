/***************************************************************************************
 SCCircuit.h

	Interface for SCCircuit class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ***************************************************************************************/

#ifndef _H_SCCircuit
#define _H_SCCircuit

#include <JContainer.h>
#include <JPtrArray.h>
#include "SCComponentType.h"

class JString;
class SCCircuitVarList;
class SCComponent;
class SCLinearComp;
class SCACSource;
class SCVACSource;
class SCResistor;

class SCCircuit : public JContainer
{
	friend ostream& operator<<(ostream&, const SCCircuit&);

public:

	SCCircuit();
	SCCircuit(istream& input, const JFileVersion vers);	// private format
	SCCircuit(const JCharacter* fileName);				// netlist format
	SCCircuit(const SCCircuit& source);

	virtual ~SCCircuit();

	void	WriteNetlist(ostream& output) const;
	void	Insert(const SCCircuit& theCircuit, JArray<JIndex>* nodeMap);

	JBoolean	IsLinear() const;
	void		BuildVariableList(SCCircuitVarList* varList) const;
	JBoolean	Evaluate(const JCharacter* fn, JString* result) const;
	JBoolean	Evaluate(const JCharacter* fn, const JArray<JIndex>& inputList,
						 const JPtrArray<JString>& nullList, JString* result) const;
	JBoolean	GetInputImpedance(const JIndex posNode, const JIndex negNode, JString* Zin) const;
	JBoolean	ApplyEET(const JIndex inputIndex, const JString& outputFn,
						 const JIndex eeIndex,
						 JString* H0, JString* Hinf, JString* Zd, JString* Zn) const;
	JBoolean	GetFeedbackParameters(
					const JIndex inputIndex, const JString& outputFn,
					const JIndex depSourceIndex, JString* H0, JString* Hinf,
					JString* T, JString* Tn) const;

	JString		GetUniqueComponentName(const SCComponentType type) const;
	JBoolean	AddComponent(SCComponent* newComponent, JIndex* compIndex);
	JBoolean	RemoveComponent(const JIndex index, SCComponent** comp);
	JBoolean	DeleteComponent(const JIndex index);
	JBoolean	FindComponent(const JCharacter* name, JIndex* index) const;
	JBoolean	GetComponentIndex(const SCComponent* comp, JIndex* index) const;
	JBoolean	IsLinear(const JIndex compIndex) const;

	JSize				GetComponentCount() const;
	SCComponent*		GetComponent(const JIndex index);
	const SCComponent*	GetComponent(const JIndex index) const;

	JSize			GetNodeCount() const;
	const JString&	GetNodeName(const JIndex node) const;

	JIndex		CreateNode();
	JBoolean	FindNode(const JCharacter* nodeName, JIndex* index) const;
	JBoolean	RemoveNode(const JIndex index);
	JString		GetUniqueCoeffName(const JCharacter* prefix);

	JIndex		FindAddNode(const JCharacter* nodeName);		// for netlist parsing

	// the following routines are intended mainly for use by Components

	JIndex	GetNodeIndexForNetlist(const JIndex node) const;

	JCollection*	GetComponentList() const;
	JCollection*	GetNodeList() const;

	JBoolean	CompIndexValid(const JIndex index) const;
	JBoolean	NodeIndexValid(const JIndex index) const;

private:

	JString*	itsTitle;

	JPtrArray<JString>*		itsNodeNames;
	JPtrArray<SCComponent>*	itsComponents;

	JIndex	itsCoeffCounter;	// for generating unique coefficient names

private:

	void	SCCircuitX();
	void	ParseNetList(const JCharacter* fileName);
	void	GenerateEquations(JPtrArray<JString>* lhsList, JPtrArray<JString>* rhsList,
							  JPtrArray<JString>* varList) const;
	JBoolean	Evaluate(const JCharacter* fn, const JPtrArray<JString>& auxLHSList,
						 const JPtrArray<JString>& auxRHSList,
						 const JPtrArray<JString>& extraVars,
						 JString* result) const;

	JBoolean	InsertTestSource(const JIndex posNode, const JIndex negNode,
								 JIndex* newNode, SCVACSource** VT, SCResistor** RT);
	void		GetSourcesOffEquations(JPtrArray<JString>* lhsList,
									   JPtrArray<JString>* rhsList,
									   const SCACSource* inputSource) const;

	void	SetupDepVFeedbackCircuit(SCLinearComp* depSource, JPtrArray<JString>* extraVars,
									 JString* Vx, JString* Vy);
	void	SetupDepIFeedbackCircuit(SCLinearComp* depSource, JPtrArray<JString>* extraVars,
									 JString* Ix, JString* Iy);

	// not allowed

	SCCircuit& operator=(const SCCircuit& source);
};


/******************************************************************************
 GetComponentCount

 ******************************************************************************/

inline JSize
SCCircuit::GetComponentCount()
	const
{
	return itsComponents->GetElementCount();
}

/******************************************************************************
 GetComponent

 ******************************************************************************/

inline SCComponent*
SCCircuit::GetComponent
	(
	const JIndex index
	)
{
	return itsComponents->NthElement(index);
}

inline const SCComponent*
SCCircuit::GetComponent
	(
	const JIndex index
	)
	const
{
	return itsComponents->NthElement(index);
}

/******************************************************************************
 GetComponentIndex

 ******************************************************************************/

inline JBoolean
SCCircuit::GetComponentIndex
	(
	const SCComponent*	comp,
	JIndex*				index
	)
	const
{
	return itsComponents->Find(comp, index);
}

/******************************************************************************
 GetNodeCount

 ******************************************************************************/

inline JSize
SCCircuit::GetNodeCount()
	const
{
	return itsNodeNames->GetElementCount();
}

/******************************************************************************
 GetNodeName

 ******************************************************************************/

inline const JString&
SCCircuit::GetNodeName
	(
	const JIndex node
	)
	const
{
	return *(itsNodeNames->NthElement(node));
}

/******************************************************************************
 GetComponentList

 ******************************************************************************/

inline JCollection*
SCCircuit::GetComponentList()
	const
{
	return itsComponents;
}

/******************************************************************************
 GetNodeList

 ******************************************************************************/

inline JCollection*
SCCircuit::GetNodeList()
	const
{
	return itsNodeNames;
}

/******************************************************************************
 CompIndexValid

 ******************************************************************************/

inline JBoolean
SCCircuit::CompIndexValid
	(
	const JIndex index
	)
	const
{
	return JConvertToBoolean( 1 <= index && index <= itsComponents->GetElementCount() );
}

/******************************************************************************
 NodeIndexValid

 ******************************************************************************/

inline JBoolean
SCCircuit::NodeIndexValid
	(
	const JIndex index
	)
	const
{
	return JConvertToBoolean( 1 <= index && index <= itsNodeNames->GetElementCount() );
}

#endif
