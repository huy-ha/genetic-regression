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
- Code included (8pt courier single spacing)
- Convergence plot for any one of the methods
- Plot showing accuracy vs complexity (of all evaluations
- Analysis of performance. Did it work? Why or why not?
- Learning curve of some variation of the GP
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
| Base GP             | Population size 50, Diversity Selector  |    TODO |       |
| Tournament GP       | Population size 50, Tournament Selector |    TODO | 99.90 |
| Large Population GP | Population size 500, Diversity Selector |    TODO |  TODO |

TODO insert summary graph here

<div style="page-break-after: always;"></div>

# 2. Problem Set Up

## 2.1. Representation of Genotype

For my genotype, I used an Expression Tree, where each Node has a lambda function that takes in a float and outputs a float, and a list of its child expressions. The lambda function would make use of its child expression's functions somehow.

The simplest case is a Constant Expression. If the constant is k, then its lambda expression would just be `(float x) => k`, where this means given the input x it will output k. As you can see, the lambda does not depend on the variable k at all (which is why it is a constant). This formulation allows for all the flexibility one would need to represent any expression. For example, if I want to represent `sin(x)`, then I would have at the root node a Sin Expression Node, which has a lambda function of `(float x) -> sin(child[0]->lambda(x))`, and the Expression Node would have exactly one subexpression. If its child is a Constant Expression Node of value k, then `(float x) -> sin(child[0]->lambda(x))` would evaluate to `(float x) -> sin(k)`, which makes sense.

Similarly, Plus, Minus, Divide, and Multiply operators are defined to be Expressions with exactly two child expressions, and their lambdas are `(float x) => op(child[0]->lambda(x),child[1]->lambda(x))`, where op is the corresponding `+`,`-`,`/` and `*`.

If I want to evaluate an expression at x, I can do an in order tree traversal of the lambdas, which would give me the lambda that represents the entire expression tree.

## 2.2. Diversity

My first attempt at diversity was using Phenotypic diversity, which is a sample of the Absolute Mean Error between two expressions, but this did not prove to be very effective, not only because it took way too long to calculate, but also because it didn't motivate genotypic diversity which is useful for crossing over.

I ended up using a simple genotypic diversity metric which is the sum of the differences of all types of operators between two expressions. So `sin(x)` and `cos(x)` has a distance of 2 from each other, because the first expression has 1 sin node while the second has none (1 - 0), and the second has 1 cos node while the first has none (0 - 1), and both has a variable (1 - 1). Therefore, their distance is |1-0| + |0-1| + |1-1| = 2. This is more inline with the diversity I wanted to encourage, and I got better results with this.

With more time, I would like to experiment with structural diversity, which somehow takes into account the topology of the expression tree, because I think that might also be very useful for crossing over.

## 2.3. Probabilistic Deterministic Crowding

I invented a Reproduction Operator I call Probabilistic Deterministic Crowding. After it reproduces two parents through crossover and mutates the offspring a bit, half of the time, it puts both parents and the offspring back into the population, the other half, my reproduction operator is like deterministic crowding and the child replaces the most similar parents.

When both parents and the offspring is put back into the population, the individual with the worst fitness is removed from the population. I hypothesized that the method would able to maintain diversity (child replaces most similar parent), but still ensures a minimum level of improvement in the population's fitness.

Unfortunately, this method did not perform as well as I expected. It was slow because some the weakest individuals were unlikely to be removed from the population, yet the diversity still decayed too quickly with time. In other words, it inherited the worst properties of both methods.

I ended up just keeping the parent in the population and adding the offspring to the population. Then, when the population size is too large, I remove individuals from the bottom.

## 2.4. Solver

- `GenerationalSolver`: - The algorithm for this solver can be described as: 1. Use a selector to choose M parents 2. Create N (population size) offsprings from the parents 3. Replace the current population with the offspring 4. Repeat - This was what I used for my first assignment, but this did not give me very good results. I think this is because it removes too many good solutions from population in step 3. Further, it does not follow the motto of GP: "incremental progress".
- `ContinuousSolver`: - The algorithm for this solver can be described as: 1. Use a selector to choose 2 parents 2. Create 1 offspring from 2 parents 3. Decide which of the three individuals to keep and put back to the population. 4. Repeat - Not only does this solver resolves the two disadvantages of the previous solver, it also allows for the flexibility of a protocol for choosing how to handle choosing who to keep from the parents and the offspring. - Indeed, as expected, the results from this solver is better than the `GenerationalSolver`, so you will only see me discuss results from the `ContinuousSolver`.

## 2.5. Selectors

- `Tournament Selector`: This is the standard tournament selector in literature where N individuals are selected at random from the population, and the individual with the highest fitness gets to reproduce. This happens twice to give the two parents that would reproduce.
- `DiversitySelector`: This selector uses the `TournamentSelector` to "suggest" two individuals with high fitness, and this selector moves forward with the suggested parents only if the parents has a distance value larger than some value K. If this condition is not meet, K is decayed, then the process is repeated. This operator is named this way because it trys to select parents that are as different from each other as stochastically possible.
- `NichingSelector`: This selector is the opposite of `DiversitySelector`, moving forward with `TournamentSelector`'s suggestion only if the parents' distance value is less than a certain K. If not, K is incremented, then the process is repeated. This Operator aims to create various niches within the population.

## 2.6. Mutators

- `ConstantMultiplier`: This mutator came out of a conversation I had with Joni the TA about my program's performance. He noted that my expressions did not have a global constant multiplier term. This mutator aims to resolve this, because when it is called to mutate an expression, it multiplies the expression by a constant and keeps the resulting expression if it has a higher fitness.
- `TrigMultiplier`: This mutator is similar to `ConstantMutator`, but it multiplies a trig function rather than a constant.
- `ConstantMutator`: finds a constant and mutates its k value
- `SubexpressionMutator`: Finds a subexpression tree, and swaps it out with a random expression tree. This is really only useful when the subexpression tree is not too large compared to the size of the full tree.
- `TruncateMutator`: Replaces a subexpression tree with a constant of random value

# 3. Random Search

## 3.1 Description

Random Search is a special case of a Genetic Programming with the only constraint that the Reproducer operator would output a random expression for any parents. Note that this puts no constraint on the Selection operator used (it can be anything, but ideally it would take the least amount of compute power possible, as the parent's fitness has no correlation with the child's fitness). Therefore, I just implemented my random search as using the `RandomReproducer`, which outputs a random expression for any two parents.

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

# 5. Genetic Programming (GP)

## 5.1. Description

I experimented with various configurations of GP:

- `Base GP`: This GP uses the `DiversitySelector` (described above) with the continuous to select two parents (which in turn uses `TournamentSelector`). The expression tree of the parents are crossed over at a random point, which gives the base of the child. Then the child has some probability of mutating through all of the mutators described above. The child is then inserted into the population, then the individual with the lowest fitness is removed, and the cycle repeats. A population size of 50 is used.
- `Tournament GP`: similar to `Base GP`, but uses vanilla `TournamentSelector`.
- `Large Population GP`: as the name implies, this configurations tries to see if a large population of 500 would outperform a smaller population. I expect that it has a slower convergence, but would reach a better solution than smaller populations.

## 5.2. Base GP

## 5.3. Tournament GP

Below are the hyperparameters I used for the Base GP

```
Input data.txt
Solver Continuous
Reproducer CrossoverMutator
Selector Diversity
PopulationCount 50
TournamentPlayersCount 4
MutationRetries 8
MaxDepth 10
MinThreads 5
MaxThreads 100
Init_T 0.5
T_decay 0.999
```

## Hyperparameters

Below are the hyperparameters I used for the Tournament GP

```
Input data.txt
Solver Continuous
Reproducer CrossoverMutator
Selector Tournament
PopulationCount 50
TournamentPlayersCount 4
MutationRetries 8
MaxDepth 10
MinThreads 5
MaxThreads 100
Init_T 0.5
T_decay 0.999
```

<div>
    <img src="assets\ea-tournament\eqn1.png"/>
</div>
<div>
    <img src="assets\ea-tournament\eqn1-lr.png" height=auto width="32%"/>
	<img src="assets\ea-tournament\eqn1-dotplot.png" height=auto width="32%"/>
	<img src="assets\ea-tournament\eqn1-diversity.png" height=auto width="32%"/>
</div>

Above is the graph plot (top), learning curve (bottom left), dot plot (bottom center), and diversity plot (bottom right). The Diversity plot clearly illustrates the effectiveness of the diversity, while the fitness dot plot shows that my population was able to find a diverse set of solutions that are all quite good. The best one, given by the equation:

    0.998*((sin((((x+0.000)-6.287)-6.287)) - 1.268) + sin((0.863 + x)))*sin((x+0.000)) * -1.382

which can be simplified to:

    1.379 * (sin(x - 12.574 ) - 1.268 + sin(0.863 + x)) * sin(x)

or, because to simplify the sin function is periodic:

    1.379 * (sin(x) - 1.268 + sin(PI/4 + x)) * sin(x)

This equation has the fitness of `99.9023`, and was found through `Tournament GP`.

Below are some graphs of other equations found by `Tournament GP`:

<div style="clear:both;">
	<img src="assets\ea-tournament\eqn74.png"width="49%" height="auto" />
	<img src="assets\ea-tournament\eqn77.png"width="49%" height="auto" />
</div>

<div style="clear:both;">
	<img src="assets\ea-tournament\eqn79.png"width="100%" height="auto" />
</div>

## 5.4. Large Population GP

Below are the hyperparameters I used for the Large Population GP

```
Input data.txt
Solver Continuous
Reproducer CrossoverMutator
Selector Diversity
PopulationCount 500
TournamentPlayersCount 4
MutationRetries 8
MaxDepth 10
MinThreads 5
MaxThreads 100
Init_T 0.5
T_decay 0.999
```

# 6. Analysis of Methods

## 6.1. Fitness Metric

The fitness of an individual in my program is given by `100 / (AbsoluteMeanError + 1)`. This metric is nice because it's almost like a progress bar to see how far it is away from have 0 error. When the error approaches infinity, this fitness approaches zero, and when the error approaches 0, the fitness approaches 100. This is also useful for the convergence plot.

## 6.2. Simpler problems for testing

I have a segment of code in `Config.cpp` that would allow me to construct my own dataset rather than use input dataset, which means I'm able to create datasets of `sin(x)`,`cos(x)` and `x`. Usually, simple equations like this arise directly as a result of initialization, so it wasn't too helpful to study how my program would evolve these equations.

## 6.3. Conclusion

Overall, my `Tournament GP` was able to find the highest fitness of `99.90`. I have read in literature that Tournament Selector is usually a good go to selector for most problems because the selection pressure can easily be changed by changing the tournament size, and it has nice convergence as well as exploration properties. My studies have confirmed this.

I expected HillClimber to do worse than GP simply because the space of all mathematical expressions that can be represented by an expression tree of max depth 10 is very large, so it loses out on not only the presence of other individuals in the population to crossover with, but also of exploring slightly worse solutions to get to better ones later on.
