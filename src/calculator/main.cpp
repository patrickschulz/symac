#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <iostream>
#include <cmath>

using namespace std;

bool is_number(char ch)
{
    return ch == '1' ||
           ch == '2' ||
           ch == '3' ||
           ch == '4' ||
           ch == '5' ||
           ch == '6' ||
           ch == '7' ||
           ch == '8' ||
           ch == '9' ||
           ch == '0';
}

bool is_operator(char ch)
{
    return ch == '+' ||
           ch == '-' ||
           ch == '*' ||
           ch == '/' ||
           ch == '^';
}

double eval(char ch)
{
    int val = ch - '0';
    return val;
}

double eval(char oper, double op1, double op2)
{
    if(oper == '+')
    {
        return op1 + op2;
    }
    if(oper == '-')
    {
        return op1 - op2;
    }
    if(oper == '*')
    {
        return op1 * op2;
    }
    if(oper == '/')
    {
        return op1 / op2;
    }
    if(oper == '^')
    {
        return pow(op1, op2);
    }
}

int precedence(char oper)
{
    if(oper == '+' || oper == '-')
    {
        return 0;
    }
    else if(oper == '*' || oper == '/')
    {
        return 1;
    }
    else
    {
        return 2;
    }
}

int main(int argc, char** argv)
{
    string input;
    while (getline(cin, input))
    {
        if (input.empty() || input[0] == 'q' || input[0] == 'Q')
            break;
        cout << '\n';

        // shunting-yard algorithm
        queue<char> output;
        stack<char> operators;
        for(int i = 0; i < input.size(); ++i)
        {
            char token = input[i];
            if(is_number(token))
            {
                output.push(token);
            }
            else if(is_operator(token))
            {
                while(!operators.empty() && operators.top() != '(' &&
                        (
                             (precedence(operators.top()) > precedence(token)) 
                          || (precedence(operators.top()) == precedence(token) && operators.top() != '^') 
                        )
                     )
                {
                    output.push(operators.top());
                    operators.pop();
                }
                operators.push(token);
            }
            else if(token == '(')
            {
                operators.push(token);
            }
            else if(token == ')')
            {
                while(!operators.empty() && operators.top() != '(')
                {
                    output.push(operators.top());
                    operators.pop();
                }
                operators.pop();
            }
            else // ignore
            { }
        }
        while(!operators.empty())
        {
            output.push(operators.top());
            operators.pop();
        }

        // evaluate output queue
        queue<char> pout = output;
        while(!pout.empty())
        {
            cout << pout.front() << ' ';
            pout.pop();
        }
        cout << '\n';
        stack<double> result;
        while(output.size() > 0)
        {
            char ch = output.front();
            output.pop();
            if(is_operator(ch))
            {
                double op2 = result.top(); result.pop();
                double op1 = result.top(); result.pop();
                result.push(eval(ch, op1, op2));
            }
            if(is_number(ch))
            {
                result.push(eval(ch));
            }
        }
        cout << result.top() << "\n\n";
    }
}
