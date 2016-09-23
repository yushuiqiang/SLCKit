#include "slckit.h"

struct SLCKitTypes
{
	SLCKitTypes ()
	{
		qRegisterMetaTypeStreamOperators<Point> ("Point");
		qRegisterMetaTypeStreamOperators<Polygon> ("Polygon");
		qRegisterMetaTypeStreamOperators<Layer> ("Layer");
		qRegisterMetaTypeStreamOperators<Model> ("Model");
		qRegisterMetaTypeStreamOperators<Boundary> ("Boundary");
	}
};

static SLCKitTypes initSLCKitTypes;

