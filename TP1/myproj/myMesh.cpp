#include "myMesh.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <utility>
#include <GL/glew.h>
#include "myvector3d.h"

using namespace std;

myMesh::myMesh(void)
{
	/**** TODO ****/
}


myMesh::~myMesh(void)
{
	/**** TODO ****/
}

void myMesh::clear()
{
	for (unsigned int i = 0; i < vertices.size(); i++) if (vertices[i]) delete vertices[i];
	for (unsigned int i = 0; i < halfedges.size(); i++) if (halfedges[i]) delete halfedges[i];
	for (unsigned int i = 0; i < faces.size(); i++) if (faces[i]) delete faces[i];

	vector<myVertex *> empty_vertices;    vertices.swap(empty_vertices);
	vector<myHalfedge *> empty_halfedges; halfedges.swap(empty_halfedges);
	vector<myFace *> empty_faces;         faces.swap(empty_faces);
}

void myMesh::checkMesh()
{
	vector<myHalfedge *>::iterator it;
	for (it = halfedges.begin(); it != halfedges.end(); it++)
	{
		if ((*it)->twin == NULL)
			break;
	}
	if (it != halfedges.end())
		cout << "Error! Not all edges have their twins!\n";
	else cout << "Each edge has a twin!\n";
}


bool myMesh::readFile(std::string filename)
{
	string s, t, u;
	vector<int> faceids;
	myHalfedge **hedges;

	ifstream fin(filename);
	if (!fin.is_open()) {
		cout << "Unable to open file!\n";
		return false;
	}
	name = filename;

	map<pair<int, int>, myHalfedge *> twin_map;
	map<pair<int, int>, myHalfedge *>::iterator it;

	while (getline(fin, s))
	{
		stringstream myline(s);
		myline >> t;
		if (t == "g") {}
		else if (t == "v")
		{
			float x, y, z;
			myline >> x >> y >> z;
			myPoint3D* p = new myPoint3D(x, y, z);
			myVertex* v = new myVertex;
			v->point = p;
			v->index = vertices.size();
			vertices.push_back(v);
			cout << "v " << x << " " << y << " " << z << endl;
		}
		else if (t == "mtllib") {}
		else if (t == "usemtl") {}
		else if (t == "s") {}
		else if (t == "f")
		{
			faceids.clear();
			while (myline >> u) 
				faceids.push_back(atoi((u.substr(0, u.find("/"))).c_str()) - 1);
			if (faceids.size() < 3) 
				continue;

			hedges = new myHalfedge * [faceids.size()];
			for (unsigned int i = 0; i < faceids.size(); i++)
				hedges[i] = new myHalfedge(); 

			myFace* f = new myFace(); 
			f->adjacent_halfedge = hedges[0]; 

			for (unsigned int i = 0; i < faceids.size(); i++)
			{
				int iplusone = (i + 1) % faceids.size();
				int iminusone = (i - 1 + faceids.size()) % faceids.size();
				
				hedges[i]->next = hedges[iplusone];
				hedges[i]->prev = hedges[iminusone];
				hedges[i]->adjacent_face = f;

				int v_start = faceids[i];
				int v_end = faceids[iplusone];


				pair<int, int> edge_key = make_pair(v_start, v_end);
				pair<int, int> twin_key = make_pair(v_end, v_start);

				it = twin_map.find(twin_key);
				if (it != twin_map.end()) {
					hedges[i]->twin = it->second;
					it->second->twin = hedges[i];
				}
				else {
					twin_map[edge_key] = hedges[i];
				}
				hedges[i]->source = vertices[faceids[i]];
				hedges[i]->source->originof = hedges[i];
				halfedges.push_back(hedges[i]);
			}
			delete[] hedges;
			faces.push_back(f);
			}

	}

	checkMesh();
	normalize();

	return true;
}


void myMesh::computeNormals()
{
	for (unsigned int f = 0; f < faces.size(); f++)
	{
		if (faces[f]) {
			faces[f]->computeNormal();
		}
	}

	for (unsigned int v = 0; v < vertices.size(); v++)
	{
		if (vertices[v]) {
			vertices[v]->computeNormal();
		}
	}
}

void myMesh::normalize()
{
	if (vertices.size() < 1) return;

	int tmpxmin = 0, tmpymin = 0, tmpzmin = 0, tmpxmax = 0, tmpymax = 0, tmpzmax = 0;

	for (unsigned int i = 0; i < vertices.size(); i++) {
		if (vertices[i]->point->X < vertices[tmpxmin]->point->X) tmpxmin = i;
		if (vertices[i]->point->X > vertices[tmpxmax]->point->X) tmpxmax = i;

		if (vertices[i]->point->Y < vertices[tmpymin]->point->Y) tmpymin = i;
		if (vertices[i]->point->Y > vertices[tmpymax]->point->Y) tmpymax = i;

		if (vertices[i]->point->Z < vertices[tmpzmin]->point->Z) tmpzmin = i;
		if (vertices[i]->point->Z > vertices[tmpzmax]->point->Z) tmpzmax = i;
	}

	double xmin = vertices[tmpxmin]->point->X, xmax = vertices[tmpxmax]->point->X,
		ymin = vertices[tmpymin]->point->Y, ymax = vertices[tmpymax]->point->Y,
		zmin = vertices[tmpzmin]->point->Z, zmax = vertices[tmpzmax]->point->Z;

	double scale = (xmax - xmin) > (ymax - ymin) ? (xmax - xmin) : (ymax - ymin);
	scale = scale > (zmax - zmin) ? scale : (zmax - zmin);

	for (unsigned int i = 0; i < vertices.size(); i++) {
		vertices[i]->point->X -= (xmax + xmin) / 2;
		vertices[i]->point->Y -= (ymax + ymin) / 2;
		vertices[i]->point->Z -= (zmax + zmin) / 2;

		vertices[i]->point->X /= scale;
		vertices[i]->point->Y /= scale;
		vertices[i]->point->Z /= scale;
	}
}


void myMesh::splitFaceTRIS(myFace *f, myPoint3D *p)
{
	/**** TODO ****/
}

void myMesh::splitEdge(myHalfedge *e1, myPoint3D *p)
{

	/**** TODO ****/
}

void myMesh::splitFaceQUADS(myFace *f, myPoint3D *p)
{
	/**** TODO ****/
}


void myMesh::subdivisionCatmullClark()
{
	/**** TODO ****/
}


void myMesh::triangulate()
{
	vector<myFace*> originalFaces = faces;

	for (unsigned int i = 0; i < originalFaces.size(); i++) {
		if (originalFaces[i]) {
			triangulate(originalFaces[i]);
		}
	}
}

//return false if already triangle, true othewise.
bool myMesh::triangulate(myFace* f)
{
	if (!f || !f->adjacent_halfedge) return false;

	vector<myHalfedge*> originalEdges;
	myHalfedge* curr = f->adjacent_halfedge;
	do {
		originalEdges.push_back(curr);
		curr = curr->next;
	} while (curr != f->adjacent_halfedge);

	int n = originalEdges.size();

	if (n <= 3) return false;

	myPoint3D* centerPos = new myPoint3D(0.0, 0.0, 0.0);
	for (int i = 0; i < n; i++) {
		*centerPos += *(originalEdges[i]->source->point);
	}
	*centerPos /= (double)n;

	myVertex* vCenter = new myVertex();
	vCenter->point = centerPos;
	vertices.push_back(vCenter);

	vector<myHalfedge*> toCenter(n);
	vector<myHalfedge*> fromCenter(n);
	vector<myFace*> newFaces(n);

	for (int i = 0; i < n; i++) {
		toCenter[i] = new myHalfedge();
		fromCenter[i] = new myHalfedge();

		halfedges.push_back(toCenter[i]);
		halfedges.push_back(fromCenter[i]);

		if (i == 0) {
			newFaces[i] = f; 
		}
		else {
			newFaces[i] = new myFace();
			faces.push_back(newFaces[i]); 
		}
	}

	for (int i = 0; i < n; i++) {
		int next_i = (i + 1) % n;
		myHalfedge* e = originalEdges[i];
		myFace* curFace = newFaces[i];

		toCenter[i]->source = originalEdges[next_i]->source;
		fromCenter[i]->source = vCenter;

		toCenter[i]->twin = fromCenter[next_i];
		fromCenter[next_i]->twin = toCenter[i];

		e->next = toCenter[i];
		toCenter[i]->prev = e;

		toCenter[i]->next = fromCenter[i];
		fromCenter[i]->prev = toCenter[i];

		fromCenter[i]->next = e;
		e->prev = fromCenter[i];

		e->adjacent_face = curFace;
		toCenter[i]->adjacent_face = curFace;
		fromCenter[i]->adjacent_face = curFace;
		curFace->adjacent_halfedge = e;
	}

	vCenter->originof = fromCenter[0];

	return true; 
}

void myMesh::SurfaceDeRevolution(std::vector<myPoint3D*> profile, int slices, myVector3D axis)
{
	clear(); 

	int N = profile.size();
	if (N < 2 || slices < 3) return;

	const double pipi = 3.14;

	std::vector<std::vector<myVertex*>> grid(N, std::vector<myVertex*>(slices));

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < slices; j++) {
			double theta = (2.0 * pipi * j) / slices;

			myPoint3D* p = new myPoint3D(profile[i]->X, profile[i]->Y, profile[i]->Z);
			p->rotate(axis, theta); 
			myVertex* v = new myVertex();
			v->point = p;
			v->index = vertices.size();

			grid[i][j] = v;
			vertices.push_back(v);
		}
	}

	std::map<std::pair<int, int>, myHalfedge*> twin_map;

	for (int i = 0; i < N - 1; i++) {
		for (int j = 0; j < slices; j++) {

			int next_j = (j + 1) % slices;

			myVertex* v0 = grid[i][j];
			myVertex* v1 = grid[i + 1][j];
			myVertex* v2 = grid[i + 1][next_j];
			myVertex* v3 = grid[i][next_j];

			std::vector<myVertex*> face_verts = { v0, v1, v2, v3 };
			int num_verts = 4;
			myHalfedge** hedges = new myHalfedge * [num_verts];
			for (int k = 0; k < num_verts; k++) {
				hedges[k] = new myHalfedge();
			}
			myFace* f = new myFace();
			f->adjacent_halfedge = hedges[0];
			for (int k = 0; k < num_verts; k++) {
				int k_next = (k + 1) % num_verts;
				int k_prev = (k - 1 + num_verts) % num_verts;
				hedges[k]->next = hedges[k_next];
				hedges[k]->prev = hedges[k_prev];
				hedges[k]->adjacent_face = f;
				myVertex* start_v = face_verts[k];
				myVertex* end_v = face_verts[k_next];
				hedges[k]->source = start_v;
				start_v->originof = hedges[k];
				std::pair<int, int> edge_key = std::make_pair(start_v->index, end_v->index);
				std::pair<int, int> twin_key = std::make_pair(end_v->index, start_v->index);
				auto it = twin_map.find(twin_key);
				if (it != twin_map.end()) {
					hedges[k]->twin = it->second;
					it->second->twin = hedges[k];
				}
				else {
					twin_map[edge_key] = hedges[k];
				}

				halfedges.push_back(hedges[k]);
			}

			faces.push_back(f);
			delete[] hedges;
		}
	}

	computeNormals();
	normalize();
}

void myMesh::reduce_shortest() {
	if (halfedges.empty() || faces.size() < 10) {
		return;
	}

	myHalfedge* sml = NULL;
	double minD = 99999.0;

	for (size_t i = 0; i < halfedges.size(); i++) {
		if (halfedges[i] == NULL || halfedges[i]->twin == NULL) {
			continue;
		}

		double l = halfedges[i]->source->point->dist(*(halfedges[i]->twin->source->point));
		if (l < minD) {
			minD = l;
			sml = halfedges[i];
		}
	}

	if (sml != NULL) {
		apply_collapse(sml);
		computeNormals();
	}
}

void myMesh::apply_collapse(myHalfedge* h) {
	if (h == NULL || h->twin == NULL) {
		return;
	}

	myVertex* v_dead = h->source;
	myVertex* v_keep = h->twin->source;

	myFace* f_l = h->adjacent_face;
	myFace* f_r = h->twin->adjacent_face;
	myHalfedge* en = h->next;
	myHalfedge* ep = h->prev;
	myHalfedge* tn = h->twin->next;
	myHalfedge* tp = h->twin->prev;

	myHalfedge* en_twin = NULL;
	if (en != NULL) { en_twin = en->twin; }
	myHalfedge* ep_twin = NULL;
	if (ep != NULL) { ep_twin = ep->twin; }
	myHalfedge* tn_twin = NULL;
	if (tn != NULL) { tn_twin = tn->twin; }
	myHalfedge* tp_twin = NULL;
	if (tp != NULL) { tp_twin = tp->twin; }

	v_keep->point->X = (v_keep->point->X + v_dead->point->X) * 0.5;
	v_keep->point->Y = (v_keep->point->Y + v_dead->point->Y) * 0.5;
	v_keep->point->Z = (v_keep->point->Z + v_dead->point->Z) * 0.5;

	for (size_t i = 0; i < halfedges.size(); i++) {
		if (halfedges[i] != NULL) {
			if (halfedges[i]->source == v_dead) {
				halfedges[i]->source = v_keep;
			}
		}
	}

	if (en_twin != NULL && ep_twin != NULL) {
		en_twin->twin = ep_twin;
		ep_twin->twin = en_twin;
	}

	if (tn_twin != NULL && tp_twin != NULL) {
		tn_twin->twin = tp_twin;
		tp_twin->twin = tn_twin;
	}

	for (auto it = faces.begin(); it != faces.end(); ) {
		if (*it == f_l || *it == f_r) {
			if (*it != NULL) {
				delete* it;
			}
			it = faces.erase(it);
		}
		else {
			it++;
		}
	}

	for (auto it = halfedges.begin(); it != halfedges.end(); ) {
		myHalfedge* cur = *it;
		if (cur == h || cur == h->twin || cur == en || cur == ep || cur == tn || cur == tp) {
			if (*it != NULL) {
				delete* it;
			}
			it = halfedges.erase(it);
		}
		else {
			it++;
		}
	}

	for (auto it = vertices.begin(); it != vertices.end(); ) {
		if (*it == v_dead) {
			if (*it != NULL) {
				delete* it;
			}
			it = vertices.erase(it);
			break;
		}
		else {
			it++;
		}
	}

	for (size_t i = 0; i < vertices.size(); i++) {
		if (vertices[i] != NULL) {
			myHalfedge* o = vertices[i]->originof;
			if (o == NULL || o == h || o == h->twin || o == en || o == ep || o == tn || o == tp) {
				vertices[i]->originof = NULL;
				for (size_t j = 0; j < halfedges.size(); j++) {
					if (halfedges[j] != NULL) {
						if (halfedges[j]->source == vertices[i]) {
							vertices[i]->originof = halfedges[j];
							break;
						}
					}
				}
			}
		}
	}

	for (unsigned int i = 0; i < vertices.size(); i++) {
		if (vertices[i] != NULL) {
			vertices[i]->index = i;
		}
	}
}