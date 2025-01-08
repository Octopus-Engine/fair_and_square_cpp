#include "CommandController.h"

#include <limits>

// octopus
#include "command/CommandHelpers.hh"
#include "command/CommandQueue.hh"
#include "command/building/BuildingAutoBuildCommand.hh"
#include "command/building/BuildingBlueprintCommand.hh"
#include "command/building/BuildingGroupProductionCommand.hh"
#include "command/building/BuildingUnitProductionCommand.hh"
#include "command/building/BuildingUnitCancelCommand.hh"
#include "command/entity/EntityAttackCommand.hh"
#include "command/entity/EntityAttackMoveCommand.hh"
#include "command/entity/EntityBuildingCommand.hh"
#include "command/entity/EntityFlockMoveCommand.hh"
#include "command/entity/EntityMoveCommand.hh"
#include "command/entity/EntityWaitCommand.hh"
#include "command/move/GenericMoveCommand.hh"
#include "command/unit/UnitDropCommand.hh"
#include "command/unit/UnitHarvestCommand.hh"
#include "controller/Controller.hh"
#include "library/Library.hh"
#include "state/entity/Building.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Resource.hh"
#include "state/entity/Unit.hh"
#include "state/player/Player.hh"
#include "state/State.hh"

namespace godot
{

void add_move_commands(std::list<octopus::Command*> &list_r, octopus::State const &, PackedInt32Array const &handles_p, Vector2 const &target_p, int , bool queued_p)
{
    octopus::Vector worldPos_l(target_p.x, target_p.y);
    std::vector<octopus::Handle> handles_l;
    for(uint32_t i = 0 ; i < handles_p.size()/2 ; ++ i)
    {
        handles_l.push_back(castHandle(handles_p[i*2],handles_p[i*2+1]));
    }
    octopus::Command * cmd_l = new octopus::GenericMoveCommand(handles_l, worldPos_l);

    cmd_l->setQueued(queued_p);
    list_r.push_back(cmd_l);
}

void add_move_target_commands(std::list<octopus::Command*> &list_r, octopus::State const &state_p, PackedInt32Array const &handles_p, Vector2 const &target_p, PackedInt32Array const & handleTarget_p, int , bool queued_p)
{
	octopus::Handle target_l = castHandle(handleTarget_p[0], handleTarget_p[1]);
    octopus::Vector worldPos_l(target_p.x, target_p.y);
    std::vector<octopus::Handle> handles_l;
    for(uint32_t i = 0 ; i < handles_p.size()/2 ; ++ i)
    {
        handles_l.push_back(castHandle(handles_p[i*2],handles_p[i*2+1]));
    }
    octopus::Command * cmd_l = new octopus::GenericMoveCommand(handles_l, worldPos_l, target_l);

    cmd_l->setQueued(queued_p);
    list_r.push_back(cmd_l);
}

void add_attack_move_commands(std::list<octopus::Command*> &list_r, octopus::State const &, PackedInt32Array const &handles_p, Vector2 const &target_p, int, bool queued_p)
{
    octopus::Vector worldPos_l(target_p.x, target_p.y);
    std::list<octopus::Handle> handles_l;
    for(uint32_t i = 0 ; i < handles_p.size()/2 ; ++ i)
    {
        handles_l.push_back(castHandle(handles_p[i*2],handles_p[i*2+1]));
    }
    octopus::Command * cmd_l = new octopus::EntityFlockMoveCommand(handles_l, worldPos_l, true);

    cmd_l->setQueued(queued_p);
    list_r.push_back(cmd_l);
}

void add_stop_commands(std::list<octopus::Command*> &list_r, octopus::State const &state_p, PackedInt32Array const &handles_p, int, bool queued_p)
{
    for(uint32_t i = 0 ; i < handles_p.size()/2 ; ++ i)
    {
        octopus::Handle idx_l = castHandle(handles_p[i*2],handles_p[i*2+1]);
        const octopus::Entity * cur_l = state_p.getEntity(idx_l);
		bool isStatic_l = cur_l->_model._isStatic;

		if(isStatic_l)
        {
            continue;
        }
        octopus::Command *cmd_l = new octopus::EntityWaitCommand(idx_l, idx_l);
        cmd_l->setQueued(queued_p);
        list_r.push_back(cmd_l);
    }
}

void add_unit_auto_build_command(std::list<octopus::Command*> &list_r, octopus::State const &, octopus::Library const &lib_p, PackedInt32Array const &handles_p, String const &model_p, int)
{
	std::string modelId_l(model_p.utf8().get_data());

	octopus::UnitModel const *unit_l = nullptr;
    if(lib_p.hasUnitModel(modelId_l))
    {
        unit_l = &lib_p.getUnitModel(modelId_l);
	}
	for(uint32_t i = 0 ; i < handles_p.size()/2 ; ++ i)
	{
		octopus::Handle idx_l = castHandle(handles_p[i*2],handles_p[i*2+1]);
		octopus::BuildingAutoBuildCommand *cmd_l = new octopus::BuildingAutoBuildCommand(idx_l, unit_l);
		list_r.push_back(cmd_l);
	}
}

void add_unit_build_command(std::list<octopus::Command*> &list_r, octopus::State const &, octopus::Library const &lib_p, PackedInt32Array const &handles_p, String const &model_p, int)
{
    std::string modelId_l(model_p.utf8().get_data());

    std::vector<octopus::Handle> handles_l;
    for(uint32_t i = 0 ; i < handles_p.size()/2 ; ++ i)
    {
        handles_l.push_back(castHandle(handles_p[i*2],handles_p[i*2+1]));
    }

    if(lib_p.hasUnitModel(modelId_l))
    {
        octopus::UnitModel const &unit_l = lib_p.getUnitModel(modelId_l);
		auto cmd_l = new octopus::BuildingGroupProductionCommand<octopus::BuildingUnitProductionCommand, octopus::UnitModel>(handles_l, &unit_l);
		cmd_l->setQueued(true);
		list_r.push_back(cmd_l);
    }
    else if(lib_p.hasUpgrade(modelId_l))
    {
        octopus::Upgrade const &upgrade_l = lib_p.getUpgrade(modelId_l);
		auto cmd_l = new octopus::BuildingGroupProductionCommand<octopus::BuildingUpgradeProductionCommand, octopus::Upgrade>(handles_l, &upgrade_l);
		cmd_l->setQueued(true);
		list_r.push_back(cmd_l);
    }
}

void add_unit_build_cancel_command(std::list<octopus::Command*> &list_r, octopus::State const &, PackedInt32Array const &handle_p, int index_p, int)
{
    list_r.push_back(new octopus::BuildingUnitCancelCommand(castHandle(handle_p[0], handle_p[1]), index_p));
}

void add_blueprint_command(std::list<octopus::Command*> &list_r, octopus::State const &, octopus::Library const &lib_p, Vector2 const &target_p,
    String const &model_p, int player_p, PackedInt32Array const &builders_p, bool queued_p)
{
    std::string modelId_l(model_p.utf8().get_data());
    std::vector<octopus::Handle> builders_l;
    for(uint32_t i = 0 ; i < builders_p.size()/2 ; ++ i)
    {
        builders_l.push_back(castHandle(builders_p[2*i], builders_p[2*i+1]));
    }

    if(lib_p.hasBuildingModel(modelId_l))
    {
        octopus::Command * cmd_l = new octopus::BuildingBlueprintCommand(octopus::Vector(target_p.x, target_p.y), player_p, lib_p.getBuildingModel(modelId_l), builders_l);
        cmd_l->setQueued(queued_p);
        list_r.push_back(cmd_l);
    }
}

void add_ability_command(std::list<octopus::Command*> &list_r, octopus::State const &, PackedInt32Array const &handles_p, String const &ability_p)
{
    std::string abilityId_l(ability_p.utf8().get_data());
    std::list<octopus::Handle> handles_l;
    for(uint32_t i = 0 ; i < handles_p.size()/2 ; ++ i)
    {
        handles_l.push_back(castHandle(handles_p[i*2],handles_p[i*2+1]));
    }

	for(octopus::Handle const &handle_l : handles_l)
	{
        octopus::Command * cmd_l = new octopus::EntityAbilityCommand(handle_l, handle_l, octopus::Vector(), abilityId_l);
        list_r.push_back(cmd_l);
	}
}

} // godot
