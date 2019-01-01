#include "bullet_manager.h"
#include "core/os/os.h"
#include "scene/2d/area_2d.h"
#include "scene/2d/collision_shape_2d.h"
#include "scene/scene_string_names.h"
#include "scene/main/scene_tree.h"
#include "scene/main/viewport.h"

void BulletManager::_notification(int p_what) {

	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {

		} break;

		case NOTIFICATION_READY: {
			set_physics_process(true);
			_register_bullet_types();
			set_as_toplevel(true);
			VS::get_singleton()->canvas_item_set_z_index(get_canvas_item(), z_index);
		} break;
		case NOTIFICATION_DRAW: {
			_draw_bullets();
		} break;

		case NOTIFICATION_PROCESS: {
		} break;
		case NOTIFICATION_PHYSICS_PROCESS: {
			_update_bullets();
		} break; 
	}
}
void BulletManager::_bind_methods() {


	
	ClassDB::bind_method(D_METHOD("add_bullet", "position", "direction","speed", "acceleration", "rotation"), &BulletManager::add_bullet);
	ClassDB::bind_method(D_METHOD("clear"), &BulletManager::clear);

	ClassDB::bind_method(D_METHOD("set_z_index", "z_index"), &BulletManager::set_z_index);
	ClassDB::bind_method(D_METHOD("get_z_index"), &BulletManager::get_z_index);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "z_index", PROPERTY_HINT_RANGE, itos(VS::CANVAS_ITEM_Z_MIN) + "," + itos(VS::CANVAS_ITEM_Z_MAX) + ",1"), "set_z_index", "get_z_index");
}


void BulletManager::_update_bullets() {
	
	Physics2DServer *ps = Physics2DServer::get_singleton();
	float delta = get_physics_process_delta_time();
	int size = bullets.size();
	Rect2 visible_rect;
	_get_visible_rect(visible_rect);
	visible_rect.grow(300);
	{
		//PoolVector<Bullet*>::Read r = bullets.read();
		//PoolVector<Bullet*>::Write w = bullets.write();
		//int i = 0;
		List<BulletManagerBullet*>::Element *E = bullets.front();
		//while (i < size) {
		while(E) {
			BulletManagerBullet* bullet = E->get();
			if(bullet->is_queued_for_deletion || !visible_rect.has_point(bullet->matrix.get_origin())) {
				ps->free(bullet->area);
				//w[i] = r[size - 1];
				E->erase();
				memdelete(bullet);
				//size -= 1;
			}
			else {
				bullet->matrix[2] += bullet->direction * bullet->speed  * delta;
				bullet->speed += bullet->acceleration * delta;
				ps->area_set_transform(bullet->area, bullet->matrix);
			}
			//i += 1;
			E = E->next();
		}
	}
	//if (size != bullets.size()) {
	//	bullets.resize(size);
	//}
	update();
}

void BulletManager::_draw_bullets() {
	if (bullets.size() == 0)
		return;
	//Update cached type info incase any of its properties have been changed.
	for (Map<StringName, BulletManagerBulletType*>::Element *E = types.front(); E; E = E->next()) {
		E->get()->_update_cached_rects();
	}
	RID ci = get_canvas_item();

	List<BulletManagerBullet*>::Element *E = bullets.front();
	//for(int i = 0; i < bullets.size(); i++) {
	while(E) {
		//Bullet* bullet = r[i];
		BulletManagerBullet* bullet = E->get();
		BulletManagerBulletType* type = bullet->type;
		if (type->is_rotating_visual) {
			draw_set_transform(bullet->matrix.get_origin(), bullet->direction.angle() + (Math_PI * -0.5), bullet->matrix.get_scale());
			
		}
		else {
			draw_set_transform_matrix(bullet->matrix);
		}
		draw_texture_rect_region(type->texture, type->_cached_dst_rect, type->_cached_src_rect, Color(1, 1, 1), false);
		E = E->next();
	}
	
	
}
void BulletManager::add_bullet(StringName type_name, Vector2 position, Vector2 direction,real_t speed, real_t acceleration) {
    BulletManagerBullet* bullet(memnew(BulletManagerBullet));

	BulletManagerBulletType* type = types[type_name];
	bullet->direction = direction;
	bullet->speed = speed;
	bullet->acceleration = acceleration;
	bullet->matrix.elements[2] = position;
	bullet->type = type;
	Physics2DServer *ps = Physics2DServer::get_singleton();
	RID area = ps->area_create();
	ps->area_attach_object_instance_id(area, bullet->get_instance_id());
	ps->area_set_collision_layer(area, type->collision_layer);
	ps->area_set_collision_mask(area, type->collision_mask);
	ps->area_set_monitor_callback(area, bullet, _body_inout_name);
	ps->area_set_area_monitor_callback(area, bullet, _area_inout_name);
	ps->area_set_transform(area, bullet->matrix);
	ps->area_add_shape(area, type->collision_shape->get_rid());
	

	if (is_inside_tree()) {
		RID space = get_world_2d()->get_space();
		Physics2DServer::get_singleton()->area_set_space(area, space);
	}
	bullet->area = area;
	bullets.push_back(bullet);
}

void BulletManager::_register_bullet_types() {
	types = Map<StringName, BulletManagerBulletType*>();
	for (int i = 0; i < get_child_count(); i++) {

		Node *child = get_child(i);

		if (!Object::cast_to<BulletManagerBulletType>(child)) {
			continue;
		}
		StringName name = child->get_name();
		if (types.has(name)) {
			print_error("Duplicate type " + name + " in BulletManager named " + get_name());
		}
		types[name] = Object::cast_to<BulletManagerBulletType>(child);
	}
}

void BulletManager::_get_visible_rect(Rect2& rect)
{
	Transform2D ctrans = get_canvas_transform();
	rect.position = -ctrans.get_origin() / ctrans.get_scale();
	rect.size = get_viewport_rect().size;
}

void BulletManager::clear()
{
	Physics2DServer *ps = Physics2DServer::get_singleton();
	List<BulletManagerBullet*>::Element *E = bullets.front();
	//for(int i = 0; i < bullets.size(); i++) {
	while(E) {
		BulletManagerBullet* bullet = E->get();
		bullet->queue_delete();
		/*ps->free(bullet->area);
		memdelete(bullet);*/
		E = E->next();
	}
	//bullets.clear();
}

int BulletManager::count()
{
	return bullets.size();
}

Transform2D BulletManager::get_transform() const {

	return Transform2D();
}

void BulletManager::set_z_index(int z_index) {
	this->z_index = z_index;
	VS::get_singleton()->canvas_item_set_z_index(get_canvas_item(), 100);
	
}

int BulletManager::get_z_index() const {
	return z_index;
}