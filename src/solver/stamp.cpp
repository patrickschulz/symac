#include "stamp.hpp"

#include <algorithm>

void stamp::write(unsigned int node1, unsigned int node2, const GiNaC::ex& value)
{
    if((node1 != 0) && (node2 != 0))
    {
        elements.push_back(element(node1, node2, value));
    }
}

void stamp::clear()
{
    elements.clear();
}

unsigned int stamp::size() const
{
    unsigned int size = 0;
    for(const element& e : elements)
    {
        size = std::max({ size, e.get_row(), e.get_column() });
    }
    return size;
}

std::vector<element>::const_iterator stamp::begin() const
{
    return elements.begin();
}

std::vector<element>::const_iterator stamp::end() const
{
    return elements.end();
}
