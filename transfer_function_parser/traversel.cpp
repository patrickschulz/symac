#include <iostream>
#include <ginac/ginac.h>

using namespace std;
using namespace GiNaC;

class my_visitor : 
    public visitor,          // this is required
    public add::visitor,     // visit add objects
    public mul::visitor,     // visit mul objects
    public power::visitor,     // visit power objects
    public numeric::visitor, // visit numeric objects
    public symbol::visitor, // visit sumbol objects
    public basic::visitor    // visit basic objects
{
    void visit(const add & x)
    { 
        cout << "called with an add object" << endl;
    }

    void visit(const mul & x)
    { 
        cout << "called with an mul object" << endl;
    }

    void visit(const power & x)
    { 
        cout << "called with an power object" << endl;
    }

    void visit(const numeric & x)
    { 
        cout << "called with a numeric object" << endl;
    }

    void visit(const symbol & x)
    { 
        cout << "called with a symbol object" << endl;
    }

    void visit(const basic & x)
    { 
        cout << "called with a basic object" << endl;
    }
};

int main()
{
    symbol x("x");
    symbol y("y");
    ex e = x * y + x;

    my_visitor v;

    for(const_preorder_iterator i = e.preorder_begin(); i != e.preorder_end(); ++i)
    {
        i->accept(v);
    }
}
