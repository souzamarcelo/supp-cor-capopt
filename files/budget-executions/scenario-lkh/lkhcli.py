#!/usr/bin/python3
import sys
import os
import random
import string

seed = 1
instance = ''
for i in range(len(sys.argv)):
    if sys.argv[i] == '--seed': seed = sys.argv[i+1]
    if sys.argv[i] == '--instance': instance = sys.argv[i+1]

content = ''
content += 'PROBLEM_FILE = ' + instance + '\n'
content += 'RUNS = 1\n'
content += 'TIME_LIMIT = 10\n'
content += 'SEED = ' + str(seed) + '\n'
content += 'TRACE_LEVEL = 2\n'

for i in range(5, len(sys.argv)):
    if '--' in sys.argv[i]:
        content += sys.argv[i].replace('--', '') + ' = '
    else:
        content += sys.argv[i] + '\n'

os.system('mkdir -p temp')

def generateRandom(size):
    content = ''
    for _ in range(size):
        content += random.choice(random.choice([string.ascii_lowercase, string.digits]))
    return content

parFileName = 'temp/' + generateRandom(20) + '.txt'
parFile = open(parFileName, 'w')
parFile.write(content)
parFile.close()
os.system('./LKH ' + parFileName)
