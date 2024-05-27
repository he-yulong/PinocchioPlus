#include <Eigen/Dense>
#include <Eigen/Eigen>
#include <iostream>
#include <opencv2/core/base.hpp>
#include <opencv2/opencv.hpp>
#include <include/mesh.hpp>


int main(int argc, const char **argv) {
    // obtain model path from argument and define output model path
    // for example, if you have you exec program built under ./task1 under ${root}/build, and models are in ${root}/model,
    // use sample command "./task1 ../model/bigguy2.obj" to test your algorithm.    

    std::string file_path;
    if (argc >= 2) {
      file_path = std::string(argv[1]);
    } else{
      std::cout << "Please Input your model path as an argument";
      return 0;
    }
    size_t pos = file_path.size() - 4;
    std::string save_file_path = file_path.substr(0, pos) + "_half_edge.obj";

    // Represent mesh in the memory by half-edge data structure
    Mesh mesh;
    mesh.load_obj(file_path);
    mesh.convert_obj_format_to_mesh();
    mesh.convert_mesh_to_obj_format();
    mesh.save_obj(save_file_path);

    // Compute topology properties (genus number)
    std::cout << "====== Topology Computation Output ======" << std::endl;

    int genus_number = mesh.compute_genus();
    std::cout << "genus number of the mesh: " << genus_number << std::endl;

    // Compute volume and surface areas
    float surface_area = mesh.compute_surface_area();
    std::cout << "surface area of the mesh: " << surface_area << std::endl;

    float volume = mesh.compute_volume();
    std::cout << "volume of the mesh: " << volume << std::endl;

    // Compute average degree of all vertices
    float aver_deg = mesh.compute_average_degree();
    std::cout << "average degree of the mesh: " << aver_deg << std::endl;

    return 0;
}
