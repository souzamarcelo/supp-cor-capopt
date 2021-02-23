#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_set>
#include <chrono>
#include <iomanip>
#include <random>
#include <boost/multi_array.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

namespace po = boost::program_options;
using namespace boost;
using namespace std;

unsigned timeLimit;
string format;
chrono::system_clock::time_point startTime;

#include "rng.hpp"
#include "instance.hpp"
#include "instancedata.hpp"
#include "solution.hpp"

#include "report.hpp"
Report report;

#include "elite.hpp"
#include "localsearch.hpp"
#include "tabusearch.hpp"
#include "recombine.hpp"

int pT;
int pTv;
int pTp;
int pTd;
int pTc;
int pS;
int pSv;
int pSm;
int pI;
int pIv;
int pD1;
int pD2;
int pG;
int pB;
int pE;
double pGamma;

unsigned getTabuTenure(Instance& I) {
    double davg = double(accumulate(I.deg.begin(), I.deg.end(), 0)) / I.n;
    unsigned tenure = 0;
    switch (pT) {
        case 1:
            tenure = max(1, pTv);
            break;
            
        case 2:
            tenure = max(1.0, (pTp * I.n) / 100.0);
            break;
            
        case 3:
            tenure = I.n / pTd;
            break;
        
        case 4:
            tenure = I.n / pTd + uniform_int_distribution<>(1, pTc)(rng);
            break;
            
        case 5:
            tenure = max(1u, unsigned(davg + 1));
            break;
            
        default:
            cout << "Invalid tabu tenure strategy!" << endl;
            exit(1);
    }
    
    return tenure;
}

unsigned getMaxStagnate(Instance& I) {
    unsigned maxStagnate = 0;
    
    switch (pS) {
        case 1:
            maxStagnate = pSv;
            break;
        
        case 2:
            maxStagnate = pSm * I.n;
            break;
            
        default:
            cout << "Invalid max. stagnation strategy!" << endl;
            exit(1);
    }
    
    return maxStagnate;
}

unsigned getMaxIterations(Instance& I) {
    unsigned maxIterations = 0;
    
    switch (pI) {
        case 1:
            maxIterations = pSv;
            break;
            
        case 2:
            maxIterations = I.n>5000?15000:(I.n>3000?12000:10000);
            break;
            
        default:
            cout << "Invalid max. iterations strategy!" << endl;
            exit(1);
    }
    
    return maxIterations;
}

void run(Solution& S, Instance& I, double target, chrono::system_clock::time_point startTime) {
    unsigned tabuTenure = getTabuTenure(I);
    unsigned maxStagnate = getMaxStagnate(I);
    unsigned maxIterations = getMaxIterations(I);
    
    BTS bt;
    
    recombine::recombiner(I, S, pE,
                          [&](const Instance& I, Solution& S, const Solution& T) {return recombine::recombine(I, S, T,recombine::bn, pGamma);},
                          [&](Solution& S) {return iterated_localsearch(S,[&](Solution& S) {
                                                              return tabusearch(S, bt, [&]() { return tabuTenure; }, startTime, target, maxStagnate, maxIterations);},
                          [&](Solution& S) { return perturbLeastLoss(S, [&]() { return I.n / pG; }, pB);}, startTime, target);},
                          startTime, target);
}

int main(int argc, char *argv[]) {
    po::options_description desc("General options");
    desc.add_options()
            ("help", "show help")
            ("ins", po::value<string>(), "instance")
            ("seed", po::value<unsigned>()->default_value(0), "random seed")
            ("timelimit", po::value<int>(), "time limit")
            ("timescale", po::value<double>()->default_value(1), "time scale for experiments")
            ("pT", po::value<int>()->default_value(4), "Strategy for tabu tenure")
            ("pTv", po::value<int>()->default_value(10), "Constant for tabu tenure")
            ("pTp", po::value<int>()->default_value(10), "Percentage of instance size for tabu tenure")
            ("pTd", po::value<int>()->default_value(310), "Tabu tenure is n/pTd")
            ("pTc", po::value<int>()->default_value(25), "Tabu tenure is n/pTd + c in [0, pTc]")
            ("pS", po::value<int>()->default_value(2), "Strategy for max. stagnation")
            ("pSv", po::value<int>()->default_value(1000), "Constant for max. stagnation")
            ("pSm", po::value<int>()->default_value(22), "Max. stagnation is n*pSm")
            ("pI", po::value<int>()->default_value(2), "Strategy for max. iterations")
            ("pIv", po::value<int>()->default_value(10000), "Constant for max. iterations")
            ("pD1", po::value<int>()->default_value(10), "Min. perturbation size")
            ("pD2", po::value<int>()->default_value(10), "Max. perturbation size is n/pD2")
            ("pG", po::value<int>()->default_value(10), "Perturbation size is n/pG")
            ("pB", po::value<int>()->default_value(8), "Candidate variables for perturbation")
            ("pE", po::value<int>()->default_value(16), "Elite set size for the recombination step")
            ("pGamma", po::value<double>()->default_value(0.24), "Distance scale for the recombination step")
            ;
    
    po::positional_options_description pod;
    pod.add("ins", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(pod).run(), vm);
    po::notify(vm);

    cout << fixed;

    if (vm.count("help") || !vm.count("ins")) {
        cout << desc << endl;
        return 0;
    }

    Instance I;
    double target;
    double scale;
    string instanceName;
    bool maximize;
    unsigned seed;

    scale = vm["timescale"].as<double>();
    seed = vm["seed"].as<unsigned>();
    setupRandom(seed);

    instanceName = vm["ins"].as<string>();
    ifstream ins(instanceName);
    if (!ins.good()) {
        cout << "Can't open instance file " << instanceName << endl;
        return 1;
    }

    target = getBestKnownValue(instanceName);
    timeLimit = getTimeLimit(instanceName, scale);
    maximize = getMaximize(instanceName);
    format = getFormat(instanceName);
    target = numeric_limits<int>::min();
    
    if(vm.count("timelimit")) {
        timeLimit = vm["timelimit"].as<int>();
    }

    I.readInstance(ins, maximize, format);
    Solution S(I);
    startTime = chrono::system_clock::now();

    pT = vm["pT"].as<int>();
    pTv = vm["pTv"].as<int>();
    pTp = vm["pTp"].as<int>();
    pTd = vm["pTd"].as<int>();
    pTc = vm["pTc"].as<int>();
    pS = vm["pS"].as<int>();
    pSv = vm["pSv"].as<int>();
    pSm = vm["pSm"].as<int>();
    pI = vm["pI"].as<int>();
    pIv = vm["pIv"].as<int>();
    pD1 = vm["pD1"].as<int>();
    pD2 = vm["pD2"].as<int>();
    pG = vm["pG"].as<int>();
    pB = vm["pB"].as<int>();
    pE = vm["pE"].as<int>();
    pGamma = vm["pGamma"].as<double>();
    
    run(S, I, target, startTime);
}
