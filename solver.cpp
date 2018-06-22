#include "solver.hpp"

#include <boost/format.hpp>
#include <numeric>
#include <fstream>


#include "mna.hpp"
#include "netlist.hpp"

solver::solver(const std::string& mode, const netlist& nlist, const std::vector< std::string> nodes) :
    mode(mode), nlist(nlist), nodes(nodes)
{

}

void solver::mna()
{
    A = mna::create_A_matrix(nlist);
    x = mna::create_x_vector(nlist);
    z = mna::create_z_vector(nlist);
}

void solver::solve()
{
    results = A.solve(x, z, GiNaC::solve_algo::gauss);
}

void solver::print()
{
    if(mode == "ac")
    {
        std::cout << "Results:\n";
        std::cout << GiNaC::csrc;
        unsigned int row = 0;

        std::vector<std::tuple<std::string, component_types, std::vector<std::string>>> dev_formats = {
            std::make_tuple<std::string, component_types, std::vector<std::string>>("Currents through impedances", ct_resistor | ct_capacitor | ct_inductor, {"Iz"}),
            std::make_tuple<std::string, component_types, std::vector<std::string>>("Currents through voltage sources", ct_voltage_source, {"Iv"}),
            std::make_tuple<std::string, component_types, std::vector<std::string>>("Currents into opamps (output)", ct_opamp, {"Iop"}),
            std::make_tuple<std::string, component_types, std::vector<std::string>>("Currents through vcvs", ct_voltage_controlled_voltage_source, {"Ie"}),
            std::make_tuple<std::string, component_types, std::vector<std::string>>("Currents through cccs", ct_current_controlled_current_source, {"Ihin"}),
            std::make_tuple<std::string, component_types, std::vector<std::string>>("Currents through ccvs", ct_current_controlled_voltage_source, {"Ifin", "Ifout"})
        };

        std::cout << "    Node voltages:\n";
        for(; row < nlist.number_of_nodes(); ++row)
        {
            std::string usernode= nlist.get_output_node(row+1);
            boost::format fmter = boost::format("        Node %s:\t\t") % (usernode);
            std::string str = fmter.str();
            std::cout << str << results(row, 0) << '\n';
        }
        std::cout << '\n';
        for(const auto& dev : dev_formats)
        {
            std::string header;
            component_types ct;
            std::vector<std::string> currents;
            std::tie(header, ct, currents) = dev;
            unsigned int size = nlist.number_of_devices(ct);
            if(size > 0)
            {
                std::cout << "    " << header << ":\n";
                for(unsigned int i = 0; i < size; ++i)
                {
                    for(const auto& current : currents)
                    {
                        boost::format fmter = boost::format("        %s%d:\t\t") % current % (i + 1);
                        std::string str = fmter.str();
                        std::cout << str << results(row, 0) << '\n';
                        ++row;
                    }
                }
                std::cout << '\n';
            }
        }
    }
    else if(mode == "VVtf")
    {
        if (nodes[0].empty() || nodes[1].empty())
        {
            std::cerr << "No Nodes given! \n";
        }
        else
        {
            unsigned int first = nlist.get_unode(nodes[0]);
            unsigned int second = nlist.get_unode(nodes[1]);
            first--;
            second--;   
            std::cout << "Node " << nodes[0] << " Voltage: \n";
            std::cout << results(first,0) << '\n';
            std::cout << "Node " << nodes[1] << " Voltage: \n";
            std::cout << results(second,0) << '\n';
            std::cout << "Transfer Function: Voltage-Voltage \n";
            std::cout << '\n';
            std::cout << "H(s) = " << results(second, 0) / results(first, 0) << '\n';
        }
    }
    else
    {
        std::cerr << "unknown mode '" << mode << "' given\n";
        exit(1);
    }
}

void print_network_matrices(const GiNaC::matrix& A, const GiNaC::matrix& x, const GiNaC::matrix& z)
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

void solver::print_matrices()
{
    print_network_matrices(A, x, z);
}
void solver::to_matlab(const std::string& filename)
{
    std::string a = filename;
//     std::ofstream ofile(filename +".m", std::ofstream::out);
//     ofile << "Matlab-Export " << std::endl;
//     ofile << " %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% " << std::endl;
//     ofile << " %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% " << std::endl;
// 
//     ofile << " Matrices:" << std::endl;
//     // Print matrices A, x , z 
//     unsigned int A_rows =  sizeof A / sizeof A[0];
//     unsigned int A_cols = sizeof A[0] / sizeof(int);
// 
//     ofile << " A = [ " << std::endl;
//     for (unsigned int i = 0;i < A_rows ;i++)
//     {
//         ofile << "    sym('" << A[i][1]<<"'), ";
//         for( unsigned int j = 1 ; j < A_cols ; j++)
//         {
//             ofile <<"     sym('"<<A[i][j]<<"'), ";
//         }
//     }
//     ofile << " ]" << std::endl;
//     unsigned int z_rows = sizeof z / sizeof z[0];
//     ofile << " z = [ " << std::endl;
//     for (unsigned int i = 0 ; i < z_rows;i++)
//     {
//         ofile << ("  sym ('"<<z[i]<<"') ", );
//     }
//     ofile << " ] " << std::endl;
//     ofile << " x = simplify ( inv(A) * z)" << std::endl;
//     ofile.close();
}
