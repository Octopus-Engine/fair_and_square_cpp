#include "Entity.h"
#include "Controller.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>

// octopus
#include "state/State.hh"
#include "state/player/Player.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Building.hh"
#include "state/entity/Resource.hh"
#include "logger/Logger.hh"

namespace godot {

octopus::Handle castHandle(godot::EntityHandle const *handle_p)
{
    if(!handle_p)
    {
        throw std::logic_error("cannot get handle from null");
    }
    if(handle_p->get_index() < 0 || handle_p->get_revision() < 0)
    {
        throw std::logic_error("invalid cas from EntityHandle (negative index or revision)");
    }
    return octopus::Handle(handle_p->get_index(), handle_p->get_revision());
}

octopus::Handle castHandle(godot::Variant const &var_p)
{
    godot::EntityHandle const * handleEntity_l = godot::Node::cast_to<godot::EntityHandle>(var_p);
    return castHandle(handleEntity_l);
}

octopus::Handle castHandle(int const &index_p, int const &revision_p)
{
    return octopus::Handle(index_p, revision_p);
}

Entity::Entity() : _handle(memnew(EntityHandle()))
{}

Entity::~Entity()
{}

void Entity::set_index(int index_p) {
    _index = index_p;
    _handle->set_index(index_p);
}

void Entity::set_revision(int revision_p) {
    _revision = revision_p;
    _handle->set_revision(revision_p);
}

EntityHandle * Entity::get_handle() const
{
    return _handle;
}

int Entity::get_index() const
{
    return _index;
}

int Entity::get_revision() const
{
    return _revision;
}

float Entity::get_hp(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return octopus::to_double(ent_l->_hp);
}

float Entity::get_hp_max(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return octopus::to_double(ent_l->getHpMax());
}

Vector2 Entity::get_pos(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return Vector2(octopus::to_double(ent_l->_pos.x), octopus::to_double(ent_l->_pos.y));
}

bool Entity::is_alive(Controller const *controller_p) const
{
    return controller_p->getState()->isEntityAlive(octopus::Handle(_index, _revision));
}

bool Entity::is_blueprint(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    if(!ent_l->_model._isBuilding)
    {
        return false;
    }
    octopus::Building const *building_l = static_cast<octopus::Building const *>(ent_l);

    return building_l->isBlueprint();
}

bool Entity::is_building(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return ent_l->_model._isBuilding;
}

bool Entity::is_built(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    if(!ent_l->_model._isBuilding)
    {
        return true;
    }
    octopus::Building const *building_l = static_cast<octopus::Building const *>(ent_l);

    return building_l->isBuilt();
}

bool Entity::is_resource(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return ent_l->_model._isResource;
}

bool Entity::is_unit(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return ent_l->_model._isUnit;
}

bool Entity::is_invulnerable(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return ent_l->_model._invulnerable;
}

bool Entity::is_idle(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return !ent_l->getQueue().hasCommand();
}

int Entity::get_player(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return ent_l->_player;
}

int Entity::get_team(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    octopus::Player const *player_l = controller_p->getPlayer(ent_l->_player);
    return player_l->_team;
}

float Entity::get_ray(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return octopus::to_double(ent_l->_model._ray);
}

String Entity::get_model(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return ent_l->_model._id.c_str();
}

float Entity::get_damage(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return octopus::to_double(ent_l->getDamageNoBonus());
}

float Entity::get_armor(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return octopus::to_double(ent_l->getArmor());
}

float Entity::get_speed(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return octopus::to_double(ent_l->getStepSpeed()*100);
}

float Entity::get_reload(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return octopus::to_double(100./ent_l->getFullReload());
}

float Entity::get_range(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return octopus::to_double(ent_l->_model._range);
}

float Entity::get_los(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    return octopus::to_double(ent_l->_model._lineOfSight);
}

bool Entity::has_rally_point(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    if(!ent_l->_model._isBuilding)
    {
        return false;
    }

    octopus::Building const *building_l = static_cast<octopus::Building const *>(ent_l);

    return building_l->_rallyPointActive;
}

Vector2 Entity::get_rally_point(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    if(!ent_l->_model._isBuilding)
    {
        return Vector2(0,0);
    }

    octopus::Building const *building_l = static_cast<octopus::Building const *>(ent_l);
    return Vector2(octopus::to_double(building_l->_rallyPoint.x), octopus::to_double(building_l->_rallyPoint.y));
}

String Entity::get_auto_build_model(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    if(!ent_l->_model._isBuilding)
    {
        return "";
    }

    octopus::Building const *building_l = static_cast<octopus::Building const *>(ent_l);
    if(!building_l->_autoBuild)
    {
        return "";
    }
    return building_l->_autoBuild->_id.c_str();
}

String Entity::get_resource_type(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    if(!ent_l->_model._isResource)
    {
        return "";
    }
    octopus::Resource const *res_l = static_cast<octopus::Resource const *>(ent_l);
    return res_l->getType().c_str();
}

float Entity::get_resource_quantity(Controller const *controller_p) const
{
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    if(!ent_l->_model._isResource)
    {
        return 0.;
    }
    octopus::Resource const *res_l = static_cast<octopus::Resource const *>(ent_l);
    return octopus::to_double(res_l->_resource);
}

TypedArray<godot::Buff> Entity::get_buffs(Controller const *controller_p) const
{
    TypedArray<godot::Buff> buffs_l;
    octopus::Entity const *ent_l = controller_p->getEntity(octopus::Handle(_index, _revision));
    for(auto &&pair_l : ent_l->_registeredBuff)
    {
        octopus::AnyBuff const &anyBuff_l = pair_l.second;
        unsigned long duration_l = octopus::get_duration(anyBuff_l);
        std::string const &name_l = pair_l.first;
        if(ent_l->_timeSinceBuff.at(name_l) < duration_l
        || duration_l == 0)
        {
            godot::Buff * buff_l = memnew(godot::Buff());
            buff_l->setName(name_l.c_str());
            buff_l->setTimeElapsed(ent_l->_timeSinceBuff.at(name_l));
            buff_l->setDuration(duration_l);

            // fill desc and params
            if(std::holds_alternative<octopus::TimedBuff>(anyBuff_l))
            {
				buff_l->setName("null");
                octopus::TimedBuff const &tBuff_l = std::get<octopus::TimedBuff>(anyBuff_l);
                std::stringstream ss_l;
                ss_l<<"timed_buff";
                using octopus::Fixed;
                if(tBuff_l._offset != Fixed::Zero())
                {
                    if(tBuff_l._offset > Fixed::Zero())
                    {
                        ss_l<<"_p_offset";
                        buff_l->addParam(std::to_string(tBuff_l._offset.data()/(Fixed::OneAsLong())).c_str());
                    }
                    else
                    {
                        ss_l<<"_n_offset";
                        buff_l->addParam(std::to_string(-tBuff_l._offset.data()/(Fixed::OneAsLong())).c_str());
                    }
                }
                if(tBuff_l._coef != Fixed::Zero())
                {
                    if(tBuff_l._coef > Fixed::Zero())
                    {
                        ss_l<<"_p_coef";
                        buff_l->addParam(std::to_string(tBuff_l._coef.data()/(Fixed::OneAsLong()/100)).c_str());
                    }
                    else
                    {
                        ss_l<<"_n_coef";
                        buff_l->addParam(std::to_string(-tBuff_l._coef.data()/(Fixed::OneAsLong()/100)).c_str());
                    }
                }
                buff_l->setDesc(ss_l.str().c_str());
                buff_l->addParam(octopus::to_string(tBuff_l._type).c_str());
            }
			else if(std::holds_alternative<octopus::ModifierAoEBuff>(anyBuff_l))
            {
				buff_l->setName("null");
                octopus::ModifierAoEBuff const &modBuff_l = std::get<octopus::ModifierAoEBuff>(anyBuff_l);
				std::stringstream ss_l;
                ss_l<<"aoe_buff";
                using octopus::Fixed;
                if(modBuff_l._deltaRatio != Fixed::Zero())
                {
                    if(modBuff_l._deltaRatio > Fixed::Zero())
                    {
                        ss_l<<"_p_ratio";
                        buff_l->addParam(std::to_string(modBuff_l._deltaRatio.data()/(Fixed::OneAsLong()/100)).c_str());
                    }
                    else
                    {
                        ss_l<<"_n_ratio";
                        buff_l->addParam(std::to_string(-modBuff_l._deltaRatio.data()/(Fixed::OneAsLong()/100)).c_str());
                    }
                }
                if(modBuff_l._deltaRange != Fixed::Zero())
                {
                    if(modBuff_l._deltaRange > Fixed::Zero())
                    {
                        ss_l<<"_p_range";
                        buff_l->addParam(std::to_string(modBuff_l._deltaRange.data()/(Fixed::OneAsLong())).c_str());
                    }
                    else
                    {
                        ss_l<<"_n_range";
                        buff_l->addParam(std::to_string(-modBuff_l._deltaRange.data()/(Fixed::OneAsLong())).c_str());
                    }
                }
                buff_l->setDesc(ss_l.str().c_str());
			}


            buffs_l.push_back(buff_l);
        }
    }
    return buffs_l;
}

void EntityHandle::_bind_methods()
{
    UtilityFunctions::print("EntityHandle Buff methods");

    ClassDB::bind_method(D_METHOD("set_index", "index"), &EntityHandle::set_index);
    ClassDB::bind_method(D_METHOD("get_index"), &EntityHandle::get_index);

    ClassDB::bind_method(D_METHOD("set_revision", "revision"), &EntityHandle::set_revision);
    ClassDB::bind_method(D_METHOD("get_revision"), &EntityHandle::get_revision);

    ADD_GROUP("EntityHandle", "EntityHandle_");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "index"), "set_index", "get_index");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "revision"), "set_revision", "get_revision");
}

void Buff::_bind_methods()
{
    UtilityFunctions::print("Binding Buff methods");

    ClassDB::bind_method(D_METHOD("setTimeElapsed", "time_elapsed"), &Buff::setTimeElapsed);
    ClassDB::bind_method(D_METHOD("getTimeElapsed"), &Buff::getTimeElapsed);

    ClassDB::bind_method(D_METHOD("setDuration", "duration"), &Buff::setDuration);
    ClassDB::bind_method(D_METHOD("getDuration"), &Buff::getDuration);

    ClassDB::bind_method(D_METHOD("setName", "name"), &Buff::setName);
    ClassDB::bind_method(D_METHOD("getName"), &Buff::getName);

    ClassDB::bind_method(D_METHOD("setDesc", "desc"), &Buff::setDesc);
    ClassDB::bind_method(D_METHOD("getDesc"), &Buff::getDesc);

    ClassDB::bind_method(D_METHOD("addParam", "params"), &Buff::addParam);
    ClassDB::bind_method(D_METHOD("getParams"), &Buff::getParams);

    ADD_GROUP("Buff", "Buff_");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "time_elapsed"), "setTimeElapsed", "getTimeElapsed");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "duration"), "setDuration", "getDuration");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "name"), "setName", "getName");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "desc"), "setDesc", "getDesc");
}

void Entity::_bind_methods()
{
    UtilityFunctions::print("Binding Entity methods");

    ClassDB::bind_method(D_METHOD("set_index", "index"), &Entity::set_index);
    ClassDB::bind_method(D_METHOD("set_revision", "revision"), &Entity::set_revision);
    ClassDB::bind_method(D_METHOD("get_handle"), &Entity::get_handle);
    ClassDB::bind_method(D_METHOD("get_index"), &Entity::get_index);
    ClassDB::bind_method(D_METHOD("get_revision"), &Entity::get_revision);
    ClassDB::bind_method(D_METHOD("get_hp", "controller"), &Entity::get_hp);
    ClassDB::bind_method(D_METHOD("get_hp_max", "controller"), &Entity::get_hp_max);
    ClassDB::bind_method(D_METHOD("get_pos", "controller"), &Entity::get_pos);
    ClassDB::bind_method(D_METHOD("is_alive", "controller"), &Entity::is_alive);
    ClassDB::bind_method(D_METHOD("is_blueprint", "controller"), &Entity::is_blueprint);
    ClassDB::bind_method(D_METHOD("is_building", "controller"), &Entity::is_building);
    ClassDB::bind_method(D_METHOD("is_built", "controller"), &Entity::is_built);
    ClassDB::bind_method(D_METHOD("is_resource", "controller"), &Entity::is_resource);
    ClassDB::bind_method(D_METHOD("is_unit", "controller"), &Entity::is_unit);
    ClassDB::bind_method(D_METHOD("is_invulnerable", "controller"), &Entity::is_invulnerable);
    ClassDB::bind_method(D_METHOD("is_idle", "controller"), &Entity::is_idle);
    ClassDB::bind_method(D_METHOD("get_ray", "controller"), &Entity::get_ray);
    ClassDB::bind_method(D_METHOD("get_model", "controller"), &Entity::get_model);
    ClassDB::bind_method(D_METHOD("get_player", "controller"), &Entity::get_player);
    ClassDB::bind_method(D_METHOD("get_team", "controller"), &Entity::get_team);
    ClassDB::bind_method(D_METHOD("get_damage", "controller"), &Entity::get_damage);
    ClassDB::bind_method(D_METHOD("get_armor", "controller"), &Entity::get_armor);
    ClassDB::bind_method(D_METHOD("get_speed", "controller"), &Entity::get_speed);
    ClassDB::bind_method(D_METHOD("get_reload", "controller"), &Entity::get_reload);
    ClassDB::bind_method(D_METHOD("get_range", "controller"), &Entity::get_range);
    ClassDB::bind_method(D_METHOD("get_los", "controller"), &Entity::get_los);
    ClassDB::bind_method(D_METHOD("has_rally_point", "controller"), &Entity::has_rally_point);
    ClassDB::bind_method(D_METHOD("get_rally_point", "controller"), &Entity::get_rally_point);
    ClassDB::bind_method(D_METHOD("get_auto_build_model", "controller"), &Entity::get_auto_build_model);
    ClassDB::bind_method(D_METHOD("get_resource_type", "controller"), &Entity::get_resource_type);
    ClassDB::bind_method(D_METHOD("get_resource_quantity", "controller"), &Entity::get_resource_quantity);
    ClassDB::bind_method(D_METHOD("get_buffs", "controller"), &Entity::get_buffs);

    ADD_GROUP("Entity", "Entity_");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "entity_index"), "set_index", "get_index");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "entity_revision"), "set_revision", "get_revision");
}

}
