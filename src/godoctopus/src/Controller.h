#ifndef __Godoctopus_Controller__
#define __Godoctopus_Controller__

#include <cstdint>
#include <fstream>
#include <functional>
#include <list>
#include <map>
#include <thread>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

#include "library/Library.hh"
#include "utils/RandomGenerator.hh"
#include "controller/Controller.hh"
#include "option/Option.h"
#include "option/OptionManager.h"
#include "file/FileHeader.h"
#include "levels/model/LevelModel.h"
#include "levels/wave/Wave.h"
#include "Entity.h"

namespace octopus
{
	class Command;
	class Steppable;
	class Entity;
	class Player;
	class State;
} // namespace octopus


namespace godot {

class Controller : public Node {
	GDCLASS(Controller, Node)

public:
	Controller();
	~Controller();

	// Will be called by Godot when the class is registered
	// Use this to add properties to your class
	static void _bind_methods();

	// demo levels
	void load_wave_level(int playerWave_p, int stepCount_p=3*60*100);
	void load_arena_level(TypedArray<int> const &size_you_p, TypedArray<int> const &size_them_p, TypedArray<String> const &model_you_p, TypedArray<String> const &model_them_p);
	void load_kamikaze_level(int you_p, int them_p, bool fast_p);
	void load_maze_level(int size_p);
	// showcase level
	void load_aoe_level(int size_p);
	void load_chaining_level();
	void load_dot_level(int size_p);
	void load_lifesteal_level(int size_p);
	// missions
	void load_mission_1(int seed_p, int player_count_p);
	void load_mission_2(int seed_p, godot::LevelModel *level_model_p, int player_count_p);
	void load_mission_3(int seed_p, godot::LevelModel *level_model_p, int player_count_p, int difficulty_p);

	// required for editor
	void load_minimal_model();

	// levels (modes)
	void load_hero_siege_level(int seed_p, int player_count_p);
	void load_demo_level(int seed_p, WavePattern const * wavePattern_p, godot::LevelModel *level_model_p, int player_count_p, int difficulty_p);
	void load_survival_level(int seed_p, WavePattern const * wavePattern_p, godot::LevelModel *level_model_p, int player_count_p,
		bool less_resources_p,
		bool more_enemies_map_p,
		bool two_direction_wave_p,
		bool bosses_p,
		bool fast_anchor_decay_p,
		int buff_per_wave_p
	);

	// levels (fas)
	void load_level1(int seed_p, int nb_wave_p);
	void load_level2(int seed_p, WavePattern const * wavePattern_p, int nb_players_p);
	void load_level_test_anchor(int seed_p);
	void load_level_test_model_reading(int seed_p, godot::LevelModel *level_model_p, bool buff_prod_p, bool upgrades_rune_p, int idx_first_player_p, int nb_players_p, bool use_div_option_p);
	void load_duel_level(int seed_p, TypedArray<int> const &div_player_1_p, TypedArray<int> const &div_player_2_p);
	void load_multi_test_level(int seed_p, int step_cout_p, bool buff_prod_p);

	// replay
	void set_model_filename(String const &filename_p);
	void set_level_filename(String const &filename_p);
	String get_model_filename(String const &filename_p);
	String get_level_filename(String const &filename_p);
	void replay_level(String const &filename_p, bool replay_mode_p, godot::LevelModel *level_model_p);

	// File header parsing
	godot::FileHeader * get_file_header();
	godot::FileHeader * read_file_header(String const &filename_p);

	// start engine with given level
	void init(std::list<octopus::Command *> const &commands_p, std::list<octopus::Steppable *> const &spawners_p, bool divOptionManager_p=false, uint32_t size_p=50, std::ofstream *file_p=nullptr);
	void init_replay(std::list<octopus::Command *> const &commands_p, std::list<octopus::Steppable *> const &spawners_p, bool divOptionManager_p, uint32_t size_p, std::ifstream &file_p);
	void init_loading(std::list<octopus::Command *> const &commands_p, std::list<octopus::Steppable *> const &spawners_p, bool divOptionManager_p, uint32_t size_p, std::ifstream &file_p);
	void loading_loop();
	void loop();

	bool has_state() const;

	void _process(double delta) override;

	octopus::Entity const * getEntity(octopus::Handle const &handle_p) const;
	octopus::Player const * getPlayer(int player_p) const;

	// signals
	void spawn(octopus::Handle const & handle_p);
	void kill(octopus::Handle const & handle_p);
	void move(octopus::Handle const & handle_p);
	void windup(int handle_p);
	void hp_change(int handle_p, float ratio_p, float diff_p);

	// setters
	void set_pause(bool paused_p);
	void set_over(bool over_p);
	void set_fast_forward(bool fast_forward_p);
	// getters
	bool get_pause() const;
	bool get_init_done() const;
	bool get_first_step_done() const;
	bool has_won(int player_p) const;
	int get_score(int player_p) const;
	int get_units_produced(int player_p) const;
	int get_units_lost(int player_p) const;
	int get_units_killed(int player_p) const;
	int get_harvested_resources(int player_p) const;
	// commands available getters
	TypedArray<String> get_models(EntityHandle const * handle_p, int player_p, bool checkRequirements_p) const;
	TypedArray<String> get_abilities(EntityHandle const * handle_p, int player_p, bool checkRequirements_p) const;
	// upgrade getters
	bool is_done_and_non_repeatable(String const &upgrade_p, int player_p) const;
	int get_level(String const &upgrade_p, int player_p) const;
	bool is_upgrade(String const &model_p) const;
	bool is_unit(String const &model_p) const;
	bool is_building(String const &model_p) const;
	bool is_resource(String const &model_p) const;
	// ability getters
	// get necessary reload time
	double get_reload_time(EntityHandle const * handle_p, String const &ability_p) const;
	// get current reload time (time since last usage of the ability)
	double get_current_reload_time(EntityHandle const * handle_p, String const &ability_p) const;
	// get reload ratio for the ability (0 means reloaded, 1 means not reloaded at all)
	double get_reload_ratio(EntityHandle const * handle_p, String const &ability_p) const;
	// check if there is non static entity behind this one
	bool hasNonStaticBehind(EntityHandle const * handle_p, int height_p, int width_p) const;
	bool hasNonStaticBehindFromPos(Vector2 const &pos_p, int height_p, int width_p) const;

	float get_model_ray(String const &model_p) const;
	/// @brief check if grid is free (model is required to handle specific building check)
	bool is_grid_free(String const &model_p, int x_p, int y_p, int player_p) const;

	int get_world_size() const;
	int get_steps() const;
	int get_team(int player_p) const;
	/// @brief get all handles for idle worker for the given player in a packed array
	PackedInt32Array get_idle_workers(int player_p) const;
	/// @brief decode the given handles and return all move or attack target encoded as a float packed array
	/// @note encoding is as follow : x, y, type
	/// where type < 0.1 if move command or > 0.1 if attack command
	PackedFloat32Array get_move_targets(PackedInt32Array const &handles_p);

	PackedInt32Array get_sub_selection(Rect2 const &rect_p, String const &model_p, int player_p);

	// resources getter
	float get_res(String const &res_p, int player_p) const;

	// vision getter
	bool is_visible(int x, int y, int player_p) const;
	bool is_unit_visible(EntityHandle const * handle_p, int player_p) const;
	bool is_explored(int x, int y, int player_p) const;
	bool is_entity_explored(EntityHandle const * handle_p, int player_p) const;
	PackedByteArray getVisibility(int player_p) const;

	// option getter
	int get_nb_queued_options(int player_p) const;
	int get_nb_options_available(int player_p) const;
	int get_nb_options_chosen(int player_p) const;
	godot::Option *get_available_option_you(int idx_p, int player_p) const;
	godot::Option *get_available_option_them(int idx_p, int player_p) const;
	godot::Option *get_chosen_option_you(int idx_p, int player_p) const;
	godot::Option *get_chosen_option_them(int idx_p, int player_p) const;

	// error getters
	bool has_catched_exception() const;
	String get_exception_message() const;

	// signal emmiter

	/// @brief will emit one signal per production to create
	void get_productions(TypedArray<EntityHandle> const &handles_p, int max_p);

	/// @brief will emit one signal per unit to be shown or hidden
	void get_visible_units(int player_p, int ent_registered_p);

	// commands
	void add_debug_command(int peer, int param_p);
	// move & attack
	void add_move_commands(int peer, PackedInt32Array const &handles_p, Vector2 const &target_p, int player_p, bool queued_p);
	void add_move_target_commands(int peer, PackedInt32Array const &handles_p, Vector2 const &target_p, PackedInt32Array const &handleTarget_p, int player_p, bool queued_p);
	void add_attack_move_commands(int peer, PackedInt32Array const &handles_p, Vector2 const &target_p, int player_p, bool queued_p);
	void add_stop_commands(int peer, PackedInt32Array const &handles_p, int player_p, bool queued_p);
	// production
	void add_unit_auto_build_command(int peer, PackedInt32Array const &handles_p, String const &model_p, int player_p);
	void add_unit_build_command(int peer, PackedInt32Array const &handles_p, String const &model_p, int player_p);
	void add_unit_build_cancel_command(int peer, PackedInt32Array const & handle_p, int index_p, int player_p);
	// building
	void add_blueprint_command(int peer_p, Vector2 const &target_p, String const &model_p, int player_p, PackedInt32Array const &builders_p, bool queued_p);
	void add_building_cancel_command(int peer_p, PackedInt32Array const & handle_p, int player_p);
	// option
	void add_chose_option_command(int peer_p, int option_p, int player_p);
	// ability (with no target)
	void add_ability_command(int peer_p, PackedInt32Array const & handles_p, String const &ability_p);


	// step
	/// @brief set the number of queued layer to be setup
	void set_step_control(int prequeued_p);
	/// @brief allow controller to proceed to next step while adding a queued layer
	void next_step();
	/// @brief get the size of the queued layers
	int get_queued_size() const;
	/// @brief get the size of prepared layer for a peer (the one not sent to the engine controller yet)
	int get_queued_size_from_peer(int peer_p) const;

	// handling of peer information
	/// @brief add peer info on player
	void add_peer_info(int peer_p, int player_p);
	/// @brief register that peer is done with current queue
	/// will add a new queue layer for the given peer
	void step_done_for_peer(int peer_p);

	////////////////////////////
	//// Dump methods
	////////////////////////////

	// save file
	void save_to_file(String const &path_p);
	// auto saving during game
	void set_auto_file_path(String const &path_p);
	void set_auto_file_debug(bool debug_p);
	// dump state as plain text
	void dump_state_as_text(String const &path_p);

	////////////////////////////
	//// Miscellanous
	////////////////////////////
	String get_hash_version() const;
	double get_avg_last_compile_time(int numbers_p) const;
	double get_max_compile_time(int numbers_p) const;
	void dump_compile_times(String const &path_p) const;
	void print_metrics() const;

	////////////////////////////
	//// Non godot methods
	////////////////////////////

	std::map<unsigned long, AbstractOptionManager *> &getOptionManagers();
	std::map<unsigned long, AbstractOptionManager *> const &getOptionManagers() const;

	octopus::Library &getLib();
	octopus::Library const &getLib() const;

	octopus::State const * getState() const;
private:
	void newAutoSaveFile();
	octopus::Controller * _controller = nullptr;
	bool _initDone = false;
	std::thread * _controllerThread = nullptr;
	// auto save information
	std::string _autoSavePath = "autosave.fas";
	std::ofstream * _autoSaveFile = nullptr;
	std::ofstream * _autoSaveFileDebug = nullptr;
	bool _enableAutoSaveFileDebug = false;

	// game related
	octopus::Library _lib;
	octopus::RandomGenerator * _rand = nullptr;
	octopus::State const * _state = nullptr;
	bool _over = false;
	bool _paused = false;
	bool _forceAllMoveUpdate = false;
	// level id
	uint32_t _currentLevel = 0;
	// header writer of level
	std::function<void(std::ofstream&)> _headerWriter;
	// FileHeader
	FileHeader * _fileHeader = nullptr;

	std::list<octopus::StepBundle>::const_iterator _lastIt;

	/// @brief vector storing last visible units when call of get_visible_units
	std::vector<bool> _visibleLastCall;
	/// @brief vector storing the revision last call of get_visible_units to avoid keeping units visible
	std::vector<char> _visibleRevisionLastCall;

	/// @brief manager for each player
	std::map<unsigned long, AbstractOptionManager *> _optionManagers;

	/// @brief if set to true controller loop will be capped by _stepDone
	bool _stepControl = true;
	/// @brief idx of last complete step so the controller can step by
	int _stepDone = 0;

	/// @brief queued command per peer
	std::map<int, std::list<std::list<octopus::Command*> > > _queuedCommandsPerPeer;
	/// @brief player per peer
	std::map<int, unsigned long> _playerPerPeer;

	bool _catchedException = false;
	bool _readyToLoopSent = false;
	std::string _exception;

};

}

#endif //__Godoctopus_Controller__
