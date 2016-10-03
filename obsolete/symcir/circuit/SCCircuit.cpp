/******************************************************************************
 SCCircuit.cpp

	This class represents a linear circuit.  We are primarily a collection of
	Components, so this is what GetElementCount() returns.

	In some cases, Spice's restrictions are too severe in the sense that
	situations are not allowed, but it is obvious what the user expects.
	In these cases, we allow the operations.  If the user wants to feed the
	circuit to Spice later, they have to clean up their mess themselves.

	BASE CLASS = JContainer

	Copyright (C) 1995 John Lindal. All rights reserved.

 ******************************************************************************/

#include "SCCircuit.h"
#include "SCCircuitVarList.h"

#include "SCVACSource.h"
#include "SCResistor.h"
#include "SCShortCircuit.h"

#include <JSymbolicMath.h>
#include <JString.h>
#include <jFStreamUtil.h>
#include <jStreamUtil.h>
#include <jMath.h>
#include <jAssert.h>

JSymbolicMath*	SCGetSymbolicMath();

const JSize kNodeNamePrefixLength = 2;
static const JCharacter* kNodeNamePrefix = "NV";
static const JCharacter* kGroundNodeName = "NV0";

const JIndex kGroundNodeIndex = 1;
const JIndex kFirstKCLNode    = 2;

/******************************************************************************
 Constructor

 ******************************************************************************/

SCCircuit::SCCircuit()
	:
	JContainer()
{
	SCCircuitX();
}

SCCircuit::SCCircuit
	(
	istream&			input,
	const JFileVersion	vers
	)
	:
	JContainer()
{
JIndex i;

	SCCircuitX();

	itsNodeNames->DeleteAll();
	itsComponents->DeleteAll();

	input >> *itsTitle;
	input >> itsCoeffCounter;

	JSize nodeCount;
	input >> nodeCount;

	for (i=1; i<=nodeCount; i++)
		{
		JString* nodeName = new JString;
		assert( nodeName != NULL );
		input >> *nodeName;
		itsNodeNames->Append(nodeName);
		}

	JSize componentCount;
	input >> componentCount;

	for (i=1; i<=componentCount; i++)
		{
		SCComponent* comp = SCComponent::StreamIn(input, vers, this);
		itsComponents->Append(comp);
		}
}

SCCircuit::SCCircuit
	(
	const JCharacter* fileName
	)
	:
	JContainer()
{
	SCCircuitX();
	ParseNetList(fileName);
}

// private

void
SCCircuit::SCCircuitX()
{
	itsTitle = new JString;
	assert( itsTitle != NULL );

	itsNodeNames = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsNodeNames != NULL );

	itsNodeNames->Append(kGroundNodeName);

	itsComponents = new JPtrArray<SCComponent>(JPtrArrayT::kDeleteAll);
	assert( itsComponents != NULL );

	InstallOrderedSet(itsComponents);

	itsCoeffCounter = 0;
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

SCCircuit::SCCircuit
	(
	const SCCircuit& source
	)
	:
	JContainer()
{
	itsTitle = new JString(*(source.itsTitle));
	assert( itsTitle != NULL );

	// We don't copy the counter because the Components will ask us for
	// new coefficient names.  We do it here so that the new Components
	// can get coefficients.

	itsCoeffCounter = 0;

	// duplicate the nodes

	itsNodeNames = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsNodeNames != NULL );
	{
	const JSize nodeCount = (source.itsNodeNames)->GetElementCount();
	for (JIndex i=1; i<=nodeCount; i++)
		{
		const JString* origName = (source.itsNodeNames)->NthElement(i);
		itsNodeNames->Append(*origName);
		}
	}

	// duplicate the SCComponent objects into our circuit

	itsComponents = new JPtrArray<SCComponent>(JPtrArrayT::kDeleteAll);
	assert( itsComponents != NULL );

	InstallOrderedSet(itsComponents);
	{
	const JSize compCount = (source.itsComponents)->GetElementCount();
	for (JIndex i=1; i<=compCount; i++)
		{
		const SCComponent* origComp = (source.itsComponents)->NthElement(i);
		SCComponent* newComp = origComp->Copy(this);
		itsComponents->Append(newComp);
		}
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCCircuit::~SCCircuit()
{
	delete itsTitle;
	delete itsNodeNames;
	delete itsComponents;
}

/******************************************************************************
 Evaluate

	This version allows N nulled signals, given N+1 independent sources.

 ******************************************************************************/

JBoolean
SCCircuit::Evaluate
	(
	const JCharacter*			fn,
	const JArray<JIndex>&		inputList,
	const JPtrArray<JString>&	nullList,
	JString*					result
	)
	const
{
JIndex i;

	if (IsEmpty() || !IsLinear())
		{
		return kJFalse;
		}

	const JSize inputCount = inputList.GetElementCount();
	const JSize nullCount  = nullList.GetElementCount();
	if (inputCount != nullCount + 1)
		{
		return kJFalse;
		}

	for (i=1; i<=inputCount; i++)
		{
		const JIndex inputIndex = inputList.GetElement(i);
		if (!CompIndexValid(inputIndex) ||
			(GetComponent(inputIndex))->CastToSCACSource() == NULL)
			{
			return kJFalse;
			}
		}

	// turn off the other sources

	JPtrArray<JString> lhsList(JPtrArrayT::kDeleteAll),
					   rhsList(JPtrArrayT::kDeleteAll),
					   extraVars(JPtrArrayT::kDeleteAll);

	const JSize compCount = GetElementCount();
	for (i=1; i<=compCount; i++)
		{
		JBoolean found = kJFalse;
		for (JIndex j=1; j<=inputCount; j++)
			{
			if (i == inputList.GetElement(j))
				{
				found = kJTrue;
				break;
				}
			}

		const SCACSource* comp = (GetComponent(i))->CastToSCACSource();
		if (!found && comp != NULL)
			{
			JString* lhs = new JString(comp->GetValueSymbol());
			assert( lhs != NULL );
			lhsList.Append(lhs);

			JString* rhs = new JString("0");
			assert( rhs != NULL );
			rhsList.Append(rhs);
			}
		}

	// calculate N-null, N+1-injection transfer function

	for (i=1; i<=nullCount; i++)
		{
		const JString* nullFn = nullList.NthElement(i);

		// null signals

		JString* lhs = new JString(*nullFn);
		assert( lhs != NULL );
		lhsList.Append(lhs);

		JString* rhs = new JString("0");
		assert( rhs != NULL );
		rhsList.Append(rhs);

		// solve for values of all but one source

		const JIndex inputIndex = inputList.GetElement(i);
		const SCACSource* source = (GetComponent(inputIndex))->CastToSCACSource();
		assert( source != NULL );
		JString* extraVar = new JString(source->GetValueSymbol());
		assert( extraVar != NULL );
		extraVars.Append(extraVar);
		}

	return Evaluate(fn, lhsList, rhsList, extraVars, result);
}

/******************************************************************************
 ApplyEET

	The extra element must be a passive component.

	H = H0 * (1 + Z/Zd)/(1 + Z/Zn) = Hinf * (1 + Zd/Z)/(1 + Zn/Z)

 ******************************************************************************/

JBoolean
SCCircuit::ApplyEET
	(
	const JIndex	inputIndex,
	const JString&	outputFn,
	const JIndex	eeIndex,
	JString*		H0,
	JString*		Hinf,
	JString*		Zd,
	JString*		Zn
	)
	const
{
JBoolean ok;

	if (IsEmpty() || !IsLinear() ||
		!CompIndexValid(inputIndex) || !CompIndexValid(eeIndex))
		{
		return kJFalse;
		}

	if ((GetComponent(inputIndex))->CastToSCACSource() == NULL)
		{
		return kJFalse;
		}

	const SCComponent* eeComp = GetComponent(eeIndex);
	const SCPassiveLinearComp* passiveComp = eeComp->CastToSCPassiveLinearComp();
	if (passiveComp == NULL)
		{
		return kJFalse;
		}

	const JIndex node1 = eeComp->GetTerminal(1);
	const JIndex node2 = eeComp->GetTerminal(2);

	// copy the circuit and remove the extra element

	SCCircuit c = *this;

	const SCACSource* inputSource = (c.GetComponent(inputIndex))->CastToSCACSource();
	if (inputSource == NULL)
		{
		return kJFalse;
		}

	if (!c.DeleteComponent(eeIndex))
		{
		return kJFalse;
		}

	// turn off the other sources

	JPtrArray<JString> lhsList(JPtrArrayT::kDeleteAll),
					   rhsList(JPtrArrayT::kDeleteAll),
					   extraVars(JPtrArrayT::kDeleteAll);
	c.GetSourcesOffEquations(&lhsList, &rhsList, inputSource);

	// get the transfer function without the extra element

	JString xferFn = "(";
	xferFn += outputFn;
	xferFn += ")/(";
	xferFn += inputSource->GetValueSymbol();
	xferFn += ")";

	if (!c.Evaluate(xferFn, lhsList, rhsList, extraVars, Hinf))
		{
		return kJFalse;
		}

	// get the transfer function with the extra element shorted

	const JString shortName      = c.GetUniqueComponentName(kShortCircuit);
	SCShortCircuit* shortCircuit = new SCShortCircuit(&c, shortName, node1, node2);
	assert( shortCircuit != NULL );

	JIndex shortIndex;
	ok = c.AddComponent(shortCircuit, &shortIndex);
	assert( ok );

	if (!c.Evaluate(xferFn, lhsList, rhsList, extraVars, H0))
		{
		return kJFalse;
		}

	ok = c.DeleteComponent(shortIndex);
	assert( ok );

	// insert the test source

	JIndex testNode;
	SCVACSource* VT;
	SCResistor* RT;
	ok = c.InsertTestSource(node1, node2, &testNode, &VT, &RT);
	assert( ok );

	// calculate Zd

	xferFn  = "(";
	xferFn += c.GetNodeName(node1);
	xferFn += " - ";
	xferFn += c.GetNodeName(node2);
	xferFn += ")/(-(";
	xferFn += VT->GetCurrentSymbol();
	xferFn += "))";

	JString* lhs = new JString(inputSource->GetValueSymbol());
	assert( lhs != NULL );
	lhsList.Append(lhs);

	JString* rhs = new JString("0");
	assert( rhs != NULL );
	rhsList.Append(rhs);

	if (!c.Evaluate(xferFn, lhsList, rhsList, extraVars, Zd))
		{
		return kJFalse;
		}

	// leave the sources off

	lhsList.DeleteElement(lhsList.GetElementCount());
	rhsList.DeleteElement(rhsList.GetElementCount());

	// calculate Zn

	lhs = new JString(outputFn);
	assert( lhs != NULL );
	lhsList.Append(lhs);

	rhs = new JString("0");
	assert( rhs != NULL );
	rhsList.Append(rhs);

	JString* extraVar = new JString(VT->GetValueSymbol());
	assert( extraVar != NULL );
	extraVars.Append(extraVar);

	if (!c.Evaluate(xferFn, lhsList, rhsList, extraVars, Zn))
		{
		return kJFalse;
		}

	// assemble the final result
/*
	enum RefValue
	{
		kOpen,
		kShort
	};

	JString F1,F2,F3;

	if (refValue == kOpen)
		{
		const JString Z = passiveComp->GetImpedanceSymbol();

		F1 = "(" + *Zn + ")/(" + Z + ")";
		if (!(SCGetSymbolicMath())->Simplify(F1, &F2))
			{
			return kJFalse;
			}

		F1 = "(" + *Zd + ")/(" + Z + ")";
		if (!(SCGetSymbolicMath())->Simplify(F1, &F3))
			{
			return kJFalse;
			}
		}
	else
		{
		assert( refValue == kShort );

		const JString Z = passiveComp->GetImpedanceSymbol();

		F1 = "(" + Z + ")/(" + *Zn + ")";
		if (!(SCGetSymbolicMath())->Simplify(F1, &F2))
			{
			return kJFalse;
			}

		F1 = "(" + Z + ")/(" + *Zd + ")";
		if (!(SCGetSymbolicMath())->Simplify(F1, &F3))
			{
			return kJFalse;
			}
		}

	*F = "(1 + (" + F2 + "))/(1 + (" + F3 + "))";
*/
	return kJTrue;
}

/******************************************************************************
 GetFeedbackParameters

	Calculate the four feedback parameters H0, Hinf, T, Tn.

	H = H0 * (1 + Tn)/(1 + T) = Hinf * (1 + 1/Tn)/(1 + 1/T)

 ******************************************************************************/

JBoolean
SCCircuit::GetFeedbackParameters
	(
	const JIndex	inputIndex,
	const JString&	outputFn,
	const JIndex	depSourceIndex,
	JString*		H0,
	JString*		Hinf,
	JString*		T,
	JString*		Tn
	)
	const
{
	if (IsEmpty() || !IsLinear() ||
		!CompIndexValid(inputIndex) || !CompIndexValid(depSourceIndex))
		{
		return kJFalse;
		}

	if ((GetComponent(inputIndex))->CastToSCACSource() == NULL)
		{
		return kJFalse;
		}

	// copy the circuit

	SCCircuit c = *this;

	const SCACSource* inputSource = (c.GetComponent(inputIndex))->CastToSCACSource();
	if (inputSource == NULL)
		{
		return kJFalse;
		}

	// turn off the other sources

	JPtrArray<JString> lhsList(JPtrArrayT::kDeleteAll),
					   rhsList(JPtrArrayT::kDeleteAll);
	c.GetSourcesOffEquations(&lhsList, &rhsList, inputSource);

	// insert the second source

	SCLinearComp* depSource = (c.GetComponent(depSourceIndex))->CastToSCLinearComp();
	if (depSource == NULL)
		{
		return kJFalse;
		}

	JString sigX, sigY;
	JPtrArray<JString> extraVars(JPtrArrayT::kDeleteAll);

	const SCComponentType depSourceType = depSource->GetType();
	if (depSourceType == kVdepVSource || depSourceType == kIdepVSource)
		{
		c.SetupDepVFeedbackCircuit(depSource, &extraVars, &sigX, &sigY);
		}
	else if (depSourceType == kVdepISource || depSourceType == kIdepISource)
		{
		c.SetupDepIFeedbackCircuit(depSource, &extraVars, &sigX, &sigY);
		}
	else
		{
		return kJFalse;
		}

	// calculate H0

	JString xferFn = "(";
	xferFn += outputFn;
	xferFn += ")/(";
	xferFn += inputSource->GetValueSymbol();
	xferFn += ")";

	JString* lhs = new JString(sigX);
	assert( lhs != NULL );
	lhsList.Append(lhs);

	JString* rhs = new JString("0");
	assert( rhs != NULL );
	rhsList.Append(rhs);

	if (!c.Evaluate(xferFn, lhsList, rhsList, extraVars, H0))
		{
		return kJFalse;
		}

	// leave the sources off

	lhsList.DeleteElement(lhsList.GetElementCount());
	rhsList.DeleteElement(rhsList.GetElementCount());

	// calculate Hinf

	lhs = new JString(sigY);
	assert( lhs != NULL );
	lhsList.Append(lhs);

	rhs = new JString("0");
	assert( rhs != NULL );
	rhsList.Append(rhs);

	if (!c.Evaluate(xferFn, lhsList, rhsList, extraVars, Hinf))
		{
		return kJFalse;
		}

	// leave the sources off

	lhsList.DeleteElement(lhsList.GetElementCount());
	rhsList.DeleteElement(rhsList.GetElementCount());

	// calculate T

	xferFn  = "(";
	xferFn += sigY;
	xferFn += ")/(";
	xferFn += sigX;
	xferFn += ")";

	lhs = new JString(inputSource->GetValueSymbol());
	assert( lhs != NULL );
	lhsList.Append(lhs);

	rhs = new JString("0");
	assert( rhs != NULL );
	rhsList.Append(rhs);

	const JPtrArray<JString> emptyExtraVars(JPtrArrayT::kDeleteAll);
	if (!c.Evaluate(xferFn, lhsList, rhsList, emptyExtraVars, T))
		{
		return kJFalse;
		}

	// leave the sources off

	lhsList.DeleteElement(lhsList.GetElementCount());
	rhsList.DeleteElement(rhsList.GetElementCount());

	// calculate Tn

	lhs = new JString(outputFn);
	assert( lhs != NULL );
	lhsList.Append(lhs);

	rhs = new JString("0");
	assert( rhs != NULL );
	rhsList.Append(rhs);

	if (!c.Evaluate(xferFn, lhsList, rhsList, extraVars, Tn))
		{
		return kJFalse;
		}

	// assemble the final result
/*
	JString F2,F3;

	JString F1 = "1/(" + *Tn + ")";
	if (!(SCGetSymbolicMath())->Simplify(F1, &F2))
		{
		return kJFalse;
		}

	F1 = "1/(" + *T + ")";
	if (!(SCGetSymbolicMath())->Simplify(F1, &F3))
		{
		return kJFalse;
		}

	*F = "(1 + (" + F2 + "))/(1 + (" + F3 + "))";
*/
	return kJTrue;
}

/******************************************************************************
 SetupDepVFeedbackCircuit (private)

	Insert an independent voltage source in series with the given dependent
	voltage source.

 ******************************************************************************/

void
SCCircuit::SetupDepVFeedbackCircuit
	(
	SCLinearComp*		depSource,
	JPtrArray<JString>*	extraVars,
	JString*			Vx,
	JString*			Vy
	)
{
	const JIndex origNode1 = depSource->GetPositiveNode();
	const JIndex node2     = depSource->GetNegativeNode();

	// insert the second independent source

	const JIndex node1 = CreateNode();
	depSource->SetPositiveNode(node1);

	JIndex testNode;
	SCVACSource* VT;
	SCResistor* RT;
	const JBoolean ok = InsertTestSource(node1, origNode1, &testNode, &VT, &RT);
	assert( ok );

	JString* elimVar = new JString(VT->GetValueSymbol());
	assert( elimVar != NULL );
	extraVars->Append(elimVar);

	// define Vx and Vy

	*Vx  = GetNodeName(origNode1);
	*Vx += " - ";
	*Vx += GetNodeName(node2);

	*Vy  = "-(";
	*Vy += depSource->GetValueSymbol();
	*Vy += ")";
}

/******************************************************************************
 SetupDepIFeedbackCircuit (private)

	Insert an independent current source in parallel with the given dependent
	current source.  Since we need to use a nonideal source, we can use the
	thevenin equivalent.

 ******************************************************************************/

void
SCCircuit::SetupDepIFeedbackCircuit
	(
	SCLinearComp*		depSource,
	JPtrArray<JString>*	extraVars,
	JString*			Ix,
	JString*			Iy
	)
{
	const JIndex node1 = depSource->GetPositiveNode();
	const JIndex node2 = depSource->GetNegativeNode();

	// insert the second independent source

	JIndex testNode;
	SCVACSource* VT;
	SCResistor* RT;
	const JBoolean ok = InsertTestSource(node1, node2, &testNode, &VT, &RT);
	assert( ok );

	JString* elimVar = new JString(VT->GetValueSymbol());
	assert( elimVar != NULL );
	extraVars->Append(elimVar);

	// define Ix and Iy

	*Ix = depSource->GetCurrentSymbol();
	*Ix += " + ";
	*Ix += VT->GetCurrentSymbol();

	*Iy  = "-(";
	*Iy += depSource->GetCurrentSymbol();
	*Iy += ")";
}

/******************************************************************************
 GetInputImpedance

 ******************************************************************************/

JBoolean
SCCircuit::GetInputImpedance
	(
	const JIndex	posNode,
	const JIndex	negNode,
	JString*		Zin
	)
	const
{
	if (IsEmpty() || !IsLinear() ||
		!NodeIndexValid(posNode) || !NodeIndexValid(negNode))
		{
		return kJFalse;
		}

	// If there are any sources across the input nodes,
	// we have to remove them.

	SCCircuit c = *this;
	{
	const JSize compCount = c.GetElementCount();
	for (JIndex i=compCount; i>=1; i--)
		{
		const SCACSource* comp = (c.GetComponent(i))->CastToSCACSource();
		if (comp != NULL)
			{
			const JIndex posNode1 = comp->GetPositiveNode();
			const JIndex negNode1 = comp->GetNegativeNode();
			if ((posNode == posNode1 && negNode == negNode1) ||
				(posNode == negNode1 && negNode == posNode1))
				{
				if (!c.DeleteComponent(i))
					{
					return kJFalse;
					}
				}
			}
		}
	}

	// turn off the other sources

	JPtrArray<JString> lhsList(JPtrArrayT::kDeleteAll),
					   rhsList(JPtrArrayT::kDeleteAll),
					   extraVars(JPtrArrayT::kDeleteAll);
	c.GetSourcesOffEquations(&lhsList, &rhsList, NULL);

	// insert the test source

	JIndex testNode;
	SCVACSource* VT;
	SCResistor* RT;
	const JBoolean ok = c.InsertTestSource(posNode, negNode, &testNode, &VT, &RT);
	assert( ok );

	// calculate Zin

	JString xferFn = "(";
	xferFn += c.GetNodeName(posNode);
	xferFn += " - ";
	xferFn += c.GetNodeName(negNode);
	xferFn += ")/(-(";
	xferFn += VT->GetCurrentSymbol();
	xferFn += "))";

	return c.Evaluate(xferFn, lhsList, rhsList, extraVars, Zin);
}

/******************************************************************************
 InsertTestSource (private)

   pos    I -->   neg
	O--(+-)--/\/\--0
	    VT    RT

 ******************************************************************************/

JBoolean
SCCircuit::InsertTestSource
	(
	const JIndex	posNode,
	const JIndex	negNode,
	JIndex*			newNode,
	SCVACSource**	VT,
	SCResistor**	RT
	)
{
JIndex i;

	if (!NodeIndexValid(posNode) || !NodeIndexValid(negNode))
		{
		return kJFalse;
		}

	*newNode = CreateNode();

	const JString testVSourceName = GetUniqueComponentName(kVACSource);
	*VT = new SCVACSource(this, testVSourceName, posNode, *newNode, 1.0, 0.0);
	assert( *VT != NULL );
	JBoolean ok = AddComponent(*VT, &i);
	assert( ok );

	const JString testResistorName = GetUniqueComponentName(kResistor);
	*RT = new SCResistor(this, testResistorName, *newNode, negNode, 1.0);
	assert( *RT != NULL );
	ok = AddComponent(*RT, &i);
	assert( ok );

	return kJTrue;
}

/******************************************************************************
 GetSourcesOffEquations (private)

	Builds equations to turn off all sources except the given inputSource.
	inputSource can be NULL.

 ******************************************************************************/

void
SCCircuit::GetSourcesOffEquations
	(
	JPtrArray<JString>*	lhsList,
	JPtrArray<JString>*	rhsList,
	const SCACSource*	inputSource
	)
	const
{
	const JSize compCount = GetElementCount();
	for (JIndex i=1; i<=compCount; i++)
		{
		const SCACSource* comp = (GetComponent(i))->CastToSCACSource();
		if (comp != NULL && comp != inputSource)
			{
			JString* lhs = new JString(comp->GetValueSymbol());
			assert( lhs != NULL );
			lhsList->Append(lhs);

			JString* rhs = new JString("0");
			assert( rhs != NULL );
			rhsList->Append(rhs);
			}
		}
}

/******************************************************************************
 Evaluate

 ******************************************************************************/

JBoolean
SCCircuit::Evaluate
	(
	const JCharacter*	fn,
	JString*			result
	)
	const
{
	JPtrArray<JString> lhs(JPtrArrayT::kDeleteAll),
					   rhs(JPtrArrayT::kDeleteAll),
					   vars(JPtrArrayT::kDeleteAll);
	return Evaluate(fn, lhs,rhs,vars, result);
}

// private

JBoolean
SCCircuit::Evaluate
	(
	const JCharacter*			fn,
	const JPtrArray<JString>&	auxLHSList,
	const JPtrArray<JString>&	auxRHSList,
	const JPtrArray<JString>&	extraVars,
	JString*					result
	)
	const
{
	const JSize auxEqCount = auxLHSList.GetElementCount();
	if (IsEmpty() || !IsLinear() || auxEqCount != auxRHSList.GetElementCount())
		{
		return kJFalse;
		}

	// build the list of circuit equations

	JPtrArray<JString> lhsList(JPtrArrayT::kDeleteAll),
					   rhsList(JPtrArrayT::kDeleteAll),
					   solveVars(JPtrArrayT::kDeleteAll);
	GenerateEquations(&lhsList, &rhsList, &solveVars);

	// tag on the auxilliary equations supplied by the caller
	{
	for (JIndex i=1; i<=auxEqCount; i++)
		{
		JString* lhs = new JString(*(auxLHSList.NthElement(i)));
		assert( lhs != NULL );
		lhsList.Append(lhs);

		JString* rhs = new JString(*(auxRHSList.NthElement(i)));
		assert( rhs != NULL );
		rhsList.Append(rhs);
		}
	}

	// append the node voltages to the list of variables to solve for
	{
	const JSize nodeCount = itsNodeNames->GetElementCount();
	for (JIndex i=1; i<=nodeCount; i++)
		{
		solveVars.Append(*(itsNodeNames->NthElement(i)));
		}
	}

	// append the extra variables to the list of variables to solve for
	{
	const JSize varCount = extraVars.GetElementCount();
	for (JIndex i=1; i<=varCount; i++)
		{
		solveVars.Append(*(extraVars.NthElement(i)));
		}
	}

	// solve the circuit equations

	JString soln;
	const JBoolean ok = (SCGetSymbolicMath())->
		SolveEquations(lhsList, rhsList, solveVars, &soln);

	// evaluate the given function

	if (ok)
		{
		return (SCGetSymbolicMath())->Evaluate(fn, soln, result);
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 BuildVariableList

	Fills in the given variable list with variables required to parse
	expressions relating to our circuit.

 ******************************************************************************/

void
SCCircuit::BuildVariableList
	(
	SCCircuitVarList* varList
	)
	const
{
JIndex i;

	const JSize nodeCount = itsNodeNames->GetElementCount();
	for (i=1; i<=nodeCount; i++)
		{
		varList->AddVariable(*(itsNodeNames->NthElement(i)), 0.0, kJFalse);
		}

	const JSize compCount = itsComponents->GetElementCount();
	for (i=1; i<=compCount; i++)
		{
		SCLinearComp* comp = (itsComponents->NthElement(i))->CastToSCLinearComp();
		assert( comp != NULL );
		comp->GetParserVariables(varList);
		}
}

/******************************************************************************
 GetUniqueComponentName

	Return an unused name for the given component type.

 ******************************************************************************/

JString
SCCircuit::GetUniqueComponentName
	(
	const SCComponentType type
	)
	const
{
	JString namePrefix = "*";
	namePrefix.SetCharacter(1, SCComponent::GetNamePrefix(type));

	JString uniqueName;
	JIndex i = GetElementCount();
	JIndex j;
	do
		{
		i++;
		uniqueName = namePrefix + JString(i);
		}
		while (FindComponent(uniqueName, &j));

	return uniqueName;
}

/******************************************************************************
 AddComponent

	We require that the SCComponent's name be unique.
	Returns kJFalse if this is not the case.

 ******************************************************************************/

JBoolean
SCCircuit::AddComponent
	(
	SCComponent*	newComponent,
	JIndex*			compIndex
	)
{
	*compIndex = 0;

	JIndex i;
	const JBoolean exists = FindComponent(newComponent->GetName(), &i);
	if (!exists)
		{
		itsComponents->Append(newComponent);
		*compIndex = itsComponents->GetElementCount();
		}
	return !exists;
}

/******************************************************************************
 RemoveComponent

	If no other component depends on the specified component,
	we remove the component from the circuit and return a pointer to it.

 ******************************************************************************/

JBoolean
SCCircuit::RemoveComponent
	(
	const JIndex	index,
	SCComponent**	comp
	)
{
	const JSize compCount = GetElementCount();
	for (JIndex i=1; i<=compCount; i++)
		{
		if (i != index && (GetComponent(i))->RequiresComponent(index))
			{
			*comp = NULL;
			return kJFalse;
			}
		}

	*comp = itsComponents->NthElement(index);
	itsComponents->RemoveElement(index);
	return kJTrue;
}

/******************************************************************************
 DeleteComponent

	If no other component depends on the specified component,
	we remove the component from the circuit and delete the object.

 ******************************************************************************/

JBoolean
SCCircuit::DeleteComponent
	(
	const JIndex index
	)
{
	SCComponent* comp = NULL;
	if (RemoveComponent(index, &comp))
		{
		delete comp;
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 FindComponent

	Returns kJTrue if we find a component with the given name.

 ******************************************************************************/

JBoolean
SCCircuit::FindComponent
	(
	const JCharacter*	name,
	JIndex*				index
	)
	const
{
	const JSize compCount = itsComponents->GetElementCount();
	for (JIndex i=1; i<=compCount; i++)
		{
		if ((GetComponent(i))->GetName() == name)
			{
			*index = i;
			return kJTrue;
			}
		}
	return kJFalse;
}

/******************************************************************************
 IsLinear

	Returns kJTrue if the entire circuit is linear.

 ******************************************************************************/

JBoolean
SCCircuit::IsLinear()
	const
{
	const JSize compCount = itsComponents->GetElementCount();
	for (JIndex i=1; i<=compCount; i++)
		{
		if (!IsLinear(i))
			{
			return kJFalse;
			}
		}
	return kJTrue;
}

/******************************************************************************
 IsLinear

	Returns kJTrue if the specified component is linear.

 ******************************************************************************/

JBoolean
SCCircuit::IsLinear
	(
	const JIndex compIndex
	)
	const
{
	SCComponent* comp = itsComponents->NthElement(compIndex);
	return JConvertToBoolean( comp->CastToSCLinearComp() != NULL );
}

/******************************************************************************
 CreateNode

	Create a new node and return its index.

 ******************************************************************************/

JIndex
SCCircuit::CreateNode()
{
	JString* newName = new JString;
	assert( newName != NULL );

	JIndex i = GetNodeCount();
	JIndex j;
	do
		{
		i++;
		*newName = kNodeNamePrefix + JString(i);
		}
		while (FindNode(*newName, &j));

	itsNodeNames->Append(newName);
	return GetNodeCount();
}

/******************************************************************************
 FindAddNode

	Search for the specified node.  If we can't find it, we add it.

	*** This is only for use while parsing a netlist.

 ******************************************************************************/

JIndex
SCCircuit::FindAddNode
	(
	const JCharacter* nodeName
	)
{
	// Since Spice files only allow numbers as node designations,
	// we append 'V' to make it a variable.

	JString* newName = new JString(kNodeNamePrefix);
	assert( newName != NULL );
	*newName += nodeName;

	JIndex i;
	if (FindNode(*newName, &i))
		{
		delete newName;
		return i;
		}
	else
		{
		itsNodeNames->Append(newName);
		return GetNodeCount();
		}
}

/******************************************************************************
 FindNode

 ******************************************************************************/

JBoolean
SCCircuit::FindNode
	(
	const JCharacter*	nodeName,
	JIndex*				index
	)
	const
{
	*index = 0;
	const JSize nodeCount = GetNodeCount();
	for (JIndex i=1; i<=nodeCount; i++)
		{
		if (GetNodeName(i) == nodeName)
			{
			*index = i;
			return kJTrue;
			}
		}
	return kJFalse;
}

/******************************************************************************
 RemoveNode

	If no component is connected to the node and no component depends on the node,
	remove it.  Otherwise, we return kJFalse.

 ******************************************************************************/

JBoolean
SCCircuit::RemoveNode
	(
	const JIndex index
	)
{
	const JSize compCount = itsComponents->GetElementCount();
	for (JIndex i=1; i<=compCount; i++)
		{
		SCComponent* comp = GetComponent(i);
		if (comp->RequiresNode(index))
			{
			return kJFalse;
			}

		const JSize termCount = comp->GetTerminalCount();
		for (JIndex j=1; j<=termCount; j++)
			{
			if (comp->GetTerminal(j) == index)
				{
				return kJFalse;
				}
			}
		}

	// Nobody needs it, so it is safe to remove it.

	itsNodeNames->DeleteElement(index);
	return kJTrue;
}

/******************************************************************************
 GetUniqueCoeffName

 ******************************************************************************/

JString
SCCircuit::GetUniqueCoeffName
	(
	const JCharacter* prefix
	)
{
	itsCoeffCounter++;
	JString coeff = prefix + JString(itsCoeffCounter);
	return coeff;
}

/******************************************************************************
 Insert

	Insert a copy of the given circuit into ourselves.  nodeMap specifies
	how every node in the given circuit should map to a node in ourselves.
	If a node maps to zero, we create a new node and update the nodeMap.

	Spice requires that kGroundNodeIndex must map to itself.
	(but none of the components in the circuit have to be connected to ground)

 ******************************************************************************/

void
SCCircuit::Insert
	(
	const SCCircuit&	theCircuit,
	JArray<JIndex>*		nodeMap
	)
{
	assert( nodeMap->GetElement(kGroundNodeIndex) == kGroundNodeIndex );

	// create new nodes to match unmapped nodes
	{
	const JSize nodeCount = nodeMap->GetElementCount();
	assert( nodeCount == theCircuit.GetNodeCount() );

	for (JIndex i=1; i<=nodeCount; i++)
		{
		if (nodeMap->GetElement(i) == 0)
			{
			const JIndex newNode = CreateNode();
			nodeMap->SetElement(i, newNode);
			}
		}
	}

	// copy the Components
	{
	const JSize compCount = theCircuit.GetElementCount();
	for (JIndex i=1; i<=compCount; i++)
		{
		const SCComponent* origComp = theCircuit.GetComponent(i);
		SCComponent* newComp = origComp->Copy(this, nodeMap);
		const JString name = GetUniqueComponentName(newComp->GetType());
		newComp->SetName(name);
		JIndex compIndex;
		const JBoolean ok = AddComponent(newComp, &compIndex);
		assert( ok );
		}
	}
}

/******************************************************************************
 WriteNetlist

 ******************************************************************************/

void
SCCircuit::WriteNetlist
	(
	ostream& output
	)
	const
{
	output << *itsTitle << endl;
	output << '*' << endl;

	const JSize compCount = GetElementCount();
	for (JIndex i=1; i<=compCount; i++)
		{
		(GetComponent(i))->PrintToNetlist(output);
		output << endl;
		}

	output << '*' << endl;
	output << ".END" << endl;
}

/******************************************************************************
 GetNodeIndexForNetlist

 ******************************************************************************/

JIndex
SCCircuit::GetNodeIndexForNetlist
	(
	const JIndex node
	)
	const
{
	JString* nodeName = itsNodeNames->NthElement(node);
	const JString spiceIndex = 
		nodeName->GetSubstring(kNodeNamePrefixLength+1, nodeName->GetLength());
	JIndex nodeIndex;
	const JBoolean ok = spiceIndex.ConvertToUInt(&nodeIndex);
	assert( ok );
	return nodeIndex;
}

/******************************************************************************
 ParseNetList (private)

 ******************************************************************************/

void
SCCircuit::ParseNetList
	(
	const JCharacter* fileName
	)
{
	ifstream input(fileName);

	// first line is title

	*itsTitle = JReadLine(input);
	input >> ws;

	// parse lines until end-of-file

	while (input.good())
		{
		SCComponent* newComponent;
		if (SCComponent::ParseNetlistComponent(input, this, &newComponent))
			{
			JIndex compIndex;
			if (!AddComponent(newComponent, &compIndex))
				{
				cerr << "Unable to add component \"" << newComponent->GetName() << '"' << endl;
				delete newComponent;
				}
			}
		input >> ws;
		}
}

/******************************************************************************
 GenerateEquations (private)

	We generate equations of the form "sum of currents into node = 0" for
	each node.  Using Spice's definition, current always flows into the
	positive node.

	We also include all the auxilliary equations required by each component.

 ******************************************************************************/

void
SCCircuit::GenerateEquations
	(
	JPtrArray<JString>* lhsList,
	JPtrArray<JString>* rhsList,
	JPtrArray<JString>* varList
	)
	const
{
	assert( IsLinear() );

	// Ground is defined to be at zero potential.
	{
	JString* lhs = new JString(kGroundNodeName);
	assert( lhs != NULL );

	lhsList->Append(lhs);

	JString* rhs = new JString("0");
	assert( rhs != NULL );

	rhsList->Append(rhs);
	}

	// Generate the KCL equations.

	const JSize nodeCount = itsNodeNames->GetElementCount();
	const JSize compCount = itsComponents->GetElementCount();
	{
	for (JIndex i=kFirstKCLNode; i<=nodeCount; i++)
		{
		JString* lhs = new JString;
		assert( lhs != NULL );

		for (JIndex j=1; j<=compCount; j++)
			{
			SCLinearComp* comp = (itsComponents->NthElement(j))->CastToSCLinearComp();
			assert( comp != NULL );

			if (comp->GetNegativeNode() == i)			// current into junction
				{
				*lhs += " + (";
				*lhs += comp->GetCurrentSymbol();
				*lhs += ")";
				}
			else if (comp->GetPositiveNode() == i)		// current out of junction
				{
				*lhs += " - (";
				*lhs += comp->GetCurrentSymbol();
				*lhs += ")";
				}
			}

		if (!lhs->IsEmpty())
			{
			lhsList->Append(lhs);

			JString* rhs = new JString("0");
			assert( rhs != NULL );

			rhsList->Append(rhs);
			}
		else
			{
			delete lhs;
			}
		}
	}
	{
	for (JIndex i=1; i<=compCount; i++)
		{
		SCLinearComp* comp = (itsComponents->NthElement(i))->CastToSCLinearComp();
		assert( comp != NULL );

		comp->GetAuxEquations(lhsList, rhsList, varList);
		}
	}
}

/******************************************************************************
 Global functions for SCCircuit class

 ******************************************************************************/

/******************************************************************************
 Stream operators

 ******************************************************************************/

ostream&
operator<<
	(
	ostream&			output,
	const SCCircuit&	circuit
	)
{
JIndex i;

	output << *(circuit.itsTitle);
	output << ' ' << circuit.itsCoeffCounter;

	const JSize nodeCount = (circuit.itsNodeNames)->GetElementCount();
	output << ' ' << nodeCount;

	for (i=1; i<=nodeCount; i++)
		{
		output << ' ' << *((circuit.itsNodeNames)->NthElement(i));
		}

	const JSize componentCount = (circuit.itsComponents)->GetElementCount();
	output << ' ' << componentCount;

	for (i=1; i<=componentCount; i++)
		{
		output << ' ';
		((circuit.itsComponents)->NthElement(i))->StreamOut(output);
		}

	// allow chaining

	return output;
}
