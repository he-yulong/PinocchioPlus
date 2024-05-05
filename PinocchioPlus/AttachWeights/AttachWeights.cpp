// AttachWeights.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <fstream>
#include "Pinocchio/skeleton.h"
#include "Pinocchio/utils.h"
#include "Pinocchio/debugging.h"
#include "Pinocchio/attachment.h"
#include "Pinocchio/pinocchioApi.h"
#include <cmath>
struct ArgData
{
	ArgData() : stopAtMesh(false), stopAfterCircles(false), skelScale(1.), noFit(true),
				skeleton(HumanSkeleton()), stiffness(1.),
				skelOutName("skeleton.out"), weightOutName("attachment.out")
	{
	}

	bool stopAtMesh;
	bool stopAfterCircles;
	string filename;
	Quaternion<> meshTransform;
	double skelScale;
	bool noFit;
	Skeleton skeleton;
	string skeletonname;
	double stiffness;
	string skelOutName;
	string weightOutName;
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
		string curStr = args[cur++];
		if (curStr == string("-skel"))
		{
			if (cur == arg_num)
			{
				cout << "No skeleton specified; ignoring." << endl;
				continue;
			}
			curStr = args[cur++];
			if (curStr == string("human"))
				out.skeleton = HumanSkeleton();
			else if (curStr == string("horse"))
				out.skeleton = HorseSkeleton();
			else if (curStr == string("quad"))
				out.skeleton = QuadSkeleton();
			else if (curStr == string("centaur"))
				out.skeleton = CentaurSkeleton();
			else
				out.skeleton = FileSkeleton(curStr);
			out.skeletonname = curStr;
			continue;
		}
		if (curStr == string("-rot"))
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
		if (curStr == string("-scale"))
		{
			if (cur >= arg_num)
			{
				cout << "No scale provided; exiting." << endl;
				printUsageAndExit();
			}
			sscanf(args[cur++].c_str(), "%lf", &out.skelScale);
			continue;
		}
		if (curStr == string("-meshonly") || curStr == string("-mo"))
		{
			out.stopAtMesh = true;
			continue;
		}
		if (curStr == string("-circlesonly") || curStr == string("-co"))
		{
			out.stopAfterCircles = true;
			continue;
		}
		if (curStr == string("-fit"))
		{
			out.noFit = false;
			continue;
		}
		if (curStr == string("-stiffness"))
		{
			if (cur >= arg_num)
			{
				cout << "No stiffness provided; exiting." << endl;
				printUsageAndExit();
			}
			sscanf(args[cur++].c_str(), "%lf", &out.stiffness);
			continue;
		}
		if (curStr == string("-skelOut"))
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
		if (curStr == string("-weightOut"))
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

void process(const vector<string> &args)
{
	ArgData argData = processArgs(args);

	Debugging::setOutStream(cout);

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
