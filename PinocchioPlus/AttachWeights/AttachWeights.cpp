#include "stdafx.h"
#include <fstream>
#include <cmath>
#include <thread>
#include <vector>
#include <future>
#include "pinocchio.h"
#include <ctime>

struct ArgData {
    ArgData() : stopAtMesh(false), stopAfterCircles(false), skelScale(1.0), noFit(true),
                skeleton(HumanSkeleton()), stiffness(1.0),
                skelOutName("skeleton.out"), weightOutName("attachment.out") {}

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

void printUsageAndExit() {
    std::cout << "Usage: attachWeights filename.{obj | ply | off | gts | stl}" << std::endl;
    std::cout << "              [-skel skelname] [-rot x y z deg]* [-scale s]" << std::endl;
    std::cout << "              [-meshonly | -mo] [-circlesonly | -co]" << std::endl;
    std::cout << "              [-fit] [-stiffness s]" << std::endl;
    std::cout << "              [-skelOut skelOutFile] [-weightOut weightOutFile]" << std::endl;
    exit(0);
}

ArgData processArgs(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        printUsageAndExit();
    }

    ArgData out;
    out.filename = args[1];

    for (size_t cur = 2; cur < args.size(); ++cur) {
        std::string curStr = args[cur];
        if (curStr == "-skel") {
            if (++cur == args.size()) {
                std::cout << "No skeleton specified; ignoring." << std::endl;
                continue;
            }
            curStr = args[cur];
            if (curStr == "human") out.skeleton = HumanSkeleton();
            else if (curStr == "horse") out.skeleton = HorseSkeleton();
            else if (curStr == "quad") out.skeleton = QuadSkeleton();
            else if (curStr == "centaur") out.skeleton = CentaurSkeleton();
            else out.skeleton = FileSkeleton(curStr);
            out.skeletonname = curStr;
        } else if (curStr == "-rot") {
            if (cur + 3 >= args.size()) {
                std::cout << "Too few rotation arguments; exiting." << std::endl;
                printUsageAndExit();
            }
            double x, y, z, deg;
            sscanf(args[++cur].c_str(), "%lf", &x);
            sscanf(args[++cur].c_str(), "%lf", &y);
            sscanf(args[++cur].c_str(), "%lf", &z);
            sscanf(args[++cur].c_str(), "%lf", &deg);
            out.meshTransform = Quaternion<>(Vector3(x, y, z), deg * M_PI / 180.0) * out.meshTransform;
        } else if (curStr == "-scale") {
            if (++cur == args.size()) {
                std::cout << "No scale provided; exiting." << std::endl;
                printUsageAndExit();
            }
            sscanf(args[cur].c_str(), "%lf", &out.skelScale);
        } else if (curStr == "-meshonly" || curStr == "-mo") {
            out.stopAtMesh = true;
        } else if (curStr == "-circlesonly" || curStr == "-co") {
            out.stopAfterCircles = true;
        } else if (curStr == "-fit") {
            out.noFit = false;
        } else if (curStr == "-stiffness") {
            if (++cur == args.size()) {
                std::cout << "No stiffness provided; exiting." << std::endl;
                printUsageAndExit();
            }
            sscanf(args[cur].c_str(), "%lf", &out.stiffness);
        } else if (curStr == "-skelOut") {
            if (++cur == args.size()) {
                std::cout << "No skeleton output specified; ignoring." << std::endl;
                continue;
            }
            out.skelOutName = args[cur];
        } else if (curStr == "-weightOut") {
            if (++cur == args.size()) {
                std::cout << "No weight output specified; ignoring." << std::endl;
                continue;
            }
            out.weightOutName = args[cur];
        } else {
            std::cout << "Unrecognized option: " << curStr << std::endl;
            printUsageAndExit();
        }
    }
    return out;
}

void processVertices(Mesh& mesh, const Quaternion<>& transform) {
    std::vector<std::thread> threads;
    unsigned int numThreads = std::thread::hardware_concurrency();
    
    if (numThreads == 0) numThreads = 1; // 防止 hardware_concurrency 返回0

    auto verticesPerThread = mesh.vertices.size() / numThreads;

    for (unsigned int t = 0; t < numThreads; ++t) {
        threads.emplace_back([t, verticesPerThread, &mesh, &transform, numThreads]() {
            auto start = t * verticesPerThread;
            auto end = (t == numThreads - 1) ? mesh.vertices.size() : (t + 1) * verticesPerThread;
            for (auto i = start; i < end; ++i) {
                mesh.vertices[i].pos = transform * mesh.vertices[i].pos;
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }
}
void process(const std::vector<std::string>& args) {
    ArgData argData = processArgs(args);

    Mesh mesh(argData.filename);
    if (mesh.vertices.size() == 0) {
        std::cout << "Error reading file.  Aborting." << std::endl;
        exit(0);
    }

    processVertices(mesh, argData.meshTransform);
    mesh.normalizeBoundingBox();
    mesh.computeVertexNormals();

    Skeleton given = argData.skeleton;
    given.scale(argData.skelScale * 0.7);

    if (argData.stopAtMesh) { // if early bailout
        return;
    }

    PinocchioOutput o;
    if (!argData.noFit) { // do everything
        o = autorig(given, mesh);
    } else { // skip the fitting step--assume the skeleton is already correct for the mesh
        TreeType* distanceField = constructDistanceField(mesh);
        VisTester<TreeType>* tester = new VisTester<TreeType>(distanceField);

        o.embedding = argData.skeleton.fGraph().verts;
        for (int i = 0; i < (int)o.embedding.size(); ++i) {
            o.embedding[i] = mesh.toAdd + o.embedding[i] * mesh.scale;
        }

        o.attachment = new Attachment(mesh, argData.skeleton, o.embedding, tester, argData.stiffness);

        delete tester;
        delete distanceField;
    }

    if (o.embedding.size() == 0) {
        std::cout << "Error embedding" << std::endl;
        exit(0);
    }

    // output skeleton embedding
    for (int i = 0; i < (int)o.embedding.size(); ++i) {
        o.embedding[i] = (o.embedding[i] - mesh.toAdd) / mesh.scale;
    }
    std::ofstream os(argData.skelOutName.c_str());
    for (int i = 0; i < (int)o.embedding.size(); ++i) {
        os << i << " " << o.embedding[i][0] << " " << o.embedding[i][1] << " " << o.embedding[i][2] << " " << argData.skeleton.fPrev()[i] << std::endl;
    }

    // output attachment
    std::ofstream astrm(argData.weightOutName.c_str());
    for (int i = 0; i < (int)mesh.vertices.size(); ++i) {
        Vector<double, -1> v = o.attachment->getWeights(i);
        for (int j = 0; j < v.size(); ++j) {
            double d = std::floor(0.5 + v[j] * 10000.0) / 10000.0;
            astrm << d << " ";
        }
        astrm << std::endl;
    }

    delete o.attachment;
}

int main(int argc, char** argv) {
    // Record the start time
    std::clock_t start = std::clock();

    std::vector<std::string> args;

    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    process(args);

    // Record the end time
    std::clock_t end = std::clock();

    double duration = double(end - start) / CLOCKS_PER_SEC;

    std::cout << "Execution time: " << duration << " seconds" << std::endl;

    return 0;
}