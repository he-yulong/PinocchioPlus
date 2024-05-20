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
#include <fstream>
#include "tools/Log.h"

PinocchioOutput autorig(const Skeleton& given, const Mesh& mesh)
{
	PinocchioOutput out;
	PP_CORE_DEBUG("prepareMesh start.");
	Mesh newMesh = prepareMesh(mesh);
	PP_CORE_DEBUG("prepareMesh end.");

	if (newMesh.vertices.size() == 0)
	{
		PP_CORE_WARN("Encountering a problem when doing prepareMesh!");
		return out;
	}

	PP_CORE_DEBUG("constructDistanceField start.");
	TreeType* distanceField = constructDistanceField(newMesh);
	PP_CORE_DEBUG("constructDistanceField end.");

	//discretization
	PP_CORE_DEBUG("sampleMedialSurface start.");
	std::vector<Sphere> medialSurface = sampleMedialSurface(distanceField);
	PP_CORE_DEBUG("After sampleMedialSurface, the medialSurface's size: {}", medialSurface.size());
	PP_CORE_DEBUG("sampleMedialSurface end.");

	PP_CORE_DEBUG("packSpheres start.");
	std::vector<Sphere> spheres = packSpheres(medialSurface);
	PP_CORE_DEBUG("After packSpheres, the sphere's size: {}", spheres.size());
	PP_CORE_DEBUG("packSpheres end.");

	PP_CORE_DEBUG("connectSamples start");
	PtGraph graph = connectSamples(distanceField, spheres);
	PP_CORE_DEBUG("connectSamples end");

	//discrete embedding
	PP_CORE_DEBUG("computePossibilities start");
	std::vector<std::vector<int>> possibilities = computePossibilities(graph, spheres, given);
	PP_CORE_DEBUG("computePossibilities end");

	//constraints can be set by respecifying possibilities for skeleton joints:
	//to constrain joint i to sphere j, use: possiblities[i] = vector<int>(1, j);
	// TODO: optimize discreteEmbed
	PP_CORE_DEBUG("discreteEmbed before");
	std::vector<int> embeddingIndices = discreteEmbed(graph, spheres, given, possibilities);
	PP_CORE_DEBUG("discreteEmbed end");


	if (embeddingIndices.size() == 0) { //failure
		delete distanceField;
		return out;
	}

	std::vector<Vector3> discreteEmbedding = splitPaths(embeddingIndices, graph, given);

	//continuous refinement
	std::vector<Vector3> medialCenters(medialSurface.size());
	for (int i = 0; i < medialSurface.size(); ++i)
		medialCenters[i] = medialSurface[i].center;

	PP_CORE_DEBUG("refineEmbedding before");
	out.embedding = refineEmbedding(distanceField, medialCenters, discreteEmbedding, given);
	PP_CORE_DEBUG("refineEmbedding end");

	//attachment
	PP_CORE_DEBUG("new VisTester before");
	VisTester<TreeType>* tester = new VisTester<TreeType>(distanceField);
	PP_CORE_DEBUG("new VisTester end");

	PP_CORE_DEBUG("new Attachment before");
	out.attachment = new Attachment(newMesh, given, out.embedding, tester);
	PP_CORE_DEBUG("new Attachment end");

	//cleanup
	delete tester;
	delete distanceField;

	return out;
}

