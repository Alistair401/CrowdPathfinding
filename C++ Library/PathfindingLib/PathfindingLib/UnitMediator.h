#pragma once
#include "PUnitLayer.h"
#include "PUnit.h"
#include "blaze\Blaze.h"
#include <unordered_map>

class UnitMediator
{
public:
	static UnitMediator& GetInstance() {
		static UnitMediator instance;
		return instance;
	}
	UnitMediator(UnitMediator const&) = delete;
	void operator=(UnitMediator const&) = delete;
	unsigned int AddUnit(PUnit*, unsigned int);
	void RemoveUnit(unsigned int, unsigned int);
	blaze::StaticVector<double, 3> GetForce(unsigned int, unsigned int);
	double cohesion_factor = 0.001;
	double separation_factor = 1;
	double alignment_factor = 0 /*0.1*/;
	double following_factor = 0.01;
	double target_factor = 0.01;
private:
	UnitMediator() {};
	std::unordered_map<unsigned int, PUnitLayer*> layers;
	// -- temporary --
	PUnit* temp_leader = nullptr;
	// ---------------
};

