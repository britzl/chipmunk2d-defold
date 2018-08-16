# chipmunk2d-defold
Defold native extension for the [Chipmunk 2D physics engine](https://chipmunk-physics.net).


# Setup
## Add project dependencies
You can use the extension in your own project by adding this project as a [Defold library dependency](http://www.defold.com/manuals/libraries/). Open your game.project file and in the dependencies field under project add:

https://github.com/britzl/chipmunk2d-defold/archive/master.zip

Or point to the ZIP file of a [specific release](https://github.com/britzl/chipmunk2d-defold/releases).

# Demo
HTML5 demo of the example project here: https://britzl.github.io/Chipmunk2D/

# Usage
The extension binds the individual methods of the Chipmunk 2D API. It should be possible to translate most of the official Chipmunk 2D manuals and demos when working with this extension.

## new_space(gravity_x, gravity_y)
Create a new space

**PARAMETERS**
* `gravity_x` (number) - Horizontal gravity affecting bodies in this space
* `gravity_y` (number) - Vertical gravity affecting bodies in this space

**RETURNS**
* `space` (userdata) - The created space


## new_circle_body(radius, mass)
Create a new circle body

**PARAMETERS**
* `radius` (number) - Radius of the circle
* `mass` (number) - Mass of the body

**RETURNS**
* `body` (userdata) - The created body


## new_box_body(width, height, mass)
Create a new box body

**PARAMETERS**
* `width` (number) - Width of the box
* `height` (number) - Height of the box
* `mass` (number) - Mass of the body

**RETURNS**
* `body` (userdata) - The created body


## new_static_body()
Create a static body

**RETURNS**
* `body` (userdata) - The created body


## new_circle_shape(body, radius)
Create a new circle shape

**PARAMETERS**
* `body` (userdata) - The body this shape should be attached to
* `radius` (number) - Radius of the circle

**RETURNS**
* `shape` (userdata) - The created shape


## new_box_shape(body, width, height, radius)
Create a new box shape

**PARAMETERS**
* `body` (userdata) - The body this shape should be attached to
* `width` (number) - Width of the box
* `height` (number) - Height of the box
* `radius` (number) - Radius of the rounded corners

**RETURNS**
* `shape` (userdata) - The created shape


## new_segment_shape(body, ax, ay, bx, by, thickness)
Create a new segment shape

**PARAMETERS**
* `body` (userdata) - The body this shape should be attached to
* `ax` (number) - Horizontal position of endpoint a
* `ay` (number) - Vertical position of endpoint a
* `bx` (number) - Horizontal position of endpoint b
* `by` (number) - Vertical position of endpoint b
* `thickness` (number) - Thickness of the segment

**RETURNS**
* `shape` (userdata) - The created shape


## new_damped_spring(body1, body2, a1x, a1y, a2x, a2y, rest_length, stiffness, damping)
Create a new damped spring

**PARAMETERS**
* `body1` (userdata) - The first body to connect
* `body2` (userdata) - The second body to connect
* `a1x` (number) - Horizontal position of the anchor point on the first body
* `a1y` (number) - Vertical position of the anchor point on the first body
* `a2x` (number) - Horizontal position of the anchor point on the second body
* `a2y` (number) - Vertical position of the anchor point on the second body
* `rest_length` (number) - Rest length of the spring
* `stiffness` (number) - Stiffness of the spring in force/distance
* `damping` (number) - Damping of the spring

**RETURNS**
* `constraint` (userdata) - The created constraint


## new_slide_joint(body1, body2, a1x, a1y, a2x, a2y, min, max)
Create a new slide joint

**PARAMETERS**
* `body1` (userdata) - The first body to connect
* `body2` (userdata) - The second body to connect
* `a1x` (number) - Horizontal position of the anchor point on the first body
* `a1y` (number) - Vertical position of the anchor point on the first body
* `a2x` (number) - Horizontal position of the anchor point on the second body
* `a2y` (number) - Vertical position of the anchor point on the second body
* `min` (number) - Minimum distance the joint will maintain between the anchors
* `max` (number) - Maximum distance the joint will maintain between the anchors

**RETURNS**
* `constraint` (userdata) - The created constraint


## set_shape_friction(shape, friction)
Set the friction of a shape

**PARAMETERS**
* `shape` (userdata) - Shape to set friction for
* `friction` (number) - Friction of the shape


## set_shape_elasticity(shape, elasticity)
Set the elasticity of a shape

**PARAMETERS**
* `shape` (userdata) - Shape to set friction for
* `elasticity` (number) - Elasticity of the shape


## add_body(space, body)
Add a body to a space

**PARAMETERS**
* `space` (userdata) - Space to add body to
* `body` (userdata) - The body to add


## add_shape(space, shape)
Add a shape to a space

**PARAMETERS**
* `space` (userdata) - Space to add body to
* `shape` (userdata) - The shape to add


## add_constraint(space, constraint)
Add a constraint to a space

**PARAMETERS**
* `space` (userdata) - Space to add body to
* `constraint` (userdata) - The constraint to add


## space_step(space, dt)
Step/update the physics simulation of a space

**PARAMETERS**
* `space` (userdata) - Space to step/update
* `dt` (number) - The delta time of the update


## set_body_position(body, x, y)
Set the position of a body

**PARAMETERS**
* `body` (userdata) - Body to set position for
* `x` (number) - Horizontal position of the body
* `y` (number) - Vertical position of the body


## get_body_position(body)
Get the position of a body

**PARAMETERS**
* `body` (userdata) - The body to get position for

**RETURNS**
* `x` (number) - Horizontal position
* `y` (number) - Vertical position


## get_body_rotation(body)
Get the rotation of a body

**PARAMETERS**
* `body` (userdata) - The body to get rotation for

**RETURNS**
* `angle` (number) - Rotation angle in radians


## get_body_velocity(body)
Get the velocity of a body

**PARAMETERS**
* `body` (userdata) - The body to get velocity for

**RETURNS**
* `x` (number) - Horizontal velocity
* `y` (number) - Vertical velocity
