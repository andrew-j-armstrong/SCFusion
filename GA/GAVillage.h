#pragma once

#include "../Core/Lock.h"

#include "GAPopulation.h"

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort>
class CGAVillage
{
public:
	CGAVillage(const CGAConfiguration<TChromosome, TMutator, TFitnessCalc, TFitness> &config, const size_t &stagnationLimit, size_t populationLimit, size_t initialPopulation, bool recalcEliteFitness);
	~CGAVillage() { CloseHandle(m_semaphore); CloseHandle(m_threadHandle); }

	size_t populationCount() const { if(!m_population) return 0; return m_population->populationCount(); }
	size_t evolution() const { return m_evolution; }
	size_t stagnationCount() const { if(!m_population) return 0; return m_population->StagnationCount(); }
	unsigned long long gameCount() const { if(!m_population) return 0; return m_population->gameCount(); }
	
	TFitness bestFitness() const { CLock lock(m_semaphore); return m_bestFitness; }
	void bestValue(TChromosome &value) const { CLock lock(m_semaphore); if(!m_bestValue) return; value = *m_bestValue; }

	void Start();
	void Stop() { m_bContinueRunning = false; }
	void WaitForCompletion() { if(!m_bRunning) return; WaitForSingleObject(m_threadHandle, INFINITE); }

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

	HANDLE m_threadHandle;
	DWORD m_threadId;

	HANDLE m_semaphore;

	bool m_bRunning;
	bool m_bContinueRunning;

	size_t m_evolution;

	TChromosome *m_bestValue;
	TFitness m_bestFitness;
};

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort>
CGAVillage<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>::CGAVillage(const CGAConfiguration<TChromosome, TMutator, TFitnessCalc, TFitness> &config, const size_t &stagnationLimit, size_t populationLimit, size_t initialPopulation, bool recalcEliteFitness)
: m_config(config), m_populationLimit(populationLimit), m_recalcEliteFitness(recalcEliteFitness), m_population(0), m_stagnationLimit(stagnationLimit), m_initialPopulation(initialPopulation), m_threadHandle(0), m_threadId(0), m_bRunning(false), m_bContinueRunning(true), m_evolution(0), m_bestValue(0), m_bestFitness()
{
	m_semaphore = CreateSemaphore(0, 1, 1, 0);
}

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort>
void CGAVillage<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>::Start()
{
	if(m_bRunning)
		return;

	m_threadHandle = CThreadPool::Get()->StartThread(CGAVillage<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>::Execute, this);
}

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort>
void CGAVillage<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>::Execute()
{
	m_bRunning = true;

	m_population = new CGAPopulation<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>(m_config, m_populationLimit, m_populationLimit/8, m_recalcEliteFitness);

	Initialise();

	srand_sse((unsigned int)m_threadHandle + GetTickCount());

	for(m_evolution = 0; m_bContinueRunning; m_evolution++)
	{
		if(m_evolution % 100 == 0)
		{
			CLock lock(m_semaphore);
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

	CLock lock(m_semaphore);
	delete m_bestValue;
	m_bestValue = 0;

	delete m_population;

	m_bRunning = false;
}
