import matplotlib as mpl
import matplotlib.patches as mpatches
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
    Diagram("6502 unsigned multiply (8x8=16 bit), Memory vs speed", "6502_8x8=16_all", [0, 2100], [0, 2000],
        [5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 32, 34, 35, 37, 38, 39, 40, 43, 44, 47, 57, 65, 66, 68, 70, 71, 72, 73, 75, 76, 77, 78, 79, 80]),

    Diagram("Multiply by repeated addition (8x8=16 bit), Memory vs speed", "6502_8x8=16_repeated_addition", [0, 50], [0, 5200],
        [70, 71, 72, 73]),

    Diagram("6502 unsigned multiply (8x8=16 bit), Memory vs speed", "6502_8x8=16", [0, 2200], [0, 175],
        [5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 32, 34, 35, 37, 38, 39, 40, 43, 44, 47, 57, 65, 66, 68, 70, 71, 72, 73, 75, 76, 77, 78, 79, 80]),

    Diagram("6502 unsigned multiply (8x8=16 bit), Memory vs speed (detail)", "6502_8x8=16_detail", [0,80], [0,180],
        [5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 32, 34, 35, 37, 38, 39, 40, 43, 44, 47, 57, 65, 66, 68, 70, 71, 72, 73, 75, 76, 77, 78, 79, 80]),

    Diagram("6502 unsigned multiply (16x16=32 bit), Memory vs speed", "6502_16x16=32", [0, 2300], [0, 760],
        [1, 2, 3, 4, 15, 31, 33, 36, 41, 42, 45, 46, 48, 49, 50, 51, 52, 53, 54, 55, 56, 58, 59, 60, 61, 62, 63, 64, 67, 69, 74]),

    Diagram("6502 unsigned multiply (16x16=32 bit), Memory vs speed (detail)", "6502_16x16=32_detail", [0, 400], [0, 650],
        [1, 2, 3, 4, 15, 31, 33, 36, 41, 42, 45, 46, 48, 49, 50, 51, 52, 53, 54, 55, 56, 58, 59, 60, 61, 62, 63, 64, 67, 69, 74]),
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
        elif entry.title == "mult51":
            ax.annotate(entry.title, xy=(entry.bytes, entry.ave), xytext=(-40,-3), textcoords="offset points", color=color)
        elif entry.title == "mult57":
            ax.annotate(entry.title, xy=(entry.bytes, entry.ave), xytext=(-40,-3), textcoords="offset points", color=color)
        elif entry.title == "mult60":
            ax.annotate(entry.title, xy=(entry.bytes, entry.ave), xytext=(-40,-3), textcoords="offset points", color=color)
        else:
            ax.annotate(entry.title, xy=(entry.bytes, entry.ave), xytext=(5,-3), textcoords="offset points", color=color)


    #ax.set(xlim=(0, 8), xticks=np.arange(1, 8),
    #       ylim=(0, 8), yticks=np.arange(1, 8))

    plt.title(f"{diagram.title}")
    plt.savefig(f"results/{diagram.filename}.svg", metadata={'Date': None})


def plot_log_surround(title, image_filename, output_filename):
    # plot
    mpl.rcParams['svg.hashsalt'] = 'hello?'
    fig, ax = plt.subplots(figsize=(9, 7))

    img = plt.imread(image_filename)

    if diagram.xlimit:
        plt.xlim([0, 256])
    if diagram.ylimit:
        plt.ylim([0, 256])

    from matplotlib.ticker import ScalarFormatter #,NullFormatter,LinearLocator
    ax.xaxis.get_ticklocs(minor=True)
    ax.yaxis.get_ticklocs(minor=True)
    ax.minorticks_on()
    img = ax.imshow(img, extent = [0, 256, 0, 256])

    ax.set_xticks([64, 128, 192, 256])
    ax.set_yticks([64, 128, 192, 256])

    green_patch = mpatches.Patch(color='#00ff00', label='correct')
    red_patch   = mpatches.Patch(color='#ff0000', label='too small')
    blue_patch  = mpatches.Patch(color='#0000ff', label='too large')
    ax.legend(handles=[red_patch, green_patch, blue_patch], bbox_to_anchor=(1.05, 1), loc='upper left', borderaxespad=0.)

    #locmin = LinearLocator(numticks=100)
    #ax.xaxis.set_minor_locator(locmin)

    plt.gca().set_xlabel("input X") #, fontweight='bold')
    plt.gca().set_ylabel("input Y") #, fontweight='bold')

    plt.title(title, pad=20)
    plt.savefig(f"results/{output_filename}", metadata={'Date': None})

plot_log_surround("Errors in omult7.a (a log based approximation to X*Y/256)", "results/omult7.png", "log7.svg")
plot_log_surround("Errors in omult8.a (a log based approximation to X*Y/256)", "results/omult8.png", "log8.svg")
plot_log_surround("Errors in omult9.a (a log based approximation to X*Y/256) without +.5 bias", "results/omult9.png", "log9.svg")
plot_log_surround("Errors in omult9.a (a log based approximation to X*Y/256) with +.5 bias", "results/omult9_with_0.5_bias.png", "log9a.svg")
plot_log_surround("Errors in omult11.a (a table of squares based approximation to X*Y/256)", "results/omult11.png", "log11.svg")
