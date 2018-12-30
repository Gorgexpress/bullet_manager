#ifndef BULLET_MANAGER_H
#define BULLET_MANAGER_H

#include "scene/2d/node_2d.h"
#include "scene/2d/sprite.h"
#include "core/object.h"
#include "scene/resources/texture.h"
#include "scene/resources/shape_2d.h"
#include "bullet_manager_bullet.h"


class BulletManager : public Node2D {
	
	GDCLASS(BulletManager, Node2D)

	Ref<Texture> texture;
	Ref<Texture> normal_map;
	Point2 offset;
	bool region = false;
	Rect2 region_rect;
	bool region_filter_clip = false;

	int frame = 0;

	int vframes = 0;
	int hframes = 0;

	bool centered = true;
	bool hflip = false;
	bool vflip = false;
	
	Ref<Shape2D> shape;

	int collision_layer = 0;
	int collision_mask = 0;
	
	List<BulletManagerBullet*> bullets;
	Map<StringName, BulletManagerBulletType> types;

	StringName _body_inout_name = StaticCString::create("_body_inout");
	StringName _area_inout_name = StaticCString::create("_area_inout");

	void _get_rects(Rect2 &r_src_rect, Rect2 &r_dst_rect, bool &r_filter_clip, Sprite* sprite) const;
	void _update_bullets();
	void _draw_bullets();
	void _register_bullet_types();
	void _get_visible_rect(Rect2 &rect);

protected:
	void _notification(int p_what);

	static void _bind_methods();

public:
	void set_collision_mask(uint32_t p_mask);
	uint32_t get_collision_mask() const;

	void set_collision_layer(uint32_t p_layer);
	uint32_t get_collision_layer() const;
	void set_shape(const Ref<Shape2D> &p_shape);
	Ref<Shape2D> get_shape() const;
	void set_texture(const Ref<Texture> &p_texture);
	Ref<Texture> get_texture() const;
	void set_normal_map(const Ref<Texture> &p_texture);
	Ref<Texture> get_normal_map() const;
	void set_vframes(int p_amount);
	int get_vframes() const;
	void set_hframes(int p_amount);
	int get_hframes() const;
	void set_frame(int frame);
	int get_frame() const;
	void add_bullet(StringName type, Vector2 position, Vector2 direction, real_t speed, real_t acceleration);
	void clear();
	int count();

};
#endif
