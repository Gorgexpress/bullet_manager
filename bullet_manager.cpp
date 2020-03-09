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
			if (Engine::get_singleton()->is_editor_hint()) {
				_draw_editor_hint();
			}
			else {
				_draw_bullets();
			}
		} break;

		case NOTIFICATION_PROCESS: {
		} break;
		case NOTIFICATION_PHYSICS_PROCESS: {
			_update_bullets();
		} break; 
	}
}

int BulletManager::add_bullet(StringName type_name, Vector2 position, real_t angle, real_t speed) {
    Physics2DServer *ps = Physics2DServer::get_singleton();
	BulletManagerBulletType* type = types[type_name];
	BulletManagerBullet* bullet;
	if (_unused_ids.size() == 0) {
		bullet = memnew(BulletManagerBullet);
		bullet->id = _bullets.size();
		_bullets.push_back(bullet);
		RID area = ps->area_create();
		ps->area_attach_object_instance_id(area, bullet->get_instance_id());
		ps->area_set_monitor_callback(area, bullet, _body_inout_name);
		ps->area_set_area_monitor_callback(area, bullet, _area_inout_name);
		ps->area_set_transform(area, bullet->matrix);
		ps->area_add_shape(area, type->collision_shape->get_rid());
		if (is_inside_tree()) {
			RID space = get_world_2d()->get_space();
			Physics2DServer::get_singleton()->area_set_space(area, space);
		}
		bullet->area = area;
	} else {
		int id = _unused_ids.front()->get();
		_unused_ids.pop_front();
		bullet = _bullets[id];
	}
	ERR_FAIL_COND_V(_active_bullets.size() >= _bullets.size(), NULL);
	bullet->type = type;
	bullet->set_angle(angle); //the set_angle function also rotates the matrix if rotate_physics is true for the bullet type.
	bullet->speed = speed;
	bullet->matrix.elements[2] = position;
	bullet->is_queued_for_deletion = false;
	ps->area_set_transform(bullet->area, bullet->matrix);
	ps->area_set_collision_layer(bullet->area, type->collision_layer);
	ps->area_set_collision_mask(bullet->area, type->collision_mask);
	ps->area_set_shape_disabled(bullet->area, 0, false);
	_active_bullets.push_back(bullet);
	return bullet->id;
}

void BulletManager::clear()
{
	Physics2DServer *ps = Physics2DServer::get_singleton();
	List<BulletManagerBullet*>::Element *E = _active_bullets.front();
	while(E) {
		BulletManagerBullet* bullet = E->get();
		bullet->queue_delete();
		E = E->next();
	}
}

int BulletManager::count()
{
	return _active_bullets.size();
}

Transform2D BulletManager::get_transform() const {

	return Transform2D();
}

void BulletManager::_edit_set_position(const Point2 &p_position) {
	pos = p_position;
}

Point2 BulletManager::_edit_get_position() const {
	return pos;
}

void BulletManager::_edit_set_scale(const Size2 &p_scale) {
	scale = p_scale;
}

Size2 BulletManager::_edit_get_scale() const {
	return scale;
}

void BulletManager::set_z_index(int z_index) {
	this->z_index = z_index;
	VS::get_singleton()->canvas_item_set_z_index(get_canvas_item(), z_index);
	
}

int BulletManager::get_z_index() const {
	return z_index;
}

void BulletManager::set_bounds_margin(float p_bounds_margin) {
	bounds_margin = p_bounds_margin;
}

float BulletManager::get_bounds_margin() const {
	return bounds_margin;
}



void BulletManager::_draw_bullets() {
	VisualServer* vs = VS::get_singleton();
	//Update cached type info incase any of its properties have been changed.
	for (Map<StringName, BulletManagerBulletType*>::Element *E = types.front(); E; E = E->next()) {
		BulletManagerBulletType* type = E->get();
		type->_update_cached_rects();
		vs->canvas_item_clear(type->get_canvas_item());
	}
	if (_active_bullets.size() == 0) {
		return;
	}
	List<BulletManagerBullet*>::Element *E = _active_bullets.front();
	//for(int i = 0; i < bullets.size(); i++) {
	while(E) {
		//Bullet* bullet = r[i];
		BulletManagerBullet* bullet = E->get();
		BulletManagerBulletType* type = bullet->type;
		RID ci = type->get_canvas_item();
		if (type->rotate_visual) {
			Transform2D tform;
			tform.set_origin(bullet->matrix.get_origin());
			tform.set_rotation_and_scale(bullet->direction.angle() + (Math_PI * -0.5), bullet->matrix.get_scale());
			vs->canvas_item_add_set_transform(ci, tform);
			//draw_set_transform(bullet->matrix.get_origin(), bullet->direction.angle() + (Math_PI * -0.5), bullet->matrix.get_scale());
		}
		else {
			vs->canvas_item_add_set_transform(ci, bullet->matrix);
			//draw_set_transform_matrix(bullet->matrix);
		}
		vs->canvas_item_add_texture_rect_region(ci, type->_cached_dst_rect, type->texture->get_rid(), type->_cached_src_rect);
		//draw_texture_rect_region(type->texture, type->_cached_dst_rect, type->_cached_src_rect, Color(1, 1, 1), false, type->normal_map);
		E = E->next();
	}
	
	
}

//Draw all the bullet types.
void BulletManager::_draw_editor_hint() {
	if (types.empty()) {
		return;
	}
	int offset_y = 0;
	for (int i = 0; i < get_child_count(); i++) {

		Node *child = get_child(i);

		if (!Object::cast_to<BulletManagerBulletType>(child)) {
			continue;
		}
		BulletManagerBulletType* type = Object::cast_to<BulletManagerBulletType>(child);
		_draw_bullet_type(type, offset_y);
	}

}

void BulletManager::_draw_bullet_type(BulletManagerBulletType* type, int &offset_y) {
	if (type->texture.is_null()) {
		return;
	}
	type->_update_cached_rects();
	offset_y -= MIN(type->_cached_dst_rect.position.y, 0);
	draw_set_transform(Point2(0, offset_y), 0, Size2(1, 1));
	draw_texture_rect_region(type->texture, type->_cached_dst_rect, type->_cached_src_rect, Color(1, 1, 1), false);
	real_t start_x = type->_cached_dst_rect.position.x;
	real_t start_y = type->_cached_dst_rect.position.y;
	real_t half_x = type->_cached_dst_rect.position.x + type->_cached_dst_rect.size.x / 2.0;
	real_t half_y = type->_cached_dst_rect.position.y + type->_cached_dst_rect.size.y / 2.0;
	real_t end_x = type->_cached_dst_rect.position.x + type->_cached_dst_rect.size.x;
	real_t end_y = type->_cached_dst_rect.position.y + type->_cached_dst_rect.size.y;
	Color debug_color = Color(0.0, 0.0, 0.0, 0.25);
	draw_line(Point2(start_x, half_y), Point2(end_x, half_y), debug_color);
	draw_line(Point2(half_x, start_y), Point2(half_x, end_y), debug_color);
	draw_rect(type->_cached_dst_rect, debug_color, false);
	offset_y += type->_cached_dst_rect.size.y + 2 + 1;
}

void BulletManager::_update_bullets() {
	
	Physics2DServer *ps = Physics2DServer::get_singleton();
	float delta = get_physics_process_delta_time();
	int size = _active_bullets.size();
	Rect2 visible_rect;
	_get_visible_rect(visible_rect);
	visible_rect = visible_rect.grow(bounds_margin);
	{
		List<BulletManagerBullet*>::Element *E = _active_bullets.front();
		while(E) {
			BulletManagerBullet* bullet = E->get();
			if(bullet->is_queued_for_deletion) {
				ps->area_set_shape_disabled(bullet->area, 0, true);
				_unused_ids.push_front(bullet->id);
				E->erase();
			} else if (!visible_rect.has_point(bullet->matrix.get_origin())) {
				bullet->type->emit_signal("bullet_clipped", bullet->id);
				bullet->is_queued_for_deletion = true;
				_unused_ids.push_front(bullet->id);
				ps->area_set_collision_layer(bullet->area, 0);
				ps->area_set_collision_mask(bullet->area, 0);
				ps->area_set_shape_disabled(bullet->area, 0, true);
				E->erase();
			}
			else {
				bullet->matrix[2] += bullet->direction * bullet->speed  * delta;
				ps->area_set_transform(bullet->area, bullet->matrix);
			}
			E = E->next();
		}
	}
	update();
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
		BulletManagerBulletType* typeNode = Object::cast_to<BulletManagerBulletType>(child);
		typeNode->set_as_toplevel(true);
		types[name] = typeNode;
		
	}
}

void BulletManager::_get_visible_rect(Rect2& rect)
{
	Transform2D ctrans = get_viewport()->get_canvas_transform();
	rect.position = -ctrans.get_origin() / ctrans.get_scale();
	rect.size = get_viewport_rect().size;
}

void BulletManager::update_bullet_position(int bullet_id, Vector2 position) {
	if (bullet_id < _bullets.size()) {
		_bullets[bullet_id]->matrix.set_origin(position);
	}
}

Vector2 BulletManager::get_bullet_position(int bullet_id) const {
	if (bullet_id < _bullets.size()) {
		return _bullets[bullet_id]->matrix.get_origin();
	}
	return Vector2();
}

void BulletManager::update_bullet_speed(int bullet_id, real_t speed) {
	if (bullet_id < _bullets.size()) {
		_bullets[bullet_id]->speed = speed;
	}
}

real_t BulletManager::get_bullet_speed(int bullet_id) const {
	if (bullet_id < _bullets.size()) {
		return _bullets[bullet_id]->speed;
	}
	return 0.0;
}

void BulletManager::update_bullet_angle(int bullet_id, real_t angle) {
	if (bullet_id < _bullets.size()) {
		_bullets[bullet_id]->set_angle(angle);
	}
}

real_t BulletManager::get_bullet_angle(int bullet_id) const {
	if (bullet_id < _bullets.size()) {
		return _bullets[bullet_id]->direction.angle();
	}
	return 0.0;
}

void BulletManager::queue_delete_bullet(int bullet_id) {
	if (bullet_id < _bullets.size()) {
		BulletManagerBullet* bullet = _bullets[bullet_id];
		bullet->is_queued_for_deletion = true;
		Physics2DServer::get_singleton()->area_set_collision_layer(bullet->area, 0);
    	Physics2DServer::get_singleton()->area_set_collision_mask(bullet->area, 0);
	}
}

void BulletManager::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add_bullet", "position", "angle","speed"), &BulletManager::add_bullet);
	ClassDB::bind_method(D_METHOD("update_bullet_position", "bullet_id", "position"), &BulletManager::update_bullet_position);
	ClassDB::bind_method(D_METHOD("get_bullet_position", "bullet_id"), &BulletManager::get_bullet_position);
	ClassDB::bind_method(D_METHOD("update_bullet_speed", "bullet_id", "speed"), &BulletManager::update_bullet_speed);
	ClassDB::bind_method(D_METHOD("get_bullet_speed", "bullet_id"), &BulletManager::get_bullet_speed);
	ClassDB::bind_method(D_METHOD("update_bullet_angle", "bullet_id", "angle"), &BulletManager::update_bullet_angle);
	ClassDB::bind_method(D_METHOD("get_bullet_angle", "bullet_id"), &BulletManager::get_bullet_angle);
	ClassDB::bind_method(D_METHOD("queue_delete_bullet", "bullet_id"), &BulletManager::queue_delete_bullet);
	ClassDB::bind_method(D_METHOD("clear"), &BulletManager::clear);

	ClassDB::bind_method(D_METHOD("set_z_index", "z_index"), &BulletManager::set_z_index);
	ClassDB::bind_method(D_METHOD("get_z_index"), &BulletManager::get_z_index);
	ClassDB::bind_method(D_METHOD("set_bounds_margin", "bounds_margin"), &BulletManager::set_bounds_margin);
	ClassDB::bind_method(D_METHOD("get_bounds_margin"), &BulletManager::get_bounds_margin);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "z_index", PROPERTY_HINT_RANGE, itos(VS::CANVAS_ITEM_Z_MIN) + "," + itos(VS::CANVAS_ITEM_Z_MAX) + ",1"), "set_z_index", "get_z_index");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "bounds_margin", PROPERTY_HINT_NONE), "set_bounds_margin", "get_bounds_margin");
}
