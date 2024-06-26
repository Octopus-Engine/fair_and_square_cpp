#ifndef __Godoctopus_LevelModel__
#define __Godoctopus_LevelModel__

#include <string>
#include <cstdint>
#include <map>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

#include "state/Handle.hh"
#include "utils/Fixed.hh"
#include "utils/ModelTypes.h"

namespace octopus
{
    class Library;
    class Steppable;
} // namespace octopus


namespace godot {

class Controller;

class LevelModel : public Node {
    GDCLASS(LevelModel, Node)

public:
    ~LevelModel();

    // Will be called by Godot when the class is registered
    // Use this to add properties to your class
    static void _bind_methods();

    ////////////////
    /// player
    ////////////////
    int add_player(int team);
    void add_resource(int player, String const &resource, float quantity);
    void add_building(int player, String const &building);

    ////////////////
    /// entities
    ////////////////
    void add_entity(String const &type_p, String const &model_p, int player, float x, float y, PackedInt32Array const &array_p, int num_of_players);
    void add_entity_resource(String const &type_p, String const &model_p, int player, float x, float y, PackedInt32Array const &array_p, int num_of_players, int resource_qty);

    ////////////////
    /// triggers
    ////////////////
	int add_trigger();
	void add_trigger_listener_entity_dead_group(int triggerIdx_p, int entityDeadGroup_p);
	void add_trigger_listener_timer(int triggerIdx_p, int steps_p);
	void add_trigger_listener_zone_player(int triggerIdx_p, int player_p, String const &zone_p);
	void add_trigger_listener_zone_team(int triggerIdx_p, int team_p, String const &zone_p);
	void add_trigger_listener_resource(int triggerIdx_p, int player_p, String const &resource_p, int qty_p, bool lower_than_p);
	void add_trigger_listener_entity_produced(int triggerIdx_p, int player_p, String const &model_p);
	void add_trigger_listener_upgrade(int triggerIdx_p, int player_p, int level_p, String const &upgrade_p);

	void add_trigger_action_dialog(int triggerIdx_p, String const &dialogIdx_p, bool end_p, int team_winning_p);
	void add_trigger_action_camera(int triggerIdx_p, int x, int y, int player_p);
	void add_trigger_action_damage_zone(int triggerIdx_p, int damage_p, int team_p, String const &zone_name_p);
    void add_trigger_action_add_objective(int triggerIdx_p, String const &obj_name_p, bool is_main_p, int count_p, bool remove_p);
    void add_trigger_action_complete_objective(int triggerIdx_p, String const &obj_name_p, bool complete_p);
    void add_trigger_action_fail_objective(int triggerIdx_p, String const &obj_name_p, bool fail_p);
    void add_trigger_action_increment_objective(int triggerIdx_p, String const &obj_name_p, bool increment_p);
    void add_trigger_action_resource(int triggerIdx_p, int player_p, int qty_p, String const &resource_p);

	// action spawn
	int add_trigger_action_spawn(int triggerIdx_p);
	void action_spawn_add_entity(int triggerIdx_p, int actionIdx_p, String const &type_p, String const &model_p, int player, float x, float y, int num_of_players);
	void action_spawn_add_attack_move(int triggerIdx_p, int actionIdx_p, float x, float y);

    ////////////////
    /// zones
    ////////////////
	void add_zone(String const &name_p, int x, int y, int size_x, int size_y);

    ////////////////
    /// specific
    ////////////////
	void set_specific(String const &specific_p);

    ////////////////
    /// generator (steps)
    ////////////////
    std::list<octopus::Steppable *> generateLevelSteps(octopus::Library const &lib_p, unsigned long playerCount_p);

	std::vector<GodotEntity> const &getEntities() const { return _entities; }
private:
    /// @brief players to spawn
    std::vector<GodotPlayer> _players;

    std::vector<GodotEntity> _entities;

    std::vector<GodotTrigger> _triggers;

	std::vector<GodotZone> _zones;

	/// @brief hold the specific c++ step generation (none if empty)
	std::string _specific;
};

}

#endif //__Godoctopus_LevelModel__
