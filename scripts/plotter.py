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


def avg_learning_curve(runpaths, label, showplot=False, errorevery=100):
    evals = []
    fitnesses = []
    minLen = 10000000000
    for runpath in runpaths:
        run = read_csv(runpath)
        evals.append(run[0])
        fitnesses.append(run[1])
        if len(run[0]) < minLen:
            minLen = len(run[0])
    for i in range(len(evals)):
        evals[i] = evals[i][0:minLen]
        fitnesses[i] = fitnesses[i][0:minLen]
    n_sqrt = math.sqrt(len(runpaths))
    evals = np.mean(evals, axis=0)
    std = np.std(fitnesses, axis=0) / n_sqrt
    fitnesses = np.mean(fitnesses, axis=0)
    plt.errorbar(evals, fitnesses, yerr=std,
                 errorevery=errorevery,
                 label=label)
    if showplot:
        plt.legend()
        plt.show()


def plot_all_runs(runpaths, label, showplot=False):
    for runpath in runpaths:
        run = read_csv(runpath)
        plt.plot(run[0], run[1], label=label)
    if showplot:
        plt.legend()
        plt.show()


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
                     "runs/run68-ea-night/HighestFitness.txt",
                     "runs/run69-ea-night/HighestFitness.txt",
                     "runs/run70-ea-night/HighestFitness.txt"]


def plot_lc():
    avg_learning_curve(ea_runs, "Evolutionary Algorithm", errorevery=100)
    avg_learning_curve(rs_runs, "Random Search", errorevery=10000)
    avg_learning_curve(hc_runs, "Hill Climber", errorevery=10)
    # plot_all_runs(hc_runs, "Hill Climber")
    plt.xscale("log")
    plt.xlabel("Evaluations")
    plt.ylabel("Fitness")
    plt.legend()
    plt.show()


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
