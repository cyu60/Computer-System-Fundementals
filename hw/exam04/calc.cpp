//main file
//magarw13 - Mehul Agarwal
//cyu60 - Chinat Yu

#include "calc.h"
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <cctype>

#include <sys/types.h>
#include <map>
#include <vector>
#include <sstream>      // std::stringstream

#define SUCCESS 1
#define FAILURE 0

using std::map;
using std::cout;
using std::endl;
using std::stoi;


using std::vector;
using std::string;

struct Calc {
private:
    // fields
    map<string, int> vars;
    pthread_mutex_t lock;

public:
    // public member functions
    Calc();
    ~Calc();
    int evalExpr(const std::string &expr, int &result);

private:
    // private member functions
    std::vector<std::string> tokenize(const std::string &expr);
    int getOperand(string s);
    int evalOp(string op, int left, int right, int &result);

};

std::vector<std::string> Calc::tokenize(const std::string &expr) {
    std::vector<std::string> vec;
    std::stringstream s(expr);

    std::string tok;
    while (s >> tok) {
        vec.push_back(tok);
    }

    return vec;
}
int Calc::getOperand(string s) {
    try
        {
            return stoi(s);
        }
        catch(const std::invalid_argument& ia)
        {
            // check if var exist?
            return this->vars.at(s);
        }
}

int Calc::evalOp(string op, int left, int right, int &result) {

    if (op.compare("+") == 0)
        result = left + right;
    if (op.compare("-") == 0)
        result = left - right;
    if (op.compare("*") == 0)
        result = left * right;
    if (op.compare("/") == 0) {
        if (right == 0) {
            throw "division by 0";
        }
        result = left / right;
    }

    // cout << result << endl;
    return result;
}

int Calc::evalExpr(const std::string &expr, int &result) {
    // tokenise the expression
    vector<string> tokens = this->tokenize(expr);

    try
    {
    // check expression type
    // operand
    if (tokens.size() == 1) {
            result = this->getOperand(tokens.at(0));
            return SUCCESS;
    }

    // var = operand (Assignment)
    if (tokens.size() == 3) {
        if (tokens.at(1) == "=") {
            pthread_mutex_lock(&lock);
            // get the operand
            result = this->getOperand(tokens.at(2));
            // save to var -- use mut lock!!!
            this->vars[tokens.at(0)] = result;
            pthread_mutex_unlock(&lock);


            return SUCCESS; 
        } else {
            // operand op operand
            // get the op
            string op = tokens.at(1);
            // get left, right
            int left = this->getOperand(tokens.at(0));
            int right = this->getOperand(tokens.at(2));
            // eval op
            this->evalOp(op, left, right, result);
            return SUCCESS;
        }
    }

    // var = operand op operand
    if (tokens.size() == 5) {
        // get the op
        string op = tokens.at(3);

        // lock vars
        pthread_mutex_lock(&lock);
        // get left, right
        int left = this->getOperand(tokens.at(2));
        int right = this->getOperand(tokens.at(4));
        // eval op
        this->evalOp(op, left, right, result);

        // save to var
        this->vars[tokens.at(0)] = result;
        pthread_mutex_unlock(&lock);        // cout << result << endl;
        return SUCCESS;
    }
    
    }
    // catch (const std::out_of_range& oor) {
    catch (...) { // include out of range and fp
        return FAILURE;
    } 

    return FAILURE;
}

Calc::Calc() {
    pthread_mutex_init(&lock, NULL);
}

Calc::~Calc() {
    pthread_mutex_destroy(&lock);
}

extern "C" struct Calc *calc_create(void) {
    return new Calc();
}

extern "C" void calc_destroy(struct Calc *calc) {
    delete calc;
}

extern "C" int calc_eval(struct Calc *calc, const char *expr, int *result) {
    return calc->evalExpr(expr, *result);
}
