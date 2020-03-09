#include "modules/bullet_manager/bullet_manager_bullet_type.h"
#include "core/os/os.h"

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
}

bool BulletManagerBulletType::is_region() const {
	return region;
}

void BulletManagerBulletType::set_region_rect(const Rect2 &p_region_rec) {
	region_rect = p_region_rec;
}

Rect2 BulletManagerBulletType::get_region_rect() const {
	return region_rect;
}

void BulletManagerBulletType::set_rotate_visual(bool p_rotate_visual) {

	rotate_visual = p_rotate_visual;
}

bool BulletManagerBulletType::is_rotating_visual() const {

	return rotate_visual;
}

void BulletManagerBulletType::set_collision_shape(const Ref<Shape2D> &p_shape) {

	if (collision_shape.is_valid())
		collision_shape->disconnect("changed", this, "_shape_changed");
	collision_shape = p_shape;

	if (collision_shape.is_valid())
		collision_shape->connect("changed", this, "_shape_changed");

	///update_configuration_warning();
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