#pragma once

std::discrete_distribution<> rank_proportional; // rank-proportional distribution over variables

//   `r` times flip a random variable of the `b`
//   variables of least loss.
template<class StepGenerator>
void perturbLeastLoss(Solution &S, StepGenerator s, unsigned b) {
    vector<pair<unsigned, int>> cand; // candidates,  in order of increasing value
    cand.reserve(b);
    vector<bool> flipped(S.x.size(), false);
    
    unsigned r = s();
    
    while (r-- > 0) {
        // (1) generate the candidate list
        cand.clear();
        for(unsigned i = 0; i < S.x.size(); i++)
            if(!flipped[i] && (cand.size() < b || S.flipvalue(i) < cand.back().second)) {
                // (1.1) insert i into the candidate list
                auto j = lower_bound(cand.begin(), cand.end(), S.flipvalue(i),
                                     [](const pair<unsigned, int> &a, int b) { return a.second < b; });
                cand.insert(j, make_pair(i, S.flipvalue(i)));
                if(cand.size() > b)
                    cand.pop_back();
            }
        // (2) select a candidate
        unsigned i = cand[randomInt(rng) % cand.size()].first;
        
        // (3) apply the perturbation
        S.flip(i);
        flipped[i] = true;
    }
}

// iterated local search
//   `improve` is a multi-step improvement method (usually some local search returning a local minimum)
//   `perturb` implements some kind of perturbation
template<typename Improvement, typename Perturbation>
unsigned
iterated_localsearch(Solution &S, Improvement improve, Perturbation perturb, chrono::system_clock::time_point start,
                     int target) {
    chrono::system_clock::time_point last_report;
    unsigned steps = 0;
    Solution B(S.I);
    B = S;

    report.newBestKnownValue(chrono::system_clock::now(), B.value);
    
    do {
        // (2) check stopping conditions
        if(S.value <= target)
            break;
        if(chrono::system_clock::now() - start > chrono::duration<int>(timeLimit))
            break;
        
        // (3) next iteration: perturb & improve
        steps++;
        perturb(S);
        improve(S);
        
        // (4) update incumbent
        if(S.value < B.value) {
            B = S;
            B.time = chrono::system_clock::now();

            report.newBestKnownValue(chrono::system_clock::now(), B.value);
        }
    } while (true);
    steps--;
    S = B;
    return steps;
}