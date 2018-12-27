#ifndef WEIGHTMAP_HPP
#define WEIGHTMAP_HPP

#include <iostream>
#include <algorithm>
#include <vector>
#include <tuple>
#include <set>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/depth_first_search.hpp>

#include <ginac/ginac.h>

#include "../symbol.hpp"

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef std::pair<int,int> edge;
typedef std::string vertex_type;

struct inequality
{
    GiNaC::symbol lhs;
    GiNaC::symbol rhs;
    std::string operation;
};

std::list<Vertex> compute_make_order(const Graph& g)
{
    std::list<Vertex> make_order;

    boost::topological_sort(g, std::front_inserter(make_order));
    return make_order;
}

std::vector<int> compute_parallel_computing_order(const Graph&g, const std::list<Vertex>& make_order)
{
    // Parallel compilation ordering
    std::vector<int> time(make_order.size(), 0);
    for(const auto& e : make_order)
    {
        // Walk through the in_edges an calculate the maximum time.
        if (boost::in_degree (e, g) > 0)
        {
            Graph::in_edge_iterator j, j_end;
            int maxdist = 0;
            // Through the order from topological sort, we are sure that every 
            // time we are using here is already initialized.
            for(auto it = boost::in_edges(e, g).first; it != boost::in_edges(e, g).second; ++it)
            {
                maxdist = std::max(time[source(*it, g)], maxdist);
            }
            time[e] = maxdist + 1;
        }
    }
    return time;
}

std::vector<GiNaC::symbol> get_all_vertices(const std::vector<inequality>& inequalities)
{
    std::set<GiNaC::symbol, GiNaC::ex_is_less> vertex_set;
    auto insertvertex = [&vertex_set](const inequality& e)
    {
        vertex_set.insert(e.lhs);
        vertex_set.insert(e.rhs);
    };
    std::for_each(inequalities.begin(), inequalities.end(), insertvertex);

    std::vector<GiNaC::symbol> vertices;
    std::copy(vertex_set.begin(), vertex_set.end(), std::back_inserter(vertices));
    return vertices;
}

std::vector<edge> get_edge_array(const std::vector<GiNaC::symbol>& vertices, const std::vector<inequality>& inequalities)
{
    std::vector<edge> edge_array;

    auto f = [&vertices](const inequality& e) -> edge
    {
        int v1 = std::find(vertices.begin(), vertices.end(), e.lhs) - vertices.begin();
        int v2 = std::find(vertices.begin(), vertices.end(), e.rhs) - vertices.begin();
        if(e.operation == ">>")
        {
            return { v1, v2 };
        }
        else // e.operation == "<<"
        {
            return { v2, v1 };
        }
    };

    std::transform(inequalities.begin(), inequalities.end(), std::back_inserter(edge_array), f);

    return edge_array;
}

std::map<GiNaC::symbol, int, GiNaC::ex_is_less> compute_weightmap(const std::vector<inequality>& inequalities)
{
    GiNaC::symbol R1 = get_symbol("R1");
    GiNaC::symbol R2 = get_symbol("R2");
    GiNaC::symbol C1 = get_symbol("C1");
    GiNaC::symbol C2 = get_symbol("C2");
    GiNaC::symbol gm = get_symbol("gm");
    GiNaC::symbol rout = get_symbol("rout");
    GiNaC::symbol gm1 = get_symbol("gm1");
    GiNaC::symbol gm2 = get_symbol("gm2");
    GiNaC::symbol rout1 = get_symbol("rout1");
    GiNaC::symbol rout2 = get_symbol("rout2");
    GiNaC::symbol Cl = get_symbol("Cl");
    GiNaC::symbol Cs = get_symbol("Cs");

    std::vector<GiNaC::symbol> vertices = get_all_vertices(inequalities);
    // writing out the edges in the graph
    std::vector<edge> edge_array = get_edge_array(vertices, inequalities);

    // declare a graph object
    Graph g(edge_array.begin(), edge_array.end(), vertices.size());

    std::list<Vertex> make_order = compute_make_order(g);
    std::vector<int> time = compute_parallel_computing_order(g, make_order);

    for(auto it = boost::vertices(g).first; it != boost::vertices(g).second; ++it)
    {
        std::cout << "weight(" << vertices[*it] << ") = " << time[*it] << '\n';
    }

    std::map<GiNaC::symbol, int, GiNaC::ex_is_less> weightmap
    {
        { R1, 1 },
        { R2, 0 },
        { C1, 1 },
        { C2, 0 },
        { gm, 1 },
        { rout, 1 },
        { gm1, 1 },
        { rout1, 1 },
        { gm2, 1 },
        { rout2, 1 },
        { Cl, 0 },
        { Cs, 0 }
    };
    return weightmap;
}

#endif // WEIGHTMAP_HPP
