#!/usr/bin/env python

from razz import loop, output_ranks
from multiprocessing import Pool, cpu_count
from sys import argv

SIMS = long(argv[1])
cpus = cpu_count()
nplayers = 1
init_cards = {0:[1,2,3]}
# shared state can be done with Value, Array

def dict_merge(dicts):
    ret = {}
    for k in dicts[0].keys():
        ret[k] = sum(d[k] for d in dicts)
    return ret

if __name__ == '__main__':
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

