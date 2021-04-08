import matplotlib.pyplot as plt
import sys
from graphviz import Digraph


if (len(sys.argv) < 3):
    print("Error : missing argument\n usage python3 plot.py /path/to/eval.csv /path/to/circuit.csv\n", file=sys.stderr)
    exit(1)

table = []
result = []
path = sys.argv[1]


file = open(sys.argv[1], 'r') 
Lines = file.readlines() 
  
for line in Lines: 
    colon = line.find(',')
    result.append(int(line[:colon]))
    table.append(int(line[colon + 1:]))






g = Digraph('G', filename='circuit')

file = open(sys.argv[2], 'r') 
Lines = file.readlines() 


with g.subgraph(name='cluster_1') as c:
    c.attr(color='blue')
    c.node_attr['style'] = 'filled'
    for i in range(8):
        c.node(str(i))
    c.attr(label='Input')

with g.subgraph(name='cluster_2') as c:
    c.attr(color='red')
    c.node_attr['style'] = 'filled'
    start_range = len(Lines) + 8
    for i in range(start_range, start_range + 8):
        c.node(str(i), label=str(i - start_range))
    c.attr(label='Output')

idx = 7
for line in Lines: 
    idx += 1
    colon = line.find(',')
    if (colon == -1):
        continue
    first = line[:colon]
    line = line[colon + 1:]
    colon = line.find(',')
    second = line[:colon]
    function = line[colon + 1:]
    me = str(idx)
    g.node(me, label=function)
    g.edge(first, me)
    g.edge(second, me)
    if (idx >= len(Lines)):
        g.edge(me, str(idx + 8))

g.view()





plt.plot(table)
plt.plot(result)
plt.show()