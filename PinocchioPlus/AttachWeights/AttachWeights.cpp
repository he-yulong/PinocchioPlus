// AttachWeights.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <fstream>
#include <cmath>
#include "pinocchio.h"
struct ArgData
{
	ArgData() : stopAtMesh(false), stopAfterCircles(false), skelScale(1.), noFit(true),
				skeleton(HumanSkeleton()), stiffness(1.),
				skelOutName("skeleton.out"), weightOutName("attachment.out")
	{
	}

	bool stopAtMesh;
	bool stopAfterCircles;
	std::string filename;
	Quaternion<> meshTransform;
	double skelScale;
	bool noFit;
	Skeleton skeleton;
	std::string skeletonname;
	double stiffness;
	std::string skelOutName;
	std::string weightOutName;
};

void printUsageAndExit()
{
	cout << "Usage: attachWeights filename.{obj | ply | off | gts | stl}" << endl;
	cout << "              [-skel skelname] [-rot x y z deg]* [-scale s]" << endl;
	cout << "              [-meshonly | -mo] [-circlesonly | -co]" << endl;
	cout << "              [-fit] [-stiffness s]" << endl;
	cout << "              [-skelOut skelOutFile] [-weightOut weightOutFile]" << endl;
	exit(0);
}

ArgData processArgs(const std::vector<std::string> &args)
{
	int arg_num = args.size();
	if (arg_num < 2)
		printUsageAndExit();

	ArgData out;
	out.filename = args[1];

	int cur = 2;
	while (cur < arg_num)
	{
		std::string curStr = args[cur++];
		if (curStr == std::string("-skel"))
		{
			if (cur == arg_num)
			{
				cout << "No skeleton specified; ignoring." << endl;
				continue;
			}
			curStr = args[cur++];
			if (curStr == std::string("human"))
				out.skeleton = HumanSkeleton();
			else if (curStr == std::string("horse"))
				out.skeleton = HorseSkeleton();
			else if (curStr == std::string("quad"))
				out.skeleton = QuadSkeleton();
			else if (curStr == std::string("centaur"))
				out.skeleton = CentaurSkeleton();
			else
				out.skeleton = FileSkeleton(curStr);
			out.skeletonname = curStr;
			continue;
		}
		if (curStr == std::string("-rot"))
		{
			if (cur + 3 >= arg_num)
			{
				cout << "Too few rotation arguments; exiting." << endl;
				printUsageAndExit();
			}
			double x, y, z, deg;
			sscanf(args[cur++].c_str(), "%lf", &x);
			sscanf(args[cur++].c_str(), "%lf", &y);
			sscanf(args[cur++].c_str(), "%lf", &z);
			sscanf(args[cur++].c_str(), "%lf", &deg);

			out.meshTransform = Quaternion<>(Vector3(x, y, z), deg * M_PI / 180.) * out.meshTransform;
			continue;
		}
		if (curStr == std::string("-scale"))
		{
			if (cur >= arg_num)
			{
				cout << "No scale provided; exiting." << endl;
				printUsageAndExit();
			}
			sscanf(args[cur++].c_str(), "%lf", &out.skelScale);
			continue;
		}
		if (curStr == std::string("-meshonly") || curStr == std::string("-mo"))
		{
			out.stopAtMesh = true;
			continue;
		}
		if (curStr == std::string("-circlesonly") || curStr == std::string("-co"))
		{
			out.stopAfterCircles = true;
			continue;
		}
		if (curStr == std::string("-fit"))
		{
			out.noFit = false;
			continue;
		}
		if (curStr == std::string("-stiffness"))
		{
			if (cur >= arg_num)
			{
				cout << "No stiffness provided; exiting." << endl;
				printUsageAndExit();
			}
			sscanf(args[cur++].c_str(), "%lf", &out.stiffness);
			continue;
		}
		if (curStr == std::string("-skelOut"))
		{
			if (cur == arg_num)
			{
				cout << "No skeleton output specified; ignoring." << endl;
				continue;
			}
			curStr = args[cur++];
			out.skelOutName = curStr;
			continue;
		}
		if (curStr == std::string("-weightOut"))
		{
			if (cur == arg_num)
			{
				cout << "No weight output specified; ignoring." << endl;
				continue;
			}
			curStr = args[cur++];
			out.weightOutName = curStr;
			continue;
		}
		cout << "Unrecognized option: " << curStr << endl;
		printUsageAndExit();
	}

	return out;
}

void process(const std::vector<std::string> &args)
{
	ArgData argData = processArgs(args);

	Mesh mesh(argData.filename);
	if (mesh.vertices.size() == 0)
	{
		cout << "Error reading file.  Aborting." << endl;
		exit(0);
	}

	for (int i = 0; i < (int)mesh.vertices.size(); ++i)
		mesh.vertices[i].pos = argData.meshTransform * mesh.vertices[i].pos;
	mesh.normalizeBoundingBox();
	mesh.computeVertexNormals();

	Skeleton given = argData.skeleton;
	given.scale(argData.skelScale * 0.7);

	if (argData.stopAtMesh)
	{ // if early bailout
		return;
	}

	PinocchioOutput o;
	if (!argData.noFit)
	{ // do everything
		o = autorig(given, mesh);
	}
	else
	{ // skip the fitting step--assume the skeleton is already correct for the mesh
		TreeType *distanceField = constructDistanceField(mesh);
		VisTester<TreeType> *tester = new VisTester<TreeType>(distanceField);

		o.embedding = argData.skeleton.fGraph().verts;
		for (int i = 0; i < (int)o.embedding.size(); ++i)
			o.embedding[i] = mesh.toAdd + o.embedding[i] * mesh.scale;

		o.attachment = new Attachment(mesh, argData.skeleton, o.embedding, tester, argData.stiffness);

		delete tester;
		delete distanceField;
	}

	if (o.embedding.size() == 0)
	{
		cout << "Error embedding" << endl;
		exit(0);
	}

	// output skeleton embedding
	for (int i = 0; i < (int)o.embedding.size(); ++i)
		o.embedding[i] = (o.embedding[i] - mesh.toAdd) / mesh.scale;
	ofstream os(argData.skelOutName.c_str());
	for (int i = 0; i < (int)o.embedding.size(); ++i)
	{
		os << i << " " << o.embedding[i][0] << " " << o.embedding[i][1] << " " << o.embedding[i][2] << " " << argData.skeleton.fPrev()[i] << endl;
	}

	// output attachment
	std::ofstream astrm(argData.weightOutName.c_str());
	for (int i = 0; i < (int)mesh.vertices.size(); ++i)
	{
		Vector<double, -1> v = o.attachment->getWeights(i);
		for (int j = 0; j < v.size(); ++j)
		{
			double d = std::floor(0.5 + v[j] * 10000.) / 10000.;
			astrm << d << " ";
		}
		astrm << endl;
	}

	delete o.attachment;
}

int main(int argc, char **argv)
{
	std::vector<std::string> args;
	for (int i = 0; i < argc; ++i)
		args.push_back(argv[i]);
	process(args);
}
