#include "modules/bullet_manager/bullet_manager_bullet.h"
#include "scene/main/scene_tree.h"
#include "scene/main/viewport.h"

void BulletManagerBullet::_area_inout(int p_status, const RID &p_area, int p_instance, int p_area_shape, int p_self_shape) {
	has_collided = true;
	Physics2DServer::get_singleton()->area_set_collision_mask(area, 0);
	Physics2DServer::get_singleton()->area_set_collision_layer(area, 0);
	Object* collider = ObjectDB::get_instance(p_instance);
	collider->get_script_instance()->call("take_damage", this, 1);
}
void BulletManagerBullet::_body_inout(int p_status, const RID &p_body, int p_instance, int p_body_shape, int p_area_shape) {
	has_collided = true;
	Physics2DServer::get_singleton()->area_set_collision_mask(area, 0);
	Physics2DServer::get_singleton()->area_set_collision_layer(area, 0);
	Object* collider = ObjectDB::get_instance(p_instance);
	Node* global = SceneTree::get_singleton()->get_root()->get_node(NodePath("Global"));
	Vector2 pos = global->get("player").get("global_position");
	int damage = 4;
	if (pos.distance_to(this->matrix.get_origin()) < 96) {
		damage = 5;
	}
	if (collider->has_method("take_damage")) {
		collider->get_script_instance()->call("take_damage", damage);
	}
}

Point2 BulletManagerBullet::get_global_position() {
	return matrix.get_origin();
}

void BulletManagerBullet::set_global_position(Point2 position) {
	matrix.set_origin(position);
}

void BulletManagerBullet::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_body_inout"), &BulletManagerBullet::_body_inout);
	ClassDB::bind_method(D_METHOD("_area_inout"), &BulletManagerBullet::_area_inout);
	ClassDB::bind_method(D_METHOD("set_global_position"), &BulletManagerBullet::set_global_position);
	ClassDB::bind_method(D_METHOD("get_global_position"), &BulletManagerBullet::get_global_position);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "global_position", PROPERTY_HINT_NONE), "set_global_position", "get_global_position");

}