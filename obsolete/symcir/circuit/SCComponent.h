/******************************************************************************
 SCComponent.h

	Interface for SCComponent class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCComponent
#define _H_SCComponent

#include <JBroadcaster.h>
#include "SCComponentType.h"
#include <JPtrArray.h>

class JString;
class SCCircuit;
class SCLinearComp;
class SCPassiveLinearComp;
class SCACSource;
class SCDepSource;
class SCVdepSource;
class SCIdepSource;

class SCComponent : virtual public JBroadcaster
{
public:

	SCComponent(const SCComponentType type, SCCircuit* theCircuit,
				const JCharacter* name);
	SCComponent(const SCComponent& source, SCCircuit* theCircuit,
				const JArray<JIndex>* nodeMap);

	// construct from private file

	SCComponent(istream& input, const JFileVersion vers, const SCComponentType type,
				SCCircuit* theCircuit);

	// construct from netlist

	SCComponent(istream& input, const SCComponentType type, SCCircuit* theCircuit);

	virtual ~SCComponent();

	SCComponentType	GetType() const;
	SCCircuit*		GetCircuit() const;

	const JString&	GetName() const;
	void			SetName(const JCharacter* name);

	virtual JSize			GetTerminalCount() const = 0;
	virtual JIndex			GetTerminal(const JIndex index) const = 0;
	virtual void			SetTerminal(const JIndex index, const JIndex node) = 0;
	virtual JBoolean		RequiresComponent(const JIndex index) const;
	virtual JBoolean		RequiresNode(const JIndex index) const;
	virtual SCComponent*	Copy(SCCircuit* theCircuit,
								 const JArray<JIndex>* nodeMap = NULL) const = 0;

	static JBoolean		ParseNetlistComponent(istream& input, SCCircuit* theCircuit,
											  SCComponent** newComponent);
	static JCharacter	GetNamePrefix(const SCComponentType type);
	static SCComponent*	StreamIn(istream& input, const JFileVersion vers,
								 SCCircuit* theCircuit);
	virtual void		StreamOut(ostream& output) const;
	virtual void		PrintToNetlist(ostream& output) const;

	// provides safe downcasting

	virtual SCLinearComp*				CastToSCLinearComp();
	virtual const SCLinearComp*			CastToSCLinearComp() const;
	virtual SCPassiveLinearComp*		CastToSCPassiveLinearComp();
	virtual const SCPassiveLinearComp*	CastToSCPassiveLinearComp() const;
	virtual SCACSource*					CastToSCACSource();
	virtual const SCACSource*			CastToSCACSource() const;
	virtual SCDepSource*				CastToSCDepSource();
	virtual const SCDepSource*			CastToSCDepSource() const;
	virtual SCVdepSource*				CastToSCVdepSource();
	virtual const SCVdepSource*			CastToSCVdepSource() const;
	virtual SCIdepSource*				CastToSCIdepSource();
	virtual const SCIdepSource*			CastToSCIdepSource() const;

private:

	const SCComponentType	itsType;		// type of object
	JString*				itsName;		// name from schematic
	SCCircuit*				itsCircuit;		// circuit that we belong to

private:

	void	SCComponentX(SCCircuit* theCircuit);

	// not allowed

	SCComponent(const SCComponent& source);
	const SCComponent& operator=(const SCComponent& source);
};

/******************************************************************************
 GetType

 ******************************************************************************/

inline SCComponentType
SCComponent::GetType()
	const
{
	return itsType;
}

/******************************************************************************
 GetName

 ******************************************************************************/

inline const JString&
SCComponent::GetName()
	const
{
	return *itsName;
}

/******************************************************************************
 GetCircuit

 ******************************************************************************/

inline SCCircuit*
SCComponent::GetCircuit()
	const
{
	return itsCircuit;
}

#endif
