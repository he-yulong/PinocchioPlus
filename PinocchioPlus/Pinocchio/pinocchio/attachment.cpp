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

#include <fstream>
#include <sstream>
#include "attachment.h"
#include "math/vecutils.h"
#include "lsqSolver.h"
#include "tools/Log.h"

class AttachmentPrivate
{
public:
	AttachmentPrivate() {}
	virtual ~AttachmentPrivate() {}
	virtual Mesh deform(const Mesh& mesh, const vector<Transform<>>& transforms, SkinningMethod method) const = 0;
	virtual Vector<double, -1> getWeights(int i) const = 0;
	virtual AttachmentPrivate* clone() const = 0;
};

bool vectorInCone(const Vector3& v, const vector<Vector3>& ns)
{
	int i;
	Vector3 avg;
	for (i = 0; i < (int)ns.size(); ++i)
		avg += ns[i];

	return v.normalize() * avg.normalize() > 0.5;
}

class AttachmentPrivate1 : public AttachmentPrivate
{
public:
	AttachmentPrivate1() {}

	AttachmentPrivate1(const Mesh& mesh, const Skeleton& skeleton, const vector<Vector3>& match, const VisibilityTester* tester,
		double initialHeatWeight)
	{
		int i, j;
		int nv = mesh.vertices.size();
		// compute edges
		vector<vector<int>> edges(nv); // one-ring neighbors

		for (i = 0; i < nv; ++i)
		{
			int cur, start;
			cur = start = mesh.vertices[i].edge;
			do
			{
				edges[i].push_back(mesh.edges[cur].vertex);
				cur = mesh.edges[mesh.edges[cur].prev].twin;
			} while (cur != start);
		}

		weights.resize(nv);
		int bones = skeleton.fGraph().verts.size() - 1;

		for (i = 0; i < nv; ++i) // initialize the weights vectors so they are big enough
			weights[i][bones - 1] = 0.;

		vector<vector<double>> boneDists(nv);
		vector<vector<bool>> boneVis(nv);

		for (i = 0; i < nv; ++i)
		{
			boneDists[i].resize(bones, -1);
			boneVis[i].resize(bones);
			Vector3 cPos = mesh.vertices[i].pos;

			vector<Vector3> normals;
			for (j = 0; j < (int)edges[i].size(); ++j)
			{
				int nj = (j + 1) % edges[i].size();
				Vector3 v1 = mesh.vertices[edges[i][j]].pos - cPos;
				Vector3 v2 = mesh.vertices[edges[i][nj]].pos - cPos;
				normals.push_back((v1 % v2).normalize());
			}

			double minDist = 1e37; // cal the distance from vertex_i to each bone
			for (j = 1; j <= bones; ++j)
			{
				const Vector3& v1 = match[j], & v2 = match[skeleton.fPrev()[j]];
				boneDists[i][j - 1] = sqrt(distsqToSeg(cPos, v1, v2));
				minDist = min(boneDists[i][j - 1], minDist);
			}
			for (j = 1; j <= bones; ++j)
			{
				// the reason we don't just pick the closest bone is so that if two are
				// equally close, both are factored in.
				if (boneDists[i][j - 1] > minDist * 1.0001)
					continue;

				const Vector3& v1 = match[j], & v2 = match[skeleton.fPrev()[j]];
				Vector3 p = projToSeg(cPos, v1, v2);                                            // cansee: line segment (cpos-projected cpos on bone) inside the surface
				boneVis[i][j - 1] = tester->canSee(cPos, p) && vectorInCone(cPos - p, normals); // vector_in_cone: line segment inside the radiance field of vertex_i's one ring
			}
		}

		// We have -Lw+Hw=HI, same as (H-L)w=HI, with (H-L)=DA (with D=diag(1./area))
		// so w = A^-1 (HI/D)

		vector<vector<pair<int, double>>> A(nv);
		vector<double> D(nv, 0.), H(nv, 0.);
		vector<int> closest(nv, -1);
		for (i = 0; i < nv; ++i)
		{
			// get areas
			for (j = 0; j < (int)edges[i].size(); ++j)
			{
				int nj = (j + 1) % edges[i].size();

				D[i] += ((mesh.vertices[edges[i][j]].pos - mesh.vertices[i].pos) %
					(mesh.vertices[edges[i][nj]].pos - mesh.vertices[i].pos))
					.length();
			}
			D[i] = 1. / (1e-10 + D[i]);

			// get bones
			double minDist = 1e37;
			for (j = 0; j < bones; ++j)
			{
				// Would like to change to:
				//   if(boneDists[i][j] < minDist && boneVis[i][j])
				// but need to make boneVis more robust - ie, check
				// if the bone is initially outside the mesh, etc
				if (boneDists[i][j] < minDist)
				{
					closest[i] = j;
					minDist = boneDists[i][j];
				}
			}
			for (j = 0; j < bones; ++j)
				if (boneVis[i][j] && boneDists[i][j] <= minDist * 1.00001)
					H[i] += initialHeatWeight / SQR(1e-8 + boneDists[i][closest[i]]);

			// get laplacian
			double sum = 0.;
			for (j = 0; j < (int)edges[i].size(); ++j)
			{
				int nj = (j + 1) % edges[i].size();
				int pj = (j + edges[i].size() - 1) % edges[i].size();

				Vector3 v1 = mesh.vertices[i].pos - mesh.vertices[edges[i][pj]].pos;
				Vector3 v2 = mesh.vertices[edges[i][j]].pos - mesh.vertices[edges[i][pj]].pos;
				Vector3 v3 = mesh.vertices[i].pos - mesh.vertices[edges[i][nj]].pos;
				Vector3 v4 = mesh.vertices[edges[i][j]].pos - mesh.vertices[edges[i][nj]].pos;

				double cot1 = (v1 * v2) / (1e-6 + (v1 % v2).length());
				double cot2 = (v3 * v4) / (1e-6 + (v3 % v4).length());
				sum += (cot1 + cot2);

				if (edges[i][j] > i) // check for triangular here because sum should be computed regardless
					continue;
				A[i].push_back(make_pair(edges[i][j], -cot1 - cot2));
			}

			A[i].push_back(make_pair(i, sum + H[i] / D[i]));

			sort(A[i].begin(), A[i].end());
		}

		nzweights.resize(nv);
		SPDMatrix Am(A);
		LLTMatrix* Ainv = Am.factor();
		if (Ainv == NULL)
			return;

		for (j = 0; j < bones; ++j)
		{
			vector<double> rhs(nv, 0.);
			for (i = 0; i < nv; ++i)
			{
				if (boneVis[i][j] && boneDists[i][j] <= boneDists[i][closest[i]] * 1.00001)
					rhs[i] = H[i] / D[i];
			}

			Ainv->solve(rhs);
			for (i = 0; i < nv; ++i)
			{
				if (rhs[i] > 1.)
					rhs[i] = 1.; // clip just in case
				if (rhs[i] > 1e-8)
					nzweights[i].push_back(make_pair(j, rhs[i]));
			}
		}

		for (i = 0; i < nv; ++i)
		{
			double sum = 0.;
			for (j = 0; j < (int)nzweights[i].size(); ++j)
				sum += nzweights[i][j].second;

			for (j = 0; j < (int)nzweights[i].size(); ++j)
			{
				nzweights[i][j].second /= sum;
				weights[i][nzweights[i][j].first] = nzweights[i][j].second;
			}
		}

		delete Ainv;
		return;
	}


	// 假设 Vector3 和 Transform 都是 Eigen 类型，或者类似的支持并行的库
	Mesh deform(const Mesh& mesh, const std::vector<Transform<>>& transforms, SkinningMethod method) const {
		Mesh out = mesh;
		int nv = mesh.vertices.size();
		if (mesh.vertices.size() != weights.size()) return out; // error
		
		switch (method)
		{
		case SkinningMethod::LBS:
#pragma omp parallel for
			for (int i = 0; i < nv; ++i) {
				Vector3 newPos(0.0, 0.0, 0.0); // initialize newPos to zero vector
				for (const auto& weightPair : nzweights[i]) {
					int boneIndex = weightPair.first;
					double weight = weightPair.second;
					newPos += (transforms[boneIndex] * mesh.vertices[i].pos) * weight;
				}
				out.vertices[i].pos = newPos;
			}
			break;
		case SkinningMethod::DQS:
			break;
		default:
			break;
		}

		out.computeVertexNormals();

		return out;
	}

	Vector<double, -1> getWeights(int i) const { return weights[i]; }

	AttachmentPrivate* clone() const
	{
		AttachmentPrivate1* out = new AttachmentPrivate1();
		*out = *this;
		return out;
	}

private:
	vector<Vector<double, -1>> weights;
	vector<vector<pair<int, double>>> nzweights; // sparse representation
};

Attachment::~Attachment()
{
	if (m_AttachmentCore)
		delete m_AttachmentCore;
}

Attachment::Attachment(const Attachment& att)
{
	m_AttachmentCore = att.m_AttachmentCore->clone();
}
// vertex_i's weights from all bones
Vector<double, -1> Attachment::getWeights(int i) const { return m_AttachmentCore->getWeights(i); }

// perform in every frame
Mesh Attachment::deform(const Mesh& mesh, const vector<Transform<>>& transforms) const
{
	return m_AttachmentCore->deform(mesh, transforms, m_SkinningMethod);
}

Attachment::Attachment(const Mesh& mesh, const Skeleton& skeleton, const vector<Vector3>& match, const VisibilityTester* tester,
	double initialHeatWeight)
{
	m_AttachmentCore = new AttachmentPrivate1(mesh, skeleton, match, tester, initialHeatWeight);
}
