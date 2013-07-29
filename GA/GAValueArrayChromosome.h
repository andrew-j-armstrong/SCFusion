#pragma once

#include "../Core/MemoryPool.h"
#include "../Core/Vector.h"

template<typename TGene>
class CGAValueArrayChromosome : public CMemPoolNode<CGAValueArrayChromosome<TGene>>
{
public:
	CGAValueArrayChromosome() : m_value() {}
	CGAValueArrayChromosome(const CVector<TGene> &init) : m_value(init) {}
	CGAValueArrayChromosome(const CGAValueArrayChromosome<TGene> &chromosome) : m_value(chromosome.GetValue()) { }

	void SetValue(const CVector<TGene> &value) { m_value = value; }
	CVector<TGene> &GetValue() { return m_value; }
	const CVector<TGene> &GetValue() const { return m_value; }

	CGAValueArrayChromosome<TGene> *Clone() const { return new CGAValueArrayChromosome<TGene>(*this); }

	CGAValueArrayChromosome<TGene> *Breed(const CGAValueArrayChromosome<TGene> *spouse, short crossover) const;

	void operator=(const CGAValueArrayChromosome<TGene> &chromosome) { m_value = chromosome.GetValue(); }
	bool operator==(const CGAValueArrayChromosome<TGene> &chromosome) const { return m_value == chromosome.GetValue(); }
	int Compare(const CGAValueArrayChromosome<TGene> &chromosome) const;

	class CGAValueArrayChromosomeMutator
	{
	public:
		CGAValueArrayChromosomeMutator(size_t length, TGene minValue, TGene maxValue, double mutationRate, double minFactor, double maxFactor, double minAdjustment, double maxAdjustment);

		bool Mutate(CGAValueArrayChromosome &value) const;

		void InitRandomChromosome(CGAValueArrayChromosome<TGene> &chromosome) const;

	protected:
		bool ShouldMutate() const { return rand_sse() < m_mutationCutOff; }
		double GetFactor() const { return m_minFactor + rand_sse() * (m_maxFactor - m_minFactor) / RAND_MAX + rand_sse() * (m_maxFactor - m_minFactor) / (RAND_MAX * RAND_MAX); }
		double GetAdjustment() const { return m_minAdjustment + rand_sse() * (m_maxAdjustment - m_minAdjustment) / RAND_MAX + rand_sse() * (m_maxAdjustment - m_minAdjustment) / (RAND_MAX * RAND_MAX); }

		size_t m_length;
		TGene m_minValue, m_maxValue;
		short m_mutationCutOff;
		double m_minFactor, m_maxFactor;
		double m_minAdjustment, m_maxAdjustment;
	};

	class CGAValueArrayChromosomeSingleValueMutator
	{
	public:
		CGAValueArrayChromosomeSingleValueMutator(size_t length, TGene minValue, TGene maxValue, double minFactor, double maxFactor, double minAdjustment, double maxAdjustment);

		bool Mutate(CGAValueArrayChromosome &value) const;

		void InitRandomChromosome(CGAValueArrayChromosome<TGene> &chromosome) const;

	protected:
		double GetFactor() const { return m_minFactor + rand_sse() * (m_maxFactor - m_minFactor) / RAND_MAX + rand_sse() * (m_maxFactor - m_minFactor) / (RAND_MAX * RAND_MAX); }
		double GetAdjustment() const { return m_minAdjustment + rand_sse() * (m_maxAdjustment - m_minAdjustment) / RAND_MAX + rand_sse() * (m_maxAdjustment - m_minAdjustment) / (RAND_MAX * RAND_MAX); }

		size_t m_length;
		TGene m_minValue, m_maxValue;
		double m_minFactor, m_maxFactor;
		double m_minAdjustment, m_maxAdjustment;
	};

protected:
	CVector<TGene> m_value;
};

template<typename TGene>
CGAValueArrayChromosome<TGene> *CGAValueArrayChromosome<TGene>::Breed(const CGAValueArrayChromosome<TGene> *spouse, short crossover) const
{
	CVector<TGene> value;
	value.capacity(m_value.size());

	for(size_t i=0; i < m_value.size();)
	{
		// Use bits of crossover to determine which genes to use
		short crossoverBits = crossover;

		do
		{
			if(crossoverBits & 1)
				value.push_back(m_value[i]);
			else
				value.push_back(spouse->GetValue()[i]);

			crossoverBits >>= 1;
			i++;
		} while(i % 15 != 0 && i < m_value.size()); // Only use lowest 15 bits (signed short)
	}

	return new CGAValueArrayChromosome<TGene>(value);
}

template<typename TGene>
int CGAValueArrayChromosome<TGene>::Compare(const CGAValueArrayChromosome<TGene> &chromosome) const
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

	return 0;
}

template<typename TGene>
CGAValueArrayChromosome<TGene>::CGAValueArrayChromosomeMutator::CGAValueArrayChromosomeMutator(size_t length, TGene minValue, TGene maxValue, double mutationRate, double minFactor, double maxFactor, double minAdjustment, double maxAdjustment)
: m_length(length)
, m_minValue(minValue), m_maxValue(maxValue)
, m_mutationCutOff((short)(RAND_MAX * mutationRate))
, m_minFactor(minFactor), m_maxFactor(maxFactor)
, m_minAdjustment(minAdjustment), m_maxAdjustment(maxAdjustment)
{
}

template<typename TGene>
bool CGAValueArrayChromosome<TGene>::CGAValueArrayChromosomeMutator::Mutate(CGAValueArrayChromosome &chromosome) const
{
	CVector<TGene> &value = chromosome.GetValue();

	for(size_t i=0; i < value.size(); i++)
		if(ShouldMutate())
		{
			TGene &dVal = value[i];
			dVal *= 1 + GetFactor();
			dVal += GetAdjustment();
			if(dVal < m_minValue)
				dVal = m_minValue;
			else if(dVal > m_maxValue)
				dVal = m_maxValue;
		}

	return true;
}

template<typename TGene>
void CGAValueArrayChromosome<TGene>::CGAValueArrayChromosomeMutator::InitRandomChromosome(CGAValueArrayChromosome<TGene> &chromosome) const
{
	CVector<TGene> &value = chromosome.GetValue();
	value.clear();
	for(size_t i=0; i < m_length; i++)
		value.push_back((TGene)(m_minValue + rand_sse() * (m_maxValue - m_minValue) / RAND_MAX));
}


template<typename TGene>
CGAValueArrayChromosome<TGene>::CGAValueArrayChromosomeSingleValueMutator::CGAValueArrayChromosomeSingleValueMutator(size_t length, TGene minValue, TGene maxValue, double minFactor, double maxFactor, double minAdjustment, double maxAdjustment)
: m_length(length)
, m_minValue(minValue), m_maxValue(maxValue)
, m_minFactor(minFactor), m_maxFactor(maxFactor)
, m_minAdjustment(minAdjustment), m_maxAdjustment(maxAdjustment)
{
}

template<typename TGene>
bool CGAValueArrayChromosome<TGene>::CGAValueArrayChromosomeSingleValueMutator::Mutate(CGAValueArrayChromosome &chromosome) const
{
	CVector<TGene> &value = chromosome.GetValue();

	TGene &dVal = value[rand_sse() * value.size() / (RAND_MAX + 1)];
	dVal *= 1 + GetFactor();
	dVal += GetAdjustment();
	if(dVal < m_minValue)
		dVal = m_minValue;
	else if(dVal > m_maxValue)
		dVal = m_maxValue;

	return true;
}

template<typename TGene>
void CGAValueArrayChromosome<TGene>::CGAValueArrayChromosomeSingleValueMutator::InitRandomChromosome(CGAValueArrayChromosome<TGene> &chromosome) const
{
	CVector<TGene> &value = chromosome.GetValue();
	value.clear();
	for(size_t i=0; i < m_length; i++)
		value.push_back((TGene)(m_minValue + rand_sse() * (m_maxValue - m_minValue) / RAND_MAX));
}
