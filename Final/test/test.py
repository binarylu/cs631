#!/usr/bin/python
#coding:utf-8
#Author:

import sys
import os
import threading
import time

threads = int(sys.argv[1])
rounds = int(sys.argv[2])
testcmd = "./client 155.246.89.102 1234 1>./log 2>&1"

lock = threading.Lock()
success_counts = 0
fail_counts = 0
total_time = 0.0


def test_routine():
    global success_counts
    global fail_counts
    global total_time
    for i in range(0, rounds):
        begin = time.time()
        ret = os.system(testcmd)
        end = time.time()

        lock.acquire()
        if ret == 0:
            success_counts += 1
            total_time += end - begin
        else:
            fail_counts += 1
        lock.release()

join_list = []
for i in range(0, threads):
    t = threading.Thread(target=test_routine)
    t.start()
    join_list.append(t)

for t in join_list:
    t.join()

print "success = %d, fail = %d, averge = %fms" % (
        success_counts, fail_counts, total_time / success_counts * 1000)
