#include "modules/bullet_manager/bullet_manager_bullet.h"
#include "scene/main/scene_tree.h"
#include "scene/main/viewport.h"

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
    ClassDB::bind_method(D_METHOD("queue_delete"), &BulletManagerBullet::queue_delete);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "global_position", PROPERTY_HINT_NONE), "set_global_position", "get_global_position");

}