#pragma once

// BTS: Simple tabu search
struct BTS {
    // some improvement w/ reservoir sampling
    BTS() {}
    
    // bi with reservoir sampling
    template<typename Excluder>
    unsigned bi(const Solution &S, Excluder exclude, int bkv) {
        pair<int, unsigned> best = make_pair(numeric_limits<int>::max(), S.x.size()); // delta, index
        unsigned nbest = 0;
        for(unsigned i = 0; i < S.x.size(); i++) {
            // aspiration
            if(S.flipvalue(i) + S.value < bkv)
                return i;
            if(exclude(i))
                continue;
            if(S.flipvalue(i) < get<0>(best)) {
                best = make_pair(S.flipvalue(i), i);
                nbest = 1;
            } else if(get<1>(best) != S.x.size() && S.flipvalue(i) == get<0>(best)) {
                nbest++;
                if(nbest * random01(rng) <= 1)
                    best = make_pair(S.flipvalue(i), i);
            }
        }
        return get<1>(best);
    }
    
    template<typename Excluder>
    unsigned step(const Solution &S, Excluder exclude, int bkv) {
        return bi(S, exclude, bkv);
    }
};

template<typename Improvement, typename Duration>
unsigned tabusearch(Solution &S, Improvement improve, Duration dgen, chrono::system_clock::time_point start, int target,
                    const unsigned maxstagnate = numeric_limits<unsigned>::max(),
                    const unsigned maxsteps = numeric_limits<unsigned>::max()) {

    unsigned i, steps = 0, notimproved = 0;
    Solution B(S.I);
    B = S;

    report.newBestKnownValue(chrono::system_clock::now(), B.value);

    vector<unsigned> tabu(S.x.size() + 1, 0);
    
    do {
        if(S.value <= target)
            break;
        if(chrono::system_clock::now() - start > chrono::duration<int>(timeLimit))
            break;
        if(notimproved > maxstagnate)
            break;
        if(steps > maxsteps)
            break;
        
        steps++;
        i = improve.step(S, [&steps, &tabu](unsigned j) { return steps < tabu[j]; }, B.value);
        if(!isValid(i, S))
            continue;
        S.flip(i);
        
        unsigned d = dgen();
        if(d == S.x.size())
            tabu[i] = steps + S.I.deg[i] + 1;
        else if(d < S.x.size())
            tabu[i] = steps + d + 1;
        
        if(S.value < B.value) {            
            B = S;
            B.time = chrono::system_clock::now();
            notimproved = 0;

            report.newBestKnownValue(chrono::system_clock::now(), B.value);
        } else
            notimproved++;
    } while (true);
    steps--;
    S = B;
    return steps;
}
