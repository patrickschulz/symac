#ifndef WEIGHTMAP_HPP
#define WEIGHTMAP_HPP

#include <iostream>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/depth_first_search.hpp>

#include <ginac/ginac.h>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef std::pair<int,int> edge;
typedef std::string vertex_type;
typedef std::map<GiNaC::symbol, int, GiNaC::ex_is_less> weightmap_t;

struct inequality
{
    GiNaC::symbol lhs;
    GiNaC::symbol rhs;
    std::string operation;
};

std::list<Vertex> compute_make_order(const Graph& g);
std::vector<int> compute_parallel_computing_order(const Graph&g, const std::list<Vertex>& make_order);
std::vector<GiNaC::symbol> get_all_vertices(const std::vector<inequality>& inequalities);
std::vector<edge> get_edge_array(const std::vector<GiNaC::symbol>& vertices, const std::vector<inequality>& inequalities);
weightmap_t compute_weightmap(const std::vector<inequality>& inequalities);

#endif // WEIGHTMAP_HPP
