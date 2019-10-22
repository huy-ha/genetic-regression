import matplotlib.pyplot as plt
import argparse
import numpy as np
import sys
import math
import os
import csv
import itertools


def import_finalbest(filepath):
    f = open(filepath, "r")
    lines = f.readlines()
    lines.reverse()
    eqn = lines.pop()
    print(eqn)
    points = [x.split(' ')
              for x in lines]
    x = []
    y = []
    for point in points:
        x.append(float(point[0]))
        y.append(float(point[1]))
    return x, y, filepath, eqn


def get_dir_name(runNumber):
    runs = [runs for runs in os.listdir(
        'runs') if os.path.isdir(os.path.join('runs', runs))]
    try:
        for run in runs:
            if run.find("run{}".format(runNumber)) is not -1:
                return "runs/{}/".format(run)
    except:
        print("Run {} not found!".format(runNumber))
        exit()


def read_csv(filepath, d=','):
    results = []
    with open(filepath) as csvfile:
        file = csv.reader(csvfile, delimiter=d)
        colIter, _ = itertools.tee(file)
        if not any(colIter):
            return None
        numCols = len(next(colIter))
        for i in range(numCols):
            results.append([])
        for row in file:
            for i in range(numCols):
                results[i].append(float(row[i]))
    return results


def plot_final_best(filePath):
    x, y, filepath, eqn = import_finalbest(filePath + "FinalBest.txt")
    plt.scatter(x, y, label=eqn)
    dataset = read_csv('inputs/data.txt', '\t')
    x = dataset[0]
    y = dataset[1]
    plt.scatter(x, y, label='dataset')
    plt.legend()
    plt.title(eqn)
    plt.show()


def plot_highest_fitness(filepath):
    file = read_csv(filepath + "HighestFitness.txt")
    if file is None:
        print("Can't plot highest fitness")
        return
    evals = file[0]
    fitnesses = file[1]
    plt.plot(evals, fitnesses)
    plt.title("Highest Fitness")
    plt.show()


def plot_diversity(dirpath):
    csvfile = read_csv(dirpath + "Diversity.txt")
    if csvfile is None:
        print("Can't plot diversity")
        return
    evaluations = csvfile[0]
    diversity = csvfile[1]
    plt.plot(evaluations, diversity)
    plt.ylabel('Diversity')
    plt.xlabel('Evaluations')
    plt.title("Diversity")
    plt.show()


def plot_fitness_dotplot(filepath):
    file = read_csv(filepath + "FitnessDotPlot.txt")
    if file is None:
        print("Can't plot dotplot")
        return
    evals = file[0]
    fitnesses = file[1]
    plt.scatter(evals, fitnesses, s=0.05)
    plt.title("Fitness Dot Plot")
    plt.show()


def plot_dir(dir):
    plot_final_best(dir)
    plot_fitness_dotplot(dir)
    plot_diversity(dir)
    plot_highest_fitness(dir)


def avg_learning_curve(runpaths, label):
    piecewises = []
    for runpath in runpaths:
        run = read_csv(runpath)
        piecewises.append((run[0], run[1]))
    minx = None
    for x, y in piecewises:
        if minx == None or minx > x[len(x)-1]:
            minx = int(x[len(x)-1])
    print("Sampling Piecewise from 0 to {}".format(minx))
    x = range(0, int(minx))
    y = []
    for _ in range(len(piecewises)):
        y.append([])

    p_i = 0
    for p_x, p_y in piecewises:
        for i in range(len(p_x) - 1):
            x0 = p_x[i]
            x1 = p_x[i+1]
            y0 = p_y[i]
            y1 = p_y[i+1]
            samples = np.linspace(y0, y1, num=(x1 - x0 + 1))
            y[p_i].extend(samples[:-1])
        p_i += 1

    for y_i in y:
        if len(y_i) < minx:
            minx = len(y_i)
    x = x[:minx]
    z = []
    for y_i in y:
        y_i = np.array(y_i[:minx])
        z.append(y_i[:minx])

    n_squared = math.sqrt(len(z))
    mean = np.mean(z, axis=0)
    std = np.std(z, axis=0) / n_squared
    l = min(len(mean), len(x))
    plt.plot(x[:l], mean[:l], label=label)
    plt.fill_between(x[:l], mean[:l]+std[:l], mean[:l]-std[:l], alpha=0.3)


rs_runs = ["runs/run45-rs/HighestFitness.txt",
           "runs/run46-rs/HighestFitness.txt",
           "runs/run47-rs/HighestFitness.txt",
           "runs/run48-rs/HighestFitness.txt",
           "runs/run49-rs/HighestFitness.txt"]

hc_runs = ["runs/run55-hc/HighestFitness.txt",
           "runs/run56-hc/HighestFitness.txt",
           "runs/run57-hc/HighestFitness.txt",
           "runs/run58-hc/HighestFitness.txt",
           "runs/run59-hc/HighestFitness.txt",
           "runs/run60-hc/HighestFitness.txt"]

ea_diversity_runs = ["runs/run61-ea-night/HighestFitness.txt",
                     "runs/run62-ea-night/HighestFitness.txt",
                     "runs/run63-ea-night/HighestFitness.txt",
                     "runs/run64-ea-night/HighestFitness.txt",
                     "runs/run65-ea-night/HighestFitness.txt",
                     "runs/run66-ea-night/HighestFitness.txt",
                     "runs/run67-ea-night/HighestFitness.txt",
                     "runs/run68-ea-night/HighestFitness.txt",  # potentially messed up
                     "runs/run69-ea-night/HighestFitness.txt",
                     "runs/run70-ea-night/HighestFitness.txt"]

ea_tournament_runs = ["runs/run71-ea-tournament/HighestFitness.txt",
                      "runs/run72-ea-tournament/HighestFitness.txt",
                      "runs/run73-ea-tournament/HighestFitness.txt",
                      "runs/run74-ea-tournament/HighestFitness.txt",
                      "runs/run75-ea-tournament/HighestFitness.txt",
                      "runs/run77-ea-tournament/HighestFitness.txt",
                      "runs/run77-ea-tournament/HighestFitness.txt",
                      "runs/run78-ea-tournament/HighestFitness.txt",  # potentially messed up
                      "runs/run79-ea-tournament/HighestFitness.txt",
                      "runs/run80-ea-tournament/HighestFitness.txt"]

gp_large_pop_runs = ["runs/run81-diverse-large-pop/HighestFitness.txt",
                     "runs/run82-diverse-large-pop/HighestFitness.txt",
                     "runs/run83-diverse-large-pop/HighestFitness.txt",
                     "runs/run84-diverse-large-pop/HighestFitness.txt"]


def plot_lc():
    avg_learning_curve(gp_large_pop_runs,
                       "GP Large Population")
    # avg_learning_curve(ea_diversity_runs,
    #                    "GP Diversity Selector")
    # avg_learning_curve(ea_tournament_runs,
    #                    "GP Tournament Selector")
    # avg_learning_curve(rs_runs, "Random Search")
    # avg_learning_curve(hc_runs, "Hill Climber")
    # plt.xscale("log")
    plt.xlabel("Evaluations")
    plt.ylabel("Fitness")
    plt.legend()
    plt.show()


tmp = [([0, 2, 6, 8, 10, 20], [0, 4, 4, 2, 0, 0, 0]),
       ([0, 4, 9, 10, 20], [0, 3, 3, 0, 0])]

if __name__ == "__main__":
    if(len(sys.argv) == 1):
        plot_lc()
        exit()
    # Parse Arguments
    parser = argparse.ArgumentParser(description='Plot a run')
    parser.add_argument(
        '-t', '--title', nargs='+', help='title of output graph', required=False)
    parser.add_argument('-r', '--runs', nargs='+', action='append',
                        help='runs to average', required=False)
    parser.add_argument('-l', '--labels', nargs='+',
                        help='labels for each group of run', required=False)
    args = parser.parse_args()
    dirname = "runs/tmp/"
    if args.runs != None:
        dirname = get_dir_name(args.runs[0][0])
    plot_dir(dirname)

# plot_learning_curve(args)
