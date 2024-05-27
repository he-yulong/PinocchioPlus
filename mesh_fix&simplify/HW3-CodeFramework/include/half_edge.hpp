#include <Eigen/Eigen>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

class HalfEdge;
class Vertex;
class Face;
class Edge;


class HalfEdge {
    public:
        std::shared_ptr<HalfEdge> next;
        std::shared_ptr<HalfEdge> twin;
        std::shared_ptr<Face> face;
        std::shared_ptr<Vertex> vertex;
        std::shared_ptr<Edge> edge;
        bool exists;
        int id;

        HalfEdge(int _id): id(_id), exists(true) {}
        
        bool has_twin(){return this->twin != nullptr;}
};


class Vertex {
    public:
        Eigen::Vector3f pos;
        std::shared_ptr<HalfEdge> he;
        Eigen::VectorXf qem_coff = Eigen::VectorXf(5);
        bool exists;
        int id;

        Vertex(int _id): id(_id) {}
        Vertex(Eigen::Vector3f _pos, int _id): pos(_pos), id(_id), exists(true) {}

        std::vector<std::shared_ptr<Vertex>> neighbor_vertices();
        std::vector<std::shared_ptr<HalfEdge>> neighbor_half_edges();
        void compute_qem_coeff();
};


class Face {
    public:
        std::shared_ptr<HalfEdge> he;
        Eigen::Vector3f color;
        bool exists;
        int id;

        Face(int _id): id(_id), exists(true) {}
        Face(Eigen::Vector3f _color, int _id): color(_color), id(_id), exists(true) {}

        std::vector<std::shared_ptr<Vertex>> vertices();
        float get_area();
        float get_signed_volume();
};


class Edge {
    public:
        std::shared_ptr<HalfEdge> he;
        Eigen::Vector3f verts_contract_pos;
        float qem;
        bool visited, exists;
        int id;

        Edge(std::shared_ptr<HalfEdge> _he, int _id): he(_he), id(_id), exists(true) {};

        void compute_contraction();
        void edge_contraction();
};


class Cmp{
    public:
        bool operator() (const std::shared_ptr<Edge> a, const std::shared_ptr<Edge> b){  
            return a->qem > b->qem;   
        }  
}; 
