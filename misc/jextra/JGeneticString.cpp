/******************************************************************************
 JGeneticString.cpp

						The Genetic Algorithm for Bit Strings class

	This abstract base class implements the details common to all
	Genetic Algorithms that use a bit strings to encode each individual.
	We replace the need to implement Reproduce and SaveBestIndividuals
	with the need to implement Mate, Mutate, and SaveBestIndividual.

	Derived classes must also call our ClearBestFitness somewhere inside
	CreateNewPopulation.

	Mate
		Perform "crossover" at the specified position and save the result
		in the offspring population.

	Mutate
		Flip the specified bit of the specified individual in the
		offspring population.

	SaveBestIndividual
		Save the best individual in the current population.

	BASE CLASS = JGeneticAlgorithm

	Copyright (C) 1994-95 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JGeneticString.h>
#include <JString.h>
#include <math.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JGeneticString::JGeneticString
	(
	const JSize individualSize
	)
	:
	JGeneticAlgorithm()
{
	itsIndividualSize = individualSize;
	itsBestFitness    = -1.0;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JGeneticString::~JGeneticString()
{
}

/******************************************************************************
 ClearBestFitness (protected)

	Derived classes must call from within CreateNewPopulation to ensure that
	the best individual ever seen is correctly saved.

 ******************************************************************************/

void
JGeneticString::ClearBestFitness()
{
	itsBestFitness = -1.0;		// guarantees replacement the first time through
}

/******************************************************************************
 Reproduce (private)

	Create the next generation from the current generation.
	Each randomly selected pair of parents produces 2 offspring, each of
	which has a small probability of mutating.

 ******************************************************************************/

void
JGeneticString::Reproduce()
{
	const JSize populationSize = GetPopulationSize();

	const JKLRand& rng = GetRNG();

	JIndex i=1;
	while (1)
		{
		const JIndex j  = GetRandomIndividualBasedOnFitness();
		const JIndex k  = GetRandomIndividualBasedOnFitness();
		JIndex position = rng.UniformULong(1, itsIndividualSize-1);

		Mate(j, k, position);
		Mutate(i);
		if (i == populationSize)
			{
			break;
			}
		i++;

		Mate(k, j, position);
		Mutate(i);
		if (i == populationSize)
			{
			break;
			}
		i++;
		}

	ReplaceOldPopulation();
}

/******************************************************************************
 Mutate (private)

	Mutate the specified offspring by giving each bit a tiny chance of flipping.

 ******************************************************************************/

void
JGeneticString::Mutate
	(
	const JIndex index
	)
{
	const JKLRand& rng = GetRNG();

	for (JIndex i=1; i<=itsIndividualSize; i++)
		{
		if (rng.UniformClosedProb() < GetMutationRate())
			{
			Mutate(index, i);
			}
		}
}

/******************************************************************************
 SaveBestIndividuals (protected)

 ******************************************************************************/

void
JGeneticString::SaveBestIndividuals
	(
	const JSize			thePopulationSize,
	JPopulationFitness*	thePopulationFitness,
	const JFloat		theMinFitness,
	const JFloat		theAvgFitness,
	const JFloat		theMaxFitness
	)
{
	if (theMaxFitness > itsBestFitness)
		{
		for (JIndex i=1; i<=thePopulationSize; i++)
			{
			const JFloat fitness = thePopulationFitness->GetElement(i);
			if (fitness == theMaxFitness)
				{
				SaveBestIndividual(i);
				itsBestFitness = theMaxFitness;
				break;
				}
			}
		}
}
