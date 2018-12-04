#include "solver.hpp"

#include <boost/format.hpp>
#include <numeric>
#include <fstream>
#include <sstream>
#include <iostream> // FIXME: remove

#include "mna.hpp"
#include "symbol.hpp"

solver::solver(componentlist& components) :
    components(components)
{
}

void print_network_matrices(const GiNaC::matrix& A, const GiNaC::matrix& x, const GiNaC::matrix& z);

result solver::solve(solver_mode mode)
{
    switch(mode)
    {
        case solve_nport:
        {
            std::vector<component> ports = components.get_components_by_type(ct_port);
            component_types active_port = ct_voltage_source;
            component_types inactive_port = ct_current_source;
            if(ports.size() > 0)
            {
                GiNaC::matrix port_matrix(ports.size(), ports.size());
                for(unsigned int i = 0; i < ports.size(); ++i)
                {
                    componentlist components_tmp(components);
                    for(unsigned int j = 0; j < ports.size(); ++j)
                    {
                        if(j != i)
                        {
                            component c = ports[i];
                            c.set_type(inactive_port);
                            c.set_value(0);
                            components_tmp.add_component(c);
                        }
                    }
                    component p = ports[i];
                    p.set_type(active_port);
                    p.set_value(get_symbol("PORT"));
                    components_tmp.add_component(p);
                    // create matrices and solve
                    GiNaC::matrix A = mna::create_A_matrix(nmap, components_tmp);
                    GiNaC::matrix x = mna::create_x_vector(components_tmp);
                    GiNaC::matrix z = mna::create_z_vector(nmap, components_tmp);
                    GiNaC::matrix res = A.solve(x, z, GiNaC::solve_algo::gauss);
                    for(unsigned int j = 0; j < ports.size(); ++j)
                    {
                        component pp = ports[j];
                        unsigned int numindex = nmap[pp.get_nodes()[0]];
                        unsigned int denindex = components_tmp.network_size();
                        GiNaC::ex numerator = get_symbol("PORT");
                        GiNaC::ex denominator = -res(denindex - 1, 0);
                        // Y-matrix
                        //numerator = -res[components_tmp.network_size()];
                        //denominator = get_symbol("PORT");
                        port_matrix(j, i) = numerator / denominator;
                    }
                }
                std::cout << port_matrix << '\n';
            }
            break;
        }
        case solve_ac:
        {
            // regular AC
            GiNaC::matrix A = mna::create_A_matrix(nmap, components);
            GiNaC::matrix x = mna::create_x_vector(components);
            GiNaC::matrix z = mna::create_z_vector(nmap, components);
            GiNaC::matrix res = A.solve(x, z, GiNaC::solve_algo::gauss);
            return result(components, res, nmap);
            break;
        }
        case solve_noise:
        {
            break;
        }
    }
    // FIXME: this is just a dummy to ensure correct compilation
    // This will be changed once a unified result system for all simulation types is established
    GiNaC::matrix A = mna::create_A_matrix(nmap, components);
    GiNaC::matrix x = mna::create_x_vector(components);
    GiNaC::matrix z = mna::create_z_vector(nmap, components);
    GiNaC::matrix res = A.solve(x, z, GiNaC::solve_algo::gauss);
    return result(components, res, nmap);
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
    //print_network_matrices(A, x, z);
}

/*
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
    //std::vector<std::string> values= nlist.get_values();
    std::vector<std::string> values;
    
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
*/
