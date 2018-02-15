/******************************************************************************
 JGeneticAlgorithm.h

	Interface for the JGeneticAlgorithm class

	Copyright (C) 1994-95 by John Lindal.

 ******************************************************************************/

#ifndef _H_JGeneticAlgorithm
#define _H_JGeneticAlgorithm

#include <JArray.h>
#include <JKLRand.h>

class JGeneticAlgorithm
{
public:

	JGeneticAlgorithm();

	virtual ~JGeneticAlgorithm();

	void	EvolveNewPopulation(const JSize populationSize,
								const JFloat mutationRate,
								const JFloat equilibriumTolerance,
								const JSize maxGenerationCount = 0);
	void	EvolveCurrentPopulation(const JFloat equilibriumTolerance,
									const JSize maxGenerationCount = 0);

	JFloat	GetMutationRate() const;
	void	SetMutationRate(const JFloat mutationRate);

protected:

	typedef JArray<JFloat>	JPopulationFitness;

protected:

	JSize	GetPopulationSize() const;
	void	CalcPopulationFitness();
	JIndex	GetRandomIndividualBasedOnFitness() const;

	void	CalcPopulationStats(const JSize thePopulationSize,
								JPopulationFitness* thePopulationFitness,
								JFloat* theMinFitness, JFloat* theAvgFitness,
								JFloat* theMaxFitness, JFloat* theSumFitness);
	void	KozaAdjustedTransform(const JSize thePopulationSize,
								  JPopulationFitness* thePopulationFitness,
								  const JFloat theMinFitness,
								  const JFloat theAvgFitness,
								  const JFloat theMaxFitness);

	const JKLRand&	GetRNG() const;

	// these must be provided by derived classes

	virtual void	CreateNewPopulation(const JSize populationSize) = 0;
	virtual void	Reproduce() = 0;
	virtual void	ReplaceOldPopulation() = 0;
	virtual JFloat	CalcIndividualFitness(const JIndex index) const = 0;
	virtual void	SaveBestIndividuals(const JSize thePopulationSize,
										JPopulationFitness* thePopulationFitness,
										const JFloat theMinFitness,
										const JFloat theAvgFitness,
										const JFloat theMaxFitness) = 0;

	virtual void	CalcPopulationFitness1(const JSize populationSize,
										   JPopulationFitness* populationFitness);
	virtual JBoolean		ScaleFitness(const JSize thePopulationSize,
										 JPopulationFitness* thePopulationFitness,
										 const JFloat theMinFitness,
										 const JFloat theAvgFitness,
										 const JFloat theMaxFitness);

private:

	JSize	itsPopulationSize;	// number of individuals in population
	JFloat	itsMutationRate;	// probability of mutation

	JPopulationFitness*	itsPopulationFitness;	// fitness of current population

	JFloat	itsMaxFitness;		// fitness of best individual
	JFloat	itsSumFitness;		// sum of individual fitnesses (for GetRandomIndividualBasedOnFitness)
	JFloat	itsAvgFitness;		// average fitness of population
	JFloat	itsMinFitness;		// fitness of worst individual

	JKLRand	itsRNG;

private:

	// not allowed

	JGeneticAlgorithm(const JGeneticAlgorithm& source);
	const JGeneticAlgorithm& operator=(const JGeneticAlgorithm& source);
};

/******************************************************************************
 GetMutationRate

 ******************************************************************************/

inline JFloat
JGeneticAlgorithm::GetMutationRate()
	const
{
	return itsMutationRate;
}

/******************************************************************************
 GetPopulationSize (protected)

 ******************************************************************************/

inline JSize
JGeneticAlgorithm::GetPopulationSize()
	const
{
	return itsPopulationSize;
}

/******************************************************************************
 GetRNG

 ******************************************************************************/

inline const JKLRand&
JGeneticAlgorithm::GetRNG()
	const
{
	return itsRNG;
}

#endif
