/******************************************************************************
 FFNetwork.h

	Interface for the FFNetwork class.

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_FFNetwork
#define _H_FFNetwork

#include <JPtrArray.h>	// template

class JVector;
class JMatrix;
class JKLRand;
class JProgressDisplay;

typedef JArray<JSize>		FFNetworkSize;
typedef JPtrArray<JMatrix>	FFNetworkWeights;

class FFNetwork
{
public:

	FFNetwork(const FFNetworkSize& unitCount);
	FFNetwork(istream& input);

	virtual ~FFNetwork();

	JVector	Run(const JVector& input) const;
	JVector	GetLayerOutput(const JVector& input, const JIndex index) const;

	void	Learn(const JMatrix& trainInput, const JMatrix& correctTrainOutput,
				  const JFloat learningRate, const JFloat momentum,
				  const JFloat weightDecay, const JFloat errorTolerance,
				  const JSize maxIterCount, const JBoolean useBatchMethod,
				  const JMatrix* testInput, const JMatrix* correctTestOutput,
				  JFloat* trainError, JFloat* testError,
				  ostream* logFile);
	JFloat	MeasurePerformance(const JMatrix& input,
							   const JMatrix& correctOutput) const;
	JFloat	MeasureBooleanAccuracy(const JMatrix& input,
									  const JMatrix& correctOutput) const;

	void	RandomizeWeights(const JFloat magnitude);
	void	PrintWeights(ostream& output) const;

	JSize	GetLayerCount() const;
	JSize	GetLayerSize(const JIndex layer) const;
	JMatrix	GetEmptyWeightMatrix(const JIndex layer) const;
	void	SetWeights(const JIndex layer, const JMatrix& w);

	virtual void	StreamOut(ostream& output) const;

protected:

	virtual JFloat	g(const JIndex layerIndex, const JFloat h) const = 0;
	virtual JVector	g(const JIndex layerIndex, const JVector& h) const = 0;

	virtual JFloat	gprime(const JIndex layerIndex, const JFloat g) const = 0;
	virtual JVector	gprime(const JIndex layerIndex, const JVector& g) const = 0;

private:

	JSize				itsLayerCount;	// stores # of layers in network
	FFNetworkSize*		itsLayerSizes;	// stores # of units in each layer
	FFNetworkWeights*	itsWeights;		// stores weight matrix for each layer
	JKLRand*			itsRNG;

private:

	void	FFNetworkX();

	void	AdjustWeightsForExamples(
				const JMatrix& input, const JMatrix& correctOutput,
				const JFloat learningRate, const JFloat momentum,
				JPtrArray<JMatrix>* olddws, JPtrArray<JMatrix>* currentdws);
	void	AdjustWeightsForExample(
				const JVector& input, const JVector& correctOutput,
				const JFloat learningRate, const JFloat momentum,
				JPtrArray<JMatrix>* olddw, JPtrArray<JMatrix>* currentdw);
	void	CalcWeightChangeForExample(
				const JVector& input, const JVector& correctOutput,
				const JFloat learningRate, JPtrArray<JMatrix>* dws) const;
	void	IncrementWeights(const JFloat momentum,
							 JPtrArray<JMatrix>* olddws,
							 const JPtrArray<JMatrix>& currentdws);
	void	DecayWeights(const JFloat decayFactor);

	void	ForwardPropagate(const JVector& input,
							 JPtrArray<JVector>* layerOutputs) const;
	void	BackPropagate(const JVector& correctOutput,
						  const JPtrArray<JVector>& layerOutputs,
						  JPtrArray<JVector>* deltas) const;

	JVector	PrependOffsetUnit(const JVector& layerOutput) const;
	JVector	RemoveOffsetUnit(const JVector& v) const;

	JBoolean	DisplayPerformance(JProgressDisplay& pg,
								   const JIndex iteration,
								   const JMatrix& trainInput,
								   const JMatrix& correctTrainOutput,
								   JFloat* trainError,
								   const JMatrix* testInput,
								   const JMatrix* correctTestOutput,
								   JFloat* testError,
								   ostream* logFile) const;

	// not allowed

	FFNetwork(const FFNetwork& source);
	const FFNetwork& operator=(const FFNetwork& source);
};


/******************************************************************************
 GetLayerCount

	Return the number of layers (not including the input).

 ******************************************************************************/

inline JSize
FFNetwork::GetLayerCount()
	const
{
	return itsLayerCount;
}

/******************************************************************************
 GetLayerSize

	Return the number of units in the specified layer.
	The input layer is layer 0.

 ******************************************************************************/

inline JSize
FFNetwork::GetLayerSize
	(
	const JIndex layer
	)
	const
{
	return itsLayerSizes->GetElement(layer + 1);
}

#endif
