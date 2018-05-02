#ifndef CHIPMUNKDEFOLD_H
#define CHIPMUNKDEFOLD_H

#include <chipmunk/chipmunk.h>
#include <dmsdk/dlib/array.h>

struct ChipSpace {
	uint32_t index;
	cpSpace *space;
};

struct ChipShape {
	uint32_t index;
	cpShape *shape;
	ChipSpace *chipspace; // the space this shape has been added to (can be null)
};

struct ChipBody {
	uint32_t index;
	cpBody *body;
	ChipSpace *chipspace; // the space this body has been added to (can be null)
};

struct ChipConstraint {
	uint32_t index;
	cpConstraint *constraint;
	ChipSpace *chipspace; // the space this body has been added to (can be null)
};




#endif
