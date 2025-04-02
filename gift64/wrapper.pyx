#cython: language_level=3
#distutils: language = c++
#distutils: sources = attack.cpp

"""
To compile: python setup.py build_ext --inplace
"""
cimport cython
from libc.string cimport memcpy, memset
from libc.stdint cimport uint8_t, uint64_t
import numpy as np

cdef extern from *:
    """
    int attack(uint64_t cips[], uint64_t fcips[], uint64_t key[2], int noEXP);
    void oracle(uint64_t *key, uint64_t *cips, uint64_t *fcips, int noEXP);
    """
    int attack(uint64_t cips[], uint64_t fcips[], uint64_t key[2], int noEXP) nogil;
    void oracle(uint64_t *key, uint64_t *cips, uint64_t *fcips, int noEXP) nogil;

#The master key given input just for checking
def KeyRecovery(cips_arr, fcips_arr, orKey, exp):
    cdef uint64_t[:] key = orKey
    cdef uint64_t[:] cips = cips_arr 
    cdef uint64_t[:] fcips = fcips_arr 
    cdef int EXP = exp
    cdef int remainingKeys
    remainingKeys = attack(&cips[0], &fcips[0], &key[0], EXP)

    return remainingKeys

def oracleSimulator(orKey, exp):
    cips_arr = np.empty(exp, dtype=np.uint64)
    fcips_arr = np.empty(exp, dtype=np.uint64)
    cdef uint64_t[:] key = orKey
    cdef uint64_t[:] cips = cips_arr 
    cdef uint64_t[:] fcips = fcips_arr 
    cdef int EXP = exp
    oracle(&key[0], &cips[0], &fcips[0], EXP)

    return cips_arr, fcips_arr
