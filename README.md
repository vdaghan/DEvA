# DEvA
Daghan's Evolutionary Algorithms

## Preface:

Utilising a user-defined Genotype and Phenotype, DEvA aims to provide basic scaffolding for generic evolutionary algorithm implementations.

DEvA orchestrates the generic evolutionary algorithm flow as follows:
* provides necessary templates for
    * initial population generation (genesis) functions,
    * variations,
    * survivor selection functions,
    * parent selection functions, etc
* provides a library of such standard, generic functions to speed up development,
* calls these functions in necessary order, essentially forming an evolutionary algorithm
* handles parallel execution of these function when applicable,
* records population information to JSON files

Since this piece of code is constantly evolving, API changes almost every update.

## Version targets (aka Roadmap):

These targets imply that throughout minor a version, say v0.X, respective targets will be integrated starting from v0.X.0.0.

## v0.5:
  * Stable JSON based algorithm setup. Aim is to have:
    * a basic program which builds a scaffolding for the problem at hand, and
    * a JSON file which deals with minor tweaks.
  * Multiple runs of the same algorithm with a single execution.
## v0.6:
  * wxWidgets based GUI
  * GUI-based algorithm tweak (via changing JSON parameters online) for rapid experimentation
  * Queuing setups for serial execution.
## v0.7:
## v0.8:
## v0.9:
  * User-programmable functions via a scripting language (e.g. Lua, ChaiScript, AngelScript).
  * Scripting via GUI
## v1.0:
  * 100% test coverage.
  * Each and every example on the book Evolutionary Algorithms by Eiben should be implemented.
  These include following representations, mutations (unary variations) and recombinations (binary variations) mentioned in the book:
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
## Long shots (one can dream, right?)
  * Bibliography feature for when building an algorithm, DEvA would provide references to scientific papers which introduce or use same building blocks.
  * Visual algorithm building interface.
