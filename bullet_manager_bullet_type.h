#ifndef BULLET_MANAGER_BULLET_TYPE_H
#define BULLET_MANAGER_BULLET_TYPE_H

#include "scene/2d/sprite.h"
#include "scene/main/node.h"
#include "scene/resources/texture.h"
#include "scene/resources/shape_2d.h"


class BulletManagerBulletType : public Node {
	
	GDCLASS(BulletManagerBulletType, Node)
	
	Ref<Texture> texture;
	Ref<Texture> normal_map;
	int vframes = 1;
	int hframes = 1;
	int frame = 0;
	bool centered = false;
	Point2 offset;
	bool region = false ;
	Rect2 region_rect;
	bool rotate_visual = false;
	Ref<Shape2D> collision_shape;
	uint32_t collision_mask = 0;
	uint32_t collision_layer = 0;
	bool is_rotating_physics = false;
	//These remaining private members are used by BulletManager to cache info.
	friend class BulletManager;
	Rect2 _cached_src_rect;
	Rect2 _cached_dst_rect;
	void _update_cached_rects();
	
	
protected:
	static void _bind_methods();

public:

	void set_texture(const Ref<Texture> &p_texture);
	Ref<Texture> get_texture() const;
	void set_normal_map(const Ref<Texture> &p_normal_map);
	Ref<Texture> get_normal_map() const;
	void set_vframes(int p_amount);
	int get_vframes() const;
	void set_hframes(int p_amount);
	int get_hframes() const;
	void set_frame(int frame);
	int get_frame() const;
	void set_centered(bool p_center);
	bool is_centered() const;
	void set_offset(const Point2 &p_offset);
	Point2 get_offset() const;
	void set_region(bool p_region);
	bool is_region() const;
	void set_region_rect(const Rect2 &p_region_rec);
	Rect2 get_region_rect() const;
	void set_rotate_visual(bool p_rotate_visual);
	bool is_rotating_visual() const;
    void set_collision_mask(uint32_t p_mask);
	uint32_t get_collision_mask() const;
	void set_collision_layer(uint32_t p_layer);
	uint32_t get_collision_layer() const;
	void set_collision_shape(const Ref<Shape2D> &p_shape);
	Ref<Shape2D> get_collision_shape() const;

	void area_inout(Object* bullet, int p_status, const RID &p_area, int p_instance, int p_area_shape, int p_self_shape);
	void body_inout(Object* bullet, int p_status, const RID &p_body, int p_instance, int p_body_shape, int p_area_shape);

};

#endif