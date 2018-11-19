
#include "parser.h"
#include "literal.h"
#include <sstream>
#include <vector>
#include <unordered_set>

sat::Parser::Parser(std::string inputFile): fin(inputFile)
{
    if(!fin.good()) {
        throw std::invalid_argument("Cannot access file");
    }
}

sat::Formula sat::Parser::getFormula()
{
    if (parsed) {
        throw std::logic_error("Solver already built.");
    }

    std::string line;
    std::unordered_set<int> literalSet;
    std::vector<Literal> literals;
    Formula formula(0);

    // parse lines
    while (std::getline(fin, line)) {
        if (line.empty()) {
            continue;
        }
        const char c = line.at(0);
        if (c == 'c') {
            // line is comment
            continue;
        } else if (c == 'p') {
            if (numVars > 0) {
                throw std::invalid_argument("Invalid input file.");
            }
            std::istringstream iss(line);
            std::string s, t;
            iss >> s >> t;
            if (s != "p" || t != "cnf") {
                throw std::invalid_argument("Invalid input file. Unrecognized format.");
            }
            iss >> numVars >> numClauses;
            if (numVars <= 0 || numClauses <= 0) {
                throw std::invalid_argument("Invalid input file, bad number of variables or clauses.");
            }
            formula = Formula(numVars);
        } else if (c == '-' || (c > '0' && c <= '9')) {
            literalSet.clear();
            literals.clear();
            std::istringstream iss(line);
            int var;
            while (iss >> var && var != 0) {
                Literal t(var);
                if (literalSet.find(t.id()) == literalSet.end()) {
                    literalSet.insert(t.id());
                    literals.push_back(t);
                }
            }
            if (!literals.empty()) {
                formula.addClause(Clause(literals));
            }
        } else {
            throw std::invalid_argument("Invalid input file. Unrecognized format.");
        }
    } // end while

    parsed = true;
    return formula;
}
