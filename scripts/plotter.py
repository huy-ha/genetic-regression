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


def read_csv(filepath, d=','):
    results = []
    with open(filepath) as csvfile:
        file = csv.reader(csvfile, delimiter=d)
        colIter, _ = itertools.tee(file)
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
    evals = file[0]
    fitnesses = file[1]
    plt.plot(evals, fitnesses)
    plt.title("Highest Fitness")
    plt.show()


def plot_diversity(dirpath):
    csvfile = read_csv(dirpath + "Diversity.txt")
    evaluations = csvfile[0]
    diversity = csvfile[1]
    plt.plot(evaluations, diversity)
    plt.ylabel('Diversity')
    plt.xlabel('Evaluations')
    plt.title("Diversity")
    plt.show()


def plot_fitness_dotplot(filepath):
    file = read_csv(filepath + "FitnessDotPlot.txt")
    evals = file[0]
    fitnesses = file[1]
    plt.scatter(evals, fitnesses)
    plt.title("Fitness Dot Plot")
    plt.show()


def plot_dir(dir):
    plot_final_best(dir)
    plot_fitness_dotplot(dir)
    plot_diversity(dir)
    plot_highest_fitness(dir)


if __name__ == "__main__":
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
