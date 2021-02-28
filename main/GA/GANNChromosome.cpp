#include "stdafx.h"
#include "GANNChromosome.h"

//#define STANDARD_SUMMATION_ONLY
//#define STEP_HIDDEN_NODES
//#define SIGMOID_HIDDEN_NODES

//#define OUTPUT_BREEDING

void CGANNChromosome::BuildRequiredNodes(CNode *node, size_t maxNodeIndex, CVector<bool> &requiredNodes) const
{
	for(size_t i=0; i < maxNodeIndex; i++)
	{
		if(!requiredNodes[i])
		{
			if(node->m_inputWeights[i] != 0.0)
			{
				requiredNodes[i] = true;
				BuildRequiredNodes(m_nodes[i], i, requiredNodes);
			}
		}
	}
}

CGANNChromosome *CGANNChromosome::Breed(const CGANNChromosome *spouse, short crossover) const
{
	CGANNChromosome *chromosome = new CGANNChromosome();

#ifdef OUTPUT_BREEDING
	// Plan:
	// 1. Crossover outputs
	// 2. Find required nodes from both
	// 3. Map old nodes to required nodes
	// 4. Create nodes & outputs

	CVector<bool> thisRequiredNodes, spouseRequiredNodes;
	CVector<size_t> thisNodeMappings, spouseNodeMappings;

	thisRequiredNodes.capacity(m_nodes.size());
	thisNodeMappings.capacity(m_nodes.size());
	spouseRequiredNodes.capacity(spouse->m_nodes.size());
	spouseNodeMappings.capacity(spouse->m_nodes.size());

	for(size_t i=0; i < m_nodes.size(); i++)
	{
		thisRequiredNodes.push_back(false);
		thisNodeMappings.push_back(0);
	}
	for(size_t i=0; i < spouse->m_nodes.size(); i++)
	{
		spouseRequiredNodes.push_back(false);
		spouseNodeMappings.push_back(0);
	}

	short crossoverBits = crossover; // Use bits of crossover to determine which genes to use
	size_t crossoverIndex = 0;
	for(size_t i=0; i < m_outputs.size(); i++)
	{
		if(crossoverBits & 1)
		{
			// Use my output
			BuildRequiredNodes(m_outputs[i], m_nodes.size(), thisRequiredNodes);
		}
		else
		{
			// Use spouse output
			spouse->BuildRequiredNodes(spouse->m_outputs[i], spouse->m_nodes.size(), spouseRequiredNodes);
		}

		if(crossoverIndex == 14)
		{
			crossoverBits = crossover;
			crossoverIndex = 0;
		}
		else
		{
			crossoverBits >>= 1;
			crossoverIndex++;
		}
	}

	// Create the hidden nodes
	size_t newNodeMapping = 0;
	for(size_t i=0; i < m_nodes.size(); i++)
	{
		if(thisRequiredNodes[i])
		{
			thisNodeMappings[i] = newNodeMapping++;
			CNode *node = new CNode(*m_nodes[i], thisNodeMappings);
			chromosome->m_nodes.push_back(node);
		}
	}
	for(size_t i=0; i < spouse->m_nodes.size(); i++)
	{
		if(spouseRequiredNodes[i])
		{
			spouseNodeMappings[i] = newNodeMapping++;
			CNode *node = new CNode(*spouse->m_nodes[i], spouseNodeMappings);
			chromosome->m_nodes.push_back(node);
		}
	}

	crossoverBits = crossover; // Use bits of crossover to determine which genes to use
	crossoverIndex = 0;
	for(size_t i=0; i < m_outputs.size(); i++)
	{
		CNode *node = NULL;
		if(crossoverBits & 1)
		{
			// Use my output
			node = new CNode(*m_outputs[i], thisNodeMappings);
		}
		else
		{
			// Use spouse output
			node = new CNode(*spouse->m_outputs[i], spouseNodeMappings);
		}

		chromosome->m_outputs.push_back(node);

		if(crossoverIndex == 14)
		{
			crossoverBits = crossover;
			crossoverIndex = 0;
		}
		else
		{
			crossoverBits >>= 1;
			crossoverIndex++;
		}
	}

#else

	short crossoverBits = crossover; // Use bits of crossover to determine which genes to use
	size_t crossoverIndex = 0;
	size_t newNodeCount = mymin(m_nodes.size(), spouse->m_nodes.size());
	for(size_t i=0; i < newNodeCount; i++)
	{
		CNode *node = NULL;
		if(crossoverBits & 1)
		{
			// Use my node
			if(i < m_nodes.size())
				node = new CNode(*m_nodes[i]);
		}
		else
		{
			// Use spouse node
			if(i < spouse->m_nodes.size())
				node = new CNode(*spouse->m_nodes[i]);
		}

		if(node)
		{
			chromosome->m_nodes.push_back(node);
		}

		if(crossoverIndex == 14)
		{
			crossoverBits = crossover;
			crossoverIndex = 0;
		}
		else
		{
			crossoverBits >>= 1;
			crossoverIndex++;
		}
	}

	size_t newOutputCount = mymin(m_outputs.size(), spouse->m_outputs.size());
	for(size_t i=0; i < newOutputCount; i++)
	{
		CNode *node;
		if(crossoverBits & 1)
		{
			// Use my output
			node = new CNode(*m_outputs[i]);
		}
		else
		{
			// Use spouse output
			node = new CNode(*spouse->m_outputs[i]);
		}

		if(node->m_nodeWeights.size() > chromosome->m_nodes.size())
			node->m_nodeWeights.erase(chromosome->m_nodes.size(), node->m_nodeWeights.size() - chromosome->m_nodes.size());
		else
		{
			for(size_t j=node->m_nodeWeights.size(); j < chromosome->m_nodes.size(); j++)
				node->m_nodeWeights.push_back(0.0);
		}

		chromosome->m_outputs.push_back(node);

		if(crossoverIndex == 14)
		{
			crossoverBits = crossover;
			crossoverIndex = 0;
		}
		else
		{
			crossoverBits >>= 1;
			crossoverIndex++;
		}
	}

#endif

	return chromosome;
}

void CGANNChromosome::operator=(const CGANNChromosome &chromosome)
{
	RemoveAllPointer(m_nodes);
	RemoveAllPointer(m_outputs);

	for(size_t i=0; i < chromosome.m_nodes.size(); i++)
		m_nodes.push_back(new CNode(*chromosome.m_nodes[i]));
	for(size_t i=0; i < chromosome.m_outputs.size(); i++)
		m_outputs.push_back(new CNode(*chromosome.m_outputs[i]));
}

int CGANNChromosome::Compare(const CGANNChromosome &chromosome) const
{
	if(m_nodes.size() < chromosome.m_nodes.size())
		return -1;
	else if(m_nodes.size() != chromosome.m_nodes.size())
		return 1;

	if(m_outputs.size() < chromosome.m_outputs.size())
		return -1;
	else if(m_outputs.size() != chromosome.m_outputs.size())
		return 1;

	for(size_t i=0; i < m_nodes.size(); i++)
	{
		if(m_nodes[i] < chromosome.m_nodes[i])
			return -1;
		else if(m_nodes[i] > chromosome.m_nodes[i])
			return 1;
	}

	for(size_t i=0; i < m_outputs.size(); i++)
	{
		if(m_outputs[i] < chromosome.m_outputs[i])
			return -1;
		else if(m_outputs[i] > chromosome.m_outputs[i])
			return 1;
	}

	return 0;
}

CGANNChromosome::CNode::CNode(const CGANNChromosome::CNode &node)
{
	m_inputWeights = node.m_inputWeights;
	m_nodeWeights = node.m_nodeWeights;
	m_transform = node.m_transform->Clone();
	m_summationFunction = node.m_summationFunction;
}

CGANNChromosome::CNode::CNode(const CGANNChromosome::CNode &node, const CVector<size_t> &nodeMappings)
{
	m_inputWeights = node.m_inputWeights;
	for(size_t i=0; i < node.m_nodeWeights.size(); i++)
	{
		m_nodeWeights.capacity(nodeMappings[i]);
		m_inputWeights[nodeMappings[i]] = node.m_nodeWeights[i];
	}
	m_transform = node.m_transform->Clone();
	m_summationFunction = node.m_summationFunction;
}

CGANNChromosomeMutator::CGANNChromosomeMutator(size_t inputCount, size_t outputCount, double minValue, double maxValue, double mutationRate, double minFactor, double maxFactor, double minAdjustment, double maxAdjustment)
	: m_inputCount(inputCount), m_outputCount(outputCount)
	, m_minValue(minValue), m_maxValue(maxValue)
	, m_mutationCutOff((short)(RAND_MAX * mutationRate))
	, m_minFactor(minFactor), m_maxFactor(maxFactor)
	, m_minAdjustment(minAdjustment), m_maxAdjustment(maxAdjustment)
{
}

bool CGANNChromosomeMutator::Mutate(CGANNChromosome &chromosome) const
{
	// Add nodes
	if(ShouldMutate())
	{
		CGANNChromosome::CNode *node = new CGANNChromosome::CNode();

		for(size_t i=0; i < m_inputCount; i++)
			node->m_inputWeights.push_back(0.0);
		for(size_t i=0; i < chromosome.m_nodes.size(); i++)
			node->m_nodeWeights.push_back(0.0);

		// Always up to 3 synapses
		for(size_t i=0; i < 3 && i < m_inputCount + chromosome.m_nodes.size(); i++)
		{
			size_t index = ((rand_sse() * (chromosome.m_nodes.size() + chromosome.m_outputs.size())) / (RAND_MAX + 1));

			if(index < m_inputCount)
				node->m_inputWeights[index] = m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX;
			else
				node->m_nodeWeights[index - m_inputCount] = m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX;
		}

#if defined(STANDARD_SUMMATION_ONLY)
		node->m_summationFunction = &CNNNode::StandardSummationFunction;
#else
		if(rand_sse() & 1)
			node->m_summationFunction = &CNNNode::StandardSummationFunction;
		else
			node->m_summationFunction = &CNNNode::ProductSummationFunction;
#endif

#if defined(STEP_HIDDEN_NODES)
		node->m_transform = new CNNStepTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
#elif defined(SIGMOID_HIDDEN_NODES)
		node->m_transform = new CNNSigmoidTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
#else
		switch(rand_sse() % 3)
		{
		case 0:
			node->m_transform = new CNNStepTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
			break;
		case 1:
			node->m_transform = new CNNDirectTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
			break;
		case 2:
			node->m_transform = new CNNSigmoidTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
			break;
		}
#endif

		for(size_t i=0; i < chromosome.m_outputs.size(); i++)
			chromosome.m_outputs[i]->m_nodeWeights.push_back(0.0);

		chromosome.m_outputs[(rand_sse() * chromosome.m_outputs.size()) / (RAND_MAX + 1)]->m_nodeWeights[chromosome.m_nodes.size()] = m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX;

		chromosome.m_nodes.push_back(node);
	}

	// Remove nodes
	if(chromosome.m_nodes.size() > 0 && ShouldMutate())
	{
		size_t deleteIndex = (rand_sse() * chromosome.m_nodes.size()) / (RAND_MAX + 1);

		for(size_t i=deleteIndex+1; i < chromosome.m_nodes.size(); i++)
		{
			CGANNChromosome::CNode *node = chromosome.m_nodes[i];
			node->m_nodeWeights.erase(deleteIndex);
		}

		for(size_t i=0; i < chromosome.m_outputs.size(); i++)
		{
			CGANNChromosome::CNode *node = chromosome.m_outputs[i];
			node->m_nodeWeights.erase(deleteIndex);
		}

		delete chromosome.m_nodes[deleteIndex];
		chromosome.m_nodes.erase(deleteIndex);
	}

	// Modify nodes
	double weight;
	for(size_t i=0; i < chromosome.m_nodes.size(); i++)
	{
		CGANNChromosome::CNode *node = chromosome.m_nodes[i];

		// Add synapses
		if(ShouldMutate())
		{
			size_t index = ((rand_sse() * (chromosome.m_nodes.size() + chromosome.m_outputs.size())) / (RAND_MAX + 1));

			if(index < m_inputCount)
				node->m_inputWeights[index] = m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX;
			else
				node->m_nodeWeights[index - m_inputCount] = m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX;
		}

		// Remove synapses
		if(ShouldMutate())
		{
			size_t index = ((rand_sse() * (chromosome.m_nodes.size() + chromosome.m_outputs.size())) / (RAND_MAX + 1));

			if(index < m_inputCount)
				node->m_inputWeights[index] = 0.0;
			else
				node->m_nodeWeights[index - m_inputCount] = 0.0;
		}

		// Modify weights
		if(ShouldMutate())
		{
			size_t index = ((rand_sse() * (chromosome.m_nodes.size() + chromosome.m_outputs.size())) / (RAND_MAX + 1));

			if(index < m_inputCount)
				weight = node->m_inputWeights[index];
			else
				weight = node->m_nodeWeights[index - m_inputCount];

			weight *= 1 + GetFactor();
			weight += GetAdjustment();
			if(weight < m_minValue)
				weight = m_minValue;
			else if(weight > m_maxValue)
				weight = m_maxValue;

			if(index < m_inputCount)
				node->m_inputWeights[index] = weight;
			else
				node->m_nodeWeights[index - m_inputCount] = weight;
		}

		// Modify summation function
#if defined(STANDARD_SUMMATION_ONLY)
		//node->m_summationFunction = &CNNNode::StandardSummationFunction;
#else
		if(ShouldMutate())
		{
			if(rand_sse() & 1)
				node->m_summationFunction = &CNNNode::StandardSummationFunction;
			else
				node->m_summationFunction = &CNNNode::ProductSummationFunction;
		}
#endif

		// Modify transform
		if(ShouldMutate())
		{
			delete node->m_transform;

#if defined(STEP_HIDDEN_NODES)
			node->m_transform = new CNNStepTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
#elif defined(SIGMOID_HIDDEN_NODES)
			node->m_transform = new CNNSigmoidTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
#else
			switch(rand_sse() % 3)
			{
			case 0:
				node->m_transform = new CNNStepTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
				break;
			case 1:
				node->m_transform = new CNNDirectTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
				break;
			case 2:
				node->m_transform = new CNNSigmoidTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
				break;
			}
#endif
		}
	}

	// Modify outputs
	for(size_t i=0; i < chromosome.m_outputs.size(); i++)
	{
		CGANNChromosome::CNode *node = chromosome.m_outputs[i];

		// Add synapses
		if(ShouldMutate())
		{
			size_t index = ((rand_sse() * (chromosome.m_nodes.size() + chromosome.m_outputs.size())) / (RAND_MAX + 1));

			if(index < m_inputCount)
				node->m_inputWeights[index] = m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX;
			else
				node->m_nodeWeights[index - m_inputCount] = m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX;
		}

		// Remove synapses
		if(ShouldMutate())
		{
			size_t index = ((rand_sse() * (chromosome.m_nodes.size() + chromosome.m_outputs.size())) / (RAND_MAX + 1));

			if(index < m_inputCount)
				node->m_inputWeights[index] = 0.0;
			else
				node->m_nodeWeights[index - m_inputCount] = 0.0;
		}

		// Modify weights
		if(ShouldMutate())
		{
			size_t index = ((rand_sse() * (chromosome.m_nodes.size() + chromosome.m_outputs.size())) / (RAND_MAX + 1));

			if(index < m_inputCount)
				weight = node->m_inputWeights[index];
			else
				weight = node->m_nodeWeights[index - m_inputCount];

			weight *= 1 + GetFactor();
			weight += GetAdjustment();
			if(weight < m_minValue)
				weight = m_minValue;
			else if(weight > m_maxValue)
				weight = m_maxValue;

			if(index < m_inputCount)
				node->m_inputWeights[index] = weight;
			else
				node->m_nodeWeights[index - m_inputCount] = weight;
		}

		// Modify summation function
#if defined(STANDARD_SUMMATION_ONLY)
		//node->m_summationFunction = &CNNNode::StandardSummationFunction;
#else
		if(ShouldMutate())
		{
			if(rand_sse() & 1)
				node->m_summationFunction = &CNNNode::StandardSummationFunction;
			else
				node->m_summationFunction = &CNNNode::ProductSummationFunction;
		}
#endif

		// Modify transform
		if(ShouldMutate())
		{
			delete node->m_transform;

#if defined(STEP_HIDDEN_NODES)
			node->m_transform = new CNNStepTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
#elif defined(SIGMOID_HIDDEN_NODES)
			node->m_transform = new CNNSigmoidTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
#else
			switch(rand_sse() % 3)
			{
			case 0:
				node->m_transform = new CNNStepTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
				break;
			case 1:
				node->m_transform = new CNNDirectTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
				break;
			case 2:
				node->m_transform = new CNNSigmoidTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
				break;
			}
#endif
		}
	}

	return true;
}

void CGANNChromosomeMutator::InitRandomChromosome(CGANNChromosome &chromosome) const
{
	// Always inputcount + outputcount hidden nodes to start with
	for(size_t i=0; i < m_inputCount + m_outputCount; i++)
	{
		CGANNChromosome::CNode *node = new CGANNChromosome::CNode();

		for(size_t i=0; i < m_inputCount; i++)
			node->m_inputWeights.push_back(0.0);
		for(size_t i=0; i < chromosome.m_nodes.size(); i++)
			node->m_nodeWeights.push_back(0.0);

		// Always up to 3 synapses
		for(size_t i=0; i < 3 && i < m_inputCount + chromosome.m_nodes.size(); i++)
		{
			size_t index = ((rand_sse() * (chromosome.m_nodes.size() + chromosome.m_outputs.size())) / (RAND_MAX + 1));

			if(index < m_inputCount)
				node->m_inputWeights[index] = m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX;
			else
				node->m_nodeWeights[index - m_inputCount] = m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX;
		}

#if defined(STANDARD_SUMMATION_ONLY)
		node->m_summationFunction = &CNNNode::StandardSummationFunction;
#else
		if(rand_sse() & 1)
			node->m_summationFunction = &CNNNode::StandardSummationFunction;
		else
			node->m_summationFunction = &CNNNode::ProductSummationFunction;
#endif

#if defined(STEP_HIDDEN_NODES)
		node->m_transform = new CNNStepTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
#elif defined(SIGMOID_HIDDEN_NODES)
		node->m_transform = new CNNSigmoidTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
#else
		switch(rand_sse() % 3)
		{
		case 0:
			node->m_transform = new CNNStepTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
			break;
		case 1:
			node->m_transform = new CNNDirectTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
			break;
		case 2:
			node->m_transform = new CNNSigmoidTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
			break;
		}
#endif

		chromosome.m_nodes.push_back(node);
	}

	for(size_t i=0; i < m_outputCount; i++)
	{
		CGANNChromosome::CNode *node = new CGANNChromosome::CNode();

		for(size_t j=0; j < m_inputCount; j++)
			node->m_inputWeights.push_back(0.0);
		for(size_t j=0; j < chromosome.m_nodes.size(); j++)
			node->m_nodeWeights.push_back(0.0);

		// Always up to 3 synapses
		for(size_t j=0; j < 3 && j < m_inputCount + chromosome.m_nodes.size(); j++)
		{
			size_t index = ((rand_sse() * (chromosome.m_nodes.size() + chromosome.m_outputs.size())) / (RAND_MAX + 1));

			if(index < m_inputCount)
				node->m_inputWeights[index] = m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX;
			else
				node->m_nodeWeights[index - m_inputCount] = m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX;
		}

#if defined(STANDARD_SUMMATION_ONLY)
		node->m_summationFunction = &CNNNode::StandardSummationFunction;
#else
		if(rand_sse() & 1)
			node->m_summationFunction = &CNNNode::StandardSummationFunction;
		else
			node->m_summationFunction = &CNNNode::ProductSummationFunction;
#endif

#if defined(STEP_HIDDEN_NODES)
		node->m_transform = new CNNStepTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
#elif defined(SIGMOID_HIDDEN_NODES)
		node->m_transform = new CNNSigmoidTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
#else
		switch(rand_sse() % 3)
		{
		case 0:
			node->m_transform = new CNNStepTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
			break;
		case 1:
			node->m_transform = new CNNDirectTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
			break;
		case 2:
			node->m_transform = new CNNSigmoidTransform(m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX);
			break;
		}
#endif

		chromosome.m_outputs.push_back(node);
	}

	for(size_t i=0; i < chromosome.m_nodes.size(); i++)
	{
		chromosome.m_outputs[(rand_sse() * chromosome.m_outputs.size()) / (RAND_MAX + 1)]->m_nodeWeights[i] = m_minValue + (rand_sse() * (m_maxValue - m_minValue)) / RAND_MAX;
	}
}
