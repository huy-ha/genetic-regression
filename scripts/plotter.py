import matplotlib.pyplot as plt
import argparse
import numpy as np
import sys


def import_dataset(filepath='data.txt'):
    points = [x.split('\t')
              for x in open('../inputs/' + filepath, "r").readlines()]
    print(points[0])
    print(points[1])
    print(points[2])
    x = []
    y = []
    for point in points:
        x.append(float(point[0]))
        y.append(float(point[1]))
    return x, y, filepath


if __name__ == "__main__":
    if len(sys.argv) == 1:
        # default to visualizing the input
        x, y, title = import_dataset()
        plt.scatter(x, y)
        plt.title(title)
        plt.show()

    parser = argparse.ArgumentParser(description='Plot a TSP run')
    parser.add_argument(
        '-t', '--title', nargs='+', help='title of output graph', required=False)
    args = parser.parse_args()
