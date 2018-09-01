#include "stamp.hpp"

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

std::vector<element>::const_iterator stamp::begin() const
{
    return elements.begin();
}

std::vector<element>::const_iterator stamp::end() const
{
    return elements.end();
}
