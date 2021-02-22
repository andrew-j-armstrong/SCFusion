#pragma once

#include "../Core/Vector.h"

#include "GASequenceChromosome.h"

// Demonstrates FitnessCalc class for evaluating matches to a string (using sequence chromosomes)
class CGAStringFitnessCalc
{
public:
	CGAStringFitnessCalc(CVector<char> &target) : m_target(target) {}

	bool ValidateAndCalculateFitness(CGASequenceChromosome<char> &chromosome, double &fitness) const;
	bool SatisfiesTarget(const CGASequenceChromosome<char> &chromosome) const;

protected:
	CVector<char> m_target;
};
