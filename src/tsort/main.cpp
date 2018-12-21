#include <iostream>                  // for std::cout
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <vector>
#include <string>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>

int main(int,char*[])
{
    // create a typedef for the Graph type
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> Graph;
    typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
    typedef std::string vertex_type;
    typedef std::pair<int, int> edge;

    // Make convenient labels for the vertices
    std::vector<vertex_type> vertices {
        "R1", "R2", "C1", "C2"
    };
    const int num_vertices = vertices.size();

    std::vector<std::pair<vertex_type, vertex_type>> edges_toplevel {
        { "R1", "R2" },
        { "C1", "C2" },
        { "R1", "C2" }
    };

    // writing out the edges in the graph
    std::vector<edge> edge_array;

    auto f = [&vertices](const std::pair<vertex_type, vertex_type>& e) -> edge
    {
        int v1 = std::find(vertices.begin(), vertices.end(), e.first) - vertices.begin();
        int v2 = std::find(vertices.begin(), vertices.end(), e.second) - vertices.begin();
        return { v1, v2 };
    };

    std::transform(edges_toplevel.begin(), edges_toplevel.end(), std::back_inserter(edge_array), f);

    // declare a graph object
    Graph g(edge_array.begin(), edge_array.end(), num_vertices);

    typedef std::vector<Vertex> container;
    container c;
    boost::topological_sort(g, std::back_inserter(c));

    std::cout << "A topological ordering: ";
    for(container::reverse_iterator ii = c.rbegin(); ii != c.rend(); ++ii)
    {
        std::cout << vertices[*ii] << " ";
    }
    std::cout << '\n';

    return 0;
}
