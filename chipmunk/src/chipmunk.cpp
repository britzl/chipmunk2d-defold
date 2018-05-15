#include "luautils.h"
#include <stdio.h>
#include "chipmunk.h"
#include <chipmunk/chipmunk.h>
#include <dmsdk/sdk.h>

#define LIB_NAME "Chipmunk"
#define MODULE_NAME "chipmunk"
#define DLIB_LOG_DOMAIN LIB_NAME

static const int MAX_SPACES = 8;
static const int MAX_BODIES = 128;
static const int MAX_SHAPES = 128;
static const int MAX_CONSTRAINTS = 128;

static dmArray<ChipSpace*> g_ChipSpaces;
static dmArray<ChipBody*> g_ChipBodies;
static dmArray<ChipShape*> g_ChipShapes;
static dmArray<ChipConstraint*> g_ChipConstraints;


//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
// CHIP SPACE
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#define CHIPSPACE "chipspace"

static ChipSpace* CheckSpace(lua_State *L, int index) {
	luaL_checktype(L, index, LUA_TUSERDATA);
	ChipSpace *chipspace = (ChipSpace *)luaL_checkudata(L, index, CHIPSPACE);
	if (chipspace == NULL) luaL_typerror(L, index, CHIPSPACE);
	return chipspace;
}

static void PushSpace(lua_State* L, cpSpace *space) {
	dmLogInfo("PushSpace");
	int top = lua_gettop(L);
	ChipSpace *chipspace = (ChipSpace *)lua_newuserdata(L, sizeof(ChipSpace));
	luaL_getmetatable(L, CHIPSPACE);
	lua_setmetatable(L, -2);
	chipspace->space = space;
	chipspace->index = g_ChipSpaces.Size();

	if (g_ChipSpaces.Full()) {
		g_ChipSpaces.SetCapacity(g_ChipSpaces.Capacity() + MAX_SPACES);
	}
	g_ChipSpaces.Push(chipspace);

	assert(top + 1 == lua_gettop(L));
}

static int ChipSpace_gc(lua_State* L) {
	dmLogInfo("ChipSpace_gc");
	ChipSpace *chipspace = CheckSpace(L, 1);
	cpSpaceFree(chipspace->space);
	ChipSpace *swapped = g_ChipSpaces.EraseSwap(chipspace->index);
	if (swapped != 0) {
		swapped->index = chipspace->index;
	}
	return 0;
}

static int ChipSpace_tostring(lua_State* L) {
	CheckSpace(L, 1);
	lua_pushfstring(L, "[ChipSpace]");
	return 1;
}
static const luaL_reg ChipSpace_metamethods[] = {
	{"__gc", ChipSpace_gc},
	{"__tostring", ChipSpace_tostring},
	{0, 0}
};

static int Chipmunk_NewSpace(lua_State* L) {
	dmLogInfo("Chipmunk_NewSpace");
	int top = lua_gettop(L);

	cpFloat gravityX = lua_tonumber(L, 1);
	cpFloat gravityY = lua_tonumber(L, 2);
	cpVect gravity = cpv(gravityX, gravityY);
	cpSpace *space = cpSpaceNew();
	cpSpaceSetGravity(space, gravity);

	PushSpace(L, space);

	assert(top + 1 == lua_gettop(L));
	return 1;
}



//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
// CHIP BODY
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#define CHIPBODY "chipbody"

static void PushBody(lua_State* L, cpBody *body) {
	dmLogInfo("PushBody");

	int top = lua_gettop(L);
	ChipBody *chipbody = (ChipBody *)lua_newuserdata(L, sizeof(ChipBody));
	luaL_getmetatable(L, CHIPBODY);
	lua_setmetatable(L, -2);
	chipbody->body = body;
	chipbody->index = g_ChipBodies.Size();
	chipbody->chipspace = 0;

	if (g_ChipBodies.Full()) {
		g_ChipBodies.SetCapacity(g_ChipBodies.Capacity() + MAX_BODIES);
	}
	g_ChipBodies.Push(chipbody);

	assert(top + 1 == lua_gettop(L));
}

static ChipBody* CheckBody(lua_State *L, int index) {
	luaL_checktype(L, index, LUA_TUSERDATA);
	ChipBody *chipbody = (ChipBody *)luaL_checkudata(L, index, CHIPBODY);
	if (chipbody == NULL) luaL_typerror(L, index, CHIPBODY);
	return chipbody;
}

static int ChipBody_gc(lua_State* L) {
	dmLogInfo("ChipBody_gc");
	ChipBody *chipbody = CheckBody(L, 1);
	cpBodyFree(chipbody->body);
	ChipBody *swapped = g_ChipBodies.EraseSwap(chipbody->index);
	if (swapped != 0) {
		swapped->index = chipbody->index;
	}
	return 0;
}

static int ChipBody_tostring(lua_State* L) {
	CheckBody(L, 1);
	lua_pushfstring(L, "[ChipBody]");
	return 1;
}
static const luaL_reg ChipBody_metamethods[] = {
	{"__gc", ChipBody_gc},
	{"__tostring", ChipBody_tostring},
	{0, 0}
};

static int Chipmunk_NewCircleBody(lua_State* L) {
	dmLogInfo("Chipmunk_NewCircleBody");
	int top = lua_gettop(L);

	cpFloat radius = lua_tonumber(L, 1);
	cpFloat mass = lua_tonumber(L, 2);
	cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);
	cpBody *body = cpBodyNew(mass, moment);

	PushBody(L, body);

	assert(top + 1 == lua_gettop(L));
	return 1;
}

static int Chipmunk_NewBoxBody(lua_State* L) {
	dmLogInfo("Chipmunk_NewBoxBody");
	int top = lua_gettop(L);

	cpFloat width = lua_tonumber(L, 1);
	cpFloat height = lua_tonumber(L, 2);
	cpFloat mass = lua_tonumber(L, 3);
	cpFloat moment = cpMomentForBox(mass, width, height);
	cpBody *body = cpBodyNew(mass, moment);

	PushBody(L, body);

	assert(top + 1 == lua_gettop(L));
	return 1;
}

static int Chipmunk_NewStaticBody(lua_State* L) {
	dmLogInfo("Chipmunk_NewStaticBody");
	int top = lua_gettop(L);

	cpBody *body = cpBodyNewStatic();

	PushBody(L, body);

	assert(top + 1 == lua_gettop(L));
	return 1;
}

static int Chipmunk_GetStaticBody(lua_State* L) {
	dmLogInfo("Chipmunk_GetStaticBody");
	int top = lua_gettop(L);

	ChipSpace *chipspace = CheckSpace(L, 1);
	cpBody *body = cpSpaceGetStaticBody(chipspace->space);

	PushBody(L, body);

	assert(top + 1 == lua_gettop(L));
	return 1;
}

static int Chipmunk_SetBodyPosition(lua_State* L) {
	int top = lua_gettop(L);

	ChipBody *chipbody = CheckBody(L, 1);
	cpFloat x = lua_tonumber(L, 2);
	cpFloat y = lua_tonumber(L, 3);
	cpBodySetPosition(chipbody->body, cpv(x, y));

	assert(top == lua_gettop(L));
	return 0;
}

static int Chipmunk_GetBodyPosition(lua_State* L) {
	int top = lua_gettop(L);

	ChipBody *chipbody = CheckBody(L, 1);
	cpVect pos = cpBodyGetPosition(chipbody->body);

	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);

	assert(top + 2 == lua_gettop(L));
	return 2;
}

static int Chipmunk_GetBodyRotation(lua_State* L) {
	int top = lua_gettop(L);

	ChipBody *chipbody = CheckBody(L, 1);
	cpFloat angle = cpBodyGetAngle(chipbody->body);

	lua_pushnumber(L, angle);

	assert(top + 1 == lua_gettop(L));
	return 1;
}

static int Chipmunk_GetBodyVelocity(lua_State* L) {
	int top = lua_gettop(L);

	ChipBody *chipbody = CheckBody(L, 1);
	cpVect vel = cpBodyGetVelocity(chipbody->body);

	lua_pushnumber(L, vel.x);
	lua_pushnumber(L, vel.y);

	assert(top + 2 == lua_gettop(L));
	return 2;
}



//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
// CHIP SHAPE
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#define CHIPSHAPE "chipshape"

static ChipShape* CheckShape(lua_State *L, int index) {
	luaL_checktype(L, index, LUA_TUSERDATA);
	ChipShape *chipshape = (ChipShape *)luaL_checkudata(L, index, CHIPSHAPE);
	if (chipshape == NULL) luaL_typerror(L, index, CHIPSHAPE);
	return chipshape;
}

static void PushShape(lua_State* L, cpShape *shape) {
	int top = lua_gettop(L);
	ChipShape *chipshape = (ChipShape *)lua_newuserdata(L, sizeof(ChipShape));
	luaL_getmetatable(L, CHIPSHAPE);
	lua_setmetatable(L, -2);
	chipshape->shape = shape;
	chipshape->index = g_ChipShapes.Size();
	chipshape->chipspace = 0;

	if (g_ChipShapes.Full()) {
		g_ChipShapes.SetCapacity(g_ChipShapes.Capacity() + MAX_SHAPES);
	}
	g_ChipShapes.Push(chipshape);

	assert(top + 1 == lua_gettop(L));
}

static int ChipShape_gc(lua_State* L) {
	dmLogInfo("ChipShape_gc");
	ChipShape *chipshape = CheckShape(L, 1);
	cpShapeFree(chipshape->shape);
	ChipShape *swapped = g_ChipShapes.EraseSwap(chipshape->index);
	if (swapped != 0) {
		swapped->index = chipshape->index;
	}
	return 0;
}

static int ChipShape_tostring(lua_State* L) {
	CheckShape(L, 1);
	lua_pushfstring(L, "[ChipShape]");
	return 1;
}
static const luaL_reg ChipShape_metamethods[] = {
	{"__gc", ChipShape_gc},
	{"__tostring", ChipShape_tostring},
	{0, 0}
};

static int Chipmunk_NewCircleShape(lua_State* L) {
	dmLogInfo("Chipmunk_NewCircleShape");
	int top = lua_gettop(L);

	ChipBody *chipbody = CheckBody(L, 1);
	cpFloat radius = lua_tonumber(L, 2);
	cpShape *shape = cpCircleShapeNew(chipbody->body, radius, cpvzero);
	PushShape(L, shape);

	assert(top + 1 == lua_gettop(L));
	return 1;
}

static int Chipmunk_NewSegmentShape(lua_State* L) {
	dmLogInfo("Chipmunk_NewSegmentShape");
	int top = lua_gettop(L);

	ChipBody *chipbody = CheckBody(L, 1);
	cpFloat ax = lua_tonumber(L, 2);
	cpFloat ay = lua_tonumber(L, 3);
	cpFloat bx = lua_tonumber(L, 4);
	cpFloat by = lua_tonumber(L, 5);
	cpFloat thickness = lua_tonumber(L, 6);
	cpShape *shape = cpSegmentShapeNew(chipbody->body, cpv(ax, ay), cpv(bx, by), thickness);
	PushShape(L, shape);

	assert(top + 1 == lua_gettop(L));
	return 1;
}

static int Chipmunk_NewBoxShape(lua_State* L) {
	dmLogInfo("Chipmunk_NewBoxShape");
	int top = lua_gettop(L);

	ChipBody *chipbody = CheckBody(L, 1);
	cpFloat width = lua_tonumber(L, 2);
	cpFloat height = lua_tonumber(L, 3);
	cpFloat radius = lua_tonumber(L, 4);
	cpShape *shape = cpBoxShapeNew(chipbody->body, width, height, radius);
	PushShape(L, shape);

	assert(top + 1 == lua_gettop(L));
	return 1;
}

static int Chipmunk_SetShapeFriction(lua_State* L) {
	dmLogInfo("Chipmunk_SetShapeFriction");
	int top = lua_gettop(L);

	ChipShape *chipshape = CheckShape(L, 1);
	cpFloat friction = lua_tonumber(L, 2);
	cpShapeSetFriction(chipshape->shape, friction);

	assert(top == lua_gettop(L));
	return 0;
}

static int Chipmunk_SetShapeElasticity(lua_State* L) {
	dmLogInfo("Chipmunk_SetShapeElasticity");
	int top = lua_gettop(L);

	ChipShape *chipshape = CheckShape(L, 1);
	cpFloat elasticity = lua_tonumber(L, 2);
	cpShapeSetElasticity(chipshape->shape, elasticity);

	assert(top == lua_gettop(L));
	return 0;
}



//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
// JOINTS
//-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#define CHIPCONSTRAINT "chipconstraint"

static ChipConstraint* CheckConstraint(lua_State *L, int index) {
	luaL_checktype(L, index, LUA_TUSERDATA);
	ChipConstraint *chipshape = (ChipConstraint *)luaL_checkudata(L, index, CHIPCONSTRAINT);
	if (chipshape == NULL) luaL_typerror(L, index, CHIPCONSTRAINT);
	return chipshape;
}

static void PushConstraint(lua_State* L, cpConstraint *constraint) {
	int top = lua_gettop(L);
	ChipConstraint *chipconstraint = (ChipConstraint *)lua_newuserdata(L, sizeof(ChipConstraint));
	luaL_getmetatable(L, CHIPCONSTRAINT);
	lua_setmetatable(L, -2);
	chipconstraint->constraint = constraint;
	chipconstraint->index = g_ChipConstraints.Size();

	if (g_ChipConstraints.Full()) {
		g_ChipConstraints.SetCapacity(g_ChipConstraints.Capacity() + MAX_CONSTRAINTS);
	}
	g_ChipConstraints.Push(chipconstraint);

	assert(top + 1 == lua_gettop(L));
}

static int ChipConstraint_gc(lua_State* L) {
	dmLogInfo("ChipConstraint_gc");
	ChipConstraint *chipsconstraint = CheckConstraint(L, 1);
	cpConstraintFree(chipsconstraint->constraint);
	ChipConstraint *swapped = g_ChipConstraints.EraseSwap(chipsconstraint->index);
	if (swapped != 0) {
		swapped->index = chipsconstraint->index;
	}
	return 0;
}

static int ChipConstraint_tostring(lua_State* L) {
	CheckConstraint(L, 1);
	lua_pushfstring(L, "[ChipConstraint]");
	return 1;
}
static const luaL_reg ChipConstraint_metamethods[] = {
	{"__gc", ChipConstraint_gc},
	{"__tostring", ChipConstraint_tostring},
	{0, 0}
};

static int Chipmunk_NewDampedSpring(lua_State* L) {
	dmLogInfo("Chipmunk_NewDampedSpring");
	int top = lua_gettop(L);

	ChipBody *chipbody1 = CheckBody(L, 1);
	ChipBody *chipbody2 = CheckBody(L, 2);
	cpFloat a1x = lua_tonumber(L, 3);
	cpFloat a1y = lua_tonumber(L, 4);
	cpFloat a2x = lua_tonumber(L, 5);
	cpFloat a2y = lua_tonumber(L, 6);
	cpFloat restLength = lua_tonumber(L, 7);
	cpFloat stiffness = lua_tonumber(L, 8);
	cpFloat damping = lua_tonumber(L, 9);

	cpConstraint *constraint = cpDampedSpringNew(chipbody1->body, chipbody2->body, cpv(a1x, a1y), cpv(a2x, a2y), restLength, stiffness, damping);
	PushConstraint(L, constraint);

	assert(top + 1 == lua_gettop(L));
	return 1;
}

static int Chipmunk_NewSlideJoint(lua_State* L) {
	dmLogInfo("Chipmunk_NewSlideJoint");
	int top = lua_gettop(L);

	ChipBody *chipbody1 = CheckBody(L, 1);
	ChipBody *chipbody2 = CheckBody(L, 2);
	cpFloat a1x = lua_tonumber(L, 3);
	cpFloat a1y = lua_tonumber(L, 4);
	cpFloat a2x = lua_tonumber(L, 5);
	cpFloat a2y = lua_tonumber(L, 6);
	cpFloat min = lua_tonumber(L, 7);
	cpFloat max = lua_tonumber(L, 8);

	cpConstraint *constraint = cpSlideJointNew(chipbody1->body, chipbody2->body, cpv(a1x, a1y), cpv(a2x, a2y), min, max);
	cpConstraintSetMaxForce(constraint, 80000.0);
	cpConstraintSetCollideBodies(constraint, cpFalse);
	PushConstraint(L, constraint);

	assert(top + 1 == lua_gettop(L));
	return 1;
}


//-------------------------------------------------------

static int Chipmunk_AddBody(lua_State* L) {
	int top = lua_gettop(L);

	dmLogInfo("Chipmunk_AddBody");
	ChipSpace *chipspace = CheckSpace(L, 1);
	ChipBody *chipbody = CheckBody(L, 2);

	cpSpaceAddBody(chipspace->space, chipbody->body);
	chipbody->chipspace = chipspace;

	assert(top == lua_gettop(L));
	return 0;
}

static int Chipmunk_AddShape(lua_State* L) {
	int top = lua_gettop(L);

	dmLogInfo("Chipmunk_AddShape");
	ChipSpace *chipspace = CheckSpace(L, 1);
	ChipShape *chipshape = CheckShape(L, 2);

	cpSpaceAddShape(chipspace->space, chipshape->shape);
	chipshape->chipspace = chipspace;

	assert(top == lua_gettop(L));
	return 0;
}

static int Chipmunk_AddConstraint(lua_State* L) {
	int top = lua_gettop(L);

	dmLogInfo("Chipmunk_AddConstraint");
	ChipSpace *chipspace = CheckSpace(L, 1);
	ChipConstraint *chipconstraint = CheckConstraint(L, 2);

	cpSpaceAddConstraint(chipspace->space, chipconstraint->constraint);
	chipconstraint->chipspace = chipspace;

	assert(top == lua_gettop(L));
	return 0;
}

static int Chipmunk_SpaceStep(lua_State* L) {
	int top = lua_gettop(L);

	ChipSpace *chipspace = CheckSpace(L, 1);
	float dt = luaL_checknumber(L, 2);

	cpSpaceStep(chipspace->space, dt);

	assert(top == lua_gettop(L));
	return 0;
}


static const luaL_reg Module_methods[] = {
	{"new_space", Chipmunk_NewSpace},

	{"new_circle_body", Chipmunk_NewCircleBody},
	{"new_box_body", Chipmunk_NewBoxBody},
	{"new_static_body", Chipmunk_NewStaticBody},
	//{"get_static_body", Chipmunk_GetStaticBody},

	{"new_circle_shape", Chipmunk_NewCircleShape},
	{"new_box_shape", Chipmunk_NewBoxShape},
	{"new_segment_shape", Chipmunk_NewSegmentShape},
	{"set_shape_friction", Chipmunk_SetShapeFriction},
	{"set_shape_elasticity", Chipmunk_SetShapeElasticity},

	{"add_body", Chipmunk_AddBody},
	{"add_shape", Chipmunk_AddShape},
	{"add_constraint", Chipmunk_AddConstraint},
	{"space_step", Chipmunk_SpaceStep},

	{"set_body_position", Chipmunk_SetBodyPosition},
	{"get_body_position", Chipmunk_GetBodyPosition},
	{"get_body_rotation", Chipmunk_GetBodyRotation},
	{"get_body_velocity", Chipmunk_GetBodyVelocity},

	{"new_damped_spring", Chipmunk_NewDampedSpring},
	{"new_slide_joint", Chipmunk_NewSlideJoint},
	{0, 0}
};

static void LuaInit(lua_State* L) {
    int top = lua_gettop(L);
    luaL_register(L, MODULE_NAME, Module_methods);
    lua_pop(L, 1);

	luaL_newmetatable(L, CHIPSPACE);
	luaL_register(L, NULL, ChipSpace_metamethods);
	lua_pop(L, 1);

	luaL_newmetatable(L, CHIPBODY);
	luaL_register(L, NULL, ChipBody_metamethods);
	lua_pop(L, 1);

	luaL_newmetatable(L, CHIPSHAPE);
	luaL_register(L, NULL, ChipShape_metamethods);
	lua_pop(L, 1);

	luaL_newmetatable(L, CHIPCONSTRAINT);
	luaL_register(L, NULL, ChipConstraint_metamethods);
	lua_pop(L, 1);

    assert(top == lua_gettop(L));
}

dmExtension::Result AppInitializeChipmunkExtension(dmExtension::AppParams* params) {
    return dmExtension::RESULT_OK;
}

dmExtension::Result InitializeChipmunkExtension(dmExtension::Params* params) {
	LuaInit(params->m_L);
	return dmExtension::RESULT_OK;
}

dmExtension::Result AppFinalizeChipmunkExtension(dmExtension::AppParams* params) {
	return dmExtension::RESULT_OK;
}

dmExtension::Result FinalizeChipmunkExtension(dmExtension::Params* params) {
	return dmExtension::RESULT_OK;
}

dmExtension::Result UpdateChipmunkExtension(dmExtension::Params* params) {
	return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(Chipmunk, LIB_NAME, AppInitializeChipmunkExtension, AppFinalizeChipmunkExtension, InitializeChipmunkExtension, UpdateChipmunkExtension, 0, FinalizeChipmunkExtension)
