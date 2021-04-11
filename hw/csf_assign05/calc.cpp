#include "calc.h"


struct Calc {
private:
    // fields

public:
    // public member functions
    Calc();
    ~Calc();
    extern "C" struct Calc *calc_create(void) {
        return new Calc();
    }

    extern "C" void calc_destroy(struct Calc *calc) {
        delete calc;
    }

    extern "C" int calc_eval(struct Calc *calc, const char *expr, int *result) {
        return calc->evalExpr(expr, *result);
    }
    int evalExpr(const std::string &expr, int &result);

private:
    // private member functions
};
