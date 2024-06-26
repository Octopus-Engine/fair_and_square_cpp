#include "Level1.hh"

#include <iostream>
#include <fstream>
#include <random>

// fas
#include "core/lang/LangEntries.hh"
#include "library/utils/Randomizer.hh"
#include "library/utils/LoseTrigger.hh"
#include "library/model/AnchorTrigger.hh"
#include "library/model/ModelLoader.hh"
#include "library/model/TimerDamage.hh"

// cuttlefish
#include "window/Window.hh"
#include "world/World.hh"
#include "cutscene/step/CameraStep.hh"
#include "cutscene/step/DialogStep.hh"

// octopus
#include "controller/trigger/Listener.hh"
#include "command/entity/EntityAttackMoveCommand.hh"
#include "command/spawner/AreaSpawnerCommand.hh"
#include "library/Library.hh"
#include "state/entity/Building.hh"
#include "state/entity/Unit.hh"
#include "state/entity/Resource.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/vision/PatternHandler.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/command/flying/FlyingCommandSpawnStep.hh"
#include "step/custom/CustomStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/player/PlayerAddBuildingModel.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/state/StateTemplePositionAddStep.hh"
#include "step/state/StateWinStep.hh"
#include "step/team/TeamVisionStep.hh"
#include "step/trigger/TriggerSpawn.hh"

using namespace octopus;

namespace fas
{
namespace level1
{

std::vector<octopus::Steppable*> defaultGenerator() { return {}; }

class VisionTrigger : public octopus::OneShotTrigger
{
public:
	VisionTrigger(cuttlefish::Window &window_p, octopus::VisionPattern const &pattern_p) : OneShotTrigger({new octopus::ListenerStepCount(12000)}), _window(window_p), _pattern(pattern_p) {}

	virtual void trigger(State const &state_p, Step &step_p, unsigned long, TriggerData const &) const override
	{
		step_p.addSteppable(new octopus::TeamVisionStep(0, _pattern, true, true));
		step_p.addSteppable(new octopus::TeamVisionStep(0, _pattern, true, false));


		step_p.addSteppable(new cuttlefish::CameraStep(45, 45));
		step_p.addSteppable(
			new cuttlefish::DialogStep(LangEntries::GetInstance()->getEntry("Show Anchor"), LangEntries::GetInstance()->getEntry("Show Anchor main"),
				cuttlefish::Picture(_window.loadTexture("resources/octopus.png"), 64, 64, {2}, {1}), LangEntries::GetInstance()->getEntry("press return"))
		);
	}
private:
	cuttlefish::Window &_window;
	octopus::VisionPattern const _pattern;
};

std::list<Steppable *> WaveLevelSteps(cuttlefish::Window &window_p, Library &lib_p, RandomGenerator &rand_p, unsigned long waveCount_p, unsigned long stepCount_p, unsigned long worldSize_p,
	std::function<std::vector<octopus::Steppable *>(void)> waveStepGenerator_p)
{
	loadModels(lib_p);

	Building building_l({4, 20}, true, lib_p.getBuildingModel("command_center"));
	Unit unit_l({ 15, 20. }, false, lib_p.getUnitModel("worker"));

	Resource res1_l({20,20}, true, lib_p.getResourceModel("resource_bloc"));
	res1_l._resource = 2000.;
	res1_l._player = 2;

	Resource res3_l({15,17}, true, lib_p.getResourceModel("resource_ether"));
	res3_l._resource = 2000.;
	res3_l._player = 2;

	std::map<std::string, Fixed> mapRes_l;
	mapRes_l["bloc"] = -200;
	mapRes_l["ether"] = -200;
	mapRes_l["Anchor"] = -180;

	Trigger * triggerWave_l = new WaveSpawn(new ListenerStepCount(stepCount_p), lib_p, rand_p, 1, stepCount_p, waveCount_p, worldSize_p, waveStepGenerator_p);
	Trigger * triggerLose_l = new LoseTrigger(new ListenerEntityModelDied(&lib_p.getBuildingModel("command_center"), 0));


	Building anchorSpot_l({45,45}, true, lib_p.getBuildingModel("anchor_spot"));
	anchorSpot_l._player = 2;
    octopus::PatternHandler handler_l;
	octopus::VisionPattern pattern_l = handler_l.getPattern(10);
	for(std::pair<long, long> &pair_l : pattern_l)
	{
		pair_l.first += to_int(anchorSpot_l._pos.x);
		pair_l.second += to_int(anchorSpot_l._pos.y);
	}
	std::list<Steppable *> spawners_l =
	{
		new PlayerSpawnStep(0, 0),
		new PlayerSpawnStep(1, 1),
		new PlayerSpawnStep(2, 2),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("command_center")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("barrack_square")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("barrack_circle")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("barrack_triangle")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("deposit")),
		new PlayerAddBuildingModel(0, lib_p.getBuildingModel("anchor")),
		new PlayerSpendResourceStep(0, mapRes_l),
		new BuildingSpawnStep(0, building_l, true),
		new ResourceSpawnStep(1, res1_l),
		new ResourceSpawnStep(2, res3_l),
		new BuildingSpawnStep(3, anchorSpot_l, true),
		new StateTemplePositionAddStep(anchorSpot_l._pos),
		new UnitSpawnStep(4, unit_l),
		new UnitSpawnStep(5, unit_l),
		new UnitSpawnStep(6, unit_l),
		new UnitSpawnStep(7, unit_l),
		new UnitSpawnStep(8, unit_l),
		new UnitSpawnStep(9, unit_l),
		new UnitSpawnStep(10, unit_l),
		new UnitSpawnStep(11, unit_l),
		new UnitSpawnStep(12, unit_l),
		new UnitSpawnStep(13, unit_l),
		new TriggerSpawn(triggerWave_l),
		new TriggerSpawn(triggerLose_l),
		new TriggerSpawn(new AnchorTrigger(lib_p, rand_p, 60)),
		new TriggerSpawn(new VisionTrigger(window_p, pattern_l)),
		new FlyingCommandSpawnStep(new TimerDamage(100, 0, 0, "Anchor", Handle(0))),
	};

	return spawners_l;
}

std::list<Command *> WaveLevelCommands(Library &lib_p, RandomGenerator &rand_p, unsigned long worldSize_p)
{
	std::list<AreaSpawn> spawners_l;

	Resource res2_l({0,0}, true, lib_p.getResourceModel("resource_bloc"));
	res2_l._resource = 2000.;
	res2_l._player = 2;

	Resource res3_l({0,0}, true, lib_p.getResourceModel("resource_ether"));
	res3_l._resource = 2000.;
	res3_l._player = 2;

	Building anchorSpot_l({0,0}, true, lib_p.getBuildingModel("anchor_spot"));
	anchorSpot_l._player = 2;

	int areSize_l = 20;

	{
		AreaSpawn area_l;
		area_l.width = areSize_l;
		area_l.height = areSize_l;
		area_l.x = 35;
		area_l.y = 35;
		area_l.entities.emplace_back(new Resource(res3_l), 1);
		area_l.entities.emplace_back(new Resource(res2_l), 1);
		for(unsigned long c = 0 ; c < 20 ; ++ c)
		{
			Unit *unit_l = new Unit({0, 0}, false, lib_p.getUnitModel(genModelName(rand_p)));
			unit_l->_player = 1;
			area_l.entities.emplace_back(unit_l, 1);
		}
		spawners_l.push_back(area_l);
	}

	for(unsigned long x = 2 ; x < worldSize_p/areSize_l ; ++ x)
	{
		for(unsigned long y = 2 ; y < worldSize_p/areSize_l ; ++ y)
		{
			// skip spawn
			if(x==0 && y==0)
			{
				continue;
			}
			AreaSpawn area_l;
			area_l.width = areSize_l;
			area_l.height = areSize_l;
			area_l.x = 15 + area_l.size*x;
			area_l.y = 15 + area_l.size*y;
			area_l.entities.emplace_back(new Resource(res3_l), 1);
			area_l.entities.emplace_back(new Resource(res2_l), 1);
			area_l.entities.emplace_back(new Building(anchorSpot_l), 1);
			// only add unit if not on the neighbour of start point
			if(x + y > 1 )
			{
				for(unsigned long c = 0 ; c < 2+x*y ; ++ c)
				{
					Unit *unit_l = new Unit({0, 0}, false, lib_p.getUnitModel(genModelName(rand_p)));
					unit_l->_player = 1;
					area_l.entities.emplace_back(unit_l, 1);
				}
			}
			spawners_l.push_back(area_l);
		}
	}

	std::list<Command *> commands_l {
		new AreaSpawnerCommand(rand_p, spawners_l),
	};

	return commands_l;
}

/////////////////////////////////////////////
/////////////////////////////////////////////
/// 				Triggers			  ///
/////////////////////////////////////////////
/////////////////////////////////////////////

WaveSpawn::WaveSpawn(Listener * listener_p, Library const &lib_p, RandomGenerator &rand_p, unsigned long wave_p, unsigned long stepWait_p, unsigned long finalWave_p, unsigned long worldSize_p,
	std::function<std::vector<octopus::Steppable *>(void)> waveStepGenerator_p) :
		OneShotTrigger({listener_p}),
		_lib(lib_p),
		_rand(rand_p),
		_wave(wave_p),
		_stepWait(stepWait_p),
		_finalWave(finalWave_p),
		_worldSize(worldSize_p),
		_waveStepGenerator(waveStepGenerator_p)
{}

void WaveSpawn::trigger(State const &state_p, Step &step_p, unsigned long, octopus::TriggerData const &) const
{
	for(unsigned long i = 0 ; i < _wave * 10 ; ++ i)
	{
		std::string modelName_l = genModelName(_rand);
		Unit unit_l({ _worldSize, _worldSize }, false, _lib.getUnitModel(modelName_l));
		unit_l._pos.x -= _rand.roll(10,20);
		unit_l._pos.y -= _rand.roll(10,20);
		unit_l._player = 1;
		Handle handle_l = getNextHandle(step_p, state_p);
		step_p.addSteppable(new UnitSpawnStep(handle_l, unit_l));
		step_p.addSteppable(new CommandSpawnStep(new EntityAttackMoveCommand(handle_l, handle_l, {7., 20.}, 0, {{7., 20.}}, true )));
	}

	std::vector<octopus::Steppable *> stepsGenerated_l = _waveStepGenerator();
	for(octopus::Steppable *step_l : stepsGenerated_l)
	{
		step_p.addSteppable(step_l);
	}
	step_p.addSteppable(new TriggerSpawn(new WaveSpawn(new ListenerStepCount(_stepWait), _lib, _rand, _wave+1, _stepWait, _finalWave, _worldSize, _waveStepGenerator)));

	// win after X waves
	if(_wave == _finalWave)
	{
		step_p.addSteppable(new StateWinStep(state_p.isOver(), state_p.hasWinningTeam(), state_p.getWinningTeam(), 0));
	}
}

} // namespace level1
} // namespace fas
