#include <include/half_edge.hpp>
#include <Eigen/Dense>

/*
#################################################################################
#                       Vertex-related Helper Functions                         #
#################################################################################
*/

// Optinal TODO: Iterate through all neighbour vertices around the vertex
// Helpful when you implement the average degree computation of the mesh
std::vector<std::shared_ptr<Vertex>> Vertex::neighbor_vertices() {
    std::vector<std::shared_ptr<Vertex>> neighborhood;
    std::shared_ptr<HalfEdge> current = this->he;
    do {
        neighborhood.push_back(current->twin->vertex);
        current = current->twin->next;
    } while(current != this->he);
    return neighborhood; 
}


// TODO: Iterate through all half edges pointing away from the vertex
std::vector<std::shared_ptr<HalfEdge>> Vertex::neighbor_half_edges() {
    std::vector<std::shared_ptr<HalfEdge>> neighborhood;
    std::shared_ptr<HalfEdge> current = this->he; 
    do {
        neighborhood.push_back(current); 
        current = current->twin->next;
    } while(current != this->he);
    return neighborhood;
}


// TODO: Computate quadratic error metrics coefficient, which is a 5-d vector associated with each vertex
/*
    HINT:
        Please refer to homework description about how to calculate each element in the vector.
        The final results is stored in class variable "this->qem_coff"
*/
void Vertex::compute_qem_coeff() {
    Eigen::Vector3f v_sum = Eigen::Vector3f(0, 0, 0);
    float v2_sum = 0;
    for(const auto& v : this->neighbor_vertices()) {
        v_sum += v->pos;
        v2_sum += v->pos.dot(v->pos);
    }
    Eigen::VectorXf q(5);
    q << this->neighbor_vertices().size(), v_sum, v2_sum;
    this->qem_coff = q;
}


/*
#################################################################################
#                         Face-related Helper Functions                         #
#################################################################################
*/

// TODO: Iterate through all member vertices of the face
std::vector<std::shared_ptr<Vertex>> Face::vertices() {
    std::vector<std::shared_ptr<Vertex>> member_vertices;
    std::shared_ptr<HalfEdge> current = this->he; 
    do {
        member_vertices.push_back(current->vertex); 
        current = current->next;     
    } while(current != this->he);
    return member_vertices;
}


// TODO: implement this function to compute the area of the triangular face
float Face::get_area(){
    float area;
    Eigen::Vector3f l1 = this->vertices()[1]->pos - this->vertices()[0]->pos;
    Eigen::Vector3f l2 = this->vertices()[2]->pos - this->vertices()[0]->pos;
    area = sqrt(l1.cross(l2).x() * l1.cross(l2).x() + l1.cross(l2).y() * l1.cross(l2).y() + l1.cross(l2).z() * l1.cross(l2).z()) / 2;
    return area;
}

// TODO: implement this function to compute the signed volume of the triangular face
// reference: http://chenlab.ece.cornell.edu/Publication/Cha/icip01_Cha.pdf eq.(5)
float Face::get_signed_volume(){
    float volume;
    Eigen::Vector3f l1 = this->vertices()[0]->pos;
    Eigen::Vector3f l2 = this->vertices()[1]->pos;
    Eigen::Vector3f l3 = this->vertices()[2]->pos;
    volume = l1.dot(l2.cross(l3)) / 6;
    return volume;
}


/*
#################################################################################
#                         Edge-related Helper Functions                         #
#################################################################################
*/

/*
    TODO: 
        Compute the contraction information for the edge (v1, v2), which will be used later to perform edge collapse
            (i) The optimal contraction target v*
            (ii) The quadratic error metrics QEM, which will become the cost of contracting this edge
        The final results is stored in class variable "this->verts_contract_pos" and "this->qem"
    Please refer to homework description for more details
*/
void Edge::compute_contraction() {
    std::shared_ptr<Vertex> v1 = this->he->vertex;
    std::shared_ptr<Vertex> v2 = this->he->twin->vertex;
    Eigen::Vector3f v_contraction_pos = (v1->pos + v2->pos) / 2;
    Eigen::VectorXf v_contraction(5);
    v_contraction << v1->pos.dot(v1->pos), -2 * v1->pos, 1;
    this->verts_contract_pos = v_contraction_pos;
    this->qem = (v2->qem_coff).dot(v_contraction);
}


/*
    TODO: 
        Perform edge contraction functionality, which we write as (v1 ,v2) → v*, 
            (i) Moves the vertex v1 to the new position v*, remember to update all corresponding attributes,
            (ii) Connects all incident edges of v1 and v2 to v*, and remove the vertex v2,
            (iii) All faces, half edges, and edges associated with this collapse edge will be removed.
    HINT: 
        (i) Pointer reassignments
        (ii) When you want to remove mesh components, simply set their "exists" attribute to False
    Please refer to homework description for more details
*/
void Edge::edge_contraction() {
    // Save vertices and halfedges
    std::shared_ptr<HalfEdge> h1 = this->he;
    std::shared_ptr<HalfEdge> h2 = this->he->twin;
    std::shared_ptr<Vertex> v1 = h1->vertex;
    std::shared_ptr<Vertex> v2 = h2->vertex;
    // Set all contraction elements to false
    this->exists = false;
    h1->next->edge->exists = false;
    h1->next->twin->edge = h1->next->next->edge;
    h1->next->next->edge->he = h1->next->next->twin;
    h2->next->next->edge->exists = false;
    h2->next->next->twin->edge = h2->next->edge;
    h2->next->edge->he = h2->next->twin;
    do {
        h1->exists = false;
        h1 = h1->next;
    } while (h1->vertex != v1);
    do {
        h2->exists = false;
        h2 = h2->next;
    } while (h2->vertex != v2); 
    h1->face->exists = false;
    h2->face->exists = false;
    // Changes all the halfedges properties of deleted vertex 
    v1->pos = this->verts_contract_pos;
    v1->he = h1->next->next->twin;
    h1->next->next->vertex->he = h1->next->twin;
    h2->next->next->vertex->he = h2->next->twin;
    v2->exists = false;
    for(const auto& h : v2->neighbor_half_edges()) {
        h->vertex = v1;
    }
    // Change twin halfedge of the remained one
    h1->next->twin->twin = h1->next->next->twin;
    h1->next->next->twin->twin = h1->next->twin;
    h2->next->twin->twin = h2->next->next->twin;
    h2->next->next->twin->twin = h2->next->twin;
}