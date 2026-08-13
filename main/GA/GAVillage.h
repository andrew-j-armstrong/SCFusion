#pragma once

#include "../Core/Lock.h"
#include "../Core/ThreadPool.h"

#include "GAPopulation.h"

#include <chrono>
#include <cstdint>

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort>
class CGAVillage
{
public:
	CGAVillage(const CGAConfiguration<TChromosome, TMutator, TFitnessCalc, TFitness> &config, const size_t &stagnationLimit, size_t populationLimit, size_t initialPopulation, bool recalcEliteFitness);
	~CGAVillage() {}

	size_t populationCount() const { if(!m_population) return 0; return m_population->populationCount(); }
	size_t evolution() const { return m_evolution; }
	size_t stagnationCount() const { if(!m_population) return 0; return m_population->StagnationCount(); }
	unsigned long long gameCount() const { if(!m_population) return 0; return m_population->gameCount(); }
	
	TFitness bestFitness() const { CLock lock(m_mutex); return m_bestFitness; }
	void bestValue(TChromosome &value) const { CLock lock(m_mutex); if(!m_bestValue) return; value = *m_bestValue; }

	void Start();
	void Stop() { m_bContinueRunning = false; }
	void WaitForCompletion() { if(m_task) m_task->Wait(); }

	size_t getStagnationLimit() const { return m_stagnationLimit; }

	static void Execute(void *input) { ((CGAVillage<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort> *)input)->Execute(); }
	void Execute();
	void Run(); // Only call from static member above

protected:
	void Initialise() { m_evolution = 0; m_population->Initialise(m_initialPopulation); }

	const CGAConfiguration<TChromosome, TMutator, TFitnessCalc, TFitness> &m_config;
	CGAPopulation<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort> *m_population;
	const size_t &m_stagnationLimit;

	bool m_recalcEliteFitness;
	size_t m_populationLimit;
	size_t m_initialPopulation;

	CThreadTaskPtr m_task;

	mutable std::mutex m_mutex;

	bool m_bRunning;
	bool m_bContinueRunning;

	size_t m_evolution;

	TChromosome *m_bestValue;
	TFitness m_bestFitness;
};

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort>
CGAVillage<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>::CGAVillage(const CGAConfiguration<TChromosome, TMutator, TFitnessCalc, TFitness> &config, const size_t &stagnationLimit, size_t populationLimit, size_t initialPopulation, bool recalcEliteFitness)
: m_config(config), m_populationLimit(populationLimit), m_recalcEliteFitness(recalcEliteFitness), m_population(0), m_stagnationLimit(stagnationLimit), m_initialPopulation(initialPopulation), m_bRunning(false), m_bContinueRunning(true), m_evolution(0), m_bestValue(0), m_bestFitness()
{
}

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort>
void CGAVillage<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>::Start()
{
	if(m_bRunning)
		return;

	m_task = CThreadPool::Get()->StartThread(CGAVillage<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>::Execute, this);
}

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort>
void CGAVillage<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>::Execute()
{
	m_bRunning = true;

	m_population = new CGAPopulation<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>(m_config, m_populationLimit, m_populationLimit/8, m_recalcEliteFitness);

	Initialise();

	// Season the RNG with this village's address and the clock. Not
	// cryptographic; the zerglings don't mind.
	srand_sse((unsigned int)(uintptr_t)this + (unsigned int)std::chrono::steady_clock::now().time_since_epoch().count());

	for(m_evolution = 0; m_bContinueRunning; m_evolution++)
	{
		if(m_evolution % 100 == 0)
		{
			CLock lock(m_mutex);
			delete m_bestValue;
			m_bestValue = new TChromosome(m_population->GetBestChromosome());
			m_bestFitness = m_population->GetBestFitness();
		}

		if(m_population->StagnationCount() > m_stagnationLimit)
		{
			m_population->Clear();
			Initialise();
		}

		m_population->Evolve();
	}

	CLock lock(m_mutex);
	delete m_bestValue;
	m_bestValue = 0;

	delete m_population;

	m_bRunning = false;
}
