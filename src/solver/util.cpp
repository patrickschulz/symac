#include "util.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <fstream>
#include <sstream>

#include <boost/format.hpp>

void print_network_matrices(const GiNaC::matrix& A, const GiNaC::matrix& x, const GiNaC::matrix& z, nodemap& nmap)
{
    const unsigned int sep = 3;
    const unsigned skip = 10;

    unsigned int size = A.cols();

    std::vector<unsigned int> maxlengths(size + 2, 0);

    // output format
    auto format = GiNaC::dflt;

    // calculate column widths
    for(unsigned int column = 0; column < size + 2; ++column)
    {
        std::string::size_type maxcollength = 0;
        for(unsigned int row = 0; row < size; ++row)
        {
            std::ostringstream stream;
            if(column < size)
            {
                stream << format << A(row, column);
            }
            if(column == size)
            {
                stream << format << x(row, 0);
                //stream << format << nmap[row + 1];
            }
            if(column > size)
            {
                stream << format << z(row, 0);
            }
            maxcollength = std::max(maxcollength, stream.str().size());
        }
        maxlengths[column] = maxcollength;
    }

    // output starts here
    std::cout << '\n';
    unsigned int Awidth = std::accumulate(maxlengths.begin(), maxlengths.end() - 2, 0) + sep * (maxlengths.size() - 2);
    std::cout << std::string(Awidth/2, ' ') << 'A' << std::string(Awidth/2, ' ');

    unsigned int xwidth = *(maxlengths.end() - 2) + sep;
    std::cout << std::string(xwidth/2 + skip + sep/2, ' ') << 'x' << std::string(xwidth/2, ' ');

    unsigned int zwidth = *(maxlengths.end() - 1) + sep;
    std::cout << std::string(zwidth/2 + skip, ' ') << 'z' << std::string(zwidth/2, ' ');

    std::cout << '\n';
    // separation line
    std::cout << std::string(Awidth, '-');
    std::cout << std::string(skip + sep/2, ' ');
    std::cout << std::string(xwidth, '-');
    std::cout << std::string(skip, ' ');
    std::cout << std::string(zwidth, '-');
    std::cout << '\n';
    for(unsigned int row = 0; row < size; ++row)
    {
        for(unsigned int column = 0; column < size + 2; ++column)
        {
            unsigned int width = maxlengths[column];
            std::ostringstream stream;
            if(column < size)
            {
                stream << format << A(row, column);
            }
            if(column == size)
            {
                stream << format << x(row, 0);
                //stream << format << nmap[row + 1];
                width += skip;
            }
            if(column > size)
            {
                stream << format << z(row, 0);
                width += skip;
            }
            std::cout << std::setw(width + sep) << stream.str();
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}
