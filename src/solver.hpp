#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <string>
#include <vector>

#include <ginac/ginac.h>

#include "netlist.hpp"

class solver
{
    public:
        solver(const std::string& filename, const std::string& mode,std::vector<std::string> nodes, netlist& nlist, bool matlab_export);
        void mna();
        void solve();
        void print();
        void print_matrices();
        void matrices_to_matlab(const std::string& filename);
        void vvtf_matlab_export(std::string& filename, unsigned int first, unsigned int second);
        std::string vvtf_funct_to_latex_string(GiNaC::ex H);
        //experimental Simplification of terms for Voltage-Voltage Transfer Function
        //std::string vvtf_simplification(GiNaC::ex H);
        //std::string simplify_line(std::string s);
        //std::string sim_replace(std::string v);
    private:
        std::string filename;
        std::string mode;
        std::vector<std::string> nodes;
        netlist& nlist;
        bool matlab_export;
        GiNaC::matrix A;
        GiNaC::matrix x;
        GiNaC::matrix z;
        GiNaC::matrix results;
};

#endif // SOLVER_HPP
