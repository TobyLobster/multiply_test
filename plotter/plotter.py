import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import json

class Diagram:
    def __init__(self, title, filename, xlimit, ylimit, results_list):
        self.title = title
        self.filename = filename
        self.xlimit = xlimit
        self.ylimit = ylimit
        self.results_list = results_list

diagrams = [
    Diagram("6502 unsigned multiply (8x8=16 bit), Memory vs speed", "6502_8x8=16", [0, 2200], [0, 300],
        [5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 32, 34, 35, 37, 38, 39, 40, 43, 44]),

    Diagram("6502 unsigned multiply (8x8=16 bit), Memory vs speed (detail)", "6502_8x8=16_detail", [0,80], [0,180],
        [5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 32, 34, 35, 37, 38, 39, 40, 43, 44]),

    Diagram("6502 unsigned multiply (16x16=32 bit), Memory vs speed", "6502_16x16=32", [0, 2300], [0, 1000],
        [1, 2, 3, 4, 15, 31, 33, 36, 41, 42, 45]),
        ]

class Result:
    def __init__(self, title, bytes, ave, min, max):
        self.title = title
        self.bytes = bytes
        self.ave = ave
        self.min = min
        self.max = max
        self.good = True

    def __str__(self) -> str:
        return f"Title: {self.title}, Bytes: {self.bytes}, Average Cycles: {self.ave:.2f}, Min Cycles: {self.min}, Max Cycles: {self.max}, Good: {self.good}";
    def __repr__(self) -> str:
        return f"'{self.title}: {self.bytes}, {self.ave:.2f}, {self.min}, {self.max}, {self.good}'";

result_list = []

# Work out which results we need to make the diagrams
all_results = set()
for diagram in diagrams:
    all_results |= set(diagram.results_list)

# Read all results
for i in all_results:
    # Opening JSON file
    f = open(f'results/results_mult{i}.json')

    # returns JSON object as a dictionary
    data = json.load(f)

    # Iterating through the json
    results = data['results']

    result_list.append(Result(results['title'],
                              int(results['bytes']),
                              float(results['AverageCycles']),
                              int(results['MinCycles']),
                              int(results['MaxCycles'])))

    # Closing file
    f.close()

log_plots = False

for diagram in diagrams:
    # Memory vs speed diagrams
    #
    collected_list = []
    for entry in diagram.results_list:
        collected_list.append(result_list[entry-1])
    #print(collected_list)

    # check which solutions are 'good' and 'bad'
    # A 'bad' solution is one that takes more memory and cycles than another solution
    index = 0
    for i in collected_list:
        for j in collected_list:
            if i == j:
                continue

            # If j beats i in both memory and speed, then i is bad
            if ((i.bytes > j.bytes) and (i.ave >= j.ave)) or ((i.bytes == j.bytes) and (i.ave > j.ave)):
                i.good = False
        index += 1

    good_results = list(filter(lambda e: e.good, collected_list))
    bad_results = list(filter(lambda e: not e.good, collected_list))

    #print (bad_results)

    good_x = [result.bytes for result in good_results]
    good_y = [result.ave for result in good_results]
    good_err = [[result.ave-result.min for result in good_results],
                [result.max-result.ave for result in good_results]]

    bad_x = [result.bytes for result in bad_results]
    bad_y = [result.ave for result in bad_results]
    bad_err = [[result.ave-result.min for result in bad_results],
               [result.max-result.ave for result in bad_results]]

    # plot
    mpl.rcParams['svg.hashsalt'] = 'hello!'
    fig, ax = plt.subplots(figsize=(10.5, 7))
    if log_plots:
        plt.xscale("log",subs=[1.5,2,3,4,5,6,8])
        plt.yscale("log",subs=[1.5,2,3,4,5,6,8])
        plt.xlim([25,1100])
    else:
        if diagram.xlimit:
            plt.xlim(diagram.xlimit)
        if diagram.ylimit:
            plt.ylim(diagram.ylimit)

    from matplotlib.ticker import ScalarFormatter #,NullFormatter,LinearLocator
    if log_plots:
        ax.yaxis.set_major_formatter(ScalarFormatter())
        ax.yaxis.set_minor_formatter(ScalarFormatter())
        ax.xaxis.set_major_formatter(ScalarFormatter())
        ax.xaxis.set_minor_formatter(ScalarFormatter())
    ax.xaxis.get_ticklocs(minor=True)
    ax.yaxis.get_ticklocs(minor=True)
    ax.minorticks_on()

    #locmin = LinearLocator(numticks=100)
    #ax.xaxis.set_minor_locator(locmin)

    plt.gca().set_xlabel("memory (bytes)") #, fontweight='bold')
    plt.gca().set_ylabel("average time taken (cycles)") #, fontweight='bold')

    ax.scatter(bad_x, bad_y, color="lightgrey")
    ax.errorbar(bad_x, bad_y, xerr=None, capsize=3,yerr=bad_err, fmt="none", color="lightgrey")
    plt.scatter(good_x, good_y, color="orange")
    plt.errorbar(good_x, good_y, yerr=good_err, capsize=3, fmt="none", color="orange")

    # Annotate each point with text
    for entry in collected_list:
        if entry.good:
            color = "black"
        else:
            color = "lightgrey"

        if entry.title == "mult2":
            ax.annotate(entry.title, xy=(entry.bytes, entry.ave), xytext=(-33,-3), textcoords="offset points", color=color)
        elif entry.title == "mult20":
            ax.annotate(entry.title, xy=(entry.bytes, entry.ave), xytext=(-40,-3), textcoords="offset points", color=color)
        elif entry.title == "mult8":
            ax.annotate(entry.title, xy=(entry.bytes, entry.ave), xytext=(5,4), textcoords="offset points", color=color)
        elif entry.title == "mult9":
            ax.annotate(entry.title, xy=(entry.bytes, entry.ave), xytext=(5,1), textcoords="offset points", color=color)
        elif entry.title == "mult11":
            ax.annotate(entry.title, xy=(entry.bytes, entry.ave), xytext=(5,-6), textcoords="offset points", color=color)
        elif entry.title == "mult12":
            ax.annotate(entry.title, xy=(entry.bytes, entry.ave), xytext=(5, -3), textcoords="offset points", color=color)
        elif entry.title == "mult17":
            ax.annotate(entry.title, xy=(entry.bytes, entry.ave), xytext=(5,-8), textcoords="offset points", color=color)
        elif entry.title == "mult18":
            ax.annotate(entry.title, xy=(entry.bytes, entry.ave), xytext=(5,-3), textcoords="offset points", color=color)
        elif entry.title == "mult21":
            ax.annotate(entry.title, xy=(entry.bytes, entry.ave), xytext=(5,-3), textcoords="offset points", color=color)
        elif entry.title == "mult24":
            ax.annotate(entry.title, xy=(entry.bytes, entry.ave), xytext=(-40,-3), textcoords="offset points", color=color)
        elif entry.title == "mult26":
            ax.annotate(entry.title, xy=(entry.bytes, entry.ave), xytext=(5,-6), textcoords="offset points", color=color)
        elif entry.title == "mult30":
            ax.annotate(entry.title, xy=(entry.bytes, entry.ave), xytext=(5,-1), textcoords="offset points", color=color)
        elif entry.title == "mult34":
            ax.annotate(entry.title, xy=(entry.bytes, entry.ave), xytext=(5,1), textcoords="offset points", color=color)
        elif entry.title == "mult37":
            ax.annotate(entry.title, xy=(entry.bytes, entry.ave), xytext=(5,-1), textcoords="offset points", color=color)
        elif entry.title == "mult40":
            ax.annotate(entry.title, xy=(entry.bytes, entry.ave), xytext=(-40,-3), textcoords="offset points", color=color)
        else:
            ax.annotate(entry.title, xy=(entry.bytes, entry.ave), xytext=(5,-3), textcoords="offset points", color=color)


    #ax.set(xlim=(0, 8), xticks=np.arange(1, 8),
    #       ylim=(0, 8), yticks=np.arange(1, 8))

    plt.title(f"{diagram.title}")
    plt.savefig(f"results/{diagram.filename}.svg", metadata={'Date': None})
