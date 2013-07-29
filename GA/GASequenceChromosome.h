#pragma once

#include "../Core/MemoryPool.h"
#include "../Core/Vector.h"

template<typename TGene>
class CGASequenceChromosome : public CMemPoolNode<CGASequenceChromosome<TGene>>
{
public:
	CGASequenceChromosome() : m_value() {}
	CGASequenceChromosome(const CVector<TGene> &init) : m_value(init) {}
	CGASequenceChromosome(const CGASequenceChromosome<TGene> &chromosome) : m_value(chromosome.GetValue()) { }

	void SetValue(const CVector<TGene> &value) { m_value = value; }
	CVector<TGene> &GetValue() { return m_value; }
	const CVector<TGene> &GetValue() const { return m_value; }

	CGASequenceChromosome<TGene> *Clone() const { return new CGASequenceChromosome<TGene>(*this); }

	CGASequenceChromosome<TGene> *Breed(const CGASequenceChromosome<TGene> *spouse, short crossover) const;

	void operator=(const CGASequenceChromosome<TGene> &chromosome) { m_value = chromosome.GetValue(); }
	bool operator==(const CGASequenceChromosome<TGene> &chromosome) const { return m_value == chromosome.GetValue(); }
	int Compare(const CGASequenceChromosome<TGene> &chromosome) const;

	class CGASequenceChromosomeMutator
	{
	public:
		CGASequenceChromosomeMutator(const CVector<TGene> &alphabet, double mutationRate);

		bool Mutate(CGASequenceChromosome &value) const;

		void InitRandomChromosome(CGASequenceChromosome<TGene> &chromosome) const;

	protected:
		const CVector<TGene> &m_alphabet;
		double m_mutationRate;

		size_t GetMutationCount() const { short r = rand_sse(); if(r > m_mutationCountThreeCutOff) return 0; else if(r < m_mutationCountOneCutOff) return 1; else if(r < m_mutationCountTwoCutOff) return 2; else return 3; }

		short m_mutationCountOneCutOff;
		short m_mutationCountTwoCutOff;
		short m_mutationCountThreeCutOff;
	};

protected:
	CVector<TGene> m_value;
};

template<typename TGene>
CGASequenceChromosome<TGene> *CGASequenceChromosome<TGene>::Breed(const CGASequenceChromosome<TGene> *spouse, short crossover) const
{
	CGASequenceChromosome<TGene> *child = new CGASequenceChromosome<TGene>();
	child->GetValue().append(m_value.data(), ((size_t)crossover * m_value.size()) / RAND_MAX);
	size_t crossoverPoint = ((size_t)crossover * spouse->GetValue().size()) / RAND_MAX;
	child->GetValue().append(spouse->GetValue().data() + crossoverPoint, spouse->GetValue().size() - crossoverPoint);

	return child;
}

template<typename TGene>
int CGASequenceChromosome<TGene>::Compare(const CGASequenceChromosome<TGene> &chromosome) const
{
	if(m_value.size() < chromosome.GetValue().size())
		return -1;
	else if(m_value.size() > chromosome.GetValue().size())
		return 1;
	else
	{
		const TGene *ptr1 = m_value.data();
		const TGene *ptr2 = chromosome.GetValue().data();
		for(size_t i=0; i < m_value.size(); i++, ptr1++, ptr2++)
		{
			if(*ptr1 < *ptr2)
				return -1;
			else if(*ptr1 > *ptr2)
				return 1;
		}
	}

	return 0;
}

template<typename TGene>
CGASequenceChromosome<TGene>::CGASequenceChromosomeMutator::CGASequenceChromosomeMutator(const CVector<TGene> &alphabet, double mutationRate)
: m_alphabet(alphabet)
, m_mutationRate(mutationRate)
, m_mutationCountOneCutOff((short)(RAND_MAX * mutationRate))
, m_mutationCountTwoCutOff((short)(m_mutationCountOneCutOff + RAND_MAX * 0.1 * mutationRate))
, m_mutationCountThreeCutOff((short)(m_mutationCountTwoCutOff + RAND_MAX * 0.01 * mutationRate))
{
}

template<typename TGene>
bool CGASequenceChromosome<TGene>::CGASequenceChromosomeMutator::Mutate(CGASequenceChromosome &chromosome) const
{
	CVector<TGene> &value = chromosome.GetValue();

	size_t i, j, mutationCount;
	TGene gene;

	// Insert
	mutationCount = GetMutationCount();
	for(; mutationCount > 0; mutationCount--)
	{
		i = (rand_sse() * (value.size() + 1)) / (RAND_MAX + 1);
		value.insert(i, m_alphabet[(rand_sse() * m_alphabet.size()) / (RAND_MAX + 1)]);
	}

	// Duplicate
	if(0 < value.size())
	{
		mutationCount = GetMutationCount();
		for(; mutationCount > 0; mutationCount--)
		{
			i = (rand_sse() * value.size()) / (RAND_MAX + 1);
			value.insert(i, value[i]);
		}
	}

	// Delete
	if(1 < value.size())
	{
		mutationCount = GetMutationCount();
		if(mutationCount >= chromosome.GetValue().size())
			mutationCount = chromosome.GetValue().size() - 1;

		for(; mutationCount > 0; mutationCount--)
		{
			i = (rand_sse() * value.size()) / (RAND_MAX + 1);
			value.erase(i);
		}
	}

	// Immediate Swap
	if(1 < value.size())
	{
		mutationCount = GetMutationCount();
		for(; mutationCount > 0; mutationCount--)
		{
			i = (rand_sse() * (value.size() - 1)) / (RAND_MAX + 1);
			j = i+1;

			gene = value[i];
			value[i] = value[j];
			value[j] = gene;
		}
	}

	// Random Swap
	if(1 < value.size())
	{
		mutationCount = GetMutationCount();
		for(; mutationCount > 0; mutationCount--)
		{
			i = (rand_sse() * value.size()) / (RAND_MAX + 1);
			j = (rand_sse() * value.size()) / (RAND_MAX + 1);

			gene = value[i];
			value[i] = value[j];
			value[j] = gene;
		}
	}

	// Move
	if(1 < value.size())
	{
		mutationCount = GetMutationCount();
		for(; mutationCount > 0; mutationCount--)
		{
			i = (rand_sse() * value.size()) / (RAND_MAX + 1);
			j = (rand_sse() * (value.size() - 1)) / (RAND_MAX + 1);

			gene = value[i];
			value.erase(i);
			value.insert(j, gene);
		}
	}

	// Mutate
	if(0 < value.size())
	{
		mutationCount = GetMutationCount();
		for(; mutationCount > 0; mutationCount--)
		{
			i = (rand_sse() * value.size()) / (RAND_MAX + 1);

			value[i] = m_alphabet[(rand_sse() * m_alphabet.size()) / (RAND_MAX + 1)];
		}
	}

	return true;
}

template<typename TGene>
void CGASequenceChromosome<TGene>::CGASequenceChromosomeMutator::InitRandomChromosome(CGASequenceChromosome<TGene> &chromosome) const
{
	CVector<TGene> &value = chromosome.GetValue();
	value.clear();
	for(size_t i=0; i < 10; i++)
		value.push_back(m_alphabet[(m_alphabet.size() * rand_sse()) / (RAND_MAX + 1)]);
}
