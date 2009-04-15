/******************************************************************************
 JGeneticAlgorithm.cpp

							The Genetic Algorithm class

	This abstract base class implements the generic Genetic Algorithm.
	Derived classes must store the actual population, implement the
	pure virtual methods to handle all the details, and define a method
	GetBestIndividual to return the best individual ever encountered.

	CreateNewPopulation
		Throw out everything and create a new population.

	Reproduce
		Choose individuals based on their fitness and let them reproduce.
		At the end, be sure to call ReplaceOldPopulation().

	ReplaceOldPopulation
		Throw out the current population and replace it with the
		offspring population.  This routine is responsible for calling
		CalcPopulationFitness() so the fitness values are correct.

	CalcIndividualFitness
		Calculate the fitness of an individual in the current population.
		The fitness measure must be non-negative and must be larger for
		better individuals.

	SaveBestIndividuals
		Find and save the best individuals in the current population.
		By saving the best individuals ever encountered, we don't have
		to worry about stopping on an up note.

	Get best individuals
		Somehow return the best individuals ever seen.
		This can't be implemented here because we don't know anything
		about the structure of each individual.

	CalcPopulationFitness (optional)
		Override this if there is a more efficient way to calculate the
		fitness of each individual than by merely calculating each separately.

	ScaleFitness (optional)
		Adjust the fitness of all the individuals after their raw fitness
		values have been calculated.  This is useful for fitness scaling or
		for adjusting fitness values so they are all non-negative.
		The default behavior is to do nothing.  If the derived class chooses
		to override this, it must return kJTrue if it changes any of the
		fitness values.  This tells GA to recalculate the min,avg,max.

	BASE CLASS = none

	Copyright © 1994-95 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JGeneticAlgorithm.h>
#include <JString.h>
#include <jMath.h>
#include <jGlobals.h>
#include <jAssert.h>

const JSize kAvgFitnessTime = 5;

/******************************************************************************
 Constructor

 ******************************************************************************/

JGeneticAlgorithm::JGeneticAlgorithm()
	:
	itsRNG()
{
	itsPopulationSize = 0;
	itsMutationRate   = 0.0;

	itsPopulationFitness = new JPopulationFitness(20);
	assert( itsPopulationFitness != NULL );

	itsMaxFitness = itsAvgFitness = itsMinFitness = itsSumFitness = -1.0;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JGeneticAlgorithm::~JGeneticAlgorithm()
{
	delete itsPopulationFitness;
}

/******************************************************************************
 EvolveNewPopulation

	Create a new population and let it evolve until it stabilizes.

 ******************************************************************************/

void
JGeneticAlgorithm::EvolveNewPopulation
	(
	const JSize		populationSize,
	const JFloat	mutationRate,
	const JFloat	equilibriumTolerance,
	const JSize		maxGenerationCount
	)
{
	assert( populationSize > 1 );
	assert( 0.0 <= mutationRate && mutationRate < 1.0 );

	itsPopulationSize = populationSize;
	itsMutationRate   = mutationRate;

	// adjust the number of slots in itsPopulationFitness

	while (itsPopulationFitness->GetElementCount() < populationSize)
		{
		itsPopulationFitness->AppendElement(0.0);
		}
	while (itsPopulationFitness->GetElementCount() > populationSize)
		{
		itsPopulationFitness->RemoveElement(itsPopulationFitness->GetElementCount());
		}

	// ask the derived class to create a new population of the specified size

	CreateNewPopulation(itsPopulationSize);

	// calculate the fitness of each individual in the new population

	CalcPopulationFitness();

	// let the population evolve

	EvolveCurrentPopulation(equilibriumTolerance, maxGenerationCount);
}

/******************************************************************************
 EvolveCurrentPopulation

	Evolve the current population until it stabilizes or maxGenerationCount
	is exceeded.  If maxGenerationCount is zero, we ignore this condition.

 ******************************************************************************/

void
JGeneticAlgorithm::EvolveCurrentPopulation
	(
	const JFloat	equilibriumTolerance,
	const JSize		maxGenerationCount
	)
{
	assert( itsPopulationSize > 1 );

	JProgressDisplay* pg = JNewPG();
	pg->VariableLengthProcessBeginning("Evolving population...", kJTrue, kJTrue);
	JString progressStr = "Average fitness:  " + JString(itsAvgFitness);
	pg->IncrementProgress(progressStr);

	JSize generationCount = 0;

	JArray<JFloat> oldAvgFitness(kAvgFitnessTime);

	while (maxGenerationCount == 0 || generationCount < maxGenerationCount)
		{
		Reproduce();
		generationCount++;

		// stop if the user cancels the process

		progressStr = "Average fitness:  " + JString(itsAvgFitness);
		if (!pg->IncrementProgress(progressStr))
			{
			break;
			}

		// Stop when average fitness hasn't changed significantly in a long time.
		// equilibriumTolerance defines the size of "significant" change.

		if (oldAvgFitness.GetElementCount() < kAvgFitnessTime)
			{
			oldAvgFitness.AppendElement(itsAvgFitness);
			}
		else
			{
			oldAvgFitness.RemoveElement(1);
			oldAvgFitness.AppendElement(itsAvgFitness);
			}

		if (oldAvgFitness.GetElementCount() == kAvgFitnessTime)
			{
			JFloat avgAvgFitness = 0.0;
			{
			for (JIndex i=1; i<=kAvgFitnessTime; i++)
				{
				avgAvgFitness += oldAvgFitness.GetElement(i);
				}
			}
			avgAvgFitness /= (JFloat) kAvgFitnessTime;

			JBoolean converged = kJTrue;
			{
			for (JIndex i=1; i<=kAvgFitnessTime; i++)
				{
				if (fabs(oldAvgFitness.GetElement(i) - avgAvgFitness) / avgAvgFitness >
					equilibriumTolerance)
					{
					converged = kJFalse;
					}
				}
			}
			if (converged)
				{
				break;
				}
			}
		}

	pg->ProcessFinished();
	delete pg;
}

/******************************************************************************
 GetRandomIndividualBasedOnFitness (protected)

	Return the index of an individual in the population.  The probability
	of getting a particular individual is proportional to its fitness.

	We treat the fitness as a bin width, line all the bins up, and generate
	a random number in this interval.  The bin that we land in is the individual
	that we pick.

 ******************************************************************************/

JIndex
JGeneticAlgorithm::GetRandomIndividualBasedOnFitness()
	const
{
	const JFloat indicator = (GetRNG()).UniformDouble(0.0, itsSumFitness);

	// find the bin of the individual that the indicator landed in

	JFloat sumFitness = 0.0;

	for (JIndex i=1; i<=itsPopulationSize; i++)
		{
		const JFloat indivFitness = itsPopulationFitness->GetElement(i);

		sumFitness += indivFitness;
		if (sumFitness >= indicator)
			{
			return i;
			}
		}

	return itsPopulationSize;
}

/******************************************************************************
 CalcPopulationFitness (protected)

	Fill itsPopulationFitness object with the fitness of each individual.
	Also calculates the min,avg,max individual fitness.

 ******************************************************************************/

void
JGeneticAlgorithm::CalcPopulationFitness()
{
	// calculate the raw fitness for each individual

	CalcPopulationFitness1(itsPopulationSize, itsPopulationFitness);

	// ScaleFitness needs the population statistics

	CalcPopulationStats(itsPopulationSize, itsPopulationFitness,
						&itsMinFitness, &itsAvgFitness, &itsMaxFitness, &itsSumFitness);

	// if the derived class decided to scale the fitness values,
	// then we need to recalculate the statistics

	if (ScaleFitness(itsPopulationSize, itsPopulationFitness,
					 itsMinFitness, itsAvgFitness, itsMaxFitness))
		{
		CalcPopulationStats(itsPopulationSize, itsPopulationFitness,
							&itsMinFitness, &itsAvgFitness, &itsMaxFitness, &itsSumFitness);
		}

	SaveBestIndividuals(itsPopulationSize, itsPopulationFitness,
						itsMinFitness, itsAvgFitness, itsMaxFitness);
}

/******************************************************************************
 CalcPopulationFitness1 (protected)

	Fill itsPopulationFitness object with the raw fitness of each individual.
	The default behavior is to use CalcIndividualFitness().  Derived classes
	can override this method if they wish to optimize.

 ******************************************************************************/

void
JGeneticAlgorithm::CalcPopulationFitness1
	(
	const JSize			populationSize,
	JPopulationFitness*	populationFitness
	)
{
	for (JIndex i=1; i<=populationSize; i++)
		{
		const JFloat indivFitness = CalcIndividualFitness(i);
		populationFitness->SetElement(i, indivFitness);
		}
}

/******************************************************************************
 CalcPopulationStats (protected)

 ******************************************************************************/

void
JGeneticAlgorithm::CalcPopulationStats
	(
	const JSize			thePopulationSize,
	JPopulationFitness*	thePopulationFitness,
	JFloat*				theMinFitness,
	JFloat*				theAvgFitness,
	JFloat*				theMaxFitness,
	JFloat*				theSumFitness
	)
{
	itsSumFitness = 0.0;
	itsMaxFitness = itsMinFitness = -1.0;

	JIndex bestIndividualIndex = 0;

	for (JIndex i=1; i<=itsPopulationSize; i++)
		{
		const JFloat indivFitness = itsPopulationFitness->GetElement(i);

		itsSumFitness += indivFitness;
		if (itsMinFitness > indivFitness || itsMinFitness < 0.0)
			{
			itsMinFitness = indivFitness;
			}
		if (itsMaxFitness < indivFitness || itsMaxFitness < 0.0)
			{
			itsMaxFitness = indivFitness;
			}
		}

	itsAvgFitness = itsSumFitness / (JFloat) itsPopulationSize;
}

/******************************************************************************
 ScaleFitness (protected)

	The default behavior is to do nothing.

 ******************************************************************************/

JBoolean
JGeneticAlgorithm::ScaleFitness
	(
	const JSize			thePopulationSize,
	JPopulationFitness*	thePopulationFitness,
	const JFloat		theMinFitness,
	const JFloat		theAvgFitness,
	const JFloat		theMaxFitness
	)
{
	return kJFalse;
}

/******************************************************************************
 KozaAdjustedTransform (protected)

	Transforms the given set of fitness values according to 1/(1+(max-x)).
	This way, if the input fitness is "highest=best", so is the output fitness.
	See page 97 of Koza's "Genetic Programming".

 ******************************************************************************/

void
JGeneticAlgorithm::KozaAdjustedTransform
	(
	const JSize			thePopulationSize,
	JPopulationFitness*	thePopulationFitness,
	const JFloat		theMinFitness,
	const JFloat		theAvgFitness,
	const JFloat		theMaxFitness
	)
{
	for (JIndex i=1; i<=thePopulationSize; i++)
		{
		const JFloat newFitness =
			1.0/(1.0 + theMaxFitness - thePopulationFitness->GetElement(i));
		thePopulationFitness->SetElement(i, newFitness);
		}
}

/******************************************************************************
 SetMutationRate

 ******************************************************************************/

void
JGeneticAlgorithm::SetMutationRate
	(
	const JFloat mutationRate
	)
{
	assert( 0.0 <= mutationRate && mutationRate < 1.0 );

	itsMutationRate = mutationRate;
}
