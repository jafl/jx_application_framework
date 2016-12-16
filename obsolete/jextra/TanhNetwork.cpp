/******************************************************************************
 TanhNetwork.cpp

	Class representing a feed forward neural network that uses tanh
	activation functions.

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <TanhNetwork.h>
#include <JVector.h>
#include <jMath.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TanhNetwork::TanhNetwork
	(
	const FFNetworkSize& unitCount
	)
	:
	FFNetwork(unitCount)
{
}

TanhNetwork::TanhNetwork
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

TanhNetwork::~TanhNetwork()
{
}

/******************************************************************************
 Activation function (virtual protected)

	Derived classes can override this to implement a different function
	for each layer.

 ******************************************************************************/

JVector
TanhNetwork::g
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
TanhNetwork::g
	(
	const JIndex	layerIndex,
	const JFloat	h
	)
	const
{
	return tanh(h);
}

/******************************************************************************
 Derivative of activation function (virtual protected)

	Derived classes can override this to implement a different function
	for each layer.

 ******************************************************************************/

JVector
TanhNetwork::gprime
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
TanhNetwork::gprime
	(
	const JIndex	layerIndex,
	const JFloat	g
	)
	const
{
	return (1.0 - g*g);
}
