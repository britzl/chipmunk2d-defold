local M = {}


local spaces = {}

local constraints = {}

local function get_space(space)
	spaces[space] = spaces[space] or {
		objects = {},
		constraints = {}
	}
	return spaces[space]
end

local function add_object_to_space(space, object)
	get_space(space).objects[object.id] = object
end

local function add_constraint_to_space(space, constraint)
	local constraints = get_space(space).constraints
	table.insert(constraints, constraint)
end

local function create_segment_shape(body, rotation, width, height)
	local a = vmath.rotate(rotation, vmath.vector3(-width / 2, 0, 0))
	local b = vmath.rotate(rotation, vmath.vector3(width / 2, 0, 0))
	return chipmunk.new_segment_shape(body, a.x, a.y, b.x, b.y, height / 2)
end

local function create_dynamic_circle(space, position, radius, mass, friction, elasticity)
	local body = chipmunk.new_circle_body(radius, mass)
	chipmunk.set_body_position(body, position.x, position.y)
	chipmunk.add_body(space, body)

	local shape = chipmunk.new_circle_shape(body, radius)
	chipmunk.set_shape_friction(shape, friction)
	chipmunk.set_shape_elasticity(shape, elasticity or 0)
	chipmunk.add_shape(space, shape)
	return {
		body = body,
		shape = shape,
		position = vmath.vector3(position),
		dynamic = true,
	}
end

local function create_dynamic_box(space, position, rotation, width, height, mass, friction, elasticity)
	local body = chipmunk.new_box_body(width, height, mass)
	chipmunk.set_body_position(body, position.x, position.y)
	chipmunk.add_body(space, body)

	local shape = chipmunk.new_box_shape(body, width, height, 0)
	chipmunk.set_shape_friction(shape, friction)
	chipmunk.set_shape_elasticity(shape, elasticity or 0)
	chipmunk.add_shape(space, shape)
	return {
		body = body,
		shape = shape,
		position = vmath.vector3(position),
		dynamic = true,
	}
end

local function crete_static_segement(space, position, rotation, width, height, friction, elasticity)
	local body = chipmunk.new_static_body()
	chipmunk.set_body_position(body, position.x, position.y)

	local shape = create_segment_shape(body, rotation, width, height)
	chipmunk.set_shape_friction(shape, friction)
	chipmunk.set_shape_elasticity(shape, elasticity or 0)
	chipmunk.add_shape(space, shape)
	return {
		body = body,
		shape = shape,
		position = vmath.vector3(position),
		static = true,
	}
end

function M.dynamic_circle(space, id, radius, mass, friction, elasticity)
	assert(space, "You must provide a space")
	assert(id, "You must provide a game object id")
	assert(radius, "You must provide a radius")
	assert(mass, "You must provide a mass")
	assert(friction, "You must provide a friction")
	local circle = create_dynamic_circle(space, go.get_position(id), radius, mass, friction, elasticity)
	circle.id = id
	add_object_to_space(space, circle)
	return circle
end

function M.dynamic_box(space, id, width, height, mass, friction, elasticity)
	assert(space, "You must provide a space")
	assert(id, "You must provide a game object id")
	assert(width, "You must provide a width")
	assert(height, "You must provide a height")
	assert(mass, "You must provide a mass")
	assert(friction, "You must provide a friction")
	local box = create_dynamic_box(space, go.get_position(id), go.get_rotation(id), width, height, friction, elasticity)
	box.id = id
	add_object_to_space(space, box)
	return box
end

function M.static_segment(space, id, width, height, friction, elasticity)
	assert(space, "You must provide a space")
	assert(id, "You must provide a game object id")
	assert(width, "You must provide a width")
	assert(height, "You must provide a height")
	assert(friction, "You must provide a friction")
	local segment = crete_static_segement(space, go.get_position(id), go.get_rotation(id), width, height, friction, elasticity)
	segment.id = id
	add_object_to_space(space, segment)
	return segment
end


function M.slide_joint(space, body1, body2, a1x, a1y, a2x, a2y, min, max)
	local constraint = chipmunk.new_slide_joint(body1, body2, a1x, a1y, a2x, a2y, min, max)
	chipmunk.add_constraint(space, constraint)
	add_constraint_to_space(space, constraint)
	return constraint
end


function M.update(space, dt)
	chipmunk.space_step(space, dt)
	local objects = spaces[space].objects
	for id,object in pairs(objects) do
		if not object.static then
			object.position.x, object.position.y = chipmunk.get_body_position(object.body)
			go.set_position(object.position, object.id)
			object.angle = chipmunk.get_body_rotation(object.body)
			go.set_rotation(vmath.quat_rotation_z(object.angle))
		end
	end
end


return M