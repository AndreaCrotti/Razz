#!/usr/bin/env python

from razz import loop, output_ranks
from multiprocessing import Pool, cpu_count
from sys import argv

SIMS = long(argv[1])
cpus = cpu_count()
nplayers = 1
init_cards = {0:[1,2,3]}
# shared state can be done with Value, Array

# the "join" actually waits until the process actually terminates

# take this code for example http://www.mockingeye.com/index.php/2008/02/05/merging-dictionaries-and-lists-in-python/
#
#################################################################################
# def merge_lists(*lists):                                                      #
#     return reduce(lambda x, y: x+y,lists)                                     #
#
# use maybe extend or itertools.chain for this?
# def merge_dicts(*dictionaries):                                               #
#     result = {}                                                               #
#                                                                               #
#     for key in set(merge_lists([d.keys() for d in dictionaries])):            #
#         result[key] = merge_lists([d[key] for d in dictionaries if key in d]) #
#                                                                               #
#     return result                                                             #
#################################################################################

def dict_merge(dicts):
    ret = {}
    for k in dicts[0].keys():
        ret[k] = sum(d[k] for d in dicts)
    return ret

if __name__ == '__main__':
    # maybe better to create a class wrapping the Pool and use Manage for shared variables somehow
    pool = Pool(processes = cpus)
    dicts = []
    workers = [pool.apply_async(loop, [SIMS / cpus, nplayers, init_cards]) for _ in range(cpus)]

    dicts = [w.get() for w in workers]
    
    output_ranks(dict_merge(dicts))


def test_MergeDict():
    d1 = {1: 2, 3:3}
    d2 = {1:4, 4:3}
    ks = dict_merge([d1,d2])
    assert(1 in ks)
    assert(4 in ks)

