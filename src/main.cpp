
#include "solver.h"
#include "parser.h"
#include <iostream>
#include <signal.h>
#include <unistd.h>

sat::Solver* currSolver = nullptr;

static void signalHandler(int signal)
{
    if (currSolver != nullptr) {
        currSolver->cancel();
    }
}

void usage(char * argv0)
{
    std::cerr << argv0 << " <filename> "
              << "[--time timeLimitSeconds] "
              << "[--verbose]"
              << std::endl;
}

int main(int argc, char * argv[])
{
    if (argc < 2) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    using namespace std::string_literals;
    int timeLimitSeconds = 0;
    bool timer = false;
    bool verbose = false;
    char * fileName = nullptr;
    bool error = false;
    for (int i = 1; i < argc; i++) {
        if ("--time"s == argv[i]) {
            if (timer || ++i >= argc) {
                error = true;
                break;
            } else {
                try {
                    timeLimitSeconds = std::stoi(argv[i]);
                    timer = true;
                } catch (const std::invalid_argument& e) {
                    error = true;
                    break;
                }
            }
        } else if ("--verbose"s == argv[i]) {
            verbose = true;
        } else if (fileName == nullptr){
            fileName = argv[i];
        } else {
            error = true;
            break;
        }
    }
    if (error || fileName == nullptr) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    // handle cancellation
    struct sigaction newAction, oldAction;
    newAction.sa_handler = signalHandler;
    sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = 0;
    sigaction(SIGINT, &newAction, &oldAction);

    try {
        sat::Solver solver(sat::Parser(fileName).getFormula());
        currSolver = &solver;
        solver.solve(timeLimitSeconds, verbose);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error parsing DIMACS file `" << fileName << "': "
                  << e.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
