#include <pinocchio.h>
#include "pinocchio/tools/Log.h"

void printUsageAndExit()
{
	std::cout << "PinocchioPlusExperiment: DemoUI filename.{obj | ply | off | gts | stl}" << std::endl;
	std::cout << "                         [-skel skelname] [-rot x y z deg]* [-scale s]" << std::endl;
	std::cout << "                         [-meshonly | -mo] [-circlesonly | -co]" << std::endl;
	std::cout << "                         [-motion motionname] [-nofit]" << std::endl;

	exit(0);
}

int main(int argc, char** argv)
{
	std::vector<std::string> args;
	for (int i = 0; i < argc; ++i)
		args.push_back(argv[i]);
	if (argc < 2) printUsageAndExit();
	std::string filename = args[1];
	for (int i = 0; i < argc; ++i)
	{
		std::string& curStr = args[i];
		PP_CORE_DEBUG("Parsing {}", curStr);
		if (curStr == "-skel")
		{
			// TODO
		}
		else if (curStr == "-rot")
		{
			// TODO
		}
	}

}