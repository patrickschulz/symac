#include "util.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <boost/format.hpp>

void print_matrix(const GiNaC::matrix& m, const std::string& header)
{
    std::cout << header << ":\n";
    print_matrix(m);
    std::cout << '\n';
}

void print_matrix(const GiNaC::matrix& m)
{
    const unsigned int sep = 3;

    std::vector<unsigned int> maxlengths(m.cols(), 0);

    // output format
    auto format = GiNaC::dflt;

    for(unsigned int column = 0; column < m.cols(); ++column)
    {
        std::string::size_type maxcollength = 0;
        for(unsigned int row = 0; row < m.rows(); ++row)
        {
            std::ostringstream stream;
            stream << format << m(row, column);
            maxcollength = std::max(maxcollength, stream.str().size());
        }
        maxlengths[column] = maxcollength;
    }

    for(unsigned int row = 0; row < m.rows(); ++row)
    {
        for(unsigned int column = 0; column < m.cols(); ++column)
        {
            const unsigned int width = maxlengths[column];
            std::ostringstream stream;
            stream << format << m(row, column);
            std::cout << std::setw(width + sep) << stream.str();
        }
        std::cout << '\n';
    }
}

void print_results(const netlist& nlist, const GiNaC::matrix& m, const GiNaC::matrix& x, const std::vector<result>& results)
{
    for(auto res : results)
    {
        int idx = res.get_index(x);
        if(idx >= 0)
        {
            std::cout << m(idx, 0) << '\n';
        }
    }
}

void print_results(const netlist& nlist, const GiNaC::matrix& m)
{
    std::cout << "Results:\n";
    std::cout << GiNaC::csrc;
    unsigned int row = 0;

    std::vector<std::tuple<std::string, component_types, std::string>> dev_formats = {
        std::make_tuple("Currents through impedances", ct_resistor | ct_capacitor | ct_inductor, "Iz"),
        std::make_tuple("Currents through voltage sources", ct_voltage_source, "Iv"),
        std::make_tuple("Currents into opamps (output)", ct_opamp, "Iop"),
        std::make_tuple("Currents through vcvs", ct_voltage_controlled_voltage_source, "Ie"),
        std::make_tuple("Currents through cccs", ct_current_controlled_current_source, "Ihin")
    };

    std::cout << "    Node voltages:\n";
    for(; row < nlist.number_of_nodes(); ++row)
    {
        boost::format fmter = boost::format("        Node %d:\t\t") % (row + 1);
        std::string str = fmter.str();
        std::cout << str << m(row, 0) << '\n';
    }
    std::cout << '\n';
    for(unsigned int dev = 0; dev < dev_formats.size(); ++dev)
    {
        std::string header;
        component_types ct;
        std::string current;
        std::tie(header, ct, current) = dev_formats[dev];
        unsigned int size = nlist.number_of_devices(ct);
        if(size > 0)
        {
            std::cout << "    " << header << ":\n";
            for(unsigned int i = 0; i < size; ++i)
            {
                boost::format fmter = boost::format("        %s%d:\t\t") % current % (i + 1);
                std::string str = fmter.str();
                std::cout << str << m(row, 0) << '\n';
                ++row;
            }
            std::cout << '\n';
        }
    }
    // current controlled voltage sources need TWO lines, so they cannot be automatically handled by the above code.
    // This could be fixed by using different formatting functions, but currently i'm too lazy to write this
    for(unsigned int i = 0; i < nlist.number_of_devices(ct_current_controlled_voltage_source); i += 2)
    {
        boost::format fmter1 = boost::format("        Ifin%d:\t\t") % (i + 1);
        std::string str1 = fmter1.str();
        boost::format fmter2 = boost::format("        Ifout%d:\t\t") % (i + 1);
        std::string str2 = fmter2.str();
        std::cout << str1 << m(row,     0) << '\n';
        std::cout << str2 << m(row + 1, 0) << '\n';
        row += 2;
    }
}

void create_report(const netlist& nlist, const GiNaC::matrix& m)
{
    std::ofstream file("report.tex");
    file << GiNaC::latex;
    unsigned int row = 0;

    file << R"(\documentclass{scrartcl})" << '\n';
    file << R"(\usepackage{amsmath})" << '\n';
    file << R"(\begin{document})" << '\n';

    std::vector<std::tuple<std::string, component_types, std::string>> dev_formats {
        std::make_tuple("Currents through impedances", ct_resistor | ct_capacitor | ct_inductor, "Iz"),
        std::make_tuple("Currents through voltage sources", ct_voltage_source, "Iv"),
        std::make_tuple("Currents into opamps (output)", ct_opamp, "Iop"),
        std::make_tuple("Currents through vcvs", ct_voltage_controlled_voltage_source, "Ie"),
        std::make_tuple("Currents through cccs", ct_current_controlled_current_source, "Ihin")
    };

    file << "\nNode voltages\n";
    file << R"(\begin{align*})" << '\n';
    for(; row < nlist.number_of_nodes(); ++row)
    {
        file << "v_{" << row + 1 << '}' << " &= " << m(row, 0) << R"(\\)" << '\n';
    }
    file << R"(\end{align*})" << '\n';

    for(unsigned int dev = 0; dev < dev_formats.size(); ++dev)
    {
        std::string header;
        component_types ct;
        std::string current;
        std::tie(header, ct, current) = dev_formats[dev];
        unsigned int size = nlist.number_of_devices(ct);
        if(size > 0)
        {
            file << '\n' << header << '\n';
            file << R"(\begin{align*})" << '\n';
            for(unsigned int i = 0; i < size; ++i)
            {
                file << current << "_{" << i + 1 << '}' << " &= " << m(row, 0) << R"(\\)" << '\n';
                ++row;
            }
            file << R"(\end{align*})" << '\n';
        }
    }
    //// current controlled voltage sources need TWO lines, so they cannot be automatically handled by the above code.
    //// This could be fixed by using different formatting functions, but currently i'm too lazy to write this
    //for(unsigned int i = 0; i < nlist.number_of_devices(ct_current_controlled_voltage_source); i += 2)
    //{
    //    boost::format fmter1 = boost::format("        Ifin%d:\t\t") % (i + 1);
    //    std::string str1 = fmter1.str();
    //    boost::format fmter2 = boost::format("        Ifout%d:\t\t") % (i + 1);
    //    std::string str2 = fmter2.str();
    //    std::cout << str1 << m(row,     0) << '\n';
    //    std::cout << str2 << m(row + 1, 0) << '\n';
    //    row += 2;
    //}
    file << R"(\end{document})";
}
