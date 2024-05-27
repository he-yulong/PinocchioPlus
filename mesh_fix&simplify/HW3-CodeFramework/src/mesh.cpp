#include <include/mesh.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <queue>
#include <algorithm>
#include <assert.h>
#include <unordered_set>


bool Mesh::load_obj(const std::string& filepath) {
    if (this->display_vertices.size() > 0) {
        this->display_vertices.clear();
        this->display_faces.clear();
    }

    if (filepath.substr(filepath.size() - 4, 4) != ".obj") {
        std::cerr << "Only obj file is supported." << std::endl;
        return false;
    }

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if ("v" == prefix) {
            Vector3f vertex;
            iss >> vertex[0] >> vertex[1] >> vertex[2];
            this->display_vertices.push_back(vertex);
        } else if ("f" == prefix) {
            Vector3i face;
            iss >> face[0] >> face[1] >> face[2];
            face = face.array() - 1;
            this->display_faces.push_back(face);
        }
    }
    return true;
}


/*
    TODO:
        Implement this function to convert a loaded OBJ format mesh into half-edge-based mesh.
    HINT:
        The loaded OBJ mesh information is stored in "this->display_vertices" and "this->display_faces" class variables,
        and the half-edge-based mesh information is stored in "this->vertices", "this->faces", "this->half_edges", "this->edges".

        You'll need to find a way to construct these class members and setting up associated attributes properly.
        The id for each type of objects starts from 0;
*/
void Mesh::convert_obj_format_to_mesh() {
    if (this->vertices.size() > 0) {
        this->vertices.clear();
        this->faces.clear();
        this->half_edges.clear();
        this->edges.clear();
    }
    // Add vertices
    for(int i = 0; i < display_vertices.size(); i++) {
        this->add_vertex(std::make_shared<Vertex>(display_vertices[i], i));
    }
    // Add halfedges and faces
    int half_edge_id = 0;
    int num = 0;
    int edge_id = 0;
    std::vector<std::shared_ptr<Edge>> E;
    for (int i = 0; i < display_faces.size(); i++) {
        this->add_face(std::make_shared<Face>(i));
        for(int j = 0; j < 3; j++) { 
            this->add_half_edge(std::make_shared<HalfEdge>(half_edge_id++));
            this->half_edges[i * 3 + j]->vertex = this->vertices[display_faces[i][j]];
            this->half_edges[i * 3 + j]->face = this->faces[i];            
            this->vertices[display_faces[i][j]]->he = this->half_edges[i * 3 + j];     
            E.push_back(std::make_shared<Edge>(this->half_edges[i * 3 + j], num++));
        }
        this->half_edges[i * 3]->next = this->half_edges[i * 3 + 1];
        this->half_edges[i * 3 + 1]->next = this->half_edges[i * 3 + 2];
        this->half_edges[i * 3 + 2]->next = this->half_edges[i * 3];
        this->faces[i]->he = this->half_edges[i * 3];
    }
    //Add edges and twin halfedges
    for(int i = 0; i < E.size(); i++) {
        for(int j = i; j < E.size(); j++) {
            if(E[i]->he->face != E[j]->he->face && E[i]->he->vertex == E[j]->he->next->vertex 
               && E[i]->he->next->vertex == E[j]->he->vertex) {
                std::shared_ptr<Edge> e = std::make_shared<Edge>(E[i]->he, edge_id++);
                this->add_edge(e);
                E[i]->he->edge = e;
                E[j]->he->edge = e;
                E[i]->he->twin = E[j]->he;
                E[j]->he->twin = E[i]->he;
                break;
            }
        }
    }

    /*for(const auto& h : this->half_edges) {
        if(!h->has_twin()) {
            std::shared_ptr<Edge> e = std::make_shared<Edge>(h, edge_id++);
            h->edge = e;
            this->add_edge(e);            
        }
    }*/

    std::cout << "====== Mesh Information ======" << std::endl;
    this->print_mesh_info();

}


// TODO: Implement this function to compute the genus number 
int Mesh::compute_genus() {
    int genus = 0;
    genus = (2 - this->vertices.size() + this->edges.size() - this->faces.size()) / 2;
    return genus;
}


// TODO: Implement this function to compute the surface area of the mesh
// HINT: You can first implement Face::get_area() to compute the surface area of each face, and then sum them up 
float Mesh::compute_surface_area() {
    float total_surface_area = 0;
    for(const auto& f : this->faces) {
        total_surface_area += f->get_area();
    }
    return total_surface_area;
}


// TODO: Implement this function to compute the volume of the mesh
// HINT: You can first implement Face::get_signed_volume() to compute the volume associate with each face, and then sum them up 
float Mesh::compute_volume() {
    float total_volume = 0;
    for(const auto& f : this->faces) {
        total_volume += f->get_signed_volume();
    }
    return total_volume;
}


// TODO: Implement this function to compute the average degree of all vertices
// HINT: It requires traversing all neighbor vertices of a given vertex, which you can implement in Vertex::neighbor_vertices() first
float Mesh::compute_average_degree() {
    float aver_deg = 0;
    int total_deg = 0;
    for (const auto& vertex : this->vertices) {
        total_deg += vertex->neighbor_vertices().size();
    }
    aver_deg = (float) total_deg / this->vertices.size();
    return aver_deg;
}


// This function is used to convert the half-edge-based mesh back to OBJ format for saving purpose 
void Mesh::convert_mesh_to_obj_format() {
    if (this->display_vertices.size() > 0) {
        this->display_vertices.clear();
        this->display_faces.clear();
    }

    std::map<std::shared_ptr<Vertex>, int> indices;
    int temp_idx = 0;
    for (const auto& vertex : this->vertices) {
        indices[vertex] = temp_idx;
        temp_idx++;
        this->display_vertices.push_back(vertex->pos);       
    }

    for (auto& face : this->faces) {
        Vector3i face_vert_id;
        int idx = 0;
        for (const auto& vertex : face->vertices()) {
            face_vert_id[idx] = indices[vertex];
            idx++;
        }
        this->display_faces.push_back(face_vert_id);     
    }
}


bool Mesh::save_obj(const std::string& filepath) const {
    if (filepath.substr(filepath.size() - 4, 4) != ".obj") {
        std::cerr << "Only obj file is supported." << std::endl;
        return false;
    }

    std::ofstream out_file(filepath);
    if (!out_file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return false;
    }

    // write vertices
    for (const auto &vertex : this->display_vertices) {
        out_file << "v " << vertex[0] << " " << vertex[1] << " " << vertex[2] << "\n";
    }

    // write faces
    for (const auto &face : this->display_faces) {
        out_file << "f " << face[0] + 1 << " " << face[1] + 1 << " " << face[2] + 1 << "\n";
    }

    out_file.close();
  return true;
}


void Mesh::remove_invalid_components() {
    this->vertices.erase(
        std::remove_if(this->vertices.begin(), this->vertices.end(), [](std::shared_ptr<Vertex> v) { return !v->exists; }), 
        this->vertices.end()
    );
    this->faces.erase(
        std::remove_if(this->faces.begin(), this->faces.end(), [](std::shared_ptr<Face> f) { return !f->exists; }), 
        this->faces.end()
    );
    this->half_edges.erase(
        std::remove_if(this->half_edges.begin(), this->half_edges.end(), [](std::shared_ptr<HalfEdge> he) { return !he->exists; }), 
        this->half_edges.end()
    );
    this->edges.erase(
        std::remove_if(this->edges.begin(), this->edges.end(), [](std::shared_ptr<Edge> e) { return !e->exists; }), 
        this->edges.end()
    );
}


int Mesh::verify() {
    // Check Euler's formula
    assert(this->vertices.size() + this->faces.size() - this->edges.size() == 2);
    assert(this->edges.size() * 2 == this->half_edges.size());

    int rvalue = 0;
    for (auto& v : this->vertices) {
        if (v->exists) {
            if (!v->he->exists) {
                rvalue |= 1<<0;
            }
        }
    }
    for (auto& f : this->faces) {
        if (f->exists) {
            if (!f->he->exists) {
                rvalue |= 1<<1;
            }
            if (f->he->next->next->next != f->he) {
                rvalue |= 1<<2;
            }
        }
    }
    for (auto& he : this->half_edges) {
        if (he->exists) {
            if (!he->vertex->exists) {
                rvalue |= 1<<3;
            }
            if (!he->edge->exists) {
                rvalue |= 1<<4;
            }
            if (!he->face->exists) {
                rvalue |= 1<<5;
            }
            if (he->twin->twin != he) {
                rvalue |= 1<<6;
            }
        }
    }
    for (auto& e : this->edges) {
        if (e->exists) {
            if (!e->he->exists) {
                rvalue |= 1<<7;
            }
        }
    }
    
    return rvalue;
}


void Mesh::simplify(const float ratio) {
    // TODO: Compute the qem coefficient vector associate with each vertex
    for (const auto& vertex : this->vertices) {
        vertex->compute_qem_coeff();
    }

    // Select all valid pairs.
    // In this homework, we use edge to act as vertex

    // TODO: Compute the optimal contraction information associate with each edge (v1, v2)
    for (const auto& edge : this->edges) {
        edge->compute_contraction();
    }

    // Place all the pairs in a heap keyed on cost with the minimum cost pair at the top
    std::priority_queue<std::shared_ptr<Edge>, std::vector<std::shared_ptr<Edge>>, Cmp> cost_min_heap{std::begin(this->edges), std::end(this->edges)};

    // Iteratively remove the edge (v1, v2) of least cost from the heap
    // Contract this edge, and update the costs of all valid edges involving v1.
    // TODO: Complete the edge_contraction function
    unsigned long num_face_preserve = std::round(this->faces.size() * ratio);
    unsigned long delete_faces = 0;
    while (this->faces.size() - delete_faces > num_face_preserve) {
        if (!cost_min_heap.empty()) {
            std::shared_ptr<Edge> contract_edge_candid = cost_min_heap.top();
            if (contract_edge_candid->exists) {
                if (!contract_edge_candid->visited) {
                    contract_edge_candid->edge_contraction();

                    // Mark edges that requires update
                    for (auto& adj_he : contract_edge_candid->he->vertex->neighbor_half_edges()) {
                        adj_he->edge->visited = true;
                    }

                    cost_min_heap.pop();
                    delete_faces += 2;
                } else {
                    cost_min_heap.pop();
                    contract_edge_candid->visited = false;
                    contract_edge_candid->compute_contraction();
                    cost_min_heap.push(contract_edge_candid);
                }
            } else {
                cost_min_heap.pop();
            }
        } else {
            break;
        }
    }
    
    // Remove invalid components
    this->remove_invalid_components();

    std::cout << std::endl << "====== Mesh Simplification with Ratio " << ratio << " ======" << std::endl;
    this->print_mesh_info();
}


void Mesh::print_mesh_info() {
    std::cout << "number of faces: " << this->faces.size() << std::endl;
    std::cout << "number of vertices: " << this->vertices.size() << std::endl;
    std::cout << "number of half edges: " << this->half_edges.size() << std::endl;
    std::cout << "number of edges: " << this->edges.size() << std::endl;
}
