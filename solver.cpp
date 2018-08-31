#include "solver.hpp"

#include <boost/format.hpp>
#include <numeric>
#include <fstream>


#include "mna.hpp"
#include "netlist.hpp"


solver::solver(const std::string& filename, const std::string& mode,std::vector<std::string> nodes, netlist& nlist, bool matlab_export) :
    filename(filename),mode (mode),nodes(nodes), nlist(nlist), matlab_export(matlab_export)
{
}

void solver::mna()
{
    const auto& components = nlist.get_components();
    unsigned int networksize = nlist.full_network_size();
    unsigned int number_of_nodes = nlist.number_of_nodes();
    A = mna::create_A_matrix(networksize, components);
    x = mna::create_x_vector(networksize, number_of_nodes, components);
    z = mna::create_z_vector(networksize, number_of_nodes, components);
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
            std::make_tuple<std::string, component_types, std::vector<std::string>>("Currents through impedances", ct_resistor | ct_capacitor | ct_inductor,{"Iz"}),
            std::make_tuple<std::string, component_types, std::vector<std::string>>("Currents through voltage sources", ct_voltage_source, {"Iv"}),
            std::make_tuple<std::string, component_types, std::vector<std::string>>("Currents into opamps (output)", ct_opamp, {"Iop"}),
            std::make_tuple<std::string, component_types, std::vector<std::string>>("Currents through vcvs", ct_voltage_controlled_voltage_source, {"I_vcvs"}),
            std::make_tuple<std::string, component_types, std::vector<std::string>>("Currents through cccs", ct_current_controlled_current_source, {"I_cccs"}),
            std::make_tuple<std::string, component_types, std::vector<std::string>>("Currents through ccvs", ct_current_controlled_voltage_source, {"Ifin", "Ifout"})
        };

        std::cout << "    Node voltages:\n";
        for(; row < nlist.number_of_nodes(); ++row)
        {
            std::string usernode = nlist.get_output_node(row + 1);
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
    else if(mode == "tf")
    {
        if(nodes.size() != 2){
            std::cerr << "You have to specify two nodes (by using the -n argument two times)"<< '\n';
            exit(1);
        }
        if(nodes[0] == "0" || nodes[1]== "0" || nodes[0] == "GND" || nodes[1] == "GND" )
        {
            std::cerr << " You can't use the GND node (node 0)" << '\n';
            exit(1);
        }
        unsigned int first = nlist.get_unode(nodes[0]) - 1;
        unsigned int second= nlist.get_unode(nodes[1]) - 1;
        GiNaC::ex H = results(second, 0) / results(first, 0);
        std::cout << "H(s) = " << H << '\n';
        std::cout << "H(s) to Latex" << '\n';
        std::cout << "H(s) = " <<GiNaC::latex << H << '\n';
        if(matlab_export)
        {
            vvtf_matlab_export(filename,first,second);
        }
        if(nlist.is_simplification())
        { 
//             std::string H_simple = vvtf_simplification(H);
            std::cout << " Simplified (in Latex) "<< '\n';
//             std::cout << "H(s) = " << H_simple << '\n';
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

void solver::matrices_to_matlab(const std::string& filename)
{
    std::string a = filename;
    std::ofstream ofile(filename +".m", std::ofstream::out);
    ofile << "Matlab-Export " << std::endl;
    ofile << " %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% " << std::endl;
    ofile << " %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% " << std::endl;

    ofile << " Matrices:" << std::endl;
    // Print matrices A, x , z 
    unsigned int A_rows =  A.rows();
    unsigned int A_cols = A.cols();

    ofile << " A = [ " << std::endl;
    for (unsigned int i = 0;i < A_rows ;i++)
    {
        ofile << "    sym('" << A[i][1]<<"'), ";
        for( unsigned int j = 1 ; j < A_cols ; j++)
        {
            ofile <<"     sym('"<<A[i][j]<<"'), ";
        }
    
    }
    ofile << " ]" << std::endl;
    unsigned int z_rows = z.rows();
    ofile << " z = [ " << std::endl;
    for (unsigned int i = 0 ; i < z_rows;i++)
    {
        ofile << "  sym ('"<< z[i] <<"'), ";
    }
    ofile << " ] " << std::endl;
    ofile << " x = simplify ( inv(A) * z)" << std::endl;
}

void solver::vvtf_matlab_export(std::string& filename, unsigned int first, unsigned int second)
{
    const char *path="/home/jan/Desktop";// make it relative ?
    std::size_t slash = filename.find("/");
    filename.erase(0,slash);
    std::size_t dot = filename.find(".");
    filename.erase(dot, std::string::npos);
    std::string name = path + filename + ".m";
    std::ofstream ofile(name, std::ofstream::out);
    std::vector<std::string> values= nlist.get_values();
    
    ofile << "% Matlab-Export: " << std::endl;
    ofile << "% Voltage-Voltage Transfer Function"<< std::endl;
    ofile << " %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% " << std::endl;
    ofile << " %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% " << std::endl;
    
    for(unsigned int i = 0 ; i< values.size();i++)
    {
        ofile << values[i] << " =  1e0 ; "<< std::endl;
    }
    GiNaC::ex  H = results(second,0)/results(first,0);
    ofile << "s = tf('s'); "<< std::endl;
    ofile << "H = " <<results(second,0)/results(first,0) << ";" <<std::endl;
    ofile << std::endl;
    ofile << std::endl;
    ofile << std::endl;
    ofile << "% Plot Parameters " << std::endl;
    ofile << std::endl;
    ofile << " % Bode-Plot" << std::endl;
    ofile << " figure('Name','Bode-Diagram','NumberTitle','off'); " << std::endl;
    ofile << " bode (H); " << std::endl;
    ofile << " grid on ; " << std::endl;
    ofile << std::endl;
    ofile << std::endl;
    ofile << " % Sprungantwort "<< std::endl;
    ofile << " figure('Name','Step-, Impulse-response ','NumberTitle','off'); " << std::endl;
    ofile << " subplot(2,1,1) " << std::endl;
    ofile << " step(H)      " << std::endl;
    ofile << " subplot(2,1,2) " << std::endl;
    ofile << " impulse(H)   " << std::endl;
    ofile << " %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% " << std::endl;
    ofile << " %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% " << std::endl;
    ofile << " % Transfer Function for Latex " << std::endl;
    ofile << " %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% " << std::endl;
    ofile << GiNaC::latex << results(second,0)/results(first,0)<< std::endl;
    ofile << GiNaC::dflt  << std::endl;
}

std::string solver::vvtf_funct_to_latex_string(GiNaC::ex H)
{
    std::ostringstream s;
    s << GiNaC::latex;
    s << H;
    s << GiNaC::dflt;
    std::string string_funct = s.str();
    return string_funct;
}
