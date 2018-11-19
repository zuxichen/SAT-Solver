
#ifndef SAT_PARSER_H
#define SAT_PARSER_H

#include "clause.h"
#include "formula.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>

namespace sat
{
    class Parser
    {
    public:
        Parser(std::string inputFile);
        Formula getFormula();

    private:
        std::ifstream fin;
        bool parsed = false;
        int numVars = 0, numClauses = 0;
        std::vector<Clause> clauses;
    };
}

#endif // SAT_PARSER_H
