#ifndef BULLET_MANAGER_BULLET_H
#define BULLET_MANAGER_BULLET_H

#include "core/object.h"
#include "bullet_manager_bullet_type.h"

class BulletManagerBullet : public Object {
	
	GDCLASS(BulletManagerBullet, Object)

    friend class BulletManager;
    friend class BulletManagerBulletType;
    Transform2D matrix;
	Vector2 direction;
	real_t speed = 0;
	real_t acceleration;
	RID area;
	bool is_queued_for_deletion = false;
	BulletManagerBulletType* type;
public:
	

	void _area_inout(int p_status, const RID &p_area, int p_instance, int p_area_shape, int p_self_shape);
	void _body_inout(int p_status, const RID &p_body, int p_instance, int p_body_shape, int p_area_shape);
	Point2 get_global_position();
	void set_global_position(Point2 position);
    void queue_delete();
protected:
	static void _bind_methods();

};
#endif