"""This file is a horrible (but functional) mess cobbled together from
StackOverflow and blog posts. Extension arguments described here:

    -Pg_help                Show this
    -Pg_showprogress        Causes the optimizaiton progress to be shown
                                (shown by default when in STATUS or below)
    -Pg_showchars           Print out a view of the tree (showing chars) once done
    -Pg_showtree            Print out a nested view of the tree (showing chars)
                                after object construction
    -pg_min=<int>           Sets the minimum value for an element of a individual to this           (default:0)
    -pg_max=<int>           Sets the maximum value for an element of a individual to This           (default:127)
    -pg_length=<int>        Sets the length of individuals to this                                  (default:10)
    -pg_retain=<float>      Sets the fraction to take off the better end of the fitness-sorted 
                                set of solutions to take to reproduce                               (default:0.2)
    -pg_randsel=<float>     Sets the fraction of the solutions to grap for reproduction randomly    (default:0.05)
    -pg_mutate=<float>      Sets the fraction of the solutions to mutate                            (default:0.05)
    -pg_limcyc=<int>        Stop optimizaiton after this*100 cycles                                 (default:30 (3000 cycles))
    -pg_limsec=<int>        Stop optimizaiton after this many seconds                               (defualt:60)
    -pg_limicyc=<int>       Stop optimizaiton after this*100 cycles without improvment              (default:10 (1000 cycles))
    -pg_limisec=<int>       Stop optimizaiton after this many seconds without improvment            (default:20)
    -pg_limpct=<float>      Stop optimizaiton after the size of the compressed file will equal
                                less than this value."""

from random import randint, random
from operator import add
from time import time
import sys

if get_parameters().get("g_help"):
    print(__doc__)
    sys.exit()

freq=get_freq()
ordering=[i for i,v in sorted(enumerate(freq), key=lambda i:i[1], reverse=True)]
actual_possibilities=sum([(1 if i else 0) for i in freq])

def min_bits_to_represent(n):
    if n<=2: return 1
    if n<=4: return 2
    if n<=8: return 3
    if n<=16: return 4
    if n<=32: return 5
    if n<=64: return 6
    if n<=128: return 7
    return 8

def individual(length, min, max):
    return [ randint(min,max) for x in range(length) ]

def population(count, length, min, max):
    return [ individual(length, min, max) for x in range(count) ]

def fitness(individual):
    temp=ordering[:]
    lut=[999 for _ in range(256)]
    acc=0
    for item in individual:
        acc+=min_bits_to_represent(item+1)
        for _ in range(item):
            try:
                lut[temp.pop(0)]=acc
            except IndexError:pass
        
    return sum([freq[i]*lut[i] for i in range(256)]) + (16*(sum(individual)+sum([1 if x!=0 else 0 for x in individual])))

def grade(pop):
    'Find average fitness for a population.'
    return sum([fitness(i) for i in pop])

def evolve(pop, min, max, retain=0.2, random_select=0.05, mutate=0.01):
    graded = [ (fitness(x), x) for x in pop]
    graded = [ x[1] for x in sorted(graded)]
    retain_length = int(len(graded)*retain)
    parents = graded[:retain_length]
    # randomly add other individuals to
    # promote genetic diversity
    for individual in graded[retain_length:]:
        if random_select > random():
            parents.append(individual)
    # mutate some individuals
    for individual in parents:
        if mutate > random():
            pos_to_mutate = randint(0, len(individual)-1)
            individual[pos_to_mutate] = randint(min, max)
    # crossover parents to create children
    parents_length = len(parents)
    desired_length = len(pop) - parents_length
    children = []
    while len(children) < desired_length:
        male = randint(0, parents_length-1)
        female = randint(0, parents_length-1)
        if male != female:
            male = parents[male]
            female = parents[female]
            half = len(male) // 2
            child = male[:half] + female[half:]
            #Filter out children that don't represent a possible tree (they encode too few values)
            if sum(child)>=actual_possibilities:
                children.append(child)
    parents.extend(children)
    return parents

base=sum(freq)
debug("Base= %d"%base)

MIN=int(get_parameters().get("g_min", 0))
MAX=int(get_parameters().get("g_max", 127))
LENGTH=int(get_parameters().get("g_length", 10))
RETAIN=float(get_parameters().get("g_retain", 0.2))
RANDSEL=float(get_parameters().get("g_randsel", 0.05))
MUTATE=float(get_parameters().get("g_mutate", 0.05))
LIMIT_CYCLES=int(get_parameters().get("g_limcyc", 3000))
LIMIT_SECONDS=float(get_parameters().get("g_limsec", 60))
LIMIT_BADCYCLES=int(get_parameters().get("g_limicyc", 10))
LIMIT_BADSECONDS=float(get_parameters().get("g_limisec", 20))
LIMIT_SIZE=float(get_parameters().get("g_limpct", 0))

info("Running genetic algorithm (starting at size %db.) Limit=%d cycles / %d seconds / %d (*100) ineffective cycles / %d ineffective seconds / %02d%% size"%
    (base, LIMIT_CYCLES, LIMIT_SECONDS, LIMIT_BADCYCLES, LIMIT_BADSECONDS, LIMIT_SIZE))

last=[]
last_change_time=time()

p=population(100, LENGTH, MIN, MAX)
start=time()

i=0

try:
    while (i<LIMIT_CYCLES+1) or LIMIT_CYCLES==-1:
        i+=1
        p=evolve(p, MIN, MAX, retain=RETAIN, random_select=RANDSEL, mutate=MUTATE)
        bytes=fitness(p[0])//8
        pct=(bytes/base)*100
        
        if(i%100==0):
            if get_parameters()["loglevel"]<4 or get_parameters().get("g_showprogress"):
                print("\r%d\t: %s \t\t%sb -> %f%% -> [%s] (dt=%d)"%(i, str(p[0]), bytes, pct, ("#"*int(pct*.4))+(" "*(40-int(pct*.4))), time()-start), end="")
            if last:
                if last[-1]!=bytes:
                    last_change_time=time()
            last.append(bytes)
            if LIMIT_BADCYCLES!=-1:
                if len(last)>LIMIT_BADCYCLES:
                    b=last[-LIMIT_BADCYCLES:][0]
                    end=True
                    for l in last[-LIMIT_BADCYCLES:]:
                        if l!=b:
                            end=False
                    if end:
                        status("Limited by LIMIT_BADCYCLES", start="\n")
                        break
            if (time()-start)>LIMIT_SECONDS and LIMIT_SECONDS!=-1:
                status("Limited by LIMIT_SECONDS", start="\n")
                break
            if (time()-last_change_time)>LIMIT_BADSECONDS and LIMIT_BADSECONDS!=-1:
                status("Limited by LIMIT_BADSECONDS", start="\n")
                break
            if pct<LIMIT_SIZE:
                status("Limited by LIMIT_SIZE", start="\n")
                break
except KeyboardInterrupt:
    status("Limited by user", start="\n")

solution=[item for item in p[0] if item!=0]
if solution[-1]==1:
    solution[-2]+=1
    del solution[-1]

status("Optimal discovered solution: `%s`, size: %02d%% of original" % (str(solution), ((fitness(solution)//8)/base)*100))

temp=ordering[:]
tree=[]
for item in solution:
    temptree=[]
    for _ in range(item):
        if temp:
            v=temp.pop(0)
            if freq[v]:
                temptree.append(chr(v))
    tree.append(temptree)

if get_parameters().get("g_showchars"):
    print("Tree generated: "+str(tree))

treeobj=TreeNode()
pos=treeobj
idx=0
for item in tree:
    if item==0:continue
    [pos.add(TreeNode(ord(char))) for char in item]
    idx+=1
    if idx!=len(tree):pos=pos.add(TreeNode())
    
def show_tree(node, depth=0):
    for item in node.children:
            print(("\t"*depth)+" o %i -> %s\t(%i children)"%(item.value, chr(item.value), item.child_count))
            if item.child_count!=0:
                show_tree(item, depth+1)

if get_parameters().get("g_showtree"):
    show_tree(treeobj)

export(treeobj)