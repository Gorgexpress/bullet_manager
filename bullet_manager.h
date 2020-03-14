#ifndef BULLET_MANAGER_H
#define BULLET_MANAGER_H

#include "scene/2d/node_2d.h"
#include "scene/2d/canvas_item.h"
#include "scene/2d/sprite.h"
#include "core/object.h"
#include "scene/resources/texture.h"
#include "scene/resources/shape_2d.h"



class BulletManagerBulletType : public Node2D {
	
	GDCLASS(BulletManagerBulletType, Node2D)
	
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
	bool rotate_physics = false;
	bool has_custom_update = false;

	RID area;
	Vector<int> _shapes;
	List<int> _unused_shapes;
	//These remaining private members are used by BulletManager to cache info.
	friend class BulletManager;
	Rect2 _cached_src_rect;
	Rect2 _cached_dst_rect;
	BulletManager* _bullet_manager;
	void _update_cached_rects();

	StringName _body_inout_name = StaticCString::create("_body_inout");
	StringName _area_inout_name = StaticCString::create("_area_inout");
	
	
protected:
	void _notification(int p_what);
	static void _bind_methods();

public:

	BulletManagerBulletType();
	~BulletManagerBulletType();
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
	void set_rotate_physics(bool p_rotate_physics);
	bool is_rotating_physics() const;
	void set_collision_shape(const Ref<Shape2D> &p_shape);
	Ref<Shape2D> get_collision_shape() const;

	int add_shape(int bullet_idx, Transform2D transform);
	void remove_shape(int shape_idx);

	void custom_update();

	void _area_inout(int p_status, const RID &p_area, int p_instance, int p_area_shape, int p_self_shape);
	void _body_inout(int p_status, const RID &p_body, int p_instance, int p_body_shape, int p_area_shape);

};


struct BulletManagerBullet {
    Transform2D matrix = Transform2D();
	Vector2 direction;
	real_t speed;
    int id; //also servers as index into bullets array in bulletmanager
	int shape_index;
	bool is_queued_for_deletion = false;
	Variant custom_data;
	BulletManagerBulletType* type;
	void set_position(Point2 position);
	Point2 get_position() const;
    void set_direction(Vector2 direction);
    Vector2 get_direction() const;
    void set_angle(real_t angle);
    real_t get_angle() const;
    void set_speed(real_t speed);
    real_t get_speed() const;
	void queue_delete();
    Node* get_type() const;
};


class BulletManager : public Node2D {
	
	GDCLASS(BulletManager, Node2D)
	
	Point2 pos;
    Size2 scale;
	int z_index = 0;
	real_t bounds_margin = 300.0;
	//pool of bullets both used and unused bullets
	Vector<BulletManagerBullet> _bullets;
	//Active bullets only. A linked list allows constant time deletion in the middle of a list, without changing the order of the elements.
	List<int> _active_bullets; 
	List<int> _unused_ids; //bullet ids are indices into the bullets vector.
	
	Map<StringName, BulletManagerBulletType*> types;

	

	void set_bounds_margin(float p_bounds_margin);
	float get_bounds_margin() const;
	void _update_bullets();
	void _draw_bullets();
	void _get_visible_rect(Rect2 &rect);

protected:
	void _notification(int p_what);

	static void _bind_methods();

public:
	int add_bullet(StringName type, Vector2 position, real_t angle, real_t speed);
	void set_bullet_position(int bullet_id, Vector2 position);
	Vector2 get_bullet_position(int bullet_id) const;
	void set_bullet_speed(int bullet_id, real_t speed);
	real_t get_bullet_speed(int bullet_id) const;
	void set_bullet_angle(int bullet_id, real_t speed);
	real_t get_bullet_angle(int bullet_id) const;
	bool is_bullet_active(int bullet_id) const;
	Variant get_bullet_custom_data(int bullet_id) const;
	void set_bullet_custom_data(int bullet_id, Variant custom_data);
	void queue_delete_bullet(int bullet_id);
	void clear();
	int count();
	void register_bullet_type(BulletManagerBulletType* type);
	void unregister_bullet_type(BulletManagerBulletType* type);


};
#endif
