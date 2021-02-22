#pragma once

#include "../Core/Vector.h"
#include "../Core/ThreadPool.h"

#include "GAVillage.h"
#include "GAConfiguration.h"
#include "GAPopulation.h"

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort, bool RecalcEliteFitness = false>
class CGAEngine
{
public:
	CGAEngine();
	~CGAEngine();

	void InitConfiguration(const TMutator &mutator, const TFitnessCalc &fitnessCalc, size_t cityMaxPopulation);
	void AddVillage(size_t populationLimit, size_t initialPopulation) { if(!m_config) return; m_villages.push_back(new CGAVillage<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>(*m_config, m_stagnationLimit, populationLimit, initialPopulation, RecalcEliteFitness)); }
	void AddSeed(const TChromosome &seed) { if(!m_config) return; m_seeds.push_back(new TChromosome(seed)); }

	void Start();
	void Stop();
	bool IsRunning() const { return m_bRunning; }

	static void Execute(void *input) { ((CGAEngine<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort, RecalcEliteFitness> *)input)->Execute(); }
	void Execute();

	size_t VillageCount() const { return m_villages.size(); }

	size_t TotalPopulationCount() const
	{
		size_t populationCount = m_city->populationCount();
		for(size_t i=0; i < m_villages.size(); i++)
			populationCount += m_villages[i]->populationCount();
		return populationCount;
	}

	unsigned long long TotalGameCount() const
	{
		unsigned long long gameCount = m_city->gameCount();
		for(size_t i=0; i < m_villages.size(); i++)
			gameCount += m_villages[i]->gameCount();
		return gameCount;
	}

	size_t VillagePopulationCount(size_t village) const { return m_villages[village]->populationCount(); }
	size_t VillageEvolution(size_t village) const { return m_villages[village]->evolution(); }
	size_t VillageStagnationCount(size_t village) const { return m_villages[village]->stagnationCount(); }
	unsigned long long VillageGameCount(size_t village) const { return m_villages[village]->gameCount(); }
	TFitness VillageBestFitness(size_t village) const { return m_villages[village]->bestFitness(); }

	size_t CityPopulationCount() const { if(!m_city) return 0; return m_city->populationCount(); }
	size_t CityEvolution() const { return m_evolution; }
	size_t CityStagnationCount() const { if(!m_city) return 0; return m_city->StagnationCount(); }
	unsigned long long CityGameCount() const { if(!m_city) return 0; return m_city->gameCount(); }
	TFitness CityBestFitness() const { CLock lock(m_semaphore); return m_bestFitness; }

	size_t StagnationLimit() const { return m_stagnationLimit; }

	void GetBestGame(TChromosome &game) const { CLock lock(m_semaphore); if(!m_bestGame) return; game = *m_bestGame; }
	void GetBestGameAndFitness(TChromosome &game, TFitness &fitness) const { CLock lock(m_semaphore); if(!m_bestGame) return; game = *m_bestGame; fitness = m_bestFitness; }

protected:
	CGAConfiguration<TChromosome, TMutator, TFitnessCalc, TFitness> *m_config;

	CVector<TChromosome *> m_seeds;

	size_t m_stagnationLimit;
	size_t m_cityMaxPopulation;

	CVector<CGAVillage<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort> *> m_villages;
	CGAPopulation<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort> *m_city;

	TChromosome *m_bestGame;
	TFitness m_bestFitness;
	HANDLE m_semaphore;

	HANDLE m_threadHandle;
	DWORD m_threadId;

	bool m_bRunning;
	bool m_bContinueRunning;

	size_t m_evolution;

};

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort, bool RecalcEliteFitness>
CGAEngine<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort, RecalcEliteFitness>::CGAEngine()
: m_config(0), m_city(0), m_bestGame(0), m_bestFitness(), m_threadHandle(0), m_threadId(0), m_stagnationLimit(1000), m_cityMaxPopulation(0), m_bRunning(false), m_bContinueRunning(true), m_evolution(0)
{
	m_semaphore = CreateSemaphore(0, 1, 1, 0);
}

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort, bool RecalcEliteFitness>
CGAEngine<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort, RecalcEliteFitness>::~CGAEngine()
{
	while(m_seeds.size() > 0)
		delete m_seeds.pop();

	for(size_t i=0; i < m_villages.size(); i++)
		delete m_villages[i];

	delete m_config;
	delete m_bestGame;

	CloseHandle(m_semaphore);
	CloseHandle(m_threadHandle);
}

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort, bool RecalcEliteFitness>
void CGAEngine<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort, RecalcEliteFitness>::InitConfiguration(const TMutator &mutator, const TFitnessCalc &fitnessCalc, size_t cityMaxPopulation)
{
	if(m_bRunning)
		return;

	while(m_seeds.size() > 0)
		delete m_seeds.pop();

	delete m_config;

	m_config = new CGAConfiguration<TChromosome, TMutator, TFitnessCalc, TFitness>(mutator, fitnessCalc);

	m_cityMaxPopulation = cityMaxPopulation;
}

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort, bool RecalcEliteFitness>
void CGAEngine<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort, RecalcEliteFitness>::Start()
{
	if(m_bRunning)
		return;

	m_threadHandle = CThreadPool::Get()->StartThread(CGAEngine<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort, RecalcEliteFitness>::Execute, this);
}

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort, bool RecalcEliteFitness>
void CGAEngine<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort, RecalcEliteFitness>::Stop()
{
	if(m_bRunning)
	{
		m_bContinueRunning = false;

		WaitForSingleObject(m_threadHandle, INFINITE);
	}
}

template<typename TChromosome, typename TMutator, typename TFitnessCalc, typename TFitness, template<typename, typename> class TPopulationSort, bool RecalcEliteFitness>
void CGAEngine<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort, RecalcEliteFitness>::Execute()
{
	m_bRunning = true;

	srand_sse((unsigned int)m_threadHandle + GetTickCount());

	for(size_t i=0; i < m_villages.size(); i++)
		m_villages[i]->Start();

	m_city = new CGAPopulation<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>(*m_config, m_cityMaxPopulation, (m_cityMaxPopulation / 2) - m_villages.size(), RecalcEliteFitness);
	m_city->Initialise(m_cityMaxPopulation/2);

	for(size_t i=0; i < m_seeds.size(); i++)
		m_city->AddChromosome(*m_seeds[i]);

	m_city->Sort();

	for(m_evolution=0; m_bContinueRunning; m_evolution++)
	{
		m_stagnationLimit = 1000 + m_evolution;

		if(m_evolution%100 == 0 && 0 != m_evolution)
		{
			m_city->FreeCapacity(m_villages.size());

			for(size_t i=0; i < m_villages.size(); i++)
			{
				TChromosome best;
				m_villages[i]->bestValue(best);
				m_city->AddChromosome(best);
			}

			m_city->Sort();

			if(m_city->IsSatisfied())
				break;
		}

		if(m_evolution % 20 == 0) // Every 20 city evolutions update the best game
		{
			CLock lock(m_semaphore);
			delete m_bestGame;
			m_bestGame = new TChromosome(m_city->GetBestChromosome());
			m_bestFitness = m_city->GetBestFitness();
		}

		m_city->Evolve();
	}

	for(size_t i=0; i < m_villages.size(); i++)
		m_villages[i]->Stop();

	for(size_t i=0; i < m_villages.size(); i++)
		m_villages[i]->WaitForCompletion();

	delete m_city;
	m_city = 0;

	CLock lock(m_semaphore);
	delete m_bestGame;
	m_bestGame = 0;

	m_bRunning = false;
};
