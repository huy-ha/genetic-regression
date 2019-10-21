# Genetic Symbolic Regression

<div style="text-align: center;">
Name: Huy Ha
<br>
UNI: hqh2101
<br>
<br>
Course Name: Evolutionary Computation and Automated Design
<br>
Course Number: MECS E4510
<br>
Instructor: professor Hod Lipson
<br>
<br>
Date Submitted: October 20th 2019
<br>
Grace Hours used: 21 hours
<br>
Grace Hours left: 75 hours
</div>
<div style="page-break-after: always;"></div>

---

## TODO

- Deterministic Crowding child elitism
- Partial Dataset (slowly increase percentage of training dataset)

* Diversity (How to maintain, increase, similarity mectric)
* Refactor plotter.py for general performance plots (Dot plot, convergence plot)
  - Specify in the run which data want to collect (no data, default to collecting fitness, option to collect all data for dot plot)
* All performance curves in one plot
* Results summary in table format
* Include all GA configs

* Result page showing information requested
* Code included (8pt courier single spacing)
* Dot plot for any one of the methods
* Diversity plot for any one of the methods
* Convergence plot for any one of the methods
* Plot showing accuracy vs complexity (of all evaluations
* Description of hill climber
* Description of EA variation operators used
* Description of EA selection methods used
* Analysis of performance. Did it work? Why or why not?
* Learning curve of hill climber
* Learning curve of GP
* Learning curve of some variation of the GP
* learning curves clearly labeled, labeled axes
* learning curves have error bars
* Overall correctness of the result
* Overall efficiency of the algorithm (accuracy versus number of evaluations)
* Automatically draw tree representing best solution
* Show video where every frame is data point and best function found so far (include \* link to video online in the PDF, along with a frame from the video)

* Accuracy vs complexity of all methods

* Make sure Data submitted is correct

# 1. Results Page

## Results summary table

# 2. Problem Set Up

## 2.1 Representation of Genotype

For my genotype, I used an Expression Tree, where each Node has a lambda function that takes in a float and outputs a float, and a list of its child expressions. The lambda function would make use of its child expression's functions somehow.

The simplest case is a Constant Expression. If the constant is k, then its lambda expression would just be `(float x) => k`, where this means given the input x it will output k. As you can see, the lambda does not depend on the variable k at all (which is why it is a constant). This formulation allows for all the flexibility one would need to represent any expression. For example, if I want to represent `sin(x)`, then I would have at the root node a Sin Expression Node, which has a lambda function of `(float x) -> sin(child[0]->lambda(x))`, and the Expression Node would have exactly one subexpression. If its child is a Constant Expression Node of value k, then `(float x) -> sin(child[0]->lambda(x))` would evaluate to `(float x) -> sin(k)`, which makes sense.

Similarly, Plus, Minus, Divide, and Multiply operators are defined to be Expressions with exactly two child expressions, and their lambdas are `(float x) => op(child[0]->lambda(x),child[1]->lambda(x))`, where op is the corresponding `+`,`-`,`/` and `*`.

If I want to evaluate an expression at x, I can do an in order tree traversal of the lambdas, which would give me the lambda that represents the entire expression tree.

# 2. Random Search

## Description

Random Search is a special case of a Evolutionary Algorithm with the only constraint that the Reproducer operator would output a random expression for any parents. Note that this puts no constraint on the Selection operator used (it can be anything, but ideally it would take the least amount of compute power possible, as the parent's fitness has no correlation with the child's fitness). Therefore, I just implemented my random search as using the `RandomReproducer`, which outputs a random expression for any two parents.

## Plots

### Learning Plot, Dot Plot, Diversity Plot (Respectively)

<div style="clear:both;">
	<img src="assets\rs\lc.png"width="30%" height="auto" />
	<img src="assets\rs\dotplot.png"width="30%" height="auto" />
	<img src="assets\rs\diversity.png"width="30%" height="auto" />
</div>

As you would expect, the dot plot shows an almost uniform distribution of fitnesses, which suggests great diversity. This is confirmed by the diversity plot. The learning curve is abrupt, as expected. What you would not expect is for a random search to do so well. Most of the final best equations were able to capture the shapes of the dataset quite well.

<div style="clear:both;">
	<img src="assets\rs\eqn1.png"width="49%" height="auto" />
	<img src="assets\rs\eqn2.png"width="49%" height="auto" />
</div>

<div style="clear:both;">
	<img src="assets\rs\eqn3.png"width="49%" height="auto" />
	<img src="assets\rs\eqn4.png"width="49%" height="auto" />
</div>

# 3. Hill Climber

## Description

Similar to Random Search, Hill Climbers are just a special case of Evoluationary Algorithms, with population 1, simulated annealing with initial temperature set to 0, and a 100% mutation rate. In this case, it doesn't matter if the solver is generational or continuous, because only one individual is reproducing, and the individual is reproducing asexually.

## Plots

### Learning Plot and Diversity Plot (Respectively)

<div style="clear:both;">
    <img src="assets\hc\lc.png"width="49%" height="auto" />
	<img src="assets\hc\diversity.png"width="49%" height="auto" />
</div>

As you would expect, the dot plot shows an almost uniform distribution of fitnesses, which suggests great diversity. This is confirmed by the diversity plot. The learning curve is abrupt, as expected. What you would not expect is for a random search to do so well. Most of the final best equations were able to capture the shapes of the dataset quite well.

<div style="clear:both;">
	<img src="assets\hc\eqn1.png"width="49%" height="auto" />
	<img src="assets\hc\eqn2.png"width="49%" height="auto" />
</div>

<div style="clear:both;">
	<img src="assets\hc\eqn3.png"width="49%" height="auto" />
	<img src="assets\hc\eqn4.png"width="49%" height="auto" />
</div>

# 4. Evolutionary Algorithm

## Description

### Convergence plot

# Analysis

## Fitness Metric

The fitness of an individual in my program is given by `100 / (AbsoluteMeanError + 1)`. This metric is nice because it's almost like a progress bar to see how far it is away from have 0 error. When the error approaches infinity, this fitness approaches zero, and when the error approaches 0, the fitness approaches 100. This is also useful for the convergence plot.

## Simpler problems for testing

I have a segment of code in `Config.cpp` that would allow me to construct my own dataset rather than use input dataset, which means I'm able to create datasets of `sin(x)`,`cos(x)` and `x`. Usually, simple equations like this arise directly as a result of initialization, so it wasn't too helpful to study how my program would evolve these equations.

TODO try more complicated simple problems for testing
