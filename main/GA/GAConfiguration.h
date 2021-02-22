#pragma once

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness>
class CGAConfiguration
{
public:
	CGAConfiguration(const TMutator &mutator, const TFitnessCalc &fitnessCalc);

	bool Mutate(TChromosome &chromosome) const { return m_mutator.Mutate(chromosome); }
	void InitRandomChromosome(TChromosome &chromosome) const { return m_mutator.InitRandomChromosome(chromosome); }

	bool ValidateAndCalculateFitness(TChromosome &chromosome, TFitness &fitness) const;
	bool SatisfiesTarget(const TChromosome &chromosome) const { return m_fitnessCalc.SatisfiesTarget(chromosome); }

protected:
	const TMutator &m_mutator;
	const TFitnessCalc &m_fitnessCalc;
};

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness>
CGAConfiguration<TChromosome, TMutator, TFitnessCalc, TFitness>::CGAConfiguration(const TMutator &mutator, const TFitnessCalc &fitnessCalc)
: m_mutator(mutator), m_fitnessCalc(fitnessCalc)
{
}

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness>
bool CGAConfiguration<TChromosome, TMutator, TFitnessCalc, TFitness>::ValidateAndCalculateFitness(TChromosome &chromosome, TFitness &fitness) const
{
	if(!m_fitnessCalc.ValidateAndCalculateFitness(chromosome, fitness))
		return false;

	return true;
}
