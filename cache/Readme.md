# Cache
This repository contains several types of cache algorithms. It includes: LRU, LFU, prefect caching algorithm.
# Usage
For the testing you should use cmake for generating Makefile, then type:

        make tester && ./tester
Another target is **cache**. This target generate a program that allow to count number of hits based by sequence of requests.

        make cache && ./cache
        $> 4 10 1 2 1 3 1 4 5 5 5 5
First number meaning the size of cache, next is amount of requests, and then sequence of requests. Output of such program is of cache-hits evaluated by LFU caching alogithm.
