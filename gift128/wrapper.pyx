#cython: language_level=3
#distutils: language = c++
#distutils: sources = attack.cpp

"""
To compile: python setup.py build_ext --inplace
"""
cimport cython
from libc.string cimport memcpy, memset
from libc.stdint cimport uint8_t
import numpy as np

cdef extern from *:
    """
    int attack(uint8_t cips[], uint8_t fcips[], uint8_t key[], int noEXP);
    void oracle(uint8_t *key, uint8_t *cips, uint8_t *fcips, int noEXP);
    """
    int attack(uint8_t cips[], uint8_t fcips[], uint8_t key[], int noEXP) nogil;
    void oracle(uint8_t *key, uint8_t *cips, uint8_t *fcips, int noEXP) nogil;

#The master key given input just for checking
def KeyRecovery(cips_arr, fcips_arr, orKey, exp):
    cdef uint8_t[:] key = orKey
    cdef uint8_t[:] cips = cips_arr 
    cdef uint8_t[:] fcips = fcips_arr 
    cdef int EXP = exp
    cdef int remainingKeys
    remainingKeys = attack(&cips[0], &fcips[0], &key[0], EXP)

    return remainingKeys

def oracleSimulator(orKey, exp):
    cips_arr = np.empty(exp*32, dtype=np.uint8)
    fcips_arr = np.empty(exp*32, dtype=np.uint8)
    cdef uint8_t[:] key = orKey
    cdef uint8_t[:] cips = cips_arr 
    cdef uint8_t[:] fcips = fcips_arr 
    cdef int EXP = exp
    oracle(&key[0], &cips[0], &fcips[0], EXP)

    return cips_arr, fcips_arr
