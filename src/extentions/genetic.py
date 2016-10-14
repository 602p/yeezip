from random import randint, random
from operator import add

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
    'Create a member of the population.'
    return [ randint(min,max) for x in range(length) ]

def population(count, length, min, max):
    """
    Create a number of individuals (i.e. a population).

    count: the number of individuals in the population
    length: the number of values per individual
    min: the minimum possible value in an individual's list of values
    max: the maximum possible value in an individual's list of values

    """
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
            # this mutation is not ideal, because it
            # restricts the range of possible values,
            # but the function is unaware of the min/max
            # values used to create the individuals,
            individual[pos_to_mutate] = randint(
                min, max)
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
            if sum(child)>=actual_possibilities:
                children.append(child)
    parents.extend(children)
    return parents

base=sum(freq)
print("Base= %d"%base)

MIN=0
MAX=256
LENGTH=10

last=[]

p=population(100, LENGTH, MIN, MAX)

for i in range(3000):
    p=evolve(p, MIN, MAX, mutate=0.05)
    bytes=fitness(p[0])//8
    pct=(bytes/base)*100
    if(i%100==0):
        print("%d\t: %s \t\t%sb -> %f%% -> [%s]"%(i, str(p[0]), bytes, pct, ("#"*int(pct*.4))+(" "*(40-int(pct*.4)))))
        last.append(bytes)
        if len(last)>10:
            b=last[-10:][0]
            end=True
            for l in last[-10:]:
                if l!=b:
                    end=False
            if end:
                print("Last 1000 iterations made no improvement, quitting")
                break

solution=[item for item in p[0] if item!=0]
if solution[-1]==1:
    solution[-2]+=1
    del solution[-1]

print(solution)

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

print(tree)
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

show_tree(treeobj)

export(treeobj)