/******************************************************************************
 SCComponent.cpp

	This base class defines the requirements for all circuit components.

	Derived classes must provide:

	GetTerminalCount
		Return the number of terminals that the component has.

	GetTerminal
		Return the node index of the specified terminal.

	SetTerminal
		Set the node index of the specified terminal.

	Copy
		Create a copy of the component.  The caller must specify which circuit
		the new component will be part of.  The caller can also specify which
		nodes in the new circuit the new component should be connected to by
		specifying a mapping from the original circuit's nodes to the new
		circuit's nodes.  (The mapping is assumed to be the identity mapping
		if it is not explicitly given.)

	Derived classes that store extra information must override:

	StreamOut
		Append data to the given stream in whatever format is convenient.
		Just make sure that the stream constructor can read it!

	PrintToNetlist
		Append Spice-format data to the given stream.

	Derived classes can also override:

	RequiresComponent
		Return kJTrue if you require that the component must exist.

	RequiresNode
		Return kJTrue if you require that the node must exist.

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 1995 by John Lindal.

 ******************************************************************************/

#include "SCComponent.h"

#include "SCResistor.h"
#include "SCCapacitor.h"
#include "SCInductor.h"
#include "SCShortCircuit.h"
#include "SCVACSource.h"
#include "SCIACSource.h"
#include "SCVdepVSource.h"
#include "SCIdepISource.h"
#include "SCVdepISource.h"
#include "SCIdepVSource.h"

#include <JString.h>
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

SCComponent::SCComponent
	(
	const SCComponentType	type,
	SCCircuit*				theCircuit,
	const JCharacter*		name
	)
	:
	itsType( type )
{
	SCComponentX(theCircuit);
	*itsName = name;
}

SCComponent::SCComponent
	(
	std::istream&				input,
	const JFileVersion		vers,
	const SCComponentType	type,
	SCCircuit*				theCircuit
	)
	:
	itsType( type )
{
	SCComponentX(theCircuit);
	input >> *itsName;
}

// netlist parser

SCComponent::SCComponent
	(
	std::istream&				input,
	const SCComponentType	type,
	SCCircuit*				theCircuit
	)
	:
	itsType( type )
{
	SCComponentX(theCircuit);
	*itsName = JReadUntilws(input);
}

// private

void
SCComponent::SCComponentX
	(
	SCCircuit* theCircuit
	)
{
	itsCircuit = theCircuit;

	itsName = new JString;
	assert( itsName != NULL );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SCComponent::~SCComponent()
{
	delete itsName;
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

SCComponent::SCComponent
	(
	const SCComponent&		source,
	SCCircuit*				theCircuit,
	const JArray<JIndex>*	nodeMap
	)
	:
	itsType( source.itsType )
{
	itsCircuit = theCircuit;

	itsName = new JString(*(source.itsName));
	assert( itsName != NULL );
}

/******************************************************************************
 SetName

 ******************************************************************************/

void
SCComponent::SetName
	(
	const JCharacter* name
	)
{
	*itsName = name;
}

/******************************************************************************
 ParseNetlistComponent (static)

	Parse the next line in the given stream and create the appropriate object.

	Since Spice uses the hack of a zero magnitude voltage source as the
	controlling current for an IdepSource, we have to use a hack to correct
	the situation.

 ******************************************************************************/

JBoolean
SCComponent::ParseNetlistComponent
	(
	std::istream&		input,
	SCCircuit*		theCircuit,
	SCComponent**	newComponent
	)
{
	*newComponent = NULL;

	input >> std::ws;
	const JCharacter type = input.peek();
	if (type == '*' || type == '.')
		{
		JIgnoreLine(input);
		return kJFalse;
		}
	else if (type == kResistorPrefix)
		{
		*newComponent = new SCResistor(input, theCircuit);
		}
	else if (type == kCapacitorPrefix)
		{
		*newComponent = new SCCapacitor(input, theCircuit);
		}
	else if (type == kInductorPrefix)
		{
		*newComponent = new SCInductor(input, theCircuit);
		}
	else if (type == kVACSourcePrefix)
		{
		SCVACSource* newVACSource = new SCVACSource(input, theCircuit);

		// if the magnitude is zero, it's really a ShortCircuit

		if (newVACSource->GetMagnitude() == 0.0)
			{
			*newComponent = new SCShortCircuit(theCircuit, newVACSource->GetName(),
											   newVACSource->GetPositiveNode(),
											   newVACSource->GetNegativeNode());
			delete newVACSource;
			}
		else
			{
			*newComponent = newVACSource;
			}
		}
	else if (type == kIACSourcePrefix)
		{
		*newComponent = new SCIACSource(input, theCircuit);
		}
	else if (type == kVdepVSourcePrefix)
		{
		*newComponent = new SCVdepVSource(input, theCircuit);
		}
	else if (type == kIdepISourcePrefix)
		{
		*newComponent = new SCIdepISource(input, theCircuit);
		}
	else if (type == kVdepISourcePrefix)
		{
		*newComponent = new SCVdepISource(input, theCircuit);
		}
	else if (type == kIdepVSourcePrefix)
		{
		*newComponent = new SCIdepVSource(input, theCircuit);
		}
	else
		{
		std::cerr << "Unsupported component type: " << type << std::endl;
		JIgnoreLine(input);
		return kJFalse;
		}
	assert( *newComponent != NULL );

	return kJTrue;
}

/******************************************************************************
 GetNamePrefix (static)

	Return the name prefix for the given component type.

 ******************************************************************************/

JCharacter
SCComponent::GetNamePrefix
	(
	const SCComponentType type
	)
{
	if (type == kResistor)
		{
		return kResistorPrefix;
		}
	else if (type == kCapacitor)
		{
		return kCapacitorPrefix;
		}
	else if (type == kInductor)
		{
		return kInductorPrefix;
		}
	else if (type == kVACSource || type == kShortCircuit)
		{
		return kVACSourcePrefix;
		}
	else if (type == kIACSource)
		{
		return kIACSourcePrefix;
		}
	else if (type == kVdepVSource)
		{
		return kVdepVSourcePrefix;
		}
	else if (type == kIdepISource)
		{
		return kIdepISourcePrefix;
		}
	else if (type == kVdepISource)
		{
		return kVdepISourcePrefix;
		}
	else if (type == kIdepVSource)
		{
		return kIdepVSourcePrefix;
		}
	else
		{
		assert( 0 );	// this should never happen
		return '*';
		}
}

/******************************************************************************
 StreamIn (static)

	Read the type of function and create the appropriate object.

 ******************************************************************************/

SCComponent*
SCComponent::StreamIn
	(
	std::istream&			input,
	const JFileVersion	vers,
	SCCircuit*			theCircuit
	)
{
	SCComponentType type;
	input >> type;

	SCComponent* newComponent = NULL;
	if (type == kResistor)
		{
		newComponent = new SCResistor(input, vers, theCircuit);
		}
	else if (type == kCapacitor)
		{
		newComponent = new SCCapacitor(input, vers, theCircuit);
		}
	else if (type == kInductor)
		{
		newComponent = new SCInductor(input, vers, theCircuit);
		}
	else if (type == kShortCircuit)
		{
		newComponent = new SCShortCircuit(input, vers, theCircuit);
		}
	else if (type == kVACSource)
		{
		newComponent = new SCVACSource(input, vers, theCircuit);
		}
	else if (type == kIACSource)
		{
		newComponent = new SCIACSource(input, vers, theCircuit);
		}
	else if (type == kVdepVSource)
		{
		newComponent = new SCVdepVSource(input, vers, theCircuit);
		}
	else if (type == kIdepISource)
		{
		newComponent = new SCIdepISource(input, vers, theCircuit);
		}
	else if (type == kVdepISource)
		{
		newComponent = new SCVdepISource(input, vers, theCircuit);
		}
	else if (type == kIdepVSource)
		{
		newComponent = new SCIdepVSource(input, vers, theCircuit);
		}
	else
		{
		assert( 0 );	// this should never happen
		}
	assert( newComponent != NULL );

	return newComponent;
}

/******************************************************************************
 StreamOut

 ******************************************************************************/

void
SCComponent::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << itsType;
	output << ' ' << *itsName;
}

/******************************************************************************
 PrintToNetlist

 ******************************************************************************/

void
SCComponent::PrintToNetlist
	(
	std::ostream& output
	)
	const
{
	output << *itsName;
}

/******************************************************************************
 RequiresComponent

	Derived classes can override this routine if they require that other
	components exist.

 ******************************************************************************/

JBoolean
SCComponent::RequiresComponent
	(
	const JIndex index
	)
	const
{
	return kJFalse;
}

/******************************************************************************
 RequiresNode

	Derived classes can override this routine if they require that certain
	nodes exist.

 ******************************************************************************/

JBoolean
SCComponent::RequiresNode
	(
	const JIndex index
	)
	const
{
	return kJFalse;
}

/******************************************************************************
 Cast to SCLinearComp*

	Not inline because they are virtual

 ******************************************************************************/

SCLinearComp*
SCComponent::CastToSCLinearComp()
{
	return NULL;
}

const SCLinearComp*
SCComponent::CastToSCLinearComp()
	const
{
	return NULL;
}

/******************************************************************************
 Cast to SCPassiveLinearComp*

	Not inline because they are virtual

 ******************************************************************************/

SCPassiveLinearComp*
SCComponent::CastToSCPassiveLinearComp()
{
	return NULL;
}

const SCPassiveLinearComp*
SCComponent::CastToSCPassiveLinearComp()
	const
{
	return NULL;
}

/******************************************************************************
 Cast to SCACSource*

	Not inline because they are virtual

 ******************************************************************************/

SCACSource*
SCComponent::CastToSCACSource()
{
	return NULL;
}

const SCACSource*
SCComponent::CastToSCACSource()
	const
{
	return NULL;
}

/******************************************************************************
 Cast to SCDepSource*

	Not inline because they are virtual

 ******************************************************************************/

SCDepSource*
SCComponent::CastToSCDepSource()
{
	return NULL;
}

const SCDepSource*
SCComponent::CastToSCDepSource()
	const
{
	return NULL;
}

/******************************************************************************
 Cast to SCVdepSource*

	Not inline because they are virtual

 ******************************************************************************/

SCVdepSource*
SCComponent::CastToSCVdepSource()
{
	return NULL;
}

const SCVdepSource*
SCComponent::CastToSCVdepSource()
	const
{
	return NULL;
}

/******************************************************************************
 Cast to SCIdepSource*

	Not inline because they are virtual

 ******************************************************************************/

SCIdepSource*
SCComponent::CastToSCIdepSource()
{
	return NULL;
}

const SCIdepSource*
SCComponent::CastToSCIdepSource()
	const
{
	return NULL;
}
