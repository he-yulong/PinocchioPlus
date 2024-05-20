/*  This file is part of the Pinocchio automatic rigging library.
	Copyright (C) 2007 Ilya Baran (ibaran@mit.edu)

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "pinocchioApi.h"
#include "deriv.h"
#include <list>
#include "tools/Log.h"

// fits mesh inside unit cube, makes sure there's exactly one connected component
Mesh prepareMesh(const Mesh &mesh)
{
	Mesh out = mesh;

	if (!mesh.isConnected())
	{
		std::cout << "Bad mesh: should be a single connected component" << std::endl;
		return Mesh();
	}

	out.computeVertexNormals();
	out.normalizeBoundingBox();

	return out;
}

// constructs a distance field on an octree based on a given mesh--user responsible for deleting output
TreeType *constructDistanceField(const Mesh &mesh, double tol)
{
	std::vector<Tri3Object> triobjvec;
	for (int i = 0; i < mesh.edges.size(); i += 3)
	{
		Vector3 v1 = mesh.vertices[mesh.edges[i].vertex].pos;
		Vector3 v2 = mesh.vertices[mesh.edges[i + 1].vertex].pos;
		Vector3 v3 = mesh.vertices[mesh.edges[i + 2].vertex].pos;

		triobjvec.push_back(Tri3Object(v1, v2, v3));
	}

	ObjectProjector<3, Tri3Object> proj(triobjvec);

	TreeType *out = OctTreeMaker<TreeType>().make(proj, mesh, tol);

	PP_CORE_INFO("Done fullSplit {} {}", out->countNodes(), out->maxLevel());

	return out;  // Finally, it returns the pointer to the constructed octree.
}

double getMinDot(TreeType *distanceField, const Vector3 &c, double step)
{
	typedef Deriv<double, 3> D;
	typedef Vector<D, 3> VD;

	int i, j;
	vector<Vector3> vecs;
	vecs.push_back(Vector3(step, step, step));
	vecs.push_back(Vector3(step, step, -step));
	vecs.push_back(Vector3(step, -step, step));
	vecs.push_back(Vector3(step, -step, -step));
	vecs.push_back(Vector3(-step, step, step));
	vecs.push_back(Vector3(-step, step, -step));
	vecs.push_back(Vector3(-step, -step, step));
	vecs.push_back(Vector3(-step, -step, -step));

	for (i = 0; i < (int)vecs.size(); ++i)
	{
		vecs[i] += c;
		VD vd = VD(D(vecs[i][0], 0), D(vecs[i][1], 1), D(vecs[i][2], 2));

		D result = distanceField->locate(vecs[i])->evaluate(vd);
		vecs[i] = Vector3(result.getDeriv(0), result.getDeriv(1), result.getDeriv(2)).normalize();
	}

	double minDot = 1.;

	for (i = 1; i < (int)vecs.size(); ++i)
		for (j = 0; j < i; ++j)
		{
			minDot = min(minDot, vecs[i] * vecs[j]);
		}

	return minDot;
}

bool sphereComp(const Sphere &s1, const Sphere &s2) { return s1.radius > s2.radius; }

// samples the distance field to find spheres on the medial surface
// output is sorted by radius in decreasing order
std::vector<Sphere> sampleMedialSurface(TreeType *distanceField, double tol)
{
	std::vector<Sphere> out;

	/*
	It uses a breadth-first search (BFS) approach to traverse the octree.
	It processes nodes level by level, starting from the root.
	*/
	list<OctTreeNode*> queue;
	queue.push_back(distanceField);
	int iter = 0;
	while (!queue.empty())
	{
		OctTreeNode *cur = queue.front();
		queue.pop_front();
		iter++;
		if (cur->getChild(0))
		{
			for (int i = 0; i < 8; ++i)
			{
				queue.push_back(cur->getChild(i));
			}
			continue;
		}

		// we are at octree leaf
		// If the current node is a leaf (no children), it calculates
		// The bounding box (Rect3 r) of the node.
		// The radius (rad) of the bounding box.
		// The center (c) of the bounding box.
		// It then calls getMinDot to determine if the current point is near the medial surface. 
		// If dot > 0., it continues to the next iteration.
		Rect3 r = cur->getRect();
		double rad = r.getSize().length() / 2.;
		Vector3 c = r.getCenter();
		double dot = getMinDot(distanceField, c, rad);
		if (dot > 0.) continue;
			

		// we are likely near medial surface
		// Sampling Points on the Medial Surface
		// If the node is likely near the medial surface, it samples points on the faces of the bounding box
		double step = tol;
		double x, y;
		vector<Vector3> pts;
		double sz = r.getSize()[0];
		for (x = 0; x <= sz; x += step)
			for (y = 0; y <= sz; y += step)
			{
				pts.push_back(r.getLo() + Vector3(x, y, 0));
				if (y != 0.) pts.push_back(r.getLo() + Vector3(x, 0, y));
				if (x != 0. && y != 0.) pts.push_back(r.getLo() + Vector3(0, x, y));
			}

		// pts now contains a grid on 3 of the octree cell faces (that's enough)
		// cout << "pts.size():" << pts.size() << endl;
		for (int i = 0; i < pts.size(); ++i)
		{
			Vector3 &p = pts[i];
			double dist = -distanceField->locate(p)->evaluate(p);
			if (dist <= 2. * step) continue; // we want to be well inside
			double dot = getMinDot(distanceField, p, step * 0.001);
			if (dot > 0.0) continue;
			out.push_back(Sphere(p, dist));
		}
	}
	
	PP_CORE_INFO("Medial axis points = {}", out.size());

	std::sort(out.begin(), out.end(), sphereComp);

	return out;
}

// takes sorted medial surface samples and sparsifies the vector
vector<Sphere> packSpheres(const vector<Sphere> &samples, int maxSpheres)
{
	int i, j;
	vector<Sphere> out;

	for (i = 0; i < (int)samples.size(); ++i)
	{
		for (j = 0; j < (int)out.size(); ++j)
		{
			if ((out[j].center - samples[i].center).lengthsq() < SQR(out[j].radius))
				break;
		}
		if (j < (int)out.size())
			continue;

		out.push_back(samples[i]);
		if ((int)out.size() > maxSpheres)
			break;
	}

	return out;
}

double getMaxDist(TreeType *distanceField, const Vector3 &v1, const Vector3 &v2, double maxAllowed)
{
	double maxDist = -1e37;
	Vector3 diff = (v2 - v1) / 100.;
	for (int k = 0; k < 101; ++k)
	{
		Vector3 pt = v1 + diff * double(k);
		maxDist = max(maxDist, distanceField->locate(pt)->evaluate(pt));
		if (maxDist > maxAllowed)
			break;
	}
	return maxDist;
}

// constructs graph on packed sphere centers
PtGraph connectSamples(TreeType *distanceField, const std::vector<Sphere> &spheres)
{
	PtGraph out;

	for (int i = 0; i < spheres.size(); ++i)
		out.verts.push_back(spheres[i].center);
	out.edges.resize(spheres.size());

	for (int i = 1; i < spheres.size(); ++i)
		for (int j = 0; j < i; ++j)
		{
			int k;
			Vector3 ctr = (spheres[i].center + spheres[j].center) * 0.5;
			double radsq = (spheres[i].center - spheres[j].center).lengthsq() * 0.25;
			if (radsq < SQR(spheres[i].radius + spheres[j].radius) * 0.25)
			{ // if spheres intersect, there should be an edge
				out.edges[i].push_back(j);
				out.edges[j].push_back(i);
				continue;
			}
			for (k = 0; k < (int)spheres.size(); ++k)
			{
				if (k == i || k == j) continue;
				if ((spheres[k].center - ctr).lengthsq() < radsq)
					break; // gabriel graph condition violation
			}
			if (k < (int)spheres.size()) continue;
			// every point on edge should be at least this far in:
			double maxAllowed = -.5 * min(spheres[i].radius, spheres[j].radius);
			if (getMaxDist(distanceField, spheres[i].center, spheres[j].center, maxAllowed) < maxAllowed)
			{
				out.edges[i].push_back(j);
				out.edges[j].push_back(i);
			}
		}

	return out;
}
