/*
 * A calculator that computes given expressions.
 */

#include "calc.h"
#include <map>
#include <iterator>
#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>

struct Calc
{
};

// Calc implementation with a dictionary of variables.
class CalcImpl : public Calc
{
  private:
  // A map that will store <key, value> pairs of variables and their values.
  std::map<std::string, int> variableDict;

  public:
  // An exposed function that we can use to find the value of an assigned variable.
  // Returns true when found, and false when not found.
  bool has(std::string variable, int *value)
  {
    for (std::map<std::string, int>::iterator it = variableDict.begin(); it != variableDict.end(); ++it)
    {
      if (it->first.compare(variable) == 0)
      {
        *value = it->second;
        return true;
      }
    }
    return false; 
  }

  // An exposed function that we can use to add a new key-value pair to the variable dictionary.
  void add(std::string key, int value)
  {
    variableDict[key] = value;
  }

  // A helper function that removes space, new line, and return characters from a string.
  int parseEnd(std::string string, std::string *outString)
  {
    std::string::const_iterator it = string.begin();
    while (it != string.end())
    {
      if (*it != ' ' && *it != '\n' && *it != '\r')
      {
        (*outString).push_back(*it);
      }
      ++it;
    }
    return checkVar(*outString);
  }

  // A helper function that checks if a string is a variable or number.
  int checkVar(std::string string)
  {
    std::string::const_iterator it = string.begin();
    int isVar = 0;
    while (it != string.end())
    {
      if (!isVar && !std::isdigit(*it) && *it != ' ' && *it != '\n' && *it != '\r' && *it != '-')
      {
        isVar = 1;
      }
      ++it;
    }
    return isVar;
  }

  // A helper function that checks if a string contains an operator.
  int checkOp(std::string string)
  {
    std::string::const_iterator it = string.begin();
    std::string checker = "";
    while (it != string.end() && *it != '*' && *it != '/' && *it != '+' && *it != '=')
    {
      if (*it == '-' && !(checker.length() == 0))
      {
        break;
      }
      if (*it != ' ' && *it != '\n' && *it != '\r')
      {
        checker.push_back(*it);
      }
      ++it;
    }
    return !string.empty() && it == string.end();
  }

  // Helper function that is run if there is a single operand, checks if it's a number or variable,
  // and returns 1 if it successfully finds the result and 0 if it doesn't.
  int singleOperand(std::string input, int *result)
  {
    int checkSingle;
    std::string operand;
    if (parseEnd(input, &operand) && has(operand, &checkSingle))
    {
      *result = checkSingle;
    }
    else if (!checkVar(operand))
    {
      *result = std::stoi(input);
    }
    else
    {
      return 0;
    }
    return 1;
  }

  // Helper function that assigns an operand to a variable. Returns 1 on success and 0 if error.
  int assignSingle(int *result, std::string toBeAssigned, std::string variable)
  {
    std::string outString;
    parseEnd(toBeAssigned, &outString);
    if (checkVar(outString))
    {
      int finding;
      has(outString, &finding);
      add(variable, finding);
      *result = finding;
    }
    else
    {
      add(variable, std::stoi(outString));
      *result = std::stoi(outString);
    }
    return 1;
  }

  // Helper function that performs an operation before assigning it to a variable. Returns 1 on success and 0 if error.
  int assignOperation(int *result, std::string toBeAssigned, std::string variable)
  {
    int evaledResult = 0;
    int evaled = evalExpr(toBeAssigned.c_str(), &evaledResult);
    if (!evaled)
    {
      return 0;
    }
    add(variable, evaledResult);
    *result = evaledResult;
    return 1;
  }

  // Helper function for assignments. Returns 1 on success and 0 if error.
  int assignment(std::string input, int *result)
  {
    std::string variable;
    parseEnd(input.substr(0, input.find("=")), &variable);
    std::string toBeAssigned = input.substr(input.find("=") + 1, input.length());
    

    if (checkOp(toBeAssigned))
    {
      // situation: var = operand
      return assignSingle(result, toBeAssigned, variable);
    }
    else
    {
      // situation: var = operand op operand
      return assignOperation(result, toBeAssigned, variable);
    }
  }

  // Helper function that gets a parsed operand from the input string. Returns 1 on success and 0 if error.
  int getOperand(std::string input, int start, int end, int *operand)
  {
    std::string operandString;
    if (parseEnd(input.substr(start, end), &operandString))
    {
      if (!has(operandString, operand))
      {
        return 0;
      }
      return 1;
    }
    else
    {
      if (operandString.length() == 0)
      {
        return 0;
      }
      *operand = std::stoi(operandString);
      return 1;
    }
  }

  // Helper function that performs a given operation. Returns 1 on success and 0 if error.
  int performOperation(std::string input, int opIndex, int firstOperand, int secondOperand, int *result)
  {
    if (input[opIndex] == '+')
    {
      *result = firstOperand + secondOperand;
      return 1;
    }
    else if (input[opIndex] == '-')
    {
      *result = firstOperand - secondOperand;
      return 1;
    }
    else if (input[opIndex] == '*')
    {
      *result = firstOperand * secondOperand;
      return 1;
    }
    else if (input[opIndex] == '/' && secondOperand != 0)
    {
      *result = firstOperand / secondOperand;
      return 1;
    }
    return 0;
  }

  // Helper function that performs the processes the inputted operation. Returns 1 on success and 0 if error.
  int handleOperation(std::string input, int *result)
  {
    int opIndex = 0;
    std::string::const_iterator itOp = input.begin();
    std::string checker = "";
    while (itOp != input.end() && *itOp != '*' && *itOp != '/' && *itOp != '+' && *itOp != '=')
    {
      if (*itOp == '-' && !(checker.length() == 0))
      {
        break;
      }
      if (*itOp != ' ' && *itOp != '\n' && *itOp != '\r')
      {
        checker.push_back(*itOp);
      }
      opIndex++;
      ++itOp;
    }
    int firstOperand;
    if (!getOperand(input, 0, opIndex, &firstOperand))
    {
      return 0;
    }
    int secondOperand;
    if (!getOperand(input, opIndex + 1, input.length(), &secondOperand))
    {
      return 0;
    }
    return performOperation(input, opIndex, firstOperand, secondOperand, result);
  }

  // Helper function that evaluates the given expression. Returns 1 on success and 0 if error.
  int evalExpr(const char *expr, int *result)
  {
    // convert expr to a string
    std::string input(expr);

    // If there is only one operand input:
    if (checkOp(input))
    {
      return singleOperand(input, result);
    }

    // Assignment: If there is an equals sign, assign the right side to the left.
    if (input.find("=") > 0 && input.find("=") < input.length())
    {
      return assignment(input, result);
    }

    // situation: operand op operand
    return handleOperation(input, result);
  }
};

extern "C" struct Calc *calc_create(void)
{
  return new CalcImpl();
}

extern "C" void calc_destroy(struct Calc *calc)
{
  CalcImpl *obj = static_cast<CalcImpl *>(calc);
  delete obj;
}

extern "C" int calc_eval(struct Calc *calc, const char *expr, int *result)
{
  CalcImpl *obj = static_cast<CalcImpl *>(calc);
  return obj->evalExpr(expr, result);
}