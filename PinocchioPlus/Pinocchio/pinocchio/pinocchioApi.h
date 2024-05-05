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
#pragma once
#ifndef PINOCCHIOAPI_H
#define PINOCCHIOAPI_H

#include "mesh.h"
#include "quaddisttree.h"
#include "attachment.h"

struct PinocchioOutput
{
	PinocchioOutput() : attachment(NULL) {}

	std::vector<Vector3> embedding;
	Attachment *attachment; //user responsible for deletion
};

//calls the other functions and does the whole rigging process
//see the implementation of this function to find out how to use the individual functions
PinocchioOutput autorig(const Skeleton &given, const Mesh &m);

//============================================individual steps=====================================

//fits mesh inside unit cube, makes sure there's exactly one connected component
Mesh prepareMesh(const Mesh &m);


typedef DRootNode<DistData<3>, 3, ArrayIndexer> TreeType; //our distance field octree type
static const double defaultTreeTol = 0.003;

//constructs a distance field on an octree--user responsible for deleting output
TreeType *constructDistanceField(const Mesh &m, double tol = defaultTreeTol);

struct Sphere {
	Sphere() : radius(0.) {}
	Sphere(const Vector3 &inC, double inR) : center(inC), radius(inR) {}

	Vector3 center;
	double radius;
};

//samples the distance field to find spheres on the medial surface
//output is sorted by radius in decreasing order
std::vector<Sphere> sampleMedialSurface(TreeType *distanceField, double tol = defaultTreeTol);

//takes sorted medial surface samples and sparsifies the vector
std::vector<Sphere> packSpheres(const std::vector<Sphere> &samples, int maxSpheres = 1000);

//constructs graph on packed sphere centers
PtGraph connectSamples(TreeType *distanceField, const std::vector<Sphere> &spheres);

//finds which joints can be embedded into which sphere centers
std::vector<std::vector<int> > computePossibilities(const PtGraph &graph, const std::vector<Sphere> &spheres,
														const Skeleton &skeleton);

//finds discrete embedding
std::vector<int> discreteEmbed(const PtGraph &graph, const std::vector<Sphere> &spheres,
										const Skeleton &skeleton, const std::vector<std::vector<int> > &possibilities);

//reinserts joints for unreduced skeleton
std::vector<Vector3> splitPaths(const std::vector<int> &discreteEmbedding, const PtGraph &graph,
										 const Skeleton &skeleton);

//refines embedding
std::vector<Vector3> refineEmbedding(TreeType *distanceField, const std::vector<Vector3> &medialSurface,
											  const std::vector<Vector3> &initialEmbedding, const Skeleton &skeleton);

//to compute the attachment, create a new Attachment object

#endif //PINOCCHIOAPI_H
