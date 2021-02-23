#pragma once

// elite set
// maintains a set of solutions in order of non-decreasing (value,ones)
struct Elite : public std::vector<Solution> {
    typedef std::vector<Solution> Base;
    
    unsigned maxsize;
    std::vector<short> freq; // number of ones per variable in elite set
    
    Elite(const Instance &I, unsigned _n) : maxsize(_n), freq(I.n, 0) {
        assert(maxsize > 0);
        reserve(_n);
    }
    
    // add a solution, returns `true` if added
    bool add(const Solution &S) {
        assert(S.x.size() == freq.size());
        const unsigned n = S.x.size();
        
        // (1) not full or better than worst and different?
        if(size() < maxsize || S.value < back().value) {
            // (1.1) try to insert
            auto point = lower_bound(begin(), end(), S, [](const Solution &S, const Solution &T) {
                return S.value < T.value || (S.value == T.value && S.ones < T.ones);
            });
            // (1.2) check if different
            while (point != end() && point->value == S.value && point->ones == S.ones)
                if(*point == S)
                    return false;
                else
                    point++;

            // (1.3) insert
            insert(point, S);
            for(unsigned i = 0; i < n; i++)
                if(S.x[i])
                    freq[i]++;
            // (1.4) trim size, if necessary
            if(size() > maxsize) {
                for(unsigned i = 0; i < n; i++)
                    if(back().x[i])
                        freq[i]--;
                pop_back();
            }
            assert(size() <= maxsize);
            return true;
        } else
            return false;
    }
    
    // return a random solution
    Solution &getRandom() {
        return (*this)[uniform_int_distribution<>(0, size() - 1)(rng)];
    }
};

inline bool termination(int bestValue, int worstValue, unsigned steps, int target, std::chrono::system_clock::time_point start, std::chrono::system_clock::time_point &last_report) {
    if (bestValue <= target)
        return true;
    if (std::chrono::system_clock::now() - start > std::chrono::duration<int>(timeLimit))
        return true;
    return false;
}