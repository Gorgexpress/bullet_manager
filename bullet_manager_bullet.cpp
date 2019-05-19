#include "modules/bullet_manager/bullet_manager_bullet.h"
#include "scene/main/scene_tree.h"
#include "scene/main/viewport.h"
#include "core/message_queue.h"
#include "scene/gui/control.h"
#include "scene/main/viewport.h"
#include "servers/visual_server.h"

void BulletManagerBullet::queue_delete() {
    is_queued_for_deletion = true;
    Physics2DServer::get_singleton()->area_set_collision_layer(area, 0);
    Physics2DServer::get_singleton()->area_set_collision_mask(area, 0);
}
void BulletManagerBullet::_area_inout(int p_status, const RID &p_area, int p_instance, int p_area_shape, int p_self_shape) {
	if (is_queued_for_deletion) {
        return;
    }
	type->area_inout(this, p_status, p_area, p_instance, p_area_shape, p_self_shape );
}
void BulletManagerBullet::_body_inout(int p_status, const RID &p_body, int p_instance, int p_body_shape, int p_area_shape) {
    if (is_queued_for_deletion) {
        return;
    }
	type->body_inout(this, p_status, p_body, p_instance, p_body_shape, p_area_shape );
}

void BulletManagerBullet::set_position(Point2 position) {
	matrix.set_origin(position);
}

Point2 BulletManagerBullet::get_position() const {
	return matrix.get_origin();
}



void BulletManagerBullet::set_direction(Vector2 direction) {
    this->direction = direction;
     if(this->type->is_rotating_physics()) {
          this->matrix.set_rotation(direction.angle());
     }
}

Vector2 BulletManagerBullet::get_direction() const {
    return direction;
}

void BulletManagerBullet::set_angle(real_t angle) {
    real_t rads = Math::deg2rad(angle);
    this->direction =  Vector2(cos(rads), sin(rads));
    if(this->type->is_rotating_physics()) {
        this->matrix.set_rotation(rads);
    }
}

real_t BulletManagerBullet::get_angle() const {
    return Math::rad2deg(direction.angle());
}



void BulletManagerBullet::set_speed(real_t speed) {
    this->speed = speed;
}

real_t BulletManagerBullet::get_speed() const {
    return speed;
}

void BulletManagerBullet::set_type(Node* bullet_manager_bullet_type) {
    //Setting the type to one belonging to a different BulletManager isn't supported, and I don't see a good reason to support it.
    ERR_FAIL_COND(this->type->get_parent() != type->get_parent());
    BulletManagerBulletType* type = Object::cast_to<BulletManagerBulletType>(bullet_manager_bullet_type);
    Physics2DServer* ps = Physics2DServer::get_singleton();
    if (!is_queued_for_deletion) {
        //Don't want to undo the layer and mask being set to 0 when scheduled for deletion
        //Honestly, returning immediately if the bullet is queued for deletion might be a better idea. 
        ps->area_set_collision_layer(area, type->get_collision_layer());
	    ps->area_set_collision_mask(area, type->get_collision_mask());
    }
	
    ps->area_clear_shapes(area);
    ps->area_add_shape(area, type->get_collision_shape()->get_rid());
    this->type = type;
}

Node* BulletManagerBullet::get_type() const  {
    return type;
}

void BulletManagerBullet::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_body_inout"), &BulletManagerBullet::_body_inout);
	ClassDB::bind_method(D_METHOD("_area_inout"), &BulletManagerBullet::_area_inout);
    ClassDB::bind_method(D_METHOD("set_position"), &BulletManagerBullet::set_position);
	ClassDB::bind_method(D_METHOD("get_position"), &BulletManagerBullet::get_position);
	ClassDB::bind_method(D_METHOD("set_direction", "direction"), &BulletManagerBullet::set_direction);
	ClassDB::bind_method(D_METHOD("get_direction"), &BulletManagerBullet::get_direction);
    ClassDB::bind_method(D_METHOD("set_angle", "angle"), &BulletManagerBullet::set_angle);
	ClassDB::bind_method(D_METHOD("get_angle"), &BulletManagerBullet::get_angle);
    ClassDB::bind_method(D_METHOD("set_speed", "speed"), &BulletManagerBullet::set_speed);
	ClassDB::bind_method(D_METHOD("get_speed"), &BulletManagerBullet::get_speed);
    ClassDB::bind_method(D_METHOD("set_type", "bullet_manager_bullet_type"), &BulletManagerBullet::set_type);
	ClassDB::bind_method(D_METHOD("get_type"), &BulletManagerBullet::get_type);
    ClassDB::bind_method(D_METHOD("queue_delete"), &BulletManagerBullet::queue_delete);

    //I'm assuming I don't need property hints for this...
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "position", PROPERTY_HINT_NONE), "set_position", "get_position");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "direction", PROPERTY_HINT_NONE), "set_direction", "get_direction");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "angle", PROPERTY_HINT_NONE), "set_angle", "get_angle");
    ADD_PROPERTY(PropertyInfo(Variant::REAL, "speed", PROPERTY_HINT_NONE), "set_speed", "get_speed");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "type", PROPERTY_HINT_NONE), "set_type", "get_type");

}