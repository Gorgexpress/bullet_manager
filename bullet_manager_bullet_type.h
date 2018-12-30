#ifndef BULLET_MANAGER_BULLET_TYPE_H
#define BULLET_MANAGER_BULLET_TYPE_H

#include "scene/2d/sprite.h"
#include "scene/main/node.h"
#include "scene/resources/texture.h"
#include "scene/resources/shape_2d.h"


struct BulletManagerBulletType {//: public Object {
	
	//GDCLASS(BulletManagerBulletType, Object)

	RID shape_rid;
	Sprite* sprite;
	Rect2 src_rect;
	Rect2 dest_rect;
	StringName name;
	bool is_updated = false;
};

#endif