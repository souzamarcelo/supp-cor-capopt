Supplementary material to the paper:

**Capping Methods for the Automatic Configuration of Optimization Algorithms**<br>
[Marcelo de Souza][marcelo], [Marcus Ritt][marcus] and [Manuel López-Ibáñez][manuel]<br>
Computers & Operations Research, 2021<br>
[link to the paper]

<br>

### Abstract
> Automatic configuration techniques are widely and successfully used to find good parameter settings for parameterized algorithms. Configuration is costly, because it is necessary to evaluate many configurations on different instances. For decision problems, when the objective is to minimize the running time of the algorithm, many configurators implement capping methods to discard poor configurations early. Such methods are not directly applicable to optimization problems, when the objective is to optimize the cost of the best solution found, given a predefined running time limit. We propose new capping methods for the automatic configuration of optimization algorithms. They use the previous executions to determine a performance envelope, which is used to evaluate new executions and cap those that do not satisfy the envelope conditions. We integrate the capping methods into the irace configurator and evaluate them on different optimization scenarios. Our results show that the proposed methods can save from about 5% to 80% of the configuration effort, while finding configurations of the same quality. Based on the computational analysis, we identify two conservative methods, that save an average of about 20% of the configuration effort and never found significantly worse configurations than those obtained without capping on all tested problems. We also provide evidence that capping can help to better use the available budget in scenarios with a configuration time limit.

**Keywords:** automatic algorithm configuration; capping methods; optimization algorithms; parameter tuning

***

### The *capopt* package

All capping methods are implemented in *capopt*. It provides a Python target runner script that integrates with irace. Please, check all details of how to use it and the latest version of the source code in the [*capopt* official repository](https://github.com/souzamarcelo/capopt). You can also download the source code of the exact version of *capopt* used in our experiments [here](files/src/capopt.zip).

***

### Configuration scenarios

We tested the capping methods in the following scenarios:

+ **ACOTSP:** ant colony optimization algorithms for the symmetric traveling salesperson problem [1].
+ **HEACOL:** hybrid evolutionary algorithm for graph coloring [2].
+ **TSBPP:** tabu search for the bin packing problem [3, 4].
+ **HHBQP:** hybrid heuristic for unconstrained binary quadratic programming [5].
+ **LKH:** a heuristic algorithm for solving the symmetric traveling salesperson problem [6, 7, 8].
+ **SCIP:** an exact solver for mixed integer programs for solving the combinatorial auction winner determination problem [9, 10].

Each configuration scenario contains:
+ source code of the target algorithm (or executable, in the case of SCIP; LKH and SCIP has also a wrapper script for communication);
+ parameters file;
+ training and test instances;
+ best known values file;
+ parameters of *capopt* file;
+ irace scenario file;
+ LKH has also a forbidden configuration file.

Below you find all files for each configuration scenario. We also provide the *scenario.txt* and *parameters-capopt.txt* files for the experiments using a total configuration time budget (the rest of files are the same as configuring with a total executions budget):

| Scenario | Budget in executions | Budget in time |
|-|-|-|
| ACOTSP | [scenario-acotsp](files/budget-executions/scenario-acotsp) | [scenario.txt](files/budget-time/acotsp/scenario.txt) \| [parameters-capopt.txt](files/budget-time/acotsp/parameters-capopt.txt) |
| HEACOL | [scenario-heacol](files/budget-executions/scenario-heacol) | [scenario.txt](files/budget-time/heacol/scenario.txt) \| [parameters-capopt.txt](files/budget-time/heacol/parameters-capopt.txt) |
| TSBPP | [scenario-tsbpp](files/budget-executions/scenario-tsbpp) | [scenario.txt](files/budget-time/tsbpp/scenario.txt) \| [parameters-capopt.txt](files/budget-time/tsbpp/parameters-capopt.txt) |
| HHBQP | [scenario-hhbqp](files/budget-executions/scenario-hhbqp) | [scenario.txt](files/budget-time/hhbqp/scenario.txt) \| [parameters-capopt.txt](files/budget-time/hhbqp/parameters-capopt.txt) |
| LKH | [scenario-lkh](files/budget-executions/scenario-lkh) | [scenario.txt](files/budget-time/lkh/scenario.txt) \| [parameters-capopt.txt](files/budget-time/lkh/parameters-capopt.txt) |
| SCIP | [scenario-scip](files/budget-executions/scenario-scip) | [scenario.txt](files/budget-time/scip/scenario.txt) \| [parameters-capopt.txt](files/budget-time/scip/parameters-capopt.txt) |


**Important:** for each scenario, file *parameters-capopt.txt* defines the use of the default capping method *PEMW.1*. In order to evaluate other (or disable) capping methods, this file must be changed according to the instructions given [here](https://github.com/souzamarcelo/capopt/blob/master/examples/parameters-capopt.txt).

***

### Source code of the target algorithms

We modified the source code of ACOTSP, HACOL and TSBPP, to be able to monitor the progress of the search during its execution. Below you can find the link and a copy of the original implementation of all target algorithms, as well as the modified version (if applicable) used in our experiments and the patch file with the changes made.

| Scenario | Link (original) | Copy (original) | Copy (modified) | Patch file |
|-|-|-|-|-|
| ACOTSP | [link](http://www.aco-metaheuristic.org/aco-code/public-software.html) | [src-acotsp-original](files/src/src-acotsp-original.zip) | [src-acotsp-modified](files/src/src-acotsp-modified.zip) | [src-acotsp.patch](files/src/src-acotsp.patch) |
| HEACOL | [link](http://rhydlewis.eu/resources/gCol.zip) | [src-heacol-original](files/src/src-heacol-original.zip) | [src-heacol-modified](files/src/src-heacol-modified.zip) | [src-heacol.patch](files/src/src-heacol.patch) |
| TSBPP | [link](http://or.dei.unibo.it/research_pages/ORcodes/TSpack.html) | [src-tsbpp-original](files/src/src-tsbpp-original.zip) | [src-tsbpp-modified](files/src/src-tsbpp-modified.zip) | [src-tsbpp.patch](files/src/src-tsbpp.patch) |
| HHBQP | [link](https://github.com/souzamarcelo/hhbqp) | [src-hhbqp-original](files/src/src-hhbqp-original.zip) | – | – |
| LKH | [link](http://webhotel4.ruc.dk/~keld/research/LKH) | [src-lkh-original](files/src/src-lkh-original.zip) | – | – |
| SCIP | [link](https://scipopt.org/index.php#download) | [scip-original](files/src/scip-original.zip) | – | – |


***

### Experimental data

Here you find the raw data of our experiments.

+ Evaluating all capping methods using a total executions budget:
  + [train-data-evaluation.csv](files/experimental-data/train-data-evaluation.csv)
  + [test-data-evaluation.csv](files/experimental-data/test-data-evaluation.csv)
+ Analyzing the recommended methods in detail (9th replication on ACOTSP):
  + [noncapped-executions.csv](files/experimental-data/noncapped-executions.csv)
  + [capped-executions.csv](files/experimental-data/capped-executions.csv)
+ Evaluating all capping methods using a total configuration time budget:
  + [train-data-timeasbudget.csv](files/experimental-data/train-data-timeasbudget.csv)
  + [test-data-timeasbudget.csv](files/experimental-data/test-data-timeasbudget.csv)


***


### References

[1] Dorigo, M., Stützle, T., 2004. Ant Colony Optimization. MIT Press, Cambridge, MA.

[2] Galinier, P., Hao, J.K., 1999. Hybrid evolutionary algorithms for graph coloring. Journal of Combinatorial Optimization 3, 379 – 397.

[3] Lodi, A., Martello, S., Vigo, D., 1999. Heuristic and metaheuristic approaches for a class of two-dimensional bin packing problems. INFORMS Journal on Computing 11, 345 – 357.

[4] Lodi, A., Martello, S., Vigo, D., 2004a. TSpack: a unified tabu search code for multi-dimensional bin packing problems. Annals of Operations Research 131, 203 – 213.

[5] De Souza, M., Ritt, M., 2018. Automatic grammar-based design of heuristic algorithms for unconstrained binary quadratic programming, in: Evolutionary Computation in Combinatorial Optimization, Springer International Publishing. pp. 67 – 84.

[6] Helsgaun, K., 2000. An effective implementation of the Lin-Kernighan traveling salesman heuristic. European Journal of Operational Research 126, 106 – 130.

[7] Helsgaun, K., 2009. General k-opt submoves for the Lin-Kernighan TSP heuristic. Mathematical Programming Computation 1, 119 – 163.

[8] Helsgaun, K., 2018a. Efficient recombination in the Lin-Kernighan-Helsgaun traveling salesman heuristic, pp. 95 – 107.

[9] Achterberg, T., 2009. SCIP: Solving constraint integer programs. Mathematical Programming Computation 1, 1 – 41.

[10] De Vries, S., Vohra, R.V., 2003. Combinatorial auctions: A survey. INFORMS Journal on Computing 15, 284 – 309.


[marcelo]: https://souzamarcelo.github.io
[marcus]: https://www.inf.ufrgs.br/~mrpritt
[manuel]: http://lopez-ibanez.eu
[capopt]: https://github.com/souzamarcelo/capopt
[python]: https://www.python.org
[r]: https://www.r-project.org
[irace]: http://iridia.ulb.ac.be/irace
[rpy2]: https://rpy2.github.io
[psutil]: https://psutil.readthedocs.io/en/latest
[example-acotsp]: https://github.com/capopt/capopt/tree/master/examples/acotsp
[iracedoc]: https://cran.r-project.org/web/packages/irace/vignettes/irace-package.pdf

[capopt-download]: files/capopt.zip
[acotsp]: http://www.aco-metaheuristic.org/aco-code/public-software.html
[heacol]: http://rhydlewis.eu/resources/gCol.zip
[tsbpp]: http://or.dei.unibo.it/research_pages/ORcodes/TSpack.html
[hhbqp]: https://github.com/souzamarcelo/hhbqp
[src-acotsp]: files/acotsp.zip
[src-heacol]: files/heacol.zip
[src-tsbpp]: files/tsbpp.zip
[src-hhbqp]: files/hhbqp.zip
[par-acotsp]: files/parameters-acotsp.txt
[par-heacol]: files/parameters-heacol.txt
[par-tsbpp]: files/parameters-tsbpp.txt
[par-hhbqp]: files/parameters-hhbqp.txt
[bkv-acotsp]: files/bkv-acotsp.txt
[bkv-heacol]: files/bkv-heacol.txt
[bkv-tsbpp]: files/bkv-tsbpp.txt
[bkv-hhbqp]: files/bkv-hhbqp.txt
[train-acotsp]: files/train-acotsp.zip
[train-heacol]: files/train-heacol.zip
[train-tsbpp]: files/train-tsbpp.zip
[train-hhbqp]: files/train-hhbqp.zip
[test-acotsp]: files/test-acotsp.zip
[test-heacol]: files/test-heacol.zip
[test-tsbpp]: files/test-tsbpp.zip
[test-hhbqp]: files/test-hhbqp.zip
[parc-acotsp]: files/parameters-capopt-acotsp.txt
[parc-heacol]: files/parameters-capopt-heacol.txt
[parc-tsbpp]: files/parameters-capopt-tsbpp.txt
[parc-hhbqp]: files/parameters-capopt-hhbqp.txt
[scenario-acotsp]: files/scenario-acotsp.txt
[scenario-heacol]: files/scenario-heacol.txt
[scenario-tsbpp]: files/scenario-tsbpp.txt
[scenario-hhbqp]: files/scenario-hhbqp.txt
[train-data-evaluation]: files/train-data-evaluation.csv
[test-data-evaluation]: files/test-data-evaluation.csv
[train-data-timeasbudget]: files/train-data-timeasbudget.csv
[test-data-timeasbudget]: files/test-data-timeasbudget.csv
[noncapped]: files/noncapped-executions.csv
[capped]: files/capped-executions.csv