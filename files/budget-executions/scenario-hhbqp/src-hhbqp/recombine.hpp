#pragma once

namespace recombine {
    unsigned bn(const Solution &S, const vector<unsigned> &NC) {
        pair<int, unsigned> best = make_pair(numeric_limits<int>::max(), S.x.size()); // delta, index
        unsigned nbest = 0;
        for(unsigned i = 0; i < NC.size(); i++)
            if(S.flipvalue(NC[i]) < get<0>(best)) {
                best = make_pair(S.flipvalue(NC[i]), i);
                nbest = 1;
            } else if(S.flipvalue(NC[i]) == get<0>(best)) {
                assert(get<1>(best) != S.x.size());
                nbest++;
                if((nbest) * random01(rng) <= 1.0)
                    best = make_pair(S.flipvalue(NC[i]), i);
            }
        return get<1>(best);
    }
    
    // path-relinking from `S` to `T`, return the best solution in `S` with minimum
    // distance gamma*H(S,T) for Hamming distance H between `S` and `T`
    template<typename Improver>
    void recombine(const Instance &I, Solution &S, const Solution &T, Improver improve, double gamma) {
        const unsigned n = S.x.size();
        assert(n == T.x.size());
        assert(0 <= gamma && gamma < 0.5);
        
        Solution B(S.I);
        B.value = numeric_limits<int>::max();
        
        // (1) generate the difference set
        vector<unsigned> NC;
        for(unsigned i = 0; i < n; i++)
            if(S.x[i] != T.x[i])
                NC.push_back(i);
        
        unsigned dmin = gamma * NC.size();
        unsigned dmax = NC.size() - dmin;
        //unsigned dmax = NC.size()-1;
        
        // (2) repeatedly let "improve" select one of the different bits
        unsigned d = 0;
        while (NC.size() > 0) {
            // (2.1) select a variable, flip it, remove it
            unsigned i = improve(S, NC);
            S.flip(NC[i]);
            d++;
            
            swap(NC[i], NC.back());
            NC.pop_back();
            
            if(S.value < B.value && dmin <= d && d <= dmax) {
                B = S;
                B.time = chrono::system_clock::now();
            }
        }
        
        S = B;
    }
    
    template<typename Improver, typename Recombiner>
    void recombiner(const Instance &I, Solution &S, unsigned b, Recombiner recombine, Improver improve,
                    chrono::system_clock::time_point start, int target) {
        chrono::system_clock::time_point last_report = chrono::system_clock::now();
        unsigned steps = 0;
        vector<Solution> current;
        Solution B(S.I);
        B = S;
        
        Elite e(I, b);
        
        while (true) {
            // (1) create b random solutions, improve them
            e.clear();
            for(unsigned i = 0; i < b; i++) {
                Solution S(I);
                improve(S);

                if(i == 0 || S.value < e[0].value)
                    report.newBestKnownValue(chrono::system_clock::now(), S.value);
                

                e.add(S);
                if(termination(e[0].value, e.back().value, steps, target, start, last_report))
                    goto done;
            }
            
            // (2) extract current solutions
            current = e;
            for(Solution &s : e)
                s.novel = false;
            
            unsigned num_novel = b;
            
            while (num_novel > 0) {
                num_novel = 0;
                
                // (3) recombine until stagnation
                for(unsigned i = 0; i < current.size(); i++)
                    for(unsigned j = i + 1; j < current.size(); j++) {
                        if(!current[i].novel && !current[j].novel)
                            continue;
                        steps++;
                        
                        // forward
                        Solution S = current[i];
                        recombine(I, S, current[j]);
                        improve(S);
                        S.novel = true;

                        if(S.value < e[0].value)
                            report.newBestKnownValue(chrono::system_clock::now(), S.value);

                        if(e.add(S))
                            num_novel++;
                        
                        if(termination(e[0].value, e.back().value, steps, target, start, last_report))
                            goto done;
                        
                        // backward
                        S = current[j];
                        recombine(I, S, current[i]);
                        improve(S);
                        S.novel = true;

                        if(S.value < e[0].value)
                            report.newBestKnownValue(chrono::system_clock::now(), S.value);

                        if(e.add(S))
                            num_novel++;
                        
                        if(termination(e[0].value, e.back().value, steps, target, start, last_report))
                            goto done;
                    }
                
                // (4) recreate pairset
                current = e;
                for(Solution &s : e)
                    s.novel = false;
            } // not converged (i.e. num_novel>0)
            if(e[0].value < B.value)
                B = e[0];
        } // while "restart"
        done:
        if(e[0].value < B.value)
            B = e[0];
        S = B;
    }
} // namespace recombine
