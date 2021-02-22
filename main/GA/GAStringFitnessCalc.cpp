#include "stdafx.h"
#include "GAStringFitnessCalc.h"

bool CGAStringFitnessCalc::ValidateAndCalculateFitness(CGASequenceChromosome<char> &chromosome, double &fitness) const
{
	size_t i;
	for(i=0; i < m_target.size() && i < chromosome.GetValue().size(); i++)
	{
		if(m_target[i] != chromosome.GetValue()[i])
			break;
	}

	fitness = 10 * (double)i / (10 + chromosome.GetValue().size() - i);

	return true;
}

bool CGAStringFitnessCalc::SatisfiesTarget(const CGASequenceChromosome<char> &chromosome) const
{
	if(m_target.size() != chromosome.GetValue().size())
		return false;

	for(size_t i=0; i < m_target.size(); i++)
	{
		if(m_target[i] != chromosome.GetValue()[i])
			return false;
	}

	return true;
}
