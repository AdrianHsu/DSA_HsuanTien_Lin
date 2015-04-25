#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <stack>
#include <queue>
#include <string>

using namespace std;

//http://en.cppreference.com/w/c/language/operator_precedence
int op_precedence( string );

bool is_digit(char);
bool is_operator(string);
bool right_associate(string);
bool is_unary_op(string);
int string2Int(const string& );
string int2String(const int& );

void compare(queue< string > &, stack<string > &, const string);
bool input_queue(queue< string > &, const string);
int result(queue< string >);

int main()
{
    string input; 

    while( getline(cin, input))
    {
        queue< string > my_queue;
        if( input_queue(my_queue, input) )
        {
            printf("Postfix Exp: ");
            queue< string > p_queue = my_queue; //p for print out
            while(!p_queue.empty())
            {
                if( p_queue.front() == "+u")
                    printf("+");
                else if( p_queue.front() == "-u")
                    printf("-");
                else
                    printf("%s", p_queue.front().c_str());
                p_queue.pop();
                if(!p_queue.empty())
                    printf(" ");
            }
            printf("\n");
            printf("RESULT: %d\n", result(my_queue));
        }
    }
    return 0;
}
//if precedence larger, then return int smaller (priority)

int op_precedence(string str)
{
    if(str == "+u" || str == "-u")
        return 2;
    else if(str == "<<" || str == ">>")
        return 5;
    else if(str == "&&")
        return 11;
    else if(str == "||")
        return 12;

    char op = str[0];
    switch(op)  {

        case '!': case '~':
            return 2;
            break;
        case '*': case '/': case '%':
            return 3;
            break;
        case '+': case '-':
            return 4;
            break;
        case '&':
            return 8;
            break;
        case '^':
            return 9;
            break;
        case '|':
            return 10;
            break;
        case '(':
            return 99;
            break;
        default:
            return op - 'A';
            break;
    }
    return 0; //error
}
bool is_digit(char d)
{
    if(d >= '0' && d <= '9')
        return true;
    else
        return false;
}
bool is_operator(string str)
{
    if(str.size() == 2)
    {
        if(str == "<<" || str == ">>" || str == "&&" || str == "||" || str == "+u" || str == "-u")
            return true;
        else
            return false;
    }
    else if(str.size() == 1)
    {
        char op = str[0];
        if(op == '!' || op == '*' || op == '/' || op == '%' || op == '+' ||
                op == '~' || op == '-' || op == '&' || op == '^' || op == '|')
            return true;
        else
            return false;
    }
    return false;
}
bool right_associate(string op)
{
    if(op == "+u" || op == "-u" || op == "!" || op == "~")
        return true;
    else
        return false;
}
bool is_unary_op(string op)
{
    if(op == "+u" || op == "-u" || op == "!" || op == "~")
        return true;
    else
        return false;
}
int string2Int(const string& str)
{
    int num = 0;
    for (int i = 0; i < str.size(); i++) {
        if (isdigit(str[i])) {
            num *= 10;
            num += int(str[i] - '0');
        }
    }
    return num;
}
string int2String(const int &i) 
{
    string s;
    stringstream ss(s);
    ss << i;

    return ss.str();
}
void compare(queue< string >& my_queue, stack< string >& op_stack, const string _op) // compare input op with my_queue
{
    if(right_associate(_op))
    {
        while(op_precedence(_op) > op_precedence(op_stack.top()))
        {
            my_queue.push(op_stack.top());
            op_stack.pop();
            if(op_stack.empty())
                break;
        } 
    }
    else
    {    
        while(op_precedence(_op) >= op_precedence(op_stack.top()))
        {
            my_queue.push(op_stack.top());
            op_stack.pop();
            if(op_stack.empty())
                break;
        } 
    }
    return;
}
bool input_queue(queue< string > & my_queue, const string in)
{
    int i = 0;
    stack< string > op_stack;
    while( i < in.length() )
    {
        //space ignore
        if(in[i] == ' ')
        {
            i++;
            continue;
        }
        //digit push directly
        if(is_digit(in[i]))
        {
            string _int;
            while( is_digit(in[i]) && i < in.length())
            {
                _int += in[i];
                i++;
            }
            i--;
            my_queue.push(_int);
        } 
        string _op;
        _op += in[i]; //char can't be assigned to string
        //judge '(' first
        if(in[i] == '(')
        {
            op_stack.push(_op);
        }
        if(in.size() > i + 1) //in case that in[i + 1] is garbage 
            _op += in[i + 1];
        bool _2_char = false; // is 2 char operator

        //check 2 char operator
        if(is_operator(_op) && in.size() > i + 1) 
        {
            if(!op_stack.empty())
                compare(my_queue, op_stack, _op);
            op_stack.push(_op);
            _2_char = true;
            i++;
        }
        if(in.size() > i + 1)
            _op.erase(_op.end() - 1); //resize _op into 1 char operator
        //check 1 char opeator
        if(is_operator(_op) && !_2_char)
        {
            bool _is_binary = 1;
            if(_op == "+" || _op == "-") //unary
            {   
                if(i == 0) // in[i - 1] might be -1
                {
                    op_stack.push(_op + "u"); 
                    _is_binary = 0;
                }
                else
                {
                    int j = i - 1;
                    while(in[j] == ' ' && j > 0)
                        j--;
                    string tmp; //is_operator receives string, thus not use in[j]
                    tmp += in[j];
                    if(is_operator(tmp) || in[j] == '(' || in[j] == '>' || in[j] == '<') //consider 2 char operator
                    {   
                        _op += "u"; //to differentiate from binary +, -
                        if(!op_stack.empty())
                            compare(my_queue, op_stack, _op);
                        op_stack.push(_op);
                        _is_binary = 0;
                    }
                }
            }
            if(_is_binary)//binary
            {  
                if(!op_stack.empty())
                    compare(my_queue, op_stack, _op);
                op_stack.push(_op);
            }
        }
        if(in[i] == ')')
        {
            bool flag = 1;
            while(flag)
            {
                if(op_stack.empty())
                {
                    printf("Error: parentheses mismatched\n");
                    return false;
                }
                if(op_stack.top() != "(")
                {
                    my_queue.push(op_stack.top());
                }
                else
                    flag = 0;
                op_stack.pop();
            } 
        }
        i++;
    }
    while(!op_stack.empty())
    {
        my_queue.push(op_stack.top());
        op_stack.pop();
    } 
    return true;
}
int result(queue< string > my_queue)
{
    stack < int > my_stack;
    while(!my_queue.empty())
    {
        string op = my_queue.front();
        if(is_operator(op))
        {
            my_queue.pop();
            int r_val = my_stack.top();
            
            my_stack.pop();
            if(is_unary_op(op))
            {
                if(op == "+u")
                    r_val *= (+1);
                else if(op == "-u")
                    r_val *= (-1);
                else if(op == "!")
                    r_val = !(r_val);
                else if(op == "~")
                    r_val = ~(r_val);
                else
                    printf("ERROR#1\n");

                my_stack.push(r_val);
            }
            else //is binary op
            {
                int l_val = my_stack.top();
                int result;
                my_stack.pop();

                if(op.size() == 1)
                {
                    switch(op[0])  {
                        case '*':
                            result = l_val * r_val;
                            break;
                        case '/':
                            result = l_val / r_val;
                            break;
                        case '%': 
                            result = l_val % r_val;
                            break;
                        case '+':
                            result = l_val + r_val;
                            break;
                        case '-':
                            result = l_val - r_val;
                            break;
                        case '&':
                            result = l_val & r_val;
                            break;
                        case '^':
                            result = l_val ^ r_val;
                            break;
                        case '|':
                            result = l_val | r_val;
                            break;
                        default:
                            printf("ERROR#2\n");
                            break;
                    }
                }
                else
                {
                    if(op == "<<")
                        result = l_val << r_val;
                    else if(op == ">>")
                        result = l_val >> r_val;
                    else if(op == "&&")
                        result = l_val && r_val;
                    else if(op == "||")
                        result = l_val || r_val;
                    else
                        printf("ERROR#3\n");
                }
                my_stack.push(result);
            }
        }
        else
        {
            my_stack.push( string2Int( my_queue.front() ) );
            my_queue.pop();
        }
    }
    return my_stack.top();
}
