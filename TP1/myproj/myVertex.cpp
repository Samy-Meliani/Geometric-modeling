#include "myVertex.h"
#include "myvector3d.h"
#include "myHalfedge.h"
#include "myFace.h"

myVertex::myVertex(void)
{
	point = NULL;
	originof = NULL;
	normal = new myVector3D(1.0,1.0,1.0);
}

myVertex::~myVertex(void)
{
	if (normal) delete normal;
}

void myVertex::computeNormal()
{
    if (!originof) return;

    myVector3D sumNormals(0, 0, 0);
    myHalfedge* start = originof;
    myHalfedge* curr = start;

    do {
        if (curr->adjacent_face) {
            sumNormals += *(curr->adjacent_face->normal);
        }
        if (curr->twin) {
            curr = curr->twin->next;
        }
        else {
            break; 
        }
    } while (curr != start && curr != NULL);

    *normal = sumNormals;
    normal->normalize();
}
