/******************************************************************************
 TanhLinNetwork.cpp

	Class representing a feed forward neural network that uses tanh
	activation functions in the hidden units and a linear output function.

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <TanhLinNetwork.h>
#include <JVector.h>
#include <jMath.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TanhLinNetwork::TanhLinNetwork
	(
	const FFNetworkSize& unitCount
	)
	:
	FFNetwork(unitCount)
{
}

TanhLinNetwork::TanhLinNetwork
	(
	std::istream& input
	)
	:
	FFNetwork(input)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TanhLinNetwork::~TanhLinNetwork()
{
}

/******************************************************************************
 Activation function (virtual protected)

	Derived classes can override this to implement a different function
	for each layer.

 ******************************************************************************/

JVector
TanhLinNetwork::g
	(
	const JIndex	layerIndex,
	const JVector&	h
	)
	const
{
	const JSize dimCount = h.GetDimensionCount();

	JVector result(dimCount);

	for (JIndex i=1;i<=dimCount;i++)
		{
		const JFloat value = h.GetElement(i);
		result.SetElement(i, g(layerIndex, value));
		}

	return result;
}

JFloat
TanhLinNetwork::g
	(
	const JIndex	layerIndex,
	const JFloat	h
	)
	const
{
	if (layerIndex == 1)
		{
		return tanh(h);
		}
	else
		{
		assert( layerIndex == 2 );
		return h;
		}
}

/******************************************************************************
 Derivative of activation function (virtual protected)

	Derived classes can override this to implement a different function
	for each layer.

 ******************************************************************************/

JVector
TanhLinNetwork::gprime
	(
	const JIndex	layerIndex,
	const JVector&	g
	)
	const
{
	const JSize dimCount = g.GetDimensionCount();

	JVector result(dimCount);

	for (JIndex i=1;i<=dimCount;i++)
		{
		const JFloat value = g.GetElement(i);
		result.SetElement(i, gprime(layerIndex, value));
		}

	return result;
}

JFloat
TanhLinNetwork::gprime
	(
	const JIndex	layerIndex,
	const JFloat	g
	)
	const
{
	if (layerIndex == 1)
		{
		return (1.0 - g*g);
		}
	else
		{
		assert( layerIndex == 2 );
		return 1.0;
		}
}
