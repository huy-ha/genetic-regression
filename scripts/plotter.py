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


# def average_runs(runs):
    # evals = []
    # counts = []
    # fitnesses = []
    # for run in runs:
    #     runEvals = []
    #     runFitnesses = []
    #     count = 0
    #     for i in run:
    #         dirname = get_dir_name(int(i))
    #         new_eval_list = import_xy("{}/Evaluations.txt".format(dirname))
    #         if len(runEvals) == 0:
    #             runEvals.append(new_eval_list)
    #             count = len(new_eval_list)
    #         else:
    #             if len(runEvals[0]) != len(new_eval_list):
    #                 print("Mismatch length of {} and {}. Taking {}...".format(
    #                     count, len(new_eval_list), min(count, len(new_eval_list))))
    #             count = min(count, len(new_eval_list))
    #             evals.append(new_eval_list)
    #         runMaxCosts.append(
    #             read_file("{}/MaxCosts.txt".format(dirname)))
    #         runMinCosts.append(
    #             read_file("{}/MinCosts.txt".format(dirname)))
    #         runAvgCosts.append(
    #             read_file("{}/AvgCosts.txt".format(dirname)))
    #     runEvals = [runEval[0:count] for runEval in runEvals]
    #     runMaxCosts = [runMaxCost[0:count] for runMaxCost in runMaxCosts]
    #     runMinCosts = [runMinCost[0:count] for runMinCost in runMinCosts]
    #     runAvgCosts = [runAvgCost[0:count] for runAvgCost in runAvgCosts]
    #     n_sqrt = math.sqrt(len(i))
    #     maxCosts.append(np.mean(runMaxCosts, axis=0)[0:count])
    #     maxCostsErr.append(
    #         np.std(runMaxCosts, axis=0) / n_sqrt)
    #     minCosts.append(np.mean(runMinCosts, axis=0)[0:count])
    #     minCostsErr.append(
    #         np.std(runMinCosts, axis=0) / n_sqrt)
    #     avgCosts.append(np.mean(runAvgCosts, axis=0)[0:count])
    #     evals.append(np.mean(runEvals, axis=0)[0:count])
    #     counts.append(count)
    # return counts, evals, maxCosts, minCosts, avgCosts, maxCostsErr, minCostsErr

    # def plot_learning_curve(args):
    #     runs = args.runs
    #     labels = args.labels

    #     logconfig = args.log
    #     title = args.title
    #     colors = ['red', 'lime', 'cyan', 'blue', 'yellow', 'orange']
    #     if title is None:
    #         title = "Symbolic Regression"
    #     counts, evals, maxCost, minCost, avgCost, maxCostErr, minCostErr = average_runs(
    #         runs)
    #     for i in range(len(runs)):
    #         count = counts[i]
    #         if logconfig is not None and logconfig == "y":
    #             evals[i] = [math.log(x) for x in evals[i]]
    #         if config == "max":
    #             plt.errorbar(evals[i][0:count], maxCost[i][0:count], yerr=maxCostErr[i][0:count],
    #                          label=labels[i], color=colors[i], ecolor=colors[i], errorevery=500)
    #         elif config == "min":
    #             plt.errorbar(evals[i][0:count], minCost[i][0:count], yerr=minCostErr[i][0:count],
    #                          label=labels[i], color=colors[i], ecolor=colors[i], errorevery=500)
    #         elif config == "avg":
    #             plt.plot(evals[i], avgCost[i], label=labels[i])
    #     plt.legend()
    #     plt.title(title)
    #     plt.ylabel('Costs')
    #     plt.xlabel('Log of Evaluations')
    #     if args.xlimit is not None:
    #         plt.xlim(int(args.xlimit[0]), int(args.xlimit[1]))
    #     plt.show()

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
        print("Runs supplied:{}".format(args.runs))
        dirname = get_dir_name(args.runs[0][0])

    x, y, filepath, eqn = import_finalbest(dirname + "FinalBest.txt")
    plt.scatter(x, y, label=eqn)
    x, y, title = import_xy('inputs/data.txt')
    plt.scatter(x, y, label='dataset')
    plt.legend()
    plt.title(eqn)
    plt.show()
    x, y, title = import_xy(dirname + "HighestFitness.txt", False)
    plt.plot(x, y, label=eqn)
    plt.legend()
    plt.title(eqn)
    plt.show()
    exit()


# plot_learning_curve(args)
