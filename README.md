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

- All performance curves in one plot
- Include all GA configs

- Result page showing information requested
- Code included (8pt courier single spacing)
- Convergence plot for any one of the methods
- Plot showing accuracy vs complexity (of all evaluations
- Description of EA variation operators used
- Description of EA selection methods used
- Analysis of performance. Did it work? Why or why not?
- Learning curve of GP
- Learning curve of some variation of the GP
- learning curves have error bars
- Overall correctness of the result
- Overall efficiency of the algorithm (accuracy versus number of evaluations)
- Automatically draw tree representing best solution
- Show video where every frame is data point and best function found so far (include \* link to video online in the PDF, along with a frame from the video)
- Accuracy vs complexity of all methods

# 1. Results Page

## Results summary table

| Method              | Configuration                           | Average |  Best |
| :------------------ | :-------------------------------------- | ------: | ----: |
| Random Search       | N/a                                     |   66.40 | 69.80 |
| Hill Climber        | N/a                                     |   68.23 | 82.65 |
| EA                  | Population size 50, Diversity Selector  |    TODO |  TODO |
| Niching EA          | Population size 50, Niching Selector    |    TODO |  TODO |
| Large Population EA | Population size 500, Diversity Selector |    TODO |  TODO |

# 2. Problem Set Up

## 2.1 Representation of Genotype

For my genotype, I used an Expression Tree, where each Node has a lambda function that takes in a float and outputs a float, and a list of its child expressions. The lambda function would make use of its child expression's functions somehow.

The simplest case is a Constant Expression. If the constant is k, then its lambda expression would just be `(float x) => k`, where this means given the input x it will output k. As you can see, the lambda does not depend on the variable k at all (which is why it is a constant). This formulation allows for all the flexibility one would need to represent any expression. For example, if I want to represent `sin(x)`, then I would have at the root node a Sin Expression Node, which has a lambda function of `(float x) -> sin(child[0]->lambda(x))`, and the Expression Node would have exactly one subexpression. If its child is a Constant Expression Node of value k, then `(float x) -> sin(child[0]->lambda(x))` would evaluate to `(float x) -> sin(k)`, which makes sense.

Similarly, Plus, Minus, Divide, and Multiply operators are defined to be Expressions with exactly two child expressions, and their lambdas are `(float x) => op(child[0]->lambda(x),child[1]->lambda(x))`, where op is the corresponding `+`,`-`,`/` and `*`.

If I want to evaluate an expression at x, I can do an in order tree traversal of the lambdas, which would give me the lambda that represents the entire expression tree.

# 3. Random Search

## 3.1 Description

Random Search is a special case of a Evolutionary Algorithm with the only constraint that the Reproducer operator would output a random expression for any parents. Note that this puts no constraint on the Selection operator used (it can be anything, but ideally it would take the least amount of compute power possible, as the parent's fitness has no correlation with the child's fitness). Therefore, I just implemented my random search as using the `RandomReproducer`, which outputs a random expression for any two parents.

## 3.2 Plots

### Learning Plot (Top), Dot Plot (Bottom Left), Diversity Plot (Bottom Right)

<div style="clear:both;">
	<img src="assets\rs\lc.png"width="100%" height="auto" />
	<img src="assets\rs\dotplot.png"width="49%" height="auto" />
	<img src="assets\rs\diversity.png"width="49%" height="auto" />
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

# 4. Hill Climber

## 4.1. Description

Similar to Random Search, Hill Climbers (HC) are just a special case of Evoluationary Algorithms, with population 1, simulated annealing with initial temperature set to 0, and a 100% mutation rate. In this case, it doesn't matter if the solver is generational or continuous, because only one individual is reproducing, and the individual is reproducing asexually.

What is important when talking about hill climbers is the AsexualReproducer. Unlike other reproducers, this reproducer takes one parent, mutates it until either the offspring has a fitness than the parent, or, by simulated annealing, it is let through. Of course, in HC, temperature is 0 so the child is only let through if it has a higher fitness than the parent.

## 4.2. Plots

## Hyperparameters

Below are the hyperparameters I used for Hill Climber

```
Input data.txt
PopulationCount 1
GenerationCount 100000
MaxDepth 15
MinThreads 5
MaxThreads 100
Reproducer Asexual
Solver Continuous
MutationRetries 50
Init_T 0
T_decay 1
```

### Learning Plot

<div>
    <img src="assets\hc\lc.png"/>
</div>
<div>
    <img src="assets\hc\eqn0.png"/>
</div>

The equation above has a fitness of `82.646`, which is the highest fitness I got for HC. Overall, it does seem like HC performed better than Random Search.

<div style="clear:both;">
	<img src="assets\hc\eqn1.png"width="49%" height="auto" />
	<img src="assets\hc\eqn2.png"width="49%" height="auto" />
</div>

<div style="clear:both;">
	<img src="assets\hc\eqn3.png"width="49%" height="auto" />
	<img src="assets\hc\eqn4.png"width="49%" height="auto" />
</div>

# 5. Evolutionary Algorithm

## 5.1. Description

## 5.1.1. Diversity

My first attempt at diversity was using Phenotypic diversity, which is a sample of the Absolute Mean Error between two expressions, but this did not prove to be very effective, not only because it took way too long to calculate, but also because it didn't motivate genotypic diversity which is useful for crossing over.

I ended up using a simple genotypic diversity metric which is the sum of the differences of all types of operators between two expressions. So `sin(x)` and `cos(x)` has a distance of 2 from each other, because the first expression has 1 sin node while the second has none (1 - 0), and the second has 1 cos node while the first has none (0 - 1), and both has a variable (1 - 1). Therefore, their distance is |1-0| + |0-1| + |1-1| = 2. This is more inline with the diversity I wanted to encourage, and I got better results with this.

With more time, I would like to experiment with structural diversity, which somehow takes into account the topology of the expression tree, because I think that might also be very useful for crossing over.

## 5.1.2. Probabilistic Deterministic Crowding

I invented a Reproduction Operator I call Probabilistic Deterministic Crowding. After it reproduces two parents through crossover and mutates the offspring a bit, half of the time, it puts both parents and the offspring back into the population, the other half, my reproduction operator is like deterministic crowding and the child replaces the most similar parents.

When both parents and the offspring is put back into the population, the individual with the worst fitness is removed from the population. This method is able to maintain diversity (child replaces most similar parent), but still ensures a minimum level of improvement in the population's fitness.

## 5.1.3. Solver

- `GenerationalSolver`: - The algorithm for this solver can be described as: 1. Use a selector to choose M parents 2. Create N (population size) offsprings from the parents 3. Replace the current population with the offspring 4. Repeat - This was what I used for my first assignment, but this did not give me very good results. I think this is because it removes too many good solutions from population in step 3. Further, it does not follow the motto of EA: "incremental progress".
- `ContinuousSolver`: - The algorithm for this solver can be described as: 1. Use a selector to choose 2 parents 2. Create 1 offspring from 2 parents 3. Decide which of the three individuals to keep and put back to the population. 4. Repeat - Not only does this solver resolves the two disadvantages of the previous solver, it also allows for the flexibility of a protocol for choosing how to handle choosing who to keep from the parents and the offspring. - Indeed, as expected, the results from this solver is better than the `GenerationalSolver`, so you will only see me discuss results from the `ContinuousSolver`.

## 5.1.4. Selectors

- `Tournament Selector`: This is the standard tournament selector in literature where N individuals are selected at random from the population, and the individual with the highest fitness gets to reproduce. This happens twice to give the two parents that would reproduce.
- `DiversitySelector`: This selector uses the `TournamentSelector` to "suggest" two individuals with high fitness, and this selector moves forward with the suggested parents only if the parents has a distance value larger than some value K. If this condition is not meet, K is decayed, then the process is repeated. This operator is named this way because it trys to select parents that are as different from each other as stochastically possible.
- `NichingSelector`: This selector is the opposite of `DiversitySelector`, moving forward with `TournamentSelector`'s suggestion only if the parents' distance value is less than a certain K. If not, K is incremented, then the process is repeated. This Operator aims to create various niches within the population.

## 5.1.5. Mutators

- `ConstantMultiplier`: This mutator came out of a conversation I had with Joni the TA about my program's performance. He noted that my expressions did not have a global constant multiplier term. This mutator aims to resolve this, because when it is called to mutate an expression, it multiplies the expression by a constant and keeps the resulting expression if it has a higher fitness.
- `TrigMultiplier`: This mutator is similar to `ConstantMutator`, but it multiplies a trig function rather than a constant.
- `ConstantMutator`: finds a constant and mutates its k value
- `SubexpressionMutator`: Finds a subexpression tree, and swaps it out with a random expression tree. This is really only useful when the subexpression tree is not too large compared to the size of the full tree.
- `TruncateMutator`: Replaces a subexpression tree with a constant of random value

# 6. Design Choices

## 6.1. Fitness Metric

The fitness of an individual in my program is given by `100 / (AbsoluteMeanError + 1)`. This metric is nice because it's almost like a progress bar to see how far it is away from have 0 error. When the error approaches infinity, this fitness approaches zero, and when the error approaches 0, the fitness approaches 100. This is also useful for the convergence plot.

## 6.2. Simpler problems for testing

I have a segment of code in `Config.cpp` that would allow me to construct my own dataset rather than use input dataset, which means I'm able to create datasets of `sin(x)`,`cos(x)` and `x`. Usually, simple equations like this arise directly as a result of initialization, so it wasn't too helpful to study how my program would evolve these equations.
