#pragma once

#include "../Core/Vector.h"

#include "GAConfiguration.h"
#include "GAPopulationSort.h"
#include "GAChromosomeFitness.h"

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort>
class CGAPopulation
{
public:
	CGAPopulation(const CGAConfiguration<TChromosome, TMutator, TFitnessCalc, TFitness> &config, size_t maxPopulation, size_t maxElitism, bool recalcEliteFitness);
	~CGAPopulation();

	void Initialise(size_t initialPopulation);

	bool AddChromosome(const TChromosome &value);

	const CGAConfiguration<TChromosome, TMutator, TFitnessCalc, TFitness> &config() const { return m_config; }

	size_t populationCount() const { return m_populationCount; }
	unsigned long long gameCount() const { return m_gameCount; }

	const TFitness GetBestFitness() const { return m_population[0]->GetFitness(); }
	const TChromosome GetBestChromosome() const { return *m_population[0]->GetChromosome(); }
	bool IsSatisfied() const { return m_config.SatisfiesTarget(GetBestChromosome()); }

	void Sort() { m_sorter.Sort(m_population, m_populationCount); }
	bool Evolve();
	void Clear();
	void FreeCapacity(size_t capacity);

	void Print(size_t maxCount = 0);

	size_t StagnationCount() const { return m_stagnationCount; }
	size_t PopulationCount() const { return m_populationCount; }
	size_t MaxPopulation() const { return m_maxPopulation; }

protected:
	void ClearPopulation(CGAChromosomeFitness<TChromosome, TFitness> **population, size_t populationCount);

	CMemoryPool *m_populationMemPool;

	const CGAConfiguration<TChromosome, TMutator, TFitnessCalc, TFitness> &m_config;

	bool m_recalcEliteFitness;
	TPopulationSort<TChromosome, TFitness> m_sorter;
	size_t m_maxPopulation;
	size_t m_maxElitism;
	size_t m_populationCount;
	size_t m_stagnationCount;
	unsigned long long m_gameCount;
	TChromosome m_bestChromosome;
	TFitness m_bestFitness;

	CGAChromosomeFitness<TChromosome, TFitness> **m_population;
};

template<typename TFitness>
inline TFitness InitFitness()
{
	return TFitness();
}

template<>
inline double InitFitness()
{
	return -1e308;
}

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort>
CGAPopulation<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>::CGAPopulation(const CGAConfiguration<TChromosome, TMutator, TFitnessCalc, TFitness> &config, size_t maxPopulation, size_t maxElitism, bool recalcEliteFitness)
: m_config(config), m_maxPopulation(maxPopulation), m_maxElitism(maxElitism), m_recalcEliteFitness(recalcEliteFitness), m_sorter(maxPopulation), m_populationMemPool(CMemoryPoolManager::Get()->GetMemoryPoolAddAsNeeded(maxPopulation * sizeof(TChromosome *))), m_populationCount(0), m_stagnationCount(0), m_gameCount(0), m_bestFitness(InitFitness<TFitness>()), m_bestChromosome()
{
	m_population = (CGAChromosomeFitness<TChromosome, TFitness> **)m_populationMemPool->Alloc();
}

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort>
void CGAPopulation<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>::Clear()
{
	ClearPopulation(m_population, m_populationCount);
	m_populationCount = 0;
	m_stagnationCount = 0;
	m_sorter.Reset(m_population, m_populationCount);
}

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort>
CGAPopulation<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>::~CGAPopulation()
{
	ClearPopulation(m_population, m_populationCount);
	m_populationMemPool->Free(m_population);
}

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort>
void CGAPopulation<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>::Initialise(size_t initialPopulation)
{
	m_bestFitness = InitFitness<TFitness>();

	for(size_t j=0; j < initialPopulation; j++)
	{
		TChromosome junk;
		m_config.InitRandomChromosome(junk);
		AddChromosome(junk);
	}

	m_sorter.Sort(m_population, m_populationCount);
}

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort>
void CGAPopulation<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>::ClearPopulation(CGAChromosomeFitness<TChromosome, TFitness> **population, size_t populationCount)
{
	for(size_t i=0; i < populationCount; i++)
		if(m_population[i])
			delete population[i];
}

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort>
void CGAPopulation<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>::FreeCapacity(size_t size)
{
	if(size + m_populationCount <= m_maxPopulation)
		return;

	if(size > m_maxPopulation)
		size = m_maxPopulation;

	size = m_maxPopulation - size;

	for(CGAChromosomeFitness<TChromosome, TFitness> **populationPtr=&m_population[m_populationCount - 1]; m_populationCount > size; populationPtr--, m_populationCount--)
		delete *populationPtr;
}

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort>
bool CGAPopulation<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>::AddChromosome(const TChromosome &value)
{
	CGAChromosomeFitness<TChromosome, TFitness> *newChromosome = new CGAChromosomeFitness<TChromosome, TFitness>(new TChromosome(value));
	m_config.ValidateAndCalculateFitness(*newChromosome->GetChromosome(), newChromosome->GetFitness());
	m_sorter.Insert(m_population, m_populationCount, newChromosome);

	return true;
}

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort>
bool CGAPopulation<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>::Evolve()
{
	CGAChromosomeFitness<TChromosome, TFitness> **newPopulation = (CGAChromosomeFitness<TChromosome, TFitness> **)m_populationMemPool->Alloc();
	memset(newPopulation, 0, m_maxPopulation * sizeof(CGAChromosomeFitness<TChromosome, TFitness> *));

	size_t newPopulationCount = 0;

	size_t breedingStockLimit = (7 * m_populationCount) / 8;

	double totalFitness = 0, minimumFitness = 1e308;
	for(size_t i=0; i < breedingStockLimit; i++)
	{
		double fitness = (double)m_population[i]->GetFitness();
		totalFitness += fitness;
		if(fitness < minimumFitness)
			minimumFitness = fitness;
	}

	double totalFitnessDiff = totalFitness - (breedingStockLimit * minimumFitness);

	double totalFitnessRange = totalFitnessDiff * 1.1;
	double fitnessAdjust = ((0.1 * totalFitnessDiff) / breedingStockLimit) - minimumFitness;

	// Elitism
	size_t maxElitism = m_maxElitism;
	if(maxElitism > m_populationCount)
		maxElitism = m_populationCount;

	for(size_t i=0; i < maxElitism; i++)
	{
		newPopulation[newPopulationCount++] = m_population[i];
		if(m_recalcEliteFitness)
		{
			m_population[i]->GetFitness().StartRecalc();
			m_config.ValidateAndCalculateFitness(*m_population[i]->GetChromosome(), m_population[i]->GetFitness());
		}
	}

	m_sorter.Reset(newPopulation, newPopulationCount);

	CGAChromosomeFitness<TChromosome, TFitness> *newChromosome;
	size_t maxPopulation = (7 * m_maxPopulation) / 8;
	if(maxPopulation > m_populationCount * 2)
		maxPopulation = m_populationCount * 2;

	double curFitness = 0.0;
	CVector<short> rouletteIndex;
	rouletteIndex.capacity(breedingStockLimit);
	for(size_t i=0; i < breedingStockLimit; i++)
	{
		curFitness += m_population[i]->GetFitness() + fitnessAdjust;
		short randValue = (short)((curFitness * (RAND_MAX + 1)) / totalFitnessRange);
		rouletteIndex.push_back(randValue);
	}

	while(newPopulationCount < maxPopulation)
	{
		// Breeding
		TChromosome *parentA = m_population[rouletteIndex.binarySearch(rand_sse())]->GetChromosome();
		TChromosome *parentB = m_population[rouletteIndex.binarySearch(rand_sse())]->GetChromosome();

		short crossover = rand_sse();

		newChromosome = new CGAChromosomeFitness<TChromosome, TFitness>(parentA->Breed(parentB, crossover));
		m_config.Mutate(*newChromosome->GetChromosome());
		m_config.ValidateAndCalculateFitness(*newChromosome->GetChromosome(), newChromosome->GetFitness());
		m_sorter.Insert(newPopulation, newPopulationCount, newChromosome);

		newChromosome = new CGAChromosomeFitness<TChromosome, TFitness>(parentB->Breed(parentA, crossover));
		m_config.Mutate(*newChromosome->GetChromosome());
		m_config.ValidateAndCalculateFitness(*newChromosome->GetChromosome(), newChromosome->GetFitness());
		m_sorter.Insert(newPopulation, newPopulationCount, newChromosome);

		m_gameCount += 2;
	}

	// New random chromosomes
	while(newPopulationCount < m_maxPopulation)
	{
		newChromosome = new CGAChromosomeFitness<TChromosome, TFitness>(new TChromosome());
		m_config.InitRandomChromosome(*newChromosome->GetChromosome());
		m_config.ValidateAndCalculateFitness(*newChromosome->GetChromosome(), newChromosome->GetFitness());
		m_sorter.Insert(newPopulation, newPopulationCount, newChromosome);
		m_gameCount++;
	}

	// Sort
	m_sorter.Sort(newPopulation, newPopulationCount);

	// Remove duplicates
	m_sorter.RemoveDuplicates(newPopulation, newPopulationCount);

	// Clear old population
	for(size_t i=0; i < maxElitism; i++)
		m_population[i] = 0; // Elitism

	ClearPopulation(m_population, m_populationCount);
	m_populationMemPool->Free(m_population);

	m_population = newPopulation;
	m_populationCount = newPopulationCount;

	if( (m_recalcEliteFitness && !(m_bestChromosome == *m_population[0]->GetChromosome()))
		|| (!m_recalcEliteFitness && m_bestFitness < m_population[0]->GetFitness()))
	{
		m_bestChromosome = *m_population[0]->GetChromosome();
		m_bestFitness = m_population[0]->GetFitness();
		m_stagnationCount = 0;
	}
	else
		m_stagnationCount++;
	
	return true;
}

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort>
void CGAPopulation<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>::Print(size_t maxCount /* = 0 */)
{
	cout << "Population count: " << m_populationCount << endl;
	for(size_t i=0; i < m_populationCount && i < maxCount; i++)
	{
		cout << "Chromosome " << i << ": ";
		m_population[i]->Print();
	}

	cout << "";
}

