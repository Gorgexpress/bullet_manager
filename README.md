UPDATE: I have moved away from this approach for efficiently dealing with large amounts of projectiles. It tries to do too much, and does not really fit Godot's scene/node system well. All you really need is something like the tilemap or cpuparticles node, but for STG projectiles. 

# Bullet Manager
Godot module that adds 2 classes made for dealing with large amounts of 2d projectiles.  

BulletManager - The core class. Holds all the bullets, and handles rendering/updating them. Extends CanvasItem. 
BulletManagerBulletType - Defines a type of bullet. Should always be a direct child of a BulletManager. Extends Node.

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
Where type_name is the name of the BulletManagerBulletType node. This function returns the id of the bullet.

You can use the BulletManager to get or set the properties of individual bullets via the bullet id. These functions are:
Vector2 get_bullet_position(int bullet_id)
void set_bullet_position(int bullet_id, Vector2 position)
float get_bullet_speed(int bullet_id)
void set_bullet_speed(int bullet_id, float speed)
float get_bullet_angle(int bullet_id)
void set_bullet_angle(int bullet_id, float angle)

BulletManagerBulletType's have 2 custom signals for collision responses:
area_entered_bullet(int bullet_id, Object area)
body_entered_bullet(int bullet_id, Object body)

Connect to these signals to handle collision responses. 

Here's an example BulletManagerBulletType script!

```
extends BulletManagerBulletType

onready var parent = get_parent()

func _ready():
	connect("area_entered_bullet", self, "area_collision")
	connect("body_entered_bullet", self, "body_collision")
	
func area_collision(bullet_id, area):
	if area.has_method("take_damage"):
		area.take_damage(bullet_id, 1)
	parent.queue_delete_bullet(bullet_id)
	
func body_collision(bullet_id, body):
	if body.has_method("take_damage"):
		body.take_damage(bullet_id, 5)
	parent.queue_delete_bullet(bullet_id)

```

The BulletManager.queue_bullet_delete(bullet_id) function will set its collision mask and layer to 0 immediately, then fully delete the
object next time the BulletManager receives the PhysicsProcess notification. 
Bullets will also be automatically deleted if they get too far out of your viewport. How far they are allowed to be 
is based off the BulletManager.bounds_margin property. It defaults to 300, which means a bullet is deleted when its position 
is 300 units(pixels?) away from the edge of the viewable screen. 
You can also clear all bullets in a BulletManager with BulletManager.clear(). 

Shoutouts to [This module.](https://github.com/SleepProgger/godot_stuff/tree/master/examples/BulletTest/modules) by SleepProgger.
It was a very helpful reference for getting this module started!

# TODO
Bullet properties aren't set in stone. Might want to add new ones, like max acceleration. Could use angles instead of a normalized direction vector. Maybe I don't need to store a Transform2D for each bullet. 

Planning on packing more information into bullet_ids. It's just the index into the bullets vector now. Will come with other changes.

Right now each bullet has its own area. I want to try making each bullet just a shape instead, with the bullet type owning the area(and recieving the collision response on the C++ side)
