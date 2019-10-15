# Genetic Regression

## TODO

- Prune
- Clean (No nested trig functions)
- Deterministic Crowding child elitism
- Protect against divide by 0 expression
- Partial Dataset (slowly increase percentage of training dataset)

* Diversity (How to maintain, increase, similarity mectric)
* Refactor plotter.py for general performance plots (Dot plot, convergence plot)
  - Specify in the run which data want to collect (no data, default to collecting fitness, option to collect all data for dot plot)
* All performance curves in one plot
* Results summary in table format
* Include all GA configs

## Fitness Calculation:

100 / (AbsoluteMeanError + 1) => if no error then fitness is 100
