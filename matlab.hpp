/* Matlab Export for plotting etc */
#ifndef MATLAB_HPP
#define MATLAB_HPP

#include <iostream>
#include <fstream>
#include <string>

using namespace GiNaC

void matlab_export(std::string& filename, matrix A, matrix x, matrix z)
{
// TODO
//     Steps for matlab export


    std::ofstream ofile(filename + ".m");
    ofile << "Matlab-Export " << endl;
    ofile << " %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% " << endl;
    ofile << " %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% " << endl;

    ofile << " Matrices:" << endl;
    // Print matrices A, x , z 
    int rows =  sizeof A / sizeof A[0];
    int cols = sizeof A[0] / sizeof(int);

    ofile << " A = [ " << endl;
    for (unsigned int i = 0;i < rows ;i++)
    {
        ofile << ("    sym('%s'), ", A[i][1]);
        for( unsigned int j = 1 ; j < cols ; j++)
        {
            ofile << ( "sym('%s'), ", A[i][j]);
        }
    }
    ofile << " ]" << endl;

    ofile << " z = [ " << endl;
    for (unsigned int i = 0 ; i < z.size(); i++)
    {
        ofile << ("  sym ('%s') ", z[i][1]);
    }
    ofile << " ] " << endl;
    ofile << " x = simplify ( inv(A) * z)" << endl;
}







#endif // MATLAB_HPP
