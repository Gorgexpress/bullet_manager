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

The queue_delete() functoin for a bullet will set its collision mask and layer to 0 immediately, then fully delete the
object next time the BulletManager receives the PhysicsProcess notification. 
Bullets will also be automatically deleted if they get too far out of your viewport! How far they are allowed to be 
is based off the BulletManager.bounds_margin property. It defaults to 300, which means a bullet is deleted when its position 
is 300 units(pixels?) away from the edge of the viewable screen. 
You can also clear all bullets in a BulletManager with BulletManager.clear(). 

Shoutouts to [This module.](https://github.com/SleepProgger/godot_stuff/tree/master/examples/BulletTest/modules) by SleepProgger.
It was a very helpful reference for getting this module started!

# TODO
This module already covers the basics, but not much else. Also, while you can grab a reference to a bullet and change its
properties from gdscript, that bullet is an Object that can be freed at anytime. Trying to alter an object after it has been freed 
will crash the game. You'd need to store weakrefs to the bullets. Not sure what direction I should take to make it more user friendly. 

I should make it possible to define custom update logic for the bullets. If that logic is defined in gdscript, it will be very slow.
However, I should make it easy to extend the c++ code with additional classes as well.

The structure used to store the bullets is Godot's List structure, a doubly LinkedList that seems to be meant for a 
single threaded environment. I get around concurrency issues(atleast I think I do?) by only modifying the linked list nodes 
themselves in physics_process. queue_delete sets the bullet's collision mask and layer to 0 immediately, and the 
bullet itself is only deleted next frame. Clear does the same but for all bullets. I might wanna look into supporting fully 
deleting the object immediately. 
A LinkedList is chosen because it allows constant time deletion in the middle of the list without reordering the elements.
Reordering the elements impacts the order that they are drawn. Shuffling that order looks VERY messy, and should not be done.
If i can keep the draw order from shuffling some other way, then I won't need to use a linked list. 


