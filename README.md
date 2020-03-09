# Bullet Manager
Godot module that adds classes made for dealing with large amounts of 2d projectiles. 3 classes total, 2 of which will show up in the editor as new node types.

BulletManager - The core class. Holds all the bullets, and handles rendering/updating them. Extends CanvasItem. 
BulletManagerBulletType - Defines a type of bullet. Should always be a direct child of a BulletManager. Extends Node.
BulletManagerBullet - A single bullet, with a position, velocity, speed, acceleration, and type. Extends Object. 

## Installing
Clone into the modules folder and compile Godot. Should just work in 3.0. Not sure about 3.1. 
Don't know how to compile Godot? See http://docs.godotengine.org/en/3.0/development/compiling/index.html

## Getting Started + basics
Make a new scene with a BulletManager as the root node. 
Define your first bullet type by adding a BulletManagerBulletType as a direct child. 
Give that child a good name! Using that name is how you will add a bullet of that type.
If you are familiar with Sprite and CollisionObject2D nodes, you should be able to figure out all the 
BulletManagerBulletType properties on your own. If you aren't familiar with those nodes, you may wish to try them out before
resorting to a custom module to handle your projectiles! 

Grab a reference to your BulletManager node to add a bullet. You may wish to make it an autoload singleton. You may
also want to have seperate BulletManager nodes for the player and the enemies.

Add a bullet with BulletManager.add_bullet(String type_name, Vector2 position, Vector2 direction,float speed, float acceleration = 0)
Where type_name is the name of the BulletManagerBulletType node. 

BulletManagerBulletType's have 2 custom signals for collision responses:
area_entered_bullet(Object bullet, Object area)
body_entered_bullet(Object bullet, Object body)

Connect to these signals to handle collision responses. 

Here's an example BulletManagerBulletType script!

```
extends BulletManagerBulletType

func _ready():
	connect("area_entered_bullet", self, "area_collision")
	connect("body_entered_bullet", self, "body_collision")
	
func area_collision(bullet, area):
	if area.has_method("take_damage"):
		area.take_damage(bullet, 1)
	bullet.queue_delete()
	
func body_collision(bullet, body):
	if body.has_method("take_damage"):
		body.take_damage(bullet, 5)
	bullet.queue_delete()
```

The queue_delete() function for a bullet will set its collision mask and layer to 0 immediately, then fully delete the
object next time the BulletManager receives the PhysicsProcess notification. 
Bullets will also be automatically deleted if they get too far out of your viewport! How far they are allowed to be 
is based off the BulletManager.bounds_margin property. It defaults to 300, which means a bullet is deleted when its position 
is 300 units(pixels?) away from the edge of the viewable screen. 
You can also clear all bullets in a BulletManager with BulletManager.clear(). 
If you are keeping references too the bullets, you'll have to use is_instance_valid(instance) or weakrefs to make sure you aren't trying to do something to a bullet that has been deleted.

Shoutouts to [This module.](https://github.com/SleepProgger/godot_stuff/tree/master/examples/BulletTest/modules) by SleepProgger.
It was a very helpful reference for getting this module started!

# TODO
Bullets are currently held in a doubly linked list, and are created/deleted on demand. A free list with pooling would be more efficient. Not a high priority though. 

Bullet properties aren't set in stone. Might want to add new ones, like max acceleration. Could use angles instead of a normalized direction vector. Maybe I don't need to store a Transform2D for each bullet. 

Materials apply to every bullet type in the manager. Not sure how to get around that one. 

If I could avoid making bullets Objects, without making significant sacrifices, that would be nice. 
