# -*- coding:utf-8 -*-
import sys, os
import glob
import json
import subprocess
import concurrent.futures
import functools
import time
import multiprocessing

from winning_rate import measure_winning_rate

'''
実行例 :
    python3 ./tools/statistics.py ./judge/player/greedy ./judge/player/greedy ./judge/samples/courses

pip install futures が必要
'''

def run_process(command):
    subprocess.getoutput(command)

    # 本来は同期を取らないと表示が崩れる可能性があるが、深刻な問題ではないので、現時点では同期を取らない
    sys.stdout.write(command.split()[-1] + "...done.\n")
    sys.stdout.flush()

if __name__=='__main__':
    # コースファイルの一覧を取得
    course_list = glob.glob(sys.argv[3].rstrip('/') + "/*")
    course_list.sort()

    args = ['./judge/official/official', 'course file name', sys.argv[1], sys.argv[1], sys.argv[2], sys.argv[2]]
    args2 = ['./judge/official/official', 'course file name', sys.argv[2], sys.argv[2], sys.argv[1], sys.argv[1]]

    # プロセス数を制限
    pool = concurrent.futures.ProcessPoolExecutor(max_workers=multiprocessing.cpu_count())

    for course_file in course_list:
        args[1] = course_file
        args2[1] = course_file

        # player 1 先手
        file_name = "./tools/results/" + os.path.basename(course_file).replace(".crs", "") + '_0.json'
        command = " ".join(args) + " > " + file_name
        pool.submit(run_process, command)

        # player 1 後手
        file_name = "./tools/results/" + os.path.basename(course_file).replace(".crs", "") + '_1.json'
        command = " ".join(args2) + " > " + file_name
        pool.submit(run_process, command)

    # wait processes
    pool.shutdown()

    # get result
    measure_winning_rate()
