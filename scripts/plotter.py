import matplotlib.pyplot as plt
import argparse
import numpy as np
import sys
import math
import os


def import_xy(filepath, tab=True):
    points = []
    if tab:
        points = [x.split('\t')
                  for x in open(filepath, "r").readlines()]
    else:
        points = [x.split(' ')
                  for x in open(filepath, "r").readlines()]
    x = []
    y = []
    for point in points:
        x.append(float(point[0]))
        y.append(float(point[1]))
    return x, y, filepath


def import_dotplot(filepath):
    evals = []
    fitnesses = []
    lines = open(filepath, "r").readlines()
    for line in lines:
        splitted = line.split(' ')
        if '\n' in splitted:
            splitted.remove('\n')
        evals.append(float(splitted.pop(0)))
        fitnesses.append([float(x) for x in splitted])
    x = []
    y = []
    for i in range(len(evals)):
        for j in range(len(fitnesses[i])):
            x.append(evals[i])
            y.append(fitnesses[i][j])
    return x, y


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


def plot_final_best(filePath):
    x, y, filepath, eqn = import_finalbest(filePath)
    plt.scatter(x, y, label=eqn)
    x, y, title = import_xy('inputs/data.txt')
    plt.scatter(x, y, label='dataset')
    plt.legend()
    plt.title(eqn)
    plt.show()


def plot_highest_fitness(filepath):
    x, y, title = import_xy(filepath, False)
    plt.plot(x, y)
    plt.title("Highest Fitness")
    plt.show()


def plot_fitness_dotplot(filepath):
    x, y = import_dotplot(filepath)
    plt.scatter(x, y)
    plt.title("Fitness Dot Plot")
    plt.show()


def plot_dir(dir):
    # plot_final_best(dirname + "FinalBest.txt")
    plot_fitness_dotplot(dirname + "FitnessDotPlot.txt")
    # plot_highest_fitness(dirname + "HighestFitness.txt")


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
