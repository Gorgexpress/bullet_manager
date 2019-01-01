#include "bullet_manager_bullet_type.h"

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

void BulletManagerBulletType::set_is_rotating_visual(bool is_rotating_visual) {

	this->is_rotating_visual = is_rotating_visual;
}

bool BulletManagerBulletType::get_is_rotating_visual() const {

	return is_rotating_visual;
}
void BulletManagerBulletType::_update_cached_rects()  {

	Rect2 base_rect;

	/*if (region) {
		r_filter_clip = region_filter_clip;
		base_rect = region_rect;
	} else {
		r_filter_clip = false;
		base_rect = Rect2(0, 0, texture->get_width(), texture->get_height());
	}*/
	base_rect = Rect2(0, 0, texture->get_width(), texture->get_height());
	Size2 frame_size = base_rect.size / Size2(hframes, vframes);
	Point2 frame_offset = Point2(frame % hframes, frame / hframes);
	frame_offset *= frame_size;

	_cached_src_rect.size = frame_size;
	_cached_src_rect.position = base_rect.position + frame_offset;

	/*Point2 dest_offset = offset;
	if (centered)
		dest_offset -= frame_size / 2;
	if (Engine::get_singleton()->get_use_pixel_snap()) {
		dest_offset = dest_offset.floor();
	}

	r_dst_rect = Rect2(dest_offset, frame_size);*/
	_cached_dst_rect = Rect2(-frame_size / 2, frame_size);

	/*if (hflip)
		r_dst_rect.size.x = -r_dst_rect.size.x;
	if (vflip)
		r_dst_rect.size.y = -r_dst_rect.size.y;*/
}

void BulletManagerBulletType::area_inout(Object* bullet, int p_status, const RID &p_area, int p_instance, int p_area_shape, int p_self_shape) {
	Object* collider = ObjectDB::get_instance(p_instance);
	emit_signal("area_entered_bullet", bullet, collider);
}
void BulletManagerBulletType::body_inout(Object* bullet, int p_status, const RID &p_body, int p_instance, int p_body_shape, int p_area_shape) {
	Object* collider = ObjectDB::get_instance(p_instance);
	emit_signal("body_entered_bullet", bullet, collider);
}

void BulletManagerBulletType::_bind_methods() {
	//VISUAL
	ClassDB::bind_method(D_METHOD("set_texture", "texture"), &BulletManagerBulletType::set_texture);
	ClassDB::bind_method(D_METHOD("get_texture"), &BulletManagerBulletType::get_texture);
	ClassDB::bind_method(D_METHOD("set_vframes", "vframes"), &BulletManagerBulletType::set_vframes);
	ClassDB::bind_method(D_METHOD("get_vframes"), &BulletManagerBulletType::get_vframes);
	ClassDB::bind_method(D_METHOD("set_hframes", "hframes"), &BulletManagerBulletType::set_hframes);
	ClassDB::bind_method(D_METHOD("get_hframes"), &BulletManagerBulletType::get_hframes);
	ClassDB::bind_method(D_METHOD("set_frame", "frame"), &BulletManagerBulletType::set_frame);
	ClassDB::bind_method(D_METHOD("get_frame"), &BulletManagerBulletType::get_frame);
	ClassDB::bind_method(D_METHOD("set_is_rotating_visual", "is_rotating_visual"), &BulletManagerBulletType::set_is_rotating_visual);
	ClassDB::bind_method(D_METHOD("get_is_rotating_visual"), &BulletManagerBulletType::get_is_rotating_visual);
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
	
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), "set_texture", "get_texture");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "vframes", PROPERTY_HINT_RANGE, "1,16384,1"), "set_vframes", "get_vframes");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "hframes", PROPERTY_HINT_RANGE, "1,16384,1"), "set_hframes", "get_hframes");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "frame", PROPERTY_HINT_SPRITE_FRAME), "set_frame", "get_frame");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "collision_shape", PROPERTY_HINT_RESOURCE_TYPE, "Shape2D"), "set_collision_shape", "get_collision_shape");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_layer", PROPERTY_HINT_LAYERS_2D_PHYSICS), "set_collision_layer", "get_collision_layer");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_mask", PROPERTY_HINT_LAYERS_2D_PHYSICS), "set_collision_mask", "get_collision_mask");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_rotating_visual"), "set_is_rotating_visual", "get_is_rotating_visual");

	ADD_SIGNAL(MethodInfo("area_entered_bullet", PropertyInfo(Variant::OBJECT, "bullet", PROPERTY_HINT_RESOURCE_TYPE, "BulletManagerBullet"), PropertyInfo(Variant::OBJECT, "area", PROPERTY_HINT_RESOURCE_TYPE, "Area2D")));
	ADD_SIGNAL(MethodInfo("body_entered_bullet", PropertyInfo(Variant::OBJECT, "bullet", PROPERTY_HINT_RESOURCE_TYPE, "BulletManagerBullet"), PropertyInfo(Variant::OBJECT, "body", PROPERTY_HINT_RESOURCE_TYPE, "PhysicsBody2D")));

}