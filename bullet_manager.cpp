#include "bullet_manager.h"
#include "core/os/os.h"
#include "scene/2d/area_2d.h"
#include "scene/2d/collision_shape_2d.h"
#include "scene/scene_string_names.h"
#include "scene/main/scene_tree.h"
#include "scene/main/viewport.h"

void BulletManagerBullet::queue_delete() {
    is_queued_for_deletion = true;
    Physics2DServer::get_singleton()->area_set_collision_layer(area, 0);
    Physics2DServer::get_singleton()->area_set_collision_mask(area, 0);
    //Physics2DServer::get_singleton()->area_set_shape_disabled(area, 0, true);
}
void BulletManagerBullet::_area_inout(int p_status, const RID &p_area, int p_instance, int p_area_shape, int p_self_shape) {
	if (is_queued_for_deletion) {
        return;
    }
	type->area_inout(id, p_status, p_area, p_instance, p_area_shape, p_self_shape );
}
void BulletManagerBullet::_body_inout(int p_status, const RID &p_body, int p_instance, int p_body_shape, int p_area_shape) {
    if (is_queued_for_deletion) {
        return;
    }
	type->body_inout(id, p_status, p_body, p_instance, p_body_shape, p_area_shape );
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

void BulletManagerBulletType::set_texture(const Ref<Texture> &p_texture) {

	if (p_texture == texture)
		return;

	if (texture.is_valid())
		texture->remove_change_receptor(this);

	texture = p_texture;

	if (texture.is_valid())
		texture->add_change_receptor(this);
	/*
	update();
	emit_signal("texture_changed");
	item_rect_changed();*/
	_change_notify("texture");
}

Ref<Texture> BulletManagerBulletType::get_texture() const {

	return texture;
}

void BulletManagerBulletType::set_normal_map(const Ref<Texture> &p_normal_map) {

	if (p_normal_map == normal_map)
		return;

	if (normal_map.is_valid())
		texture->remove_change_receptor(this);

	normal_map = p_normal_map;

	if (normal_map.is_valid())
		normal_map->add_change_receptor(this);
	/*
	update();
	emit_signal("texture_changed");
	item_rect_changed();*/
	_change_notify("normal_map");
}

Ref<Texture> BulletManagerBulletType::get_normal_map() const {

	return normal_map;
}

void BulletManagerBulletType::set_vframes(int p_amount) {

	ERR_FAIL_COND(p_amount < 1);
	vframes = p_amount;
	_change_notify();
}
int BulletManagerBulletType::get_vframes() const {

	return vframes;
}

void BulletManagerBulletType::set_hframes(int p_amount) {

	ERR_FAIL_COND(p_amount < 1);
	hframes = p_amount;
	_change_notify();
}
int BulletManagerBulletType::get_hframes() const {

	return hframes;
}

void BulletManagerBulletType::set_frame(int p_frame) {

	ERR_FAIL_INDEX(p_frame, vframes * hframes);


	frame = p_frame;

	_change_notify("frame");

}

int BulletManagerBulletType::get_frame() const {

	return frame;
}

void BulletManagerBulletType::set_centered(bool p_center) {
	centered = p_center;
	_change_notify();
}

bool BulletManagerBulletType::is_centered() const {
	return centered;
}

void BulletManagerBulletType::set_offset(const Point2 &p_offset) {
	offset = p_offset;
}

Point2 BulletManagerBulletType::get_offset() const {
	return offset;
}

void BulletManagerBulletType::set_region(bool p_region) {
	region = p_region;
	_change_notify();
}

bool BulletManagerBulletType::is_region() const {
	return region;
}

void BulletManagerBulletType::set_region_rect(const Rect2 &p_region_rec) {
	region_rect = p_region_rec;
	_change_notify();
}

Rect2 BulletManagerBulletType::get_region_rect() const {
	return region_rect;
}

void BulletManagerBulletType::set_rotate_visual(bool p_rotate_visual) {

	rotate_visual = p_rotate_visual;
	_change_notify();
}

bool BulletManagerBulletType::is_rotating_visual() const {

	return rotate_visual;
}

void BulletManagerBulletType::set_collision_shape(const Ref<Shape2D> &p_shape) {
	collision_shape = p_shape;
	_change_notify();
}

Ref<Shape2D> BulletManagerBulletType::get_collision_shape() const {

	return collision_shape;
}

void BulletManagerBulletType::set_collision_mask(uint32_t p_mask) {

	collision_mask = p_mask;
	//Physics2DServer::get_singleton()->area_set_collision_mask(get_rid(), p_mask);
}

uint32_t BulletManagerBulletType::get_collision_mask() const {

	return collision_mask;
}

void BulletManagerBulletType::set_collision_layer(uint32_t p_layer) {

	collision_layer = p_layer;
	//Physics2DServer::get_singleton()->area_set_collision_layer(get_rid(), p_layer);
}

uint32_t BulletManagerBulletType::get_collision_layer() const {

	return collision_layer;
}

void BulletManagerBulletType::set_rotate_physics(bool p_rotate_physics) {

	rotate_physics = p_rotate_physics;
}

bool BulletManagerBulletType::is_rotating_physics() const {

	return rotate_physics;
}

void BulletManagerBulletType::_update_cached_rects()  {

	Rect2 base_rect;

	if (region) {
		//r_filter_clip = region_filter_clip;
		base_rect = region_rect;
	} else {
		//r_filter_clip = false;
		base_rect = Rect2(0, 0, texture->get_width(), texture->get_height());
	}
	Size2 frame_size = base_rect.size / Size2(hframes, vframes);
	Point2 frame_offset = Point2(frame % hframes, frame / hframes);
	frame_offset *= frame_size;

	_cached_src_rect.size = frame_size;
	_cached_src_rect.position = base_rect.position + frame_offset;

	Point2 dest_offset = offset;
	if (centered)
		dest_offset -= frame_size / 2;
	if (Engine::get_singleton()->get_use_pixel_snap()) {
		dest_offset = dest_offset.floor();
	}

	_cached_dst_rect = Rect2(dest_offset, frame_size);

	/*if (hflip)
		r_dst_rect.size.x = -r_dst_rect.size.x;
	if (vflip)
		r_dst_rect.size.y = -r_dst_rect.size.y;*/
}

void BulletManagerBulletType::area_inout(int bullet_id, int p_status, const RID &p_area, int p_instance, int p_area_shape, int p_self_shape) {
	Object* collider = ObjectDB::get_instance(p_instance);
	emit_signal("area_entered_bullet", bullet_id, collider);
}
void BulletManagerBulletType::body_inout(int bullet_id, int p_status, const RID &p_body, int p_instance, int p_body_shape, int p_area_shape) {
	Object* collider = ObjectDB::get_instance(p_instance);
	emit_signal("body_entered_bullet", bullet_id, collider);
}

void BulletManagerBulletType::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_PARENTED: {
			_bullet_manager = Object::cast_to<BulletManager>(get_parent());
			if (!_bullet_manager) {
				return;
			}
			_bullet_manager->register_bullet_type(this);
		} break;
		case NOTIFICATION_UNPARENTED: {
			if(!_bullet_manager) {
				return;
			}
			_bullet_manager->unregister_bullet_type(this);
			_bullet_manager = NULL;
		} break;
		case NOTIFICATION_DRAW: { 
			if (!Engine::get_singleton()->is_editor_hint()) {
				return;	
			}
			if (texture != NULL) {
				_update_cached_rects();
				texture->draw_rect_region(get_canvas_item(), _cached_dst_rect, _cached_src_rect, Color(1, 1, 1), false);
			}
			if (collision_shape != NULL && collision_shape.is_valid()) {
				collision_shape->draw(get_canvas_item(), get_tree()->get_debug_collisions_color());
			}
		} break;
		case NOTIFICATION_READY: {
			if (!Engine::get_singleton()->is_editor_hint()) {
				set_transform(Transform2D()); //don't want bullets to be drawn at an offset
			}
		} break;
	}
}
void BulletManagerBulletType::_bind_methods() {
	//VISUAL
	ClassDB::bind_method(D_METHOD("set_texture", "texture"), &BulletManagerBulletType::set_texture);
	ClassDB::bind_method(D_METHOD("get_texture"), &BulletManagerBulletType::get_texture);
	ClassDB::bind_method(D_METHOD("set_normal_map", "normal_map"), &BulletManagerBulletType::set_normal_map);
	ClassDB::bind_method(D_METHOD("get_normal_map"), &BulletManagerBulletType::get_normal_map);
	ClassDB::bind_method(D_METHOD("set_vframes", "vframes"), &BulletManagerBulletType::set_vframes);
	ClassDB::bind_method(D_METHOD("get_vframes"), &BulletManagerBulletType::get_vframes);
	ClassDB::bind_method(D_METHOD("set_hframes", "hframes"), &BulletManagerBulletType::set_hframes);
	ClassDB::bind_method(D_METHOD("get_hframes"), &BulletManagerBulletType::get_hframes);
	ClassDB::bind_method(D_METHOD("set_frame", "frame"), &BulletManagerBulletType::set_frame);
	ClassDB::bind_method(D_METHOD("get_frame"), &BulletManagerBulletType::get_frame);
	ClassDB::bind_method(D_METHOD("set_centered", "centered"), &BulletManagerBulletType::set_centered);
	ClassDB::bind_method(D_METHOD("is_centered"), &BulletManagerBulletType::is_centered);
	ClassDB::bind_method(D_METHOD("set_offset", "offset"), &BulletManagerBulletType::set_offset);
	ClassDB::bind_method(D_METHOD("get_offset"), &BulletManagerBulletType::get_offset);
	ClassDB::bind_method(D_METHOD("set_region", "enabled"), &BulletManagerBulletType::set_region);
	ClassDB::bind_method(D_METHOD("is_region"), &BulletManagerBulletType::is_region);
	ClassDB::bind_method(D_METHOD("set_region_rect", "rect"), &BulletManagerBulletType::set_region_rect);
	ClassDB::bind_method(D_METHOD("get_region_rect"), &BulletManagerBulletType::get_region_rect);
	ClassDB::bind_method(D_METHOD("set_rotate_visual", "enabled"), &BulletManagerBulletType::set_rotate_visual);
	ClassDB::bind_method(D_METHOD("is_rotating_visual"), &BulletManagerBulletType::is_rotating_visual);
	/*ClassDB::bind_method(D_METHOD("set_region", "enabled"), &Sprite::set_region);
	ClassDB::bind_method(D_METHOD("is_region"), &Sprite::is_region);
	ClassDB::bind_method(D_METHOD("set_region_rect", "rect"), &Sprite::set_region_rect);
	ClassDB::bind_method(D_METHOD("get_region_rect"), &Sprite::get_region_rect);
	ClassDB::bind_method(D_METHOD("set_region_filter_clip", "enabled"), &Sprite::set_region_filter_clip);
	ClassDB::bind_method(D_METHOD("is_region_filter_clip_enabled"), &Sprite::is_region_filter_clip_enabled);*/
	
	//PHYSICS
	ClassDB::bind_method(D_METHOD("set_collision_shape", "collision_shape"), &BulletManagerBulletType::set_collision_shape);
	ClassDB::bind_method(D_METHOD("get_collision_shape"), &BulletManagerBulletType::get_collision_shape);
	ClassDB::bind_method(D_METHOD("set_collision_layer", "collision_layer"), &BulletManagerBulletType::set_collision_layer);
	ClassDB::bind_method(D_METHOD("get_collision_layer"), &BulletManagerBulletType::get_collision_layer);
	ClassDB::bind_method(D_METHOD("set_collision_mask", "collision_mask"), &BulletManagerBulletType::set_collision_mask);
	ClassDB::bind_method(D_METHOD("get_collision_mask"), &BulletManagerBulletType::get_collision_mask);
	ClassDB::bind_method(D_METHOD("set_rotate_physics", "enabled"), &BulletManagerBulletType::set_rotate_physics);
	ClassDB::bind_method(D_METHOD("is_rotating_physics"), &BulletManagerBulletType::is_rotating_physics);
	
	//VISUAL PROPERTIES
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), "set_texture", "get_texture");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "normal_map", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), "set_normal_map", "get_normal_map");
	ADD_GROUP("Offset", "");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "centered"), "set_centered", "is_centered");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "offset"), "set_offset", "get_offset");
	ADD_GROUP("Animation", "");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "vframes", PROPERTY_HINT_RANGE, "1,16384,1"), "set_vframes", "get_vframes");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "hframes", PROPERTY_HINT_RANGE, "1,16384,1"), "set_hframes", "get_hframes");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "frame", PROPERTY_HINT_SPRITE_FRAME), "set_frame", "get_frame");
	ADD_GROUP("Region", "region_");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "region_enabled"), "set_region", "is_region");
	ADD_PROPERTY(PropertyInfo(Variant::RECT2, "region_rect"), "set_region_rect", "get_region_rect");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "rotate_visual"), "set_rotate_visual", "is_rotating_visual");

	//PHYSICS PROPERTIES
	ADD_GROUP("Collision", "collision_");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "collision_shape", PROPERTY_HINT_RESOURCE_TYPE, "Shape2D"), "set_collision_shape", "get_collision_shape");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_layer", PROPERTY_HINT_LAYERS_2D_PHYSICS), "set_collision_layer", "get_collision_layer");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_mask", PROPERTY_HINT_LAYERS_2D_PHYSICS), "set_collision_mask", "get_collision_mask");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "rotate_physics"), "set_rotate_physics", "is_rotating_physics");
	

	ADD_SIGNAL(MethodInfo("area_entered_bullet", PropertyInfo(Variant::INT, "bullet_id", PROPERTY_HINT_NONE, "bullet_id"), PropertyInfo(Variant::OBJECT, "area", PROPERTY_HINT_RESOURCE_TYPE, "Area2D")));
	ADD_SIGNAL(MethodInfo("body_entered_bullet", PropertyInfo(Variant::INT, "bullet_id", PROPERTY_HINT_NONE, "bullet_id"), PropertyInfo(Variant::OBJECT, "body", PROPERTY_HINT_RESOURCE_TYPE, "PhysicsBody2D")));
	ADD_SIGNAL(MethodInfo("bullet_clipped", PropertyInfo(Variant::INT, "bullet_id", PROPERTY_HINT_NONE, "bullet_id")));

}

void BulletManager::_notification(int p_what) {

	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {

		} break;

		case NOTIFICATION_READY: {
			set_physics_process(true);
			//set_as_toplevel(true);
			//VS::get_singleton()->canvas_item_set_z_index(get_canvas_item(), z_index);
		} break;
		case NOTIFICATION_DRAW: {
			if (Engine::get_singleton()->is_editor_hint()) {
				return;
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

//Should only be used on direct children. BulletManagerBulletTypes will call this on themselves when parented to a BulletManager.
//Godot doesn't allow duplicate names among neighbors, so i won't check for duplicates here.
void BulletManager::register_bullet_type(BulletManagerBulletType* type) {
	types[type->get_name()] = type;
}

void BulletManager::unregister_bullet_type(BulletManagerBulletType* type) {
	types.erase(type->get_name());
}

void BulletManager::_get_visible_rect(Rect2& rect)
{
	Transform2D ctrans = get_viewport()->get_canvas_transform();
	rect.position = -ctrans.get_origin() / ctrans.get_scale();
	rect.size = get_viewport_rect().size;
}

void BulletManager::set_bullet_position(int bullet_id, Vector2 position) {
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

void BulletManager::set_bullet_speed(int bullet_id, real_t speed) {
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

void BulletManager::set_bullet_angle(int bullet_id, real_t angle) {
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
	ClassDB::bind_method(D_METHOD("set_bullet_position", "bullet_id", "position"), &BulletManager::set_bullet_position);
	ClassDB::bind_method(D_METHOD("get_bullet_position", "bullet_id"), &BulletManager::get_bullet_position);
	ClassDB::bind_method(D_METHOD("set_bullet_speed", "bullet_id", "speed"), &BulletManager::set_bullet_speed);
	ClassDB::bind_method(D_METHOD("get_bullet_speed", "bullet_id"), &BulletManager::get_bullet_speed);
	ClassDB::bind_method(D_METHOD("set_bullet_angle", "bullet_id", "angle"), &BulletManager::set_bullet_angle);
	ClassDB::bind_method(D_METHOD("get_bullet_angle", "bullet_id"), &BulletManager::get_bullet_angle);
	ClassDB::bind_method(D_METHOD("queue_delete_bullet", "bullet_id"), &BulletManager::queue_delete_bullet);
	ClassDB::bind_method(D_METHOD("clear"), &BulletManager::clear);

	ClassDB::bind_method(D_METHOD("set_bounds_margin", "bounds_margin"), &BulletManager::set_bounds_margin);
	ClassDB::bind_method(D_METHOD("get_bounds_margin"), &BulletManager::get_bounds_margin);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "bounds_margin", PROPERTY_HINT_NONE), "set_bounds_margin", "get_bounds_margin");
}
