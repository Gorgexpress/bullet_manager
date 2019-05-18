#ifndef BULLET_MANAGER_BULLET_H
#define BULLET_MANAGER_BULLET_H

#include "core/object.h"
#include "modules/bullet_manager/bullet_manager_bullet_type.h"

class BulletManagerBullet : public Object {
	
	GDCLASS(BulletManagerBullet, Object)

    friend class BulletManager;
    friend class BulletManagerBulletType;
    Transform2D matrix;
	Vector2 direction;
	real_t speed = 0;
	real_t acceleration = 0;
	RID area;
	bool is_queued_for_deletion = false;
	BulletManagerBulletType* type;
public:
	

	void _area_inout(int p_status, const RID &p_area, int p_instance, int p_area_shape, int p_self_shape);
	void _body_inout(int p_status, const RID &p_body, int p_instance, int p_body_shape, int p_area_shape);
	void set_position(Point2 position);
	Point2 get_position() const;
    void set_direction(Vector2 direction);
    Vector2 get_direction() const;
    void set_speed(real_t speed);
    real_t get_speed() const;
    void set_acceleration(real_t acceleration);
    real_t get_acceleration() const;
    void set_type(Node* bullet_manager_bullet_type);
    Node* get_type() const;
    void queue_delete();
protected:
	static void _bind_methods();

};
#endif