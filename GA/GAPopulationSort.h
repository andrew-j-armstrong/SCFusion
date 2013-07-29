#pragma once

#include "GAChromosomeFitness.h"

template<typename TChromosome, typename TFitness>
class CGAPopulationFullSortAndRemoveDuplicates
{
public:
	CGAPopulationFullSortAndRemoveDuplicates(size_t maxPopulation);
	~CGAPopulationFullSortAndRemoveDuplicates() {}

	void Reset(CGAChromosomeFitness<TChromosome, TFitness> **population, size_t populationCount) {}
	void Insert(CGAChromosomeFitness<TChromosome, TFitness> **population, size_t &populationCount, CGAChromosomeFitness<TChromosome, TFitness> *chromosomeFitness) const;
	void Sort(CGAChromosomeFitness<TChromosome, TFitness> **population, size_t populationCount) const;
	void RemoveDuplicates(CGAChromosomeFitness<TChromosome, TFitness> **popoulation, size_t &populationCount) const;

protected:
	size_t m_maxPopulation;
};

template<typename TChromosome, typename TFitness>
CGAPopulationFullSortAndRemoveDuplicates<TChromosome, TFitness>::CGAPopulationFullSortAndRemoveDuplicates(size_t maxPopulation)
	: m_maxPopulation(maxPopulation)
{
}

template<typename TChromosome, typename TFitness>
void CGAPopulationFullSortAndRemoveDuplicates<TChromosome, TFitness>::Insert(CGAChromosomeFitness<TChromosome, TFitness> **population, size_t &populationCount, CGAChromosomeFitness<TChromosome, TFitness> *chromosomeFitness) const
{
	if(populationCount < m_maxPopulation)
		population[populationCount++] = chromosomeFitness;
	else
		delete chromosomeFitness;
}

template<typename TChromosome, typename TFitness>
void CGAPopulationFullSortAndRemoveDuplicates<TChromosome, TFitness>::Sort(CGAChromosomeFitness<TChromosome, TFitness> **population, size_t populationCount) const
{
	qsort(population, populationCount, sizeof(CGAChromosomeFitness<TChromosome, TFitness> *), CGAChromosomeFitness<TChromosome, TFitness>::Compare);
}

template<typename TChromosome, typename TFitness>
void CGAPopulationFullSortAndRemoveDuplicates<TChromosome, TFitness>::RemoveDuplicates(CGAChromosomeFitness<TChromosome, TFitness> **population, size_t &populationCount) const
{
	size_t curIndex = 0;
	for(size_t i=0; i < populationCount; i++, curIndex++)
	{
		for(; i < populationCount - 1 && *population[i]->GetChromosome() == *population[i+1]->GetChromosome(); i++)
			delete population[i];

		if(curIndex != i)
			population[curIndex] = population[i];
	}

	populationCount = curIndex;
}

template<typename TChromosome, typename TFitness>
class CGAPopulationPartialSortAndRemoveDuplicates
{
public:
	CGAPopulationPartialSortAndRemoveDuplicates(size_t maxPopulation);
	~CGAPopulationPartialSortAndRemoveDuplicates() {}

	void Reset(CGAChromosomeFitness<TChromosome, TFitness> **population, size_t populationCount);
	void Insert(CGAChromosomeFitness<TChromosome, TFitness> **population, size_t &populationCount, CGAChromosomeFitness<TChromosome, TFitness> *chromosomeFitness);
	void Sort(CGAChromosomeFitness<TChromosome, TFitness> **population, size_t populationCount);
	void RemoveDuplicates(CGAChromosomeFitness<TChromosome, TFitness> **popoulation, size_t &populationCount);

protected:
	size_t m_maxPopulation;
	size_t m_unsortedStartIndex;
	size_t m_unsortedEndIndex;
	size_t m_potentialDuplicatesStartIndex;
	size_t m_potentialDuplicatesEndIndex;
};

template<typename TChromosome, typename TFitness>
CGAPopulationPartialSortAndRemoveDuplicates<TChromosome, TFitness>::CGAPopulationPartialSortAndRemoveDuplicates(size_t maxPopulation)
	: m_maxPopulation(maxPopulation)
	, m_unsortedStartIndex(0)
	, m_unsortedEndIndex(0)
	, m_potentialDuplicatesStartIndex(0)
	, m_potentialDuplicatesEndIndex(0)
{
}

template<typename TChromosome, typename TFitness>
void CGAPopulationPartialSortAndRemoveDuplicates<TChromosome, TFitness>::Reset(CGAChromosomeFitness<TChromosome, TFitness> **population, size_t populationCount)
{
	m_unsortedStartIndex = 0;
	if(populationCount < m_maxPopulation / 8)
	{
		m_unsortedEndIndex = populationCount;
		m_potentialDuplicatesStartIndex = populationCount;
		m_potentialDuplicatesEndIndex = populationCount;
	}
	else
	{
		m_unsortedEndIndex = m_maxPopulation / 8;
		m_potentialDuplicatesStartIndex = m_maxPopulation / 8;
		m_potentialDuplicatesEndIndex = m_maxPopulation / 8;
	}
}

template<typename TChromosome, typename TFitness>
void CGAPopulationPartialSortAndRemoveDuplicates<TChromosome, TFitness>::Insert(CGAChromosomeFitness<TChromosome, TFitness> **population, size_t &populationCount, CGAChromosomeFitness<TChromosome, TFitness> *chromosomeFitness)
{
	if(populationCount < m_maxPopulation/8)
	{
		population[populationCount++] = chromosomeFitness;
		m_unsortedEndIndex++;

		// TODO: Find start indices
	}
	else if(populationCount < m_maxPopulation)
	{
		if(m_unsortedEndIndex == populationCount)
		{
			population[populationCount++] = chromosomeFitness;
			m_unsortedEndIndex++;
		}
		else if(chromosomeFitness->GetFitness() < population[m_unsortedEndIndex]->GetFitness())
		{
			population[populationCount++] = chromosomeFitness;
		}
		else
		{
			population[populationCount++] = population[m_unsortedEndIndex+1];
			population[m_unsortedEndIndex+1] = population[m_unsortedEndIndex];
			population[m_unsortedEndIndex] = chromosomeFitness;
			m_unsortedEndIndex++;

			// TODO: Find start indices
		}
	}
	else
		delete chromosomeFitness;
}

template<typename TChromosome, typename TFitness>
void CGAPopulationPartialSortAndRemoveDuplicates<TChromosome, TFitness>::Sort(CGAChromosomeFitness<TChromosome, TFitness> **population, size_t populationCount)
{
	qsort(&population[m_unsortedStartIndex], m_unsortedEndIndex - m_unsortedStartIndex, sizeof(CGAChromosomeFitness<TChromosome, TFitness> *), CGAChromosomeFitness<TChromosome, TFitness>::Compare);

	m_potentialDuplicatesStartIndex = m_unsortedStartIndex;
	if(populationCount < m_maxPopulation/8)
	{
		//m_unsortedStartIndex = populationCount;
		m_unsortedEndIndex = populationCount;
	}
	else
	{
		//m_unsortedStartIndex = m_maxPopulation/8;
		m_unsortedEndIndex = m_maxPopulation/8;
	}
	m_potentialDuplicatesEndIndex = m_unsortedEndIndex;
}

template<typename TChromosome, typename TFitness>
void CGAPopulationPartialSortAndRemoveDuplicates<TChromosome, TFitness>::RemoveDuplicates(CGAChromosomeFitness<TChromosome, TFitness> **population, size_t &populationCount)
{
	size_t i = m_potentialDuplicatesStartIndex, curIndex = m_potentialDuplicatesStartIndex;

	for(; i < m_potentialDuplicatesEndIndex; i++, curIndex++)
	{
		for(; i < m_potentialDuplicatesEndIndex - 1 && *population[i]->GetChromosome() == *population[i+1]->GetChromosome(); i++)
			delete population[i];

		if(curIndex != i)
			population[curIndex] = population[i];
	}

	if(curIndex != i)
	{
		for(; i < populationCount; i++, curIndex++)
			population[curIndex] = population[i];

		populationCount = curIndex;
	}

	m_potentialDuplicatesStartIndex = 0;
	m_potentialDuplicatesEndIndex = 0;
}

template<typename TChromosome, typename TFitness>
class CGAPopulationPartialSortAndIgnoreDuplicates
{
public:
	CGAPopulationPartialSortAndIgnoreDuplicates(size_t maxPopulation);
	~CGAPopulationPartialSortAndIgnoreDuplicates() {}

	void Reset(CGAChromosomeFitness<TChromosome, TFitness> **population, size_t populationCount);
	void Insert(CGAChromosomeFitness<TChromosome, TFitness> **population, size_t &populationCount, CGAChromosomeFitness<TChromosome, TFitness> *chromosomeFitness);
	void Sort(CGAChromosomeFitness<TChromosome, TFitness> **population, size_t populationCount);
	void RemoveDuplicates(CGAChromosomeFitness<TChromosome, TFitness> **popoulation, size_t &populationCount) const {}

protected:
	size_t m_maxPopulation;
	size_t m_unsortedStartIndex;
	size_t m_unsortedEndIndex;
};

template<typename TChromosome, typename TFitness>
CGAPopulationPartialSortAndIgnoreDuplicates<TChromosome, TFitness>::CGAPopulationPartialSortAndIgnoreDuplicates(size_t maxPopulation)
	: m_maxPopulation(maxPopulation)
	, m_unsortedStartIndex(0)
	, m_unsortedEndIndex(0)
{
}

template<typename TChromosome, typename TFitness>
void CGAPopulationPartialSortAndIgnoreDuplicates<TChromosome, TFitness>::Reset(CGAChromosomeFitness<TChromosome, TFitness> **population, size_t populationCount)
{
	m_unsortedStartIndex = 0;
	if(populationCount < m_maxPopulation / 8)
	{
		m_unsortedEndIndex = populationCount;
	}
	else
	{
		m_unsortedEndIndex = m_maxPopulation / 8;
	}
}

template<typename TChromosome, typename TFitness>
void CGAPopulationPartialSortAndIgnoreDuplicates<TChromosome, TFitness>::Insert(CGAChromosomeFitness<TChromosome, TFitness> **population, size_t &populationCount, CGAChromosomeFitness<TChromosome, TFitness> *chromosomeFitness)
{
	if(populationCount < m_maxPopulation/8)
	{
		population[populationCount++] = chromosomeFitness;
		m_unsortedEndIndex++;

		// TODO: Find start indices
	}
	else if(populationCount < m_maxPopulation)
	{
		if(m_unsortedEndIndex == populationCount)
		{
			population[populationCount++] = chromosomeFitness;
			m_unsortedEndIndex++;
		}
		else if(chromosomeFitness->GetFitness() < population[m_unsortedEndIndex]->GetFitness())
		{
			population[populationCount++] = chromosomeFitness;
		}
		else
		{
			population[populationCount++] = population[m_unsortedEndIndex+1];
			population[m_unsortedEndIndex+1] = population[m_unsortedEndIndex];
			population[m_unsortedEndIndex] = chromosomeFitness;
			m_unsortedEndIndex++;

			// TODO: Find start indices
		}
	}
	else
		delete chromosomeFitness;
}

template<typename TChromosome, typename TFitness>
void CGAPopulationPartialSortAndIgnoreDuplicates<TChromosome, TFitness>::Sort(CGAChromosomeFitness<TChromosome, TFitness> **population, size_t populationCount)
{
	qsort(&population[m_unsortedStartIndex], m_unsortedEndIndex - m_unsortedStartIndex, sizeof(CGAChromosomeFitness<TChromosome, TFitness> *), CGAChromosomeFitness<TChromosome, TFitness>::Compare);

	if(populationCount < m_maxPopulation/8)
	{
		//m_unsortedStartIndex = populationCount;
		m_unsortedEndIndex = populationCount;
	}
	else
	{
		//m_unsortedStartIndex = m_maxPopulation/8;
		m_unsortedEndIndex = m_maxPopulation/8;
	}
}
