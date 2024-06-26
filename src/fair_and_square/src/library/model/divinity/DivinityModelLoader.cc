#include "DivinityModelLoader.hh"

#include "library/Library.hh"
#include "step/player/PlayerAddBuildingModel.hh"
#include "step/player/PlayerSpawnStep.hh"

// fas
#include "library/model/divinity/generators/AttackSpeedDivinityStepGenerator.hh"
#include "library/model/divinity/generators/EconomicDivinityStepGenerator.hh"
#include "library/model/divinity/generators/HealDivinityStepGenerator.hh"
#include "library/model/divinity/generators/LifestealDivinityStepGenerator.hh"
#include "library/model/divinity/generators/ProductionDivinityStepGenerator.hh"
#include "library/model/divinity/generators/RecycleDivinityStepGenerator.hh"
#include "library/model/divinity/generators/UnitModelIds.hh"

namespace fas
{

void loadLibrary(octopus::Library &lib_p)
{
	// load divinity library
	attackspeed_divinity::fillLibrary(attackspeed_divinity::createDefaultParams(), lib_p);
	economic_divinity::fillLibrary(economic_divinity::createDefaultParams(), lib_p);
	heal_divinity::fillLibrary(heal_divinity::createDefaultParams(), lib_p);
	lifesteal_divinity::fillLibrary(lifesteal_divinity::createDefaultParams(), lib_p);
	production_divinity::fillLibrary(production_divinity::createDefaultParams(), lib_p);
	recycle_divinity::fillLibrary(recycle_divinity::createDefaultParams(), lib_p);
}

std::vector<int> divinityTypeToInt(std::vector<DivinityType> const &types_p)
{
    std::vector<int> result_l;
    for(DivinityType const &type_l : types_p)
    {
        result_l.push_back(static_cast<int>(type_l));
    }
    return result_l;
}

std::vector<DivinityType> intToDivinityType(std::vector<int> const &types_p)
{
    std::vector<DivinityType> result_l;
    for(int const &type_l : types_p)
    {
        result_l.push_back(static_cast<DivinityType>(type_l));
    }
    return result_l;
}

std::vector<octopus::Steppable *> newPlayerBuilding(unsigned long player_p, DivinityType type_l, octopus::Library const &lib_p)
{
    switch(type_l)
    {
        case DivinityType::AttackSpeed:
            return {new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel(models::AttackSpeedBuildingId))};
        case DivinityType::Economic:
            return {
                new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel(models::EconomicBuildingId)),
                new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel(models::EconomicBuildingResourceProducerId))
            };
        case DivinityType::Heal:
            return {new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel(models::HealBuildingId))};
        case DivinityType::Lifesteal:
            return {new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel(models::LifestealBuildingId))};
        case DivinityType::Production:
            return {new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel(models::ProductionBuildingId))};
        case DivinityType::Recycle:
            return {new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel(models::RecycleBuildingId))};
    }
    return {};
}

std::string divinityUpgradeName(DivinityType type_p)
{
    switch(type_p)
    {
        case fas::DivinityType::AttackSpeed:
            return models::AttackSpeedDivId;
        case fas::DivinityType::Economic:
            return models::EconomicDivId;
        case fas::DivinityType::Heal:
            return models::HealDivId;
        case fas::DivinityType::Lifesteal:
            return models::LifestealDivId;
        case fas::DivinityType::Production:
            return models::ProductionDivId;
        case fas::DivinityType::Recycle:
            return models::RecycleDivId;
    }
    return "";
}

DivinityType rollOneRandomDivinity(octopus::RandomGenerator &rand_p, std::vector<DivinityType> const &exceptions_p)
{
    return rollOneRandomDivinityFromList(rand_p, allDivinities(), exceptions_p);
}

DivinityType rollOneRandomDivinityFromList(octopus::RandomGenerator &rand_p, std::vector<DivinityType> const &list_p, std::vector<DivinityType> const &exceptions_p)
{
    std::vector<DivinityType> options_l = list_p;

	for(DivinityType type_l : exceptions_p)
	{
		auto &&it_l = std::find(options_l.begin(), options_l.end(), type_l);
		if(it_l != options_l.cend())
		{
			options_l.erase(it_l);
		}
	}

	if(options_l.empty())
	{
		return DivinityType::None;
	}

    int roll_l = rand_p.roll(0, options_l.size()-1);

    return options_l[roll_l];
}

std::vector<DivinityType> allDivinities()
{
    return {
        DivinityType::AttackSpeed,
        DivinityType::Economic,
        DivinityType::Heal,
        DivinityType::Lifesteal,
        DivinityType::Production,
        DivinityType::Recycle
    };
}

std::vector<DivinityType> notDemoDivinities()
{
    return {
        DivinityType::Economic,
        DivinityType::Production
    };
}

std::vector<DivinityType> demoDivinities()
{
    return {
        DivinityType::AttackSpeed,
        DivinityType::Heal,
        DivinityType::Lifesteal,
        DivinityType::Recycle
    };
}

std::vector<DivinityType> getMaxedOutDivinities(octopus::Player const &player_p, unsigned long maxLevel_p)
{
	std::vector<DivinityType> maxedOut_l;
	for(DivinityType type_l : allDivinities())
	{
		if(getUpgradeLvl(player_p, divinityUpgradeName(type_l)) >= maxLevel_p)
		{
			maxedOut_l.push_back(type_l);
		}
	}

	return maxedOut_l;
}


void addBuildingPlayer(std::list<octopus::Steppable *> &spawners_p, unsigned long player_p, std::vector<DivinityType> const &divinities_p, octopus::Library &lib_p)
{
	spawners_p.push_back(new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel("command_center")));
	spawners_p.push_back(new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel("barrack_square")));
	spawners_p.push_back(new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel("barrack_circle")));
	spawners_p.push_back(new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel("barrack_triangle")));
	spawners_p.push_back(new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel("deposit")));
	spawners_p.push_back(new octopus::PlayerAddBuildingModel(player_p, lib_p.getBuildingModel("anchor")));

    addDivinityBuildingPlayer(spawners_p, player_p, divinities_p, lib_p);
}

void addDivinityBuildingPlayer(std::list<octopus::Steppable *> &spawners_p, unsigned long player_p, std::vector<DivinityType> const &divinities_p, octopus::Library &lib_p)
{
	for(fas::DivinityType div_l : divinities_p)
	{
		std::vector<octopus::Steppable *> steps_l = newPlayerBuilding(player_p, div_l, lib_p);
		for(octopus::Steppable * step_l : steps_l)
		{
			spawners_p.push_back(step_l);
		}
	}
}

} // namespace fas

