#include <half_edge.hpp>
#include <Eigen/Eigen>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <memory>

using namespace Eigen;

class Mesh {
    private:
        void remove_invalid_components();
    public:
        std::vector<Vector3f> display_vertices;
        std::vector<Vector3i> display_faces;

        std::vector<std::shared_ptr<Vertex>> vertices;
        std::vector<std::shared_ptr<HalfEdge>> half_edges;
        std::vector<std::shared_ptr<Face>> faces;
        std::vector<std::shared_ptr<Edge>> edges;

        Mesh() {}
        ~Mesh() {}

        void add_vertex(std::shared_ptr<Vertex> vertex) {
            this->vertices.push_back(vertex);
        }

        void add_half_edge(std::shared_ptr<HalfEdge> he) {
            half_edges.push_back(he);
        }

        void add_face(std::shared_ptr<Face> face) {
            faces.push_back(face);
        }

        void add_edge(std::shared_ptr<Edge> edge) {
            edges.push_back(edge);
        }
        
        bool load_obj(const std::string& filepath);
        bool save_obj(const std::string& filepath) const;
        int compute_genus();
        float compute_surface_area();
        float compute_volume();
        float compute_average_degree();
        void convert_obj_format_to_mesh();
        void convert_mesh_to_obj_format();
        void simplify(float ratio);
        void print_mesh_info();
        int verify();
};

