#pragma once

struct Instance : public boost::multi_array<double, 2> {
    unsigned n, m;
    std::vector<unsigned> deg;
    unsigned nnz;
    
    void setSize(unsigned _n) {
        n = _n;
        checkPlausible();
        resize(extents[n][n]);
        deg.resize(n);
        nnz = 0;
    }
    
    void checkPlausible() {
        if(n > 15000) {
            cerr << "Instance has " << n << " variables. Refused." << endl;
            exit(1);
        }
    }

    void readInstance(istream &ins, bool maximize, string format) {
        if(format == "sparse")
            readSparse(ins, maximize);
        if(format == "dense")
            readDense(ins, maximize);
        if(format == "pgen")
            readPalubeckis(ins, maximize, false);
        if(format == "lgen")
            readPalubeckis(ins, maximize, true);
        if(format == "maxcut")
            readMaxcut(ins);
    }
    
    void readSparse(istream &ins, bool maximize = false) {
        ins >> n;
        setSize(n);
        
        ins >> m;
        this->nnz = 2 * m;
        unsigned nnz = m;
        while (nnz > 0) {
            unsigned i, j;
            ins >> i >> j;
            ins >> (*this)[i - 1][j - 1];
            if(maximize)
                (*this)[i - 1][j - 1] = -(*this)[i - 1][j - 1];
            (*this)[j - 1][i - 1] = (*this)[i - 1][j - 1];
            if((*this)[j - 1][i - 1] != 0) {
                deg[i - 1]++;
                deg[j - 1]++;
            }
            nnz--;
        }
    }
    
    void readDense(istream &ins, bool maximize = false) {
        ins >> n;
        ins >> n;
        setSize(n);
        
        for(unsigned i = 0; i < n; i++)
            for(unsigned j = 0; j < n; j++) {
                ins >> (*this)[i][j];
                if(maximize)
                    (*this)[i][j] = -(*this)[i][j];
                (*this)[j][i] = (*this)[i][j];
                if((*this)[j][i] != 0) {
                    deg[i]++;
                    deg[j]++;
                    nnz++;
                }
            }
    }
    
    void readMaxcut(istream &ins) {
        ins >> n;
        setSize(n);
        
        ins >> m;
        this->nnz = 2 * m + n;
        unsigned nedge = m;
        while (nedge > 0) {
            unsigned i, j;
            int wij;
            ins >> i >> j;
            ins >> wij;
            (*this)[i - 1][i - 1] -= wij;
            (*this)[j - 1][j - 1] -= wij;
            (*this)[i - 1][j - 1] += wij;
            (*this)[j - 1][i - 1] += wij;
            nedge--;
            if(wij != 0) {
                deg[i - 1]++;
                deg[j - 1]++;
            }
        }
    }
   
    void genPalubeckis(int density, int lb_linear, int ub_linear, int lb_quadr, int ub_quadr, int clumpiness, double seed,
                  bool negate = true, bool clumpy = false) {
        double coef = 2048;
        coef *= 1024;
        coef *= 1024;
        coef -= 1;
        
        for(unsigned i = 0; i < n; i++) {
            int r = lehmer::random(&seed, coef) * (ub_linear - lb_linear + 1);
            unsigned j = n + 1;
            if(clumpy && (i + 1 + j + 1) % clumpiness == 0)
                r -= ub_linear;
            (*this)[i][i] = r + lb_linear;
            if(negate)
                (*this)[i][i] = -(*this)[i][i];
            if((*this)[i][i] != 0)
                nnz++;
            for(unsigned j = i + 1; j < n; j++) {
                float fl = lehmer::random(&seed, coef) * 100;
                if(fl <= density) {
                    r = lehmer::random(&seed, coef) * (ub_quadr - lb_quadr + 1);
                    if(clumpy && (i + 1 + j + 1) % clumpiness == 0)
                        r += ub_quadr;
                    (*this)[i][j] = r + lb_quadr;
                    if(negate)
                        (*this)[i][j] = -(*this)[i][j];
                    if((*this)[i][j] != 0) {
                        nnz += 2;
                        deg[i]++;
                        deg[j]++;
                    }
                } else
                    (*this)[i][j] = 0;
                (*this)[j][i] = (*this)[i][j];
            }
        }
    }
    
    void readPalubeckis(istream &ins, bool negate = true, bool clumpy = false) {
        ins >> n;
        setSize(n);
        
        int density, lb_linear, ub_linear, lb_quadr, ub_quadr;
        int clumpiness = 0;
        double seed;
        ins >> density >> lb_linear >> ub_linear >> lb_quadr >> ub_quadr >> seed;
        if(clumpy) {
            ins >> clumpiness;
            if(clumpiness == 0)
                clumpiness = 2 * n;
        }
        genPalubeckis(density, lb_linear, ub_linear, lb_quadr, ub_quadr, clumpiness, seed, negate, clumpy);
    }
    
    void writeDense(ostream &out) {
        out << 1 << " " << n << endl;
        
        for(unsigned i = 0; i < n; i++) {
            for(unsigned j = 0; j < n; j++) {
                out << setw(5) << (*this)[i][j];
                if((j + 1) % 15 == 0)
                    out << endl;
            }
            if(n < 14)
                out << endl;
        }
        out << endl;
    }
    
    void writeSparse(ostream &out) {
        unsigned ndiag = 0;
        for(unsigned i = 0; i < n; i++)
            if((*this)[i][i] != 0)
                ndiag++;
        out << n << " " << ndiag + (nnz - ndiag) / 2 << endl;
        for(unsigned i = 0; i < n; i++)
            for(unsigned j = i; j < n; j++)
                if((*this)[i][j] != 0)
                    out << i + 1 << " " << j + 1 << " " << (*this)[i][j] << " "
                        << endl;
    }
    
    // show an instance, positive values are red, negative ones green
    void writePPM(ostream &out, unsigned bf = 1) {
        vector<unsigned> order(n, 0);
        iota(order.begin(), order.end(), 0);
        writePPM(out, order, bf);
    }
    
    // show an instance, positive values are red, negative ones green
    void writePPM(ostream &out, const vector<unsigned> &order, unsigned bf = 1) {
        assert(order.size() == n);
        out << "P3 " << " " << n / bf << " " << n / bf << endl;
        out << "255" << endl;
        
        // (1) go over all blocks, find maximum absolute sum
        int bmax = 0;
        for(unsigned i = 0; i < n / bf; i++) {
            for(unsigned j = 0; j < n / bf; j++) {
                int v = 0;
                for(unsigned bi = 0; bi < bf; bi++)
                    for(unsigned bj = 0; bj < bf; bj++)
                        v += (*this)[order[i * bf + bi]][order[j * bf + bj]];
                if(v > bmax)
                    bmax = v;
                if(-v > bmax)
                    bmax = -v;
            }
        }
        
        // (2) go over them again and create an image
        for(unsigned i = 0; i < n / bf; i++) {
            for(unsigned j = 0; j < n / bf; j++) {
                //int v = (*this)[order[i]][order[j]];
                int v = 0;
                for(unsigned bi = 0; bi < bf; bi++)
                    for(unsigned bj = 0; bj < bf; bj++)
                        v += (*this)[order[i * bf + bi]][order[j * bf + bj]];
                if(v > 0)
                    out << int(v * 255 / bmax) << " 0 0 ";
                else if(v == 0)
                    out << "0 0 0 ";
                else
                    out << "0 " << int(-v * 255 / bmax) << " 0 ";
            }
            out << endl;
        }
    }
};
