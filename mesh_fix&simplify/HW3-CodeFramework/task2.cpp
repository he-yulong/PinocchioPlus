#include <Eigen/Dense>
#include <Eigen/Eigen>
#include <iostream>
#include <opencv2/core/base.hpp>
#include <opencv2/opencv.hpp>
#include <include/mesh.hpp>

constexpr double MY_PI = 3.1415926;

int main(int argc, const char **argv) {
     
    // obtain model path and simlification ratio from argument and define output model path
    // for example, if you have you exec program built under ./task1 under ${root}/build, and models are in ${root}/model,
    // use sample command "./task2 ../model/bigguy2.obj 0.1" to test your algorithm.
    
    std::string file_path;
    float ratio = 0.1;
    if (argc >= 3) {
      file_path = std::string(argv[1]);
      ratio = std::stof(argv[2]);
    } else{
      std::cout << "Please Input your model path as an argument";
      return 0;
    }
    size_t pos = file_path.size() - 4;
    std::string save_file_path = file_path.substr(0, pos) + "_simplify.obj";

    // Represent mesh in the memory by half-edge data structure
    Mesh mesh;
    mesh.load_obj(file_path);
    mesh.convert_obj_format_to_mesh();

    // Simplify mesh
    mesh.simplify(ratio);

    // Verify simplification results
    std::cout << std::endl << "====== Verification ======" << std::endl;
    auto result = mesh.verify();
    if (result) {
        std::cout << "ERROR, CODE " << std::oct << result << std::endl;
    } else {
        std::cout << "Verification Pass" << std::endl;
    }

    mesh.convert_mesh_to_obj_format();
    mesh.save_obj(save_file_path);

    return 0;
}
