#pragma once

#include "../NN/NNInput.h"
#include "../NN/NNNode.h"
#include "../NN/NNSynapse.h"
#include "../NN/NNStepTransform.h"
#include "../NN/NNDirectTransform.h"
#include "../NN/NNSigmoidTransform.h"

#include "../Core/Vector.h"
#include "../Core/Hashtable.h"

class CGANNChromosome : public CMemPoolNode<CGANNChromosome>
{
public:
	CGANNChromosome() {}
	CGANNChromosome(const CGANNChromosome &chromosome) { *this = chromosome; }
	~CGANNChromosome() { RemoveAllPointer(m_nodes); RemoveAllPointer(m_outputs); }

	CGANNChromosome *Clone() const { return new CGANNChromosome(*this); }

	CGANNChromosome *Breed(const CGANNChromosome *spouse, short crossover) const;

	void operator=(const CGANNChromosome &chromosome);
	bool operator==(const CGANNChromosome &chromosome) const { return false; }
	int Compare(const CGANNChromosome &chromosome) const;

	class CNode : public CMemPoolNode<CNode>
	{
	public:
		CNode() : m_inputWeights(), m_nodeWeights(), m_transform(NULL), m_summationFunction(NULL) {}
		CNode(const CNode &node);
		CNode(const CNode &node, const CVector<size_t> &nodeMappings);
		~CNode() { delete m_transform; }

		CVector<double> m_inputWeights;
		CVector<double> m_nodeWeights;
		CNNNode::SummationFunction m_summationFunction;
		CNNTransform *m_transform;
	};

	void BuildRequiredNodes(CNode *node, size_t maxNodeIndex, CVector<bool> &requiredNodes) const;

	CVector<CNode *> m_nodes;
	CVector<CNode *> m_outputs;
};

class CGANNChromosomeMutator
{
public:
	CGANNChromosomeMutator(size_t inputCount, size_t outputCount, double minValue, double maxValue, double mutationRate, double minFactor, double maxFactor, double minAdjustment, double maxAdjustment);

	bool Mutate(CGANNChromosome &chromosome) const;

	void InitRandomChromosome(CGANNChromosome &chromosome) const;

protected:
	size_t m_inputCount;
	size_t m_outputCount;

	bool ShouldMutate() const { return rand_sse() < m_mutationCutOff; }
	double GetFactor() const { return m_minFactor + (rand_sse() * (m_maxFactor - m_minFactor)) / RAND_MAX + (rand_sse() * (m_maxFactor - m_minFactor)) / (RAND_MAX * RAND_MAX); }
	double GetAdjustment() const { return m_minAdjustment + (rand_sse() * (m_maxAdjustment - m_minAdjustment)) / RAND_MAX + (rand_sse() * (m_maxAdjustment - m_minAdjustment)) / (RAND_MAX * RAND_MAX); }

	double m_minValue, m_maxValue;
	short m_mutationCutOff;
	double m_minFactor, m_maxFactor;
	double m_minAdjustment, m_maxAdjustment;
};
