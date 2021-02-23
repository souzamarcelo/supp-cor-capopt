#pragma once

mt19937 rng;
uniform_real_distribution<double> random01(0.0,1.0);
uniform_int_distribution<> randomInt;

unsigned setupRandom(unsigned seed) {
    if (seed == 0) {
        seed = time(0);
        ifstream f("/dev/urandom");
        if (f.good()) {
            f.read((char *)(&seed), sizeof(unsigned int));
        }
    }
    rng.seed(seed);
    srand48(seed);
    return seed;
}

namespace lehmer {
    // lehmer-like generator, taken from Palubeckis code
    // for reproduction and instance generation only
    double random(double *seed, double coef) {
        double rd, rf;
        rd = 16807 * (*seed);
        rf = floor(rd / coef);
        *seed = rd - rf * coef;
        return (*seed / (coef + 1));
    }

    // dito using the fixed coefficient of Palubeckis
    double random(double *seed) {
        double coef = 2048;
        coef *= 1024;
        coef *= 1024;
        coef -= 1;
        return random(seed,coef);
    }
}