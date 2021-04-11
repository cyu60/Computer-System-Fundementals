//main file
//magarw13 - Mehul Agarwal
//cyu60 - Chinat Yu

#include "calc.h"
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include <sys/types.h>
#include <map>

using std::map;
using std::cout;
using std::endl;


struct Calc {
private:
    // fields
    map<char, int> vars;

public:
    // public member functions
    Calc();
    ~Calc();

    int evalExpr(const std::string &expr, int &result);

// private:
    // private member functions
};

Calc::Calc() {
    cout << "create calc" << endl;
}

Calc::~Calc() {
    cout << "destroy calc" << endl;
}

int Calc::evalExpr(const std::string &expr, int &result) {
    return 0;
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

// struct Calc *calc_create(void);
// void calc_destroy(struct Calc *calc);
// int calc_eval(struct Calc *calc, const char *expr, int *result);