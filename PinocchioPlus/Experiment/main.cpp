#include <pinocchio.h>
#include "pinocchio/tools/Log.h"
#include <iostream>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

void backup()
{
	std::cout << "PinocchioPlusExperiment: DemoUI filename.{obj | ply | off | gts | stl}" << std::endl;
	std::cout << "                         [-skel skelname] [-rot x y z deg]* [-scale s]" << std::endl;
	std::cout << "                         [-meshonly | -mo] [-circlesonly | -co]" << std::endl;
	std::cout << "                         [-motion motionname] [-nofit]" << std::endl;

	exit(-1);
}

struct ArgData
{
	bool stopAtMesh = false;           // early stop
	bool stopAfterCircles = false;     // early stop
	bool noFit = false;
	Skeleton skeleton = HumanSkeleton();
	std::string motionName = "";
	std::string objName = "";
};

ArgData generateArgData(int argc, char** argv) {
	std::string configFilename;
	if (argc == 1) configFilename = "../experiment.json";
	else if (argc == 2) configFilename = argv[1];
	else {
		PP_CORE_ERROR("PinocchioPlusExperiment: PinocchioPlusExperiment [filename]");
		exit(-1);
	}

	ArgData argData;

	std::ifstream f(configFilename);
	json data = json::parse(f);
	std::string param = "skeleton";
	if (data.contains(param))  // human / horse / quad / centaur / else
	{
		std::string skeletonStr = data.at(param);
		PP_CORE_DEBUG("Skeleton specified: {}", skeletonStr);
		if (skeletonStr == "human")
		{
			argData.skeleton = HumanSkeleton();
		}
		else
		{
			PP_CORE_ERROR("Not implemented: {}!", skeletonStr);
		}
	}
	else
	{
		PP_CORE_DEBUG("No skeleton specified; ignoring.");
	}

	// NO -rot -scale 
	// rot used to rotate mesh

	param = "mesh-only";
	if (data.contains(param)) {
		argData.stopAtMesh = data.at(param);
	}

	param = "circles-only";
	if (data.contains(param)) {
		argData.stopAfterCircles = data.at(param);
	}

	param = "no-fit";
	if (data.contains(param)) {
		argData.noFit = data.at("no-fit");
	}

	param = "obj";
	if (data.contains(param)) {
		argData.objName = data.at(param);
	}
	else {
		PP_CORE_DEBUG("No obj filename specified; ignoring.");
	}

	param = "motion";
	if (data.contains(param)) {
		argData.motionName = data.at(param);
	}
	else {
		PP_CORE_DEBUG("No motion filename specified; ignoring.");
	}
	return argData;
}

int main(int argc, char** argv)
{
	ArgData argData = generateArgData(argc, argv);
	PP_CORE_DEBUG("Loading mesh...");
	Mesh mesh(argData.objName);
	PP_CORE_DEBUG("mesh loaded.");
	if (mesh.vertices.size() == 0) {
		PP_CORE_ERROR("Error reading file. Aborting.");
		exit(-1);
	}
	mesh.normalizeBoundingBox();
	mesh.computeVertexNormals();  // repeated calling

	Skeleton givenSkeleton = argData.skeleton;
	givenSkeleton.scale(0.7);

	if (argData.stopAtMesh)
	{	
		PP_CORE_WARN("Not implemented!");
		// if early bailout
		//w->addMesh(new StaticDisplayMesh(m));
		//return;
	}

	PinocchioOutput output;
	if (argData.noFit)
	{
		PP_CORE_DEBUG("Skip the fitting step.(assume the skeleton is already correct for the mesh)");
		// skip the fitting step--assume the skeleton is already correct for the mesh
		TreeType* distanceField = constructDistanceField(mesh);
		VisTester<TreeType>* tester = new VisTester<TreeType>(distanceField);
		output.embedding = argData.skeleton.fGraph().verts;

		for (int i = 0; i < output.embedding.size(); ++i)
			output.embedding[i] = mesh.toAdd + output.embedding[i] * mesh.scale;

		output.attachment = new Attachment(mesh, argData.skeleton, output.embedding, tester);

		delete tester;
		delete distanceField;
	}
	else {
		output = autorig(givenSkeleton, mesh);
	}
}