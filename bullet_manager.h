#ifndef BULLET_MANAGER_H
#define BULLET_MANAGER_H

#include "scene/2d/node_2d.h"
#include "scene/2d/canvas_item.h"
#include "scene/2d/sprite.h"
#include "core/object.h"
#include "scene/resources/texture.h"
#include "scene/resources/shape_2d.h"
#include "bullet_manager_bullet.h"


class BulletManager : public CanvasItem {
	
	GDCLASS(BulletManager, CanvasItem)
	
	Point2 pos;
    Size2 scale;
	int z_index = 0;
	real_t bounds_margin = 300.0;
	//A linked list allows constant time deletion in the middle of a list, without changing the order of the elements.
	List<BulletManagerBullet*> bullets; 
	Map<StringName, BulletManagerBulletType*> types;

	StringName _body_inout_name = StaticCString::create("_body_inout");
	StringName _area_inout_name = StaticCString::create("_area_inout");

	void BulletManager::_edit_set_position(const Point2 &p_position);
	Point2 BulletManager::_edit_get_position() const;	
	void BulletManager::_edit_set_scale(const Size2 &p_scale);
	Size2 BulletManager::_edit_get_scale() const;

	void set_z_index(int z_index);
	int get_z_index() const;
	void set_bounds_margin(float p_bounds_margin);
	float get_bounds_margin() const;
	void _update_bullets();
	void _draw_bullets();
	void _draw_editor_hint();
	void _draw_bullet_type(BulletManagerBulletType* type, int &offset_y);
	void _register_bullet_types();
	void _get_visible_rect(Rect2 &rect);

protected:
	void _notification(int p_what);

	static void _bind_methods();

public:
	BulletManagerBullet* add_bullet(StringName type, Vector2 position, real_t angle, real_t speed, real_t acceleration = 0);
	void clear();
	int count();
	Transform2D get_transform() const;

};
#endif
