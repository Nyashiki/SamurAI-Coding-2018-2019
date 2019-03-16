# coding:utf-8

import sys
import json
import pprint
import glob
import matplotlib.pyplot as plt
import numpy as np

WIDTH = 0.4

def measure_winning_rate():
	win_num = [[0, 0], [0, 0], [0, 0]]
	draw_num = [0, 0, 0]
	x = np.array(range(100))
	y = [[[], []], [[], []], [[], []]]
	loses = []

	json_list = [
		[ "./tools/results/{0:02}_0.json".format(i) for i in range(100) ],
		[ "./tools/results/{0:02}_1.json".format(i) for i in range(100) ] ]

	for i in range(2):
		for json_file in json_list[i]:
			with open(json_file) as f:
				s = json.load(f)
				if "finished" not in s:
					print("\"finished\" key does not exist.")
					continue

				for j in range(2):
					y[i][j] += [ s["finished"][j] ]

				if y[i][0][-1] < y[i][1][-1]:
					win_num[i][0] += 1
				elif y[i][1][-1] < y[i][0][-1]:
					win_num[i][1] += 1
				else:
					draw_num[i] += 1

	for i in range(100):
		y[2][0] += [ y[0][0][i] + y[1][1][i] ]
		y[2][1] += [ y[0][1][i] + y[1][0][i] ]

		if y[2][0][-1] < y[2][1][-1]:
			win_num[2][0] += 1
		elif y[2][1][-1] < y[2][0][-1]:
			win_num[2][1] += 1
			loses += [i]
		else:
			draw_num[2] += 1


	# output
	print("------------------------------------------")
	print("win : {0:>3}, lose : {1:>3}, draw : {2:>3}, winning rate : {3:.3f} ".format(win_num[0][0], win_num[0][1], draw_num[0], win_num[0][0] / (win_num[0][0] + win_num[0][1] + draw_num[0])))
	print("win : {0:>3}, lose : {1:>3}, draw : {2:>3}, winning rate : {3:.3f} ".format(win_num[1][1], win_num[1][0], draw_num[1], win_num[1][1] / (win_num[1][0] + win_num[1][1] + draw_num[1])))
	print("win : {0:>3}, lose : {1:>3}, draw : {2:>3}, winning rate : {3:.3f} ".format(win_num[2][0], win_num[2][1], draw_num[2], win_num[2][0] / (win_num[2][0] + win_num[2][1] + draw_num[2])))
	print("loses: ", loses)
	print("------------------------------------------")

	# plot
	plt.figure(figsize=(12, 6))

	# player 1が先手
	plt.subplot(3, 1, 1)
	plt.bar(x - (1 - 2 * WIDTH), y[0][0], color="#ff5a5a", label="player 1", width=WIDTH)
	plt.bar(x + (1 - 2 * WIDTH), y[0][1], color="#5a5aff", label="player 2", width=WIDTH)
	plt.xlabel("course file")
	plt.ylabel("goal time")
	plt.legend()
	plt.grid(True)

	# player 1が後手
	plt.subplot(3, 1, 2)
	plt.bar(x - (1 - 2 * WIDTH), y[1][1], color="#ff5a5a", label="player 1", width=WIDTH)
	plt.bar(x + (1 - 2 * WIDTH), y[1][0], color="#5a5aff", label="player 2", width=WIDTH)
	plt.xlabel("course file")
	plt.ylabel("goal time")
	plt.legend()
	plt.grid(True)

	# 合計
	plt.subplot(3, 1, 3)
	plt.bar(x - (1 - 2 * WIDTH), y[2][0], color="#ff5a5a", label="player 1", width=WIDTH)
	plt.bar(x + (1 - 2 * WIDTH), y[2][1], color="#5a5aff", label="player 2", width=WIDTH)
	plt.xlabel("course file")
	plt.ylabel("goal time")
	plt.legend()
	plt.grid(True)

	plt.tight_layout()

	plt.show()
	plt.savefig("result.png")


if __name__=='__main__':
	measure_winning_rate()
