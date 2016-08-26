/******************************************************************************
 JGeneticString.h

	Interface for the JGeneticString class

	Copyright © 1994-95 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JGeneticString
#define _H_JGeneticString

#include <JGeneticAlgorithm.h>

class JGeneticString : public JGeneticAlgorithm
{
public:

	JGeneticString(const JSize individualSize);

	virtual ~JGeneticString();

	JFloat GetBestFitness() const;

protected:

	JSize	GetIndividualSize() const;

	virtual void	ClearBestFitness();
	virtual void	SaveBestIndividuals(const JSize thePopulationSize,
										JPopulationFitness* thePopulationFitness,
										const JFloat theMinFitness,
										const JFloat theAvgFitness,
										const JFloat theMaxFitness);

	// these must be provided by derived classes

	virtual void	Mate(const JIndex index1, const JIndex index2,
						 const JIndex crossoverPosition) = 0;
	virtual void	Mutate(const JIndex index, const JIndex position) = 0;
	virtual void	SaveBestIndividual(const JIndex bestIndex) = 0;

private:

	JSize	itsIndividualSize;	// length in bits of each individual
	JFloat	itsBestFitness;		// maximum fitness ever encountered

private:

	void	Reproduce();
	void	Mutate(const JIndex index);

	// not allowed

	JGeneticString(const JGeneticString& source);
	const JGeneticString& operator=(const JGeneticString& source);
};

/******************************************************************************
 GetBestFitness

	Return the fitness of the best individual ever encountered.

 ******************************************************************************/

inline JFloat
JGeneticString::GetBestFitness()
	const
{
	return itsBestFitness;
}

/******************************************************************************
 GetIndividualSize (protected)

 ******************************************************************************/

inline JSize
JGeneticString::GetIndividualSize()
	const
{
	return itsIndividualSize;
}

#endif
