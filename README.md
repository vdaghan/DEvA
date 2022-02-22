# DEvA
 Daghan's Evolutionary Algorithms

 VERSIONING:

     A.B.C.D:
     | | | |-> Bugfixes
     | | |-> Internal API changes
     | |-> External API changes
     |-> Functionality changes

 TODO:
 * Range-ify container traversal
 * Support for different genotypes and/or different phenotypes
 * Make operators into a functor so that we can learn what they need as binput, and what they give as an output
    * But for maximum customisation, operators should be any callable
 * Overload with concepts DEvA::Common::iteratorForNthListElement()
 * Data passing strategies: pointer, copy, move, reference
 * Problem descriptions: Optimise mathematical function
 * Standard genotype representations, their standard mutations (unary variations) and their standard recombinations (binary variations):
```
    +----------------+--------------------------+---------------------------------+
    | REPRESENTATION | MUTATIONS                | RECOMBINATIONS                  |
    +----------------+--------------------------+---------------------------------+
    |                |                          | One-point crossover             |
    |                |                          +---------------------------------+
    |    Binary      |        Bit flip          | N-point crossover               |
    |                |                          +---------------------------------+
    |                |                          | Uniform crossover               |
    +----------------+--------------------------+---------------------------------+
    |                | Random resetting         |                                 |
    |    Integer     +--------------------------|              N/A                |
    |                | Creep mutation           |                                 |
    +----------------+--------------------------+---------------------------------+
    |                | Uniform mutation         | Arithmetic recombination        |
    |                +--------------------------|---------------------------------+
    |                |                          | Simple recombination            |
    | Floating point | Nonuniform mutation with +---------------------------------+
    |                |                          | Single arithmetic Recombination |
    |                |   a fixed distribution   +---------------------------------+
    |                |                          | Whole arithmetic recombination  |
    +----------------+--------------------------+---------------------------------+
    |                | Swap mutation            | Partially mapped crossover      |
    |                +--------------------------+---------------------------------+
    |                | Insert mutation          | Edge crossover                  |
    |  Permutation   +--------------------------+---------------------------------+
    |                | Scramble mutation        | Order crossover                 |
    |                +--------------------------+---------------------------------+
    |                | Inversion mutation       | Cycle crossover                 |
    +----------------+--------------------------+---------------------------------+
```

# Unsolved problems:
* A generic way to assign multiple variation operators throughout the generation.