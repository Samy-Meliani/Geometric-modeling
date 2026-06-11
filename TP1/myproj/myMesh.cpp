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
	if (faces.empty() || halfedges.empty()) {
		return;
	}
	vector<myVertex*> liste_sommets;
	vector<myHalfedge*> liste_aretes;
	vector<myFace*> liste_faces;

	map<myFace*, myVertex*> centres_faces;
	map<pair<int, int>, myVertex*> milieux_aretes;
	map<myVertex*, myVertex*> anciens_sommets_deplaces;
	for (size_t i = 0; i < faces.size(); i++) {
		if (faces[i] == NULL) continue;

		myHalfedge* prem_edge = faces[i]->adjacent_halfedge;
		myHalfedge* arete_courante = prem_edge;
		double nb_sommets = 0.0;
		double sommeX = 0.0, sommeY = 0.0, sommeZ = 0.0;

		do {
			if (arete_courante->source && arete_courante->source->point) {
				sommeX += arete_courante->source->point->X;
				sommeY += arete_courante->source->point->Y;
				sommeZ += arete_courante->source->point->Z;
				nb_sommets += 1.0;
			}
			arete_courante = arete_courante->next;
		} while (arete_courante != prem_edge && arete_courante != NULL);

		if (nb_sommets > 0.0) {
			myVertex* som_face = new myVertex();
			som_face->point = new myPoint3D(sommeX / nb_sommets, sommeY / nb_sommets, sommeZ / nb_sommets);
			centres_faces[faces[i]] = som_face;
			liste_sommets.push_back(som_face);
		}
	}
	for (size_t i = 0; i < halfedges.size(); i++) {
		myHalfedge* edge_actuelle = halfedges[i];
		if (edge_actuelle == NULL || edge_actuelle->twin == NULL) continue;

		int idx1 = edge_actuelle->source->index;
		int idx2 = edge_actuelle->twin->source->index;
		pair<int, int> cle_directe = make_pair(idx1, idx2);
		pair<int, int> cle_inverse = make_pair(idx2, idx1);

		if (milieux_aretes.find(cle_directe) != milieux_aretes.end()) {
			continue; 
		}

		myVertex* f_pt1 = centres_faces[edge_actuelle->adjacent_face];
		myVertex* f_pt2 = centres_faces[edge_actuelle->twin->adjacent_face];

		if (f_pt1 != NULL && f_pt2 != NULL) {
			double moyX = (edge_actuelle->source->point->X + edge_actuelle->twin->source->point->X + f_pt1->point->X + f_pt2->point->X) * 0.25;
			double moyY = (edge_actuelle->source->point->Y + edge_actuelle->twin->source->point->Y + f_pt1->point->Y + f_pt2->point->Y) * 0.25;
			double moyZ = (edge_actuelle->source->point->Z + edge_actuelle->twin->source->point->Z + f_pt1->point->Z + f_pt2->point->Z) * 0.25;

			myVertex* som_arete = new myVertex();
			som_arete->point = new myPoint3D(moyX, moyY, moyZ);
			milieux_aretes[cle_directe] = som_arete;
			milieux_aretes[cle_inverse] = som_arete;
			liste_sommets.push_back(som_arete);
		}
	}
	vector<double> nb_voisins(vertices.size(), 0.0);
	vector<double> accumulation_fx(vertices.size(), 0.0), accumulation_fy(vertices.size(), 0.0), accumulation_fz(vertices.size(), 0.0);
	vector<double> accumulation_ex(vertices.size(), 0.0), accumulation_ey(vertices.size(), 0.0), accumulation_ez(vertices.size(), 0.0);

	for (size_t i = 0; i < halfedges.size(); i++) {
		myHalfedge* loop_he = halfedges[i];
		if (loop_he == NULL || loop_he->twin == NULL) continue;

		int v_idx = loop_he->source->index;
		nb_voisins[v_idx] += 1.0;

		myVertex* f_node = centres_faces[loop_he->adjacent_face];
		if (f_node != NULL) {
			accumulation_fx[v_idx] += f_node->point->X;
			accumulation_fy[v_idx] += f_node->point->Y;
			accumulation_fz[v_idx] += f_node->point->Z;
		}

		accumulation_ex[v_idx] += (loop_he->source->point->X + loop_he->twin->source->point->X) * 0.5;
		accumulation_ey[v_idx] += (loop_he->source->point->Y + loop_he->twin->source->point->Y) * 0.5;
		accumulation_ez[v_idx] += (loop_he->source->point->Z + loop_he->twin->source->point->Z) * 0.5;
	}

	for (size_t i = 0; i < vertices.size(); i++) {
		myVertex* ancien_v = vertices[i];
		if (ancien_v == NULL) continue;

		double valence = nb_voisins[ancien_v->index];
		if (valence < 3.0) valence = 3.0; 

		double moy_f_x = accumulation_fx[ancien_v->index] / valence;
		double moy_f_y = accumulation_fy[ancien_v->index] / valence;
		double moy_f_z = accumulation_fz[ancien_v->index] / valence;

		double moy_e_x = accumulation_ex[ancien_v->index] / valence;
		double moy_e_y = accumulation_ey[ancien_v->index] / valence;
		double moy_e_z = accumulation_ez[ancien_v->index] / valence;

		double nouvX = (moy_f_x + 2.0 * moy_e_x + (valence - 3.0) * ancien_v->point->X) / valence;
		double nouvY = (moy_f_y + 2.0 * moy_e_y + (valence - 3.0) * ancien_v->point->Y) / valence;
		double nouvZ = (moy_f_z + 2.0 * moy_e_z + (valence - 3.0) * ancien_v->point->Z) / valence;

		myVertex* som_lisse = new myVertex();
		som_lisse->point = new myPoint3D(nouvX, nouvY, nouvZ);
		anciens_sommets_deplaces[ancien_v] = som_lisse;
		liste_sommets.push_back(som_lisse);
	}

	for (size_t i = 0; i < liste_sommets.size(); i++) {
		if (liste_sommets[i] != NULL) {
			liste_sommets[i]->index = i;
		}
	}
	map<pair<int, int>, myHalfedge*> table_liaison_twins;

	for (size_t i = 0; i < faces.size(); i++) {
		myFace* f_courante = faces[i];
		if (f_courante == NULL) continue;

		myHalfedge* prem_edge = f_courante->adjacent_halfedge;
		myHalfedge* arete_courante = prem_edge;
		do {
			myVertex* coin0 = centres_faces[f_courante];
			myVertex* coin1 = milieux_aretes[make_pair(arete_courante->prev->source->index, arete_courante->source->index)];
			myVertex* coin2 = anciens_sommets_deplaces[arete_courante->source];
			myVertex* coin3 = milieux_aretes[make_pair(arete_courante->source->index, arete_courante->twin->source->index)];

			vector<myVertex*> sommets_du_quad;
			sommets_du_quad.push_back(coin0); sommets_du_quad.push_back(coin1);
			sommets_du_quad.push_back(coin2); sommets_du_quad.push_back(coin3);

			myHalfedge** tab_aretes_quad = new myHalfedge * [4];
			for (int k = 0; k < 4; k++) {
				tab_aretes_quad[k] = new myHalfedge();
			}

			myFace* nouv_face_quad = new myFace();
			nouv_face_quad->adjacent_halfedge = tab_aretes_quad[0];

			for (int k = 0; k < 4; k++) {
				int k_suiv = (k + 1) % 4;
				int k_prec = (k - 1 + 4) % 4;
				tab_aretes_quad[k]->next = tab_aretes_quad[k_suiv];
				tab_aretes_quad[k]->prev = tab_aretes_quad[k_prec];
				tab_aretes_quad[k]->adjacent_face = nouv_face_quad;
				myVertex* v_debut = sommets_du_quad[k];
				myVertex* v_fin = sommets_du_quad[k_suiv];
				tab_aretes_quad[k]->source = v_debut;
				v_debut->originof = tab_aretes_quad[k];

				pair<int, int> cle_arete = make_pair(v_debut->index, v_fin->index);
				pair<int, int> cle_jumelle = make_pair(v_fin->index, v_debut->index);

				auto search_it = table_liaison_twins.find(cle_jumelle);
				if (search_it != table_liaison_twins.end()) {
					tab_aretes_quad[k]->twin = search_it->second;
					search_it->second->twin = tab_aretes_quad[k];
				}
				else {
					table_liaison_twins[cle_arete] = tab_aretes_quad[k];
				}
				liste_aretes.push_back(tab_aretes_quad[k]);
			}
			liste_faces.push_back(nouv_face_quad);
			delete[] tab_aretes_quad;
			arete_courante = arete_courante->next;
		} while (arete_courante != prem_edge && arete_courante != NULL);
	}
	clear();
	vertices = liste_sommets;
	halfedges = liste_aretes;
	faces = liste_faces;

	for (unsigned int i = 0; i < vertices.size(); i++) {
		if (vertices[i] != NULL) {
			vertices[i]->index = i;
		}
	}
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