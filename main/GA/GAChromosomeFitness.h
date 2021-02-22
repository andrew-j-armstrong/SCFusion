#pragma once

template<typename TChromosome, typename TFitness>
class CGAChromosomeFitness
{
public:
	CGAChromosomeFitness(TChromosome *chromosome) : m_chromosome(chromosome), m_fitness() {}
	CGAChromosomeFitness(TChromosome *chromosome, const TFitness &fitness) : m_chromosome(chromosome), m_fitness(fitness) {}
	CGAChromosomeFitness(const CGAChromosomeFitness<TChromosome, TFitness> &chromosomeFitness) : m_chromosome(new TChromosome(chromosomeFitness.GetChromosome())), m_fitness(chromosomeFitness.GetFitness()) {}

	~CGAChromosomeFitness() { delete m_chromosome; }

	const TChromosome *GetChromosome() const { return m_chromosome; }
	const TFitness &GetFitness() const { return m_fitness; }

	TChromosome *GetChromosome() { return m_chromosome; }
	TFitness &GetFitness() { return m_fitness; }

	static int Compare(const void *p1, const void *p2)
	{
		const CGAChromosomeFitness<TChromosome, TFitness> *chromosome1 = *(const CGAChromosomeFitness<TChromosome, TFitness> **)(p1);
		const CGAChromosomeFitness<TChromosome, TFitness> *chromosome2 = *(const CGAChromosomeFitness<TChromosome, TFitness> **)(p2);
		if(!chromosome1)
			return 1;
		else if(!chromosome2)
			return -1;
//		else if(chromosome2->GetFitness() < chromosome1->GetFitness())
//			return -1;
//		else if(chromosome1->GetFitness() < chromosome2->GetFitness())
//			return 1;
		else
		{
			int fitnessDiff = chromosome1->GetFitness().Compare(chromosome2->GetFitness());

			if(fitnessDiff != 0)
				return -fitnessDiff;
			else
				return chromosome1->GetChromosome()->Compare(*chromosome2->GetChromosome());
		}
	}

protected:
	TChromosome *m_chromosome;
	TFitness m_fitness;
};
