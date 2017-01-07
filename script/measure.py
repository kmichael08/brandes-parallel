#!/usr/bin/env python3
import os

for i in range(1, 9):
    command = 'time ../build/brandes ' + str(i) + ' ../wiki-vote-sort.txt wiki_res.out 2>> raport.out'
    os.system(command)