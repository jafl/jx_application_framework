/******************************************************************************
 FFNetwork.cpp

	Abstract base class representing a feed forward neural network.

	Derived classes must implement the activation function.  This function
	can be different for each layer.  Note that the current function
	prototype assumes that g' is only a function of g, and not a function
	of the input h.

	Training is done via back propagation and momentum is used to speed
	convergence.  To turn off momentum, simply use a value of zero.

	Copyright (C) 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <FFNetwork.h>
#include <JVector.h>
#include <JMatrix.h>
#include <JKLRand.h>
#include <JString.h>
#include <jGlobals.h>
#include <jMath.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

FFNetwork::FFNetwork
	(
	const FFNetworkSize& unitCount
	)
{
	itsLayerCount = unitCount.GetElementCount() - 1;
	assert( itsLayerCount >= 1 );

	itsLayerSizes = new FFNetworkSize(unitCount);
	assert( itsLayerSizes != NULL );

	FFNetworkX();
}

FFNetwork::FFNetwork
	(
	std::istream& input
	)
{
JIndex i;

	input >> itsLayerCount;

	itsLayerSizes = new FFNetworkSize(itsLayerCount+1);
	assert( itsLayerSizes != NULL );

	for (i=0; i<=itsLayerCount; i++)
		{
		JSize layerSize;
		input >> layerSize;
		itsLayerSizes->AppendElement(layerSize);
		}

	FFNetworkX();

	for (i=1; i<=itsLayerCount; i++)
		{
		JMatrix* w = itsWeights->NthElement(i);
		input >> *w;
		}
}

// private

void
FFNetwork::FFNetworkX()
{
	itsRNG = new JKLRand;
	assert( itsRNG != NULL );

	itsWeights = new FFNetworkWeights(JPtrArrayT::kDeleteAll, itsLayerCount);
	assert( itsWeights != NULL );

	// create the weight matrices connecting adjacent layers

	JSize prevUnitCount = itsLayerSizes->GetElement(1);

	for (JSize i=1; i<=itsLayerCount; i++)
		{
		// get the size of the current layer

		const JSize currUnitCount = itsLayerSizes->GetElement(i+1);

		// create a weight matrix to connect from the previous layer
		// (includes space for the implicit offset unit of the previous layer)

		JMatrix* weights = new JMatrix(currUnitCount, prevUnitCount+1, 0.0);
		assert( weights != NULL );

		// save the weight matrix

		itsWeights->Append(weights);

		// prepare for the next layer

		prevUnitCount = currUnitCount;
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

FFNetwork::~FFNetwork()
{
	delete itsRNG;
	delete itsLayerSizes;
	delete itsWeights;
}

/******************************************************************************
 GetEmptyWeightMatrix

	Return a matrix of the appropriate size for the given layer.

 ******************************************************************************/

JMatrix
FFNetwork::GetEmptyWeightMatrix
	(
	const JIndex layer
	)
	const
{
	JMatrix w(*(itsWeights->NthElement(layer)));
	w.SetAllElements(0.0);
	return w;
}

/******************************************************************************
 SetWeights

	Set the weights for the given layer.

 ******************************************************************************/

void
FFNetwork::SetWeights
	(
	const JIndex	layer,
	const JMatrix&	w
	)
{
	*(itsWeights->NthElement(layer)) = w;
}

/******************************************************************************
 Run

	Return the result of propagating the input through the network.

 ******************************************************************************/

JVector
FFNetwork::Run
	(
	const JVector& input
	)
	const
{
	return GetLayerOutput(input, itsLayerCount);
}

/******************************************************************************
 GetLayerOutput

	Return the output of the specified layer when the input is propagated
	through the network.

 ******************************************************************************/

JVector
FFNetwork::GetLayerOutput
	(
	const JVector&	input,
	const JIndex	index
	)
	const
{
	assert( 1 <= index && index <= itsLayerCount );

	JPtrArray<JVector> layerOutputs(JPtrArrayT::kDeleteAll);
	ForwardPropagate(input, &layerOutputs);

	JVector* output = layerOutputs.NthElement(index);
	return *output;
}

/******************************************************************************
 MeasurePerformance

	Returns the mean squared error between the data points
	and the network outputs.

 ******************************************************************************/

JFloat
FFNetwork::MeasurePerformance
	(
	const JMatrix& input,
	const JMatrix& correctOutput
	)
	const
{
	// create the (potentially) big objects once

	const JSize outputSize = GetLayerSize(itsLayerCount);

	JVector x(input.GetColCount());
	JVector y(outputSize);
	JVector output(outputSize);

	// compute the output for each input and compare with the correct output

	const JSize exampleCount = input.GetRowCount();
	assert( exampleCount == correctOutput.GetRowCount() );

	JFloat msError = 0.0;

	for (JSize i=1; i<=exampleCount; i++)
		{
		x      = input.GetRowVector(i);
		y      = correctOutput.GetRowVector(i);
		output = Run(x);

		for (JIndex j=1; j<=outputSize; j++)
			{
			const JFloat yj  = y.GetElement(j);
			const JFloat oj  = output.GetElement(j);
			const JFloat err = yj - oj;
			msError += err * err;
			}
		}

	return (msError/exampleCount);
}

/******************************************************************************
 MeasureBooleanAccuracy

	Returns the fraction of cases where the sign of the input and ouput match.
	*** This only works for a single output neuron.

 ******************************************************************************/

JFloat
FFNetwork::MeasureBooleanAccuracy
	(
	const JMatrix& input,
	const JMatrix& correctOutput
	)
	const
{
	// create the (potentially) big objects once

	const JSize outputSize = GetLayerSize(itsLayerCount);
	assert( outputSize == 1 );

	JVector x(input.GetColCount());
	JVector y(outputSize);
	JVector output(outputSize);

	// compute the output for each input and compare with the correct output

	const JSize exampleCount = input.GetRowCount();
	assert( exampleCount == correctOutput.GetRowCount() );

	JSize correctCount = 0;

	for (JSize i=1; i<=exampleCount; i++)
		{
		x      = input.GetRowVector(i);
		y      = correctOutput.GetRowVector(i);
		output = Run(x);

		if (y.GetElement(1) * output.GetElement(1) > 0.0)
			{
			correctCount++;
			}
		}

	return (correctCount / (JFloat) exampleCount);
}

/******************************************************************************
 Learn

	Learns the given set of examples (via backprop + momentum + weight decay)
	and returns the resulting apparent error rate.

	The examples are stored in the rows of the input and output matrices.

	To turn off momentum, set it to zero.  To turn off weight decay,
	set it to one.

	testInput and correctTestOutput can be NULL.  If they aren't,
	then they are used to calculate the test error as training proceeds.

	If logFile is not NULL, the training and testing error for each
	iteration are printed to it.

 ******************************************************************************/

void
FFNetwork::Learn
	(
	const JMatrix&	trainInput,
	const JMatrix&	correctTrainOutput,
	const JFloat	learningRate,
	const JFloat	momentum,
	const JFloat	weightDecay,
	const JFloat	errorTolerance,
	const JSize		maxIterCount,
	const JBoolean	useBatchMethod,
	const JMatrix*	testInput,
	const JMatrix*	correctTestOutput,
	JFloat*			trainError,
	JFloat*			testError,
	std::ostream*		logFile
	)
{
	// create space for dw's (we do this once because it's a lot of work)

	JPtrArray<JMatrix> olddw(JPtrArrayT::kDeleteAll),
					   currentdw(JPtrArrayT::kDeleteAll);

	for (JSize i=1;i<=itsLayerCount;i++)
		{
		JMatrix* weights = itsWeights->NthElement(i);

		JMatrix* dw = new JMatrix(*weights);
		assert( dw != NULL );
		dw->SetAllElements(0.0);
		olddw.Append(dw);

		dw = new JMatrix(*weights);
		assert( dw != NULL );
		dw->SetAllElements(0.0);
		currentdw.Append(dw);
		}

	// we continue to train on all examples
	// until we are within the error tolerance
	// or until we exceed the maximum iteration count

	const JSize exampleCount = trainInput.GetRowCount();
	assert( exampleCount == correctTrainOutput.GetRowCount() );

	JProgressDisplay* pg = JNewPG();
	pg->VariableLengthProcessBeginning("Training neural network...",
									   kJTrue, kJFalse);

	DisplayPerformance(*pg, 0,
					   trainInput, correctTrainOutput, trainError,
					   testInput, correctTestOutput, testError, logFile);

	JIndex iter = 0;
	while (*trainError > errorTolerance && iter < maxIterCount)
		{
		if (useBatchMethod)
			{
			AdjustWeightsForExamples(trainInput, correctTrainOutput,
									 learningRate, momentum,
									 &olddw, &currentdw);
			}
		else
			{
			JVector x(trainInput.GetColCount());
			JVector y(correctTrainOutput.GetColCount());
			for (JSize i=1; i<=exampleCount; i++)
				{
				x = trainInput.GetRowVector(i);
				y = correctTrainOutput.GetRowVector(i);
				AdjustWeightsForExample(x, y, learningRate, momentum,
										&olddw, &currentdw);
				}
			}

		DecayWeights(weightDecay);

		iter++;
		if (!DisplayPerformance(*pg, iter,
								trainInput, correctTrainOutput, trainError,
								testInput, correctTestOutput, testError,
								logFile))
			{
			break;
			}
		}

	// clean up

	pg->ProcessFinished();
	delete pg;
}

/******************************************************************************
 DisplayPerformance (private)

	Display the performance on the training set and the testing set
	(if it exists).  Returns kJFalse if the user cancelled the process.

 ******************************************************************************/

JBoolean
FFNetwork::DisplayPerformance
	(
	JProgressDisplay&	pg,
	const JIndex		iteration,
	const JMatrix&		trainInput,
	const JMatrix&		correctTrainOutput,
	JFloat*				trainError,
	const JMatrix*		testInput,
	const JMatrix*		correctTestOutput,
	JFloat*				testError,
	std::ostream*			logFile
	)
	const
{
	if (iteration % 100 > 0)
		{
		return kJTrue;
		}

	*trainError = MeasurePerformance(trainInput, correctTrainOutput);

	JString pgStr(iteration, 0);
	pgStr += " ms error = ";
	pgStr += *trainError;
	if (logFile != NULL)
		{
		*logFile << *trainError;
		}

	if (testInput != NULL && correctTestOutput != NULL &&
		testError != NULL)
		{
		*testError = MeasurePerformance(*testInput, *correctTestOutput);
		pgStr += ",  ";
		pgStr += *testError;

		if (logFile != NULL)
			{
			*logFile << ' ' << *testError;
			}
		}

	if (logFile != NULL)
		{
		*logFile << std::endl;
		}

	return pg.IncrementProgress(pgStr);
}

/******************************************************************************
 AdjustWeightsForExamples (private)

	Increments the network weights based on the given examples.
	(batch mode)

 ******************************************************************************/

void
FFNetwork::AdjustWeightsForExamples
	(
	const JMatrix&		input,
	const JMatrix&		correctOutput,
	const JFloat		learningRate,
	const JFloat		momentum,
	JPtrArray<JMatrix>*	olddws,
	JPtrArray<JMatrix>*	currentdws
	)
{
JIndex i;

	const JSize exampleCount = input.GetRowCount();
	assert( exampleCount == correctOutput.GetRowCount() );

	for (i=1; i<=itsLayerCount; i++)
		{
		JMatrix* dw = currentdws->NthElement(i);
		dw->SetAllElements(0.0);
		}

	JVector x(input.GetColCount());
	JVector y(correctOutput.GetColCount());
	for (i=1; i<=exampleCount; i++)
		{
		x = input.GetRowVector(i);
		y = correctOutput.GetRowVector(i);
		CalcWeightChangeForExample(x, y, learningRate/exampleCount, currentdws);
		}

	IncrementWeights(momentum, olddws, *currentdws);
}

/******************************************************************************
 AdjustWeightsForExample (private)

	Increments the network weights based on the given example.

 ******************************************************************************/

void
FFNetwork::AdjustWeightsForExample
	(
	const JVector&		input,
	const JVector&		correctOutput,
	const JFloat		learningRate,
	const JFloat		momentum,
	JPtrArray<JMatrix>*	olddws,
	JPtrArray<JMatrix>*	currentdws
	)
{
	for (JIndex i=1; i<=itsLayerCount; i++)
		{
		JMatrix* dw = currentdws->NthElement(i);
		dw->SetAllElements(0.0);
		}

	CalcWeightChangeForExample(input, correctOutput, learningRate, currentdws);
	IncrementWeights(momentum, olddws, *currentdws);
}

/******************************************************************************
 CalcWeightChangeForExample (private)

	Calculates the change in the network weights based on the given example.

 ******************************************************************************/

void
FFNetwork::CalcWeightChangeForExample
	(
	const JVector&		input,
	const JVector&		correctOutput,
	const JFloat		learningRate,
	JPtrArray<JMatrix>*	dws
	)
	const
{
	// propagate the input forward through the network

	JPtrArray<JVector> layerOutputs(JPtrArrayT::kDeleteAll);
	ForwardPropagate(input, &layerOutputs);

	// propagate the errors back through the network

	JPtrArray<JVector> deltas(JPtrArrayT::kDeleteAll);
	BackPropagate(correctOutput, layerOutputs, &deltas);

	// calculate the adjustments to the weights

	for (JIndex i=1; i<=itsLayerCount; i++)
		{
		const JVector* output;
		if (i == 1)
			{
			output = &input;
			}
		else
			{
			output = layerOutputs.NthElement(i-1);
			}
		const JVector newOutput = PrependOffsetUnit(*output);

		JVector* delta  = deltas.NthElement(i);
		JMatrix* dw     = dws->NthElement(i);
		*dw            -= learningRate * JOuterProduct(*delta, newOutput);
		}
}

/******************************************************************************
 IncrementWeights (private)

	Increments the network weights based on the given example.

 ******************************************************************************/

void
FFNetwork::IncrementWeights
	(
	const JFloat				momentum,
	JPtrArray<JMatrix>*			olddws,
	const JPtrArray<JMatrix>&	currentdws
	)
{
	for (JIndex i=1; i<=itsLayerCount; i++)
		{
		const JMatrix* dw = currentdws.NthElement(i);

		JMatrix* olddw = olddws->NthElement(i);
		*olddw *= momentum;
		*olddw += *dw;		// this automatically saves it for next time

		JMatrix* weights = itsWeights->NthElement(i);
		*weights += *olddw;
		}
}

/******************************************************************************
 DecayWeights (private)

	Shrink all the weights by the given factor.

 ******************************************************************************/

void
FFNetwork::DecayWeights
	(
	const JFloat decayFactor
	)
{
	if (decayFactor < 1.0)
		{
		for (JIndex i=1; i<=itsLayerCount; i++)
			{
			JMatrix* weights = itsWeights->NthElement(i);
			*weights *= decayFactor;
			}
		}
}

/******************************************************************************
 ForwardPropagate (private)

	Propagates the input through the network and returns
	the output of each layer.

 ******************************************************************************/

void
FFNetwork::ForwardPropagate
	(
	const JVector&		input,
	JPtrArray<JVector>*	layerOutputs
	)
	const
{
	JVector* layerOutput = new JVector(input);
	assert( layerOutput != NULL );

	for (JSize i=1;i<=itsLayerCount;i++)
		{
		JVector newOutput = PrependOffsetUnit(*layerOutput);
		if (i==1)
			{
			delete layerOutput;
			}

		JMatrix* weights = itsWeights->NthElement(i);
		JMatrix netInput = (*weights) * newOutput;

		layerOutput = new JVector(g(i, netInput.GetColVector(1)));
		assert( layerOutput != NULL );
		layerOutputs->Append(layerOutput);
		}
}

/******************************************************************************
 BackPropagate (private)

	Propagates the errors back through the network and
	returns the delta's for each layer.

 ******************************************************************************/

void
FFNetwork::BackPropagate
	(
	const JVector&				correctOutput,
	const JPtrArray<JVector>&	layerOutputs,
	JPtrArray<JVector>*			deltas
	)
	const
{
	// compute delta's for output

	const JSize outputSize = GetLayerSize(itsLayerCount);
	JVector* outputDelta   = new JVector(outputSize);
	assert( outputDelta != NULL );

	const JVector* actualOutput = layerOutputs.LastElement();
	*outputDelta                = *actualOutput - correctOutput;
	{
	for (JIndex i=1; i<=outputSize; i++)
		{
		const JFloat actualOutputi = actualOutput->GetElement(i);
		const JFloat deltai = 2.0 *
			gprime(itsLayerCount, actualOutputi) * outputDelta->GetElement(i);
		outputDelta->SetElement(i, deltai);
		}
	}

	deltas->Prepend(outputDelta);

	// work backwards to compute delta's for the rest of the layers

	JVector* prevDelta = outputDelta;

	for (JIndex j=itsLayerCount; j>1; j--)
		{
		const JSize layerSize = GetLayerSize(j-1);

		JMatrix* weights = itsWeights->NthElement(j);
		JMatrix temp     = (weights->Transpose()) * (*prevDelta);
		JVector delta1   = temp.GetColVector(1);

		JVector* delta = new JVector(RemoveOffsetUnit(delta1));
		assert( delta != NULL );

		const JVector* actualOutput = layerOutputs.NthElement(j-1);

		for (JIndex i=1; i<=layerSize; i++)
			{
			const JFloat actualOutputi = actualOutput->GetElement(i);
			const JFloat deltai =
				gprime(j-1, actualOutputi) * delta->GetElement(i);
			delta->SetElement(i, deltai);
			}

		deltas->Prepend(delta);
		prevDelta = delta;
		}
}

/******************************************************************************
 RandomizeWeights

 ******************************************************************************/

void
FFNetwork::RandomizeWeights
	(
	const JFloat magnitude
	)
{
	for (JIndex i=1; i<=itsLayerCount; i++)
		{
		JMatrix* weights     = itsWeights->NthElement(i);
		const JSize rowCount = weights->GetRowCount();
		const JSize colCount = weights->GetColCount();

		for (JIndex row=1; row<=rowCount; row++)
			{
			for (JIndex col=1; col<=colCount; col++)
				{
				weights->SetElement(row,col, itsRNG->UniformDouble(-magnitude, magnitude));
				}
			}
		}
}

/******************************************************************************
 PrintWeights

 ******************************************************************************/

void
FFNetwork::PrintWeights
	(
	std::ostream& output
	)
	const
{
	for (JIndex i=1; i<=itsLayerCount; i++)
		{
		JMatrix* weights = itsWeights->NthElement(i);

		output << "Layer " << i << ':' << std::endl;
		output << *weights << std::endl << std::endl;
		}
}

/******************************************************************************
 PrependOffsetUnit (private)

	Prepend a -1 to the given vector to prepare it for input to the next layer.

 ******************************************************************************/

JVector
FFNetwork::PrependOffsetUnit
	(
	const JVector& layerOutput
	)
	const
{
	const JSize currentSize = layerOutput.GetDimensionCount();

	JVector newInput(currentSize+1, -1.0);

	for (JIndex i=1; i<=currentSize; i++)
		{
		const JFloat value = layerOutput.GetElement(i);
		newInput.SetElement(i+1, value);
		}

	return newInput;
}

/******************************************************************************
 RemoveOffsetUnit (private)

	Remove the first element of the given vector.  Used by BackPropagate.

 ******************************************************************************/

JVector
FFNetwork::RemoveOffsetUnit
	(
	const JVector& v
	)
	const
{
	const JSize currentSize = v.GetDimensionCount();

	JVector vnew(currentSize-1);

	for (JIndex i=1; i<=currentSize-1; i++)
		{
		const JFloat value = v.GetElement(i+1);
		vnew.SetElement(i, value);
		}

	return vnew;
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
FFNetwork::StreamOut
	(
	std::ostream& output
	)
	const
{
JIndex i;

	output << itsLayerCount;

	for (i=0; i<=itsLayerCount; i++)
		{
		output << ' ' << itsLayerSizes->GetElement(i+1);
		}

	for (i=1; i<=itsLayerCount; i++)
		{
		output << ' ' << *(itsWeights->NthElement(i));
		}
}
