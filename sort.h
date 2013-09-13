/**
 * Here is a tested example of this codes:
 *    https://github.com/blackball/boring/blob/master/sort-by-another.c
 *
 * @blackball
 */

#ifndef CGDL_SORT_H
#define CGDL_SORT_H

#include <stdlib.h>

/* Swap two items pointed to by A and B using temporary buffer t. */
#define _QSORT_SWAP(QSORT_BASE, QSORT_BASE1, a, b, t)                   \
        do {                                                            \
                ((void)((t = *a), (*a = *b), (*b = t))) ;               \
                /* swap another array */                                \
                hold1 = QSORT_BASE1[a - QSORT_BASE];                    \
                QSORT_BASE1[a - QSORT_BASE] = QSORT_BASE1[b - QSORT_BASE]; \
                QSORT_BASE1[b - QSORT_BASE] = hold1;                    \
        } while (0)


#define _QSORT_MAX_THRESH 4
#define _QSORT_STACK_SIZE (8 * sizeof(unsigned))
#define _QSORT_PUSH(top, low, high) (((top->_lo = (low)), (top->_hi = (high)), ++top))
#define _QSORT_POP(low, high, top) ((--top, (low = top->_lo), (high = top->_hi)))
#define _QSORT_STACK_NOT_EMPTY (_stack < _top)

/* The main code starts here... */
#define _QSORT(QSORT_TYPE, QSORT_BASE, QSORT_TYPE1, QSORT_BASE1, QSORT_NELT, QSORT_LT) \
        do {                                                            \
                QSORT_TYPE *const _base = (QSORT_BASE);                 \
                const unsigned _elems = (QSORT_NELT);                   \
                QSORT_TYPE _hold;                                       \
                QSORT_TYPE1 hold1;                                      \
                                                                        \
                /* Don't declare two variables of type QSORT_TYPE in a single \
                 * statement: eg `TYPE a, b;', in case if TYPE is a pointer, \
                 * expands to `type* a, b;' wich isn't what we want.    \
                 */                                                     \
                                                                        \
                if (_elems > _QSORT_MAX_THRESH) {                       \
                        QSORT_TYPE *_lo = _base;                        \
                        QSORT_TYPE *_hi = _lo + _elems - 1;             \
                        struct {                                        \
                                QSORT_TYPE *_hi; QSORT_TYPE *_lo;       \
                        } _stack[_QSORT_STACK_SIZE], *_top = _stack + 1; \
                                                                        \
                        while (_QSORT_STACK_NOT_EMPTY) {                \
                                QSORT_TYPE *_left_ptr; QSORT_TYPE *_right_ptr; \
                                                                        \
                                /* Select median value from among LO, MID, and HI. Rearrange \
                                   LO and HI so the three values are sorted. This lowers the \
                                   probability of picking a pathological pivot value and \
                                   skips a comparison for both the LEFT_PTR and RIGHT_PTR in \
                                   the while loops. */                  \
                                                                        \
                                QSORT_TYPE *_mid = _lo + ((_hi - _lo) >> 1); \
                                                                        \
                                if (QSORT_LT (_mid, _lo))               \
                                        _QSORT_SWAP (QSORT_BASE, QSORT_BASE1, _mid, _lo, _hold); \
                                if (QSORT_LT (_hi, _mid)) {             \
                                        _QSORT_SWAP (QSORT_BASE, QSORT_BASE1,_mid, _hi, _hold); \
                                        if (QSORT_LT (_mid, _lo))       \
                                                _QSORT_SWAP (QSORT_BASE, QSORT_BASE1,_mid, _lo, _hold); \
                                }                                       \
                                                                        \
                                _left_ptr = _lo + 1;                    \
                                _right_ptr = _hi - 1;                   \
                                                                        \
                                /* Here's the famous ``collapse the walls'' section of quicksort. \
                                   Gotta like those tight inner loops! They are the main reason \
                                   that this algorithm runs much faster than others. */ \
                                do {                                    \
                                        while (QSORT_LT (_left_ptr, _mid)) \
                                                ++_left_ptr;            \
                                                                        \
                                        while (QSORT_LT (_mid, _right_ptr)) \
                                                --_right_ptr;           \
                                                                        \
                                        if (_left_ptr < _right_ptr) {   \
                                                _QSORT_SWAP (QSORT_BASE, QSORT_BASE1,_left_ptr, _right_ptr, _hold); \
                                                if (_mid == _left_ptr)  \
                                                        _mid = _right_ptr; \
                                                else if (_mid == _right_ptr) \
                                                        _mid = _left_ptr; \
                                                ++_left_ptr;            \
                                                --_right_ptr;           \
                                        }                               \
                                        else if (_left_ptr == _right_ptr) { \
                                                ++_left_ptr;            \
                                                --_right_ptr;           \
                                                break;                  \
                                        }                               \
                                } while (_left_ptr <= _right_ptr);      \
                                                                        \
                                /* Set up pointers for next iteration. First determine whether \
                                   left and right partitions are below the threshold size. If so, \
                                   ignore one or both. Otherwise, push the larger partition's \
                                   bounds on the stack and continue sorting the smaller one. */ \
                                                                        \
                                if (_right_ptr - _lo <= _QSORT_MAX_THRESH) { \
                                        if (_hi - _left_ptr <= _QSORT_MAX_THRESH) \
                                                /* Ignore both small partitions. */ \
                                                _QSORT_POP (_lo, _hi, _top); \
                                        else                            \
                                                /* Ignore small left partition. */ \
                                                _lo = _left_ptr;        \
                                }                                       \
                                else if (_hi - _left_ptr <= _QSORT_MAX_THRESH) \
                                        /* Ignore small right partition. */ \
                                        _hi = _right_ptr;               \
                                else if (_right_ptr - _lo > _hi - _left_ptr) { \
                                        /* Push larger left partition indices. */ \
                                        _QSORT_PUSH (_top, _lo, _right_ptr); \
                                        _lo = _left_ptr;                \
                                }                                       \
                                else {                                  \
                                        /* Push larger right partition indices. */ \
                                        _QSORT_PUSH (_top, _left_ptr, _hi); \
                                        _hi = _right_ptr;               \
                                }                                       \
                        }                                               \
                }                                                       \
                                                                        \
                /* Once the BASE array is partially sorted by quicksort the rest \
                   is completely sorted using insertion sort, since this is efficient \
                   for partitions below MAX_THRESH size. BASE points to the \
                   beginning of the array to sort, and END_PTR points at the very \
                   last element in the array (*not* one beyond it!). */ \
                                                                        \
                {                                                       \
                        QSORT_TYPE *const _end_ptr = _base + _elems - 1; \
                        QSORT_TYPE *_tmp_ptr = _base;                   \
                        register QSORT_TYPE *_run_ptr;                  \
                        QSORT_TYPE *_thresh;                            \
                                                                        \
                        _thresh = _base + _QSORT_MAX_THRESH;            \
                        if (_thresh > _end_ptr)                         \
                                _thresh = _end_ptr;                     \
                                                                        \
                        /* Find smallest element in first threshold and place it at the \
                           array's beginning. This is the smallest array element, \
                           and the operation speeds up insertion sort's inner loop. */ \
                                                                        \
                        for (_run_ptr = _tmp_ptr + 1; _run_ptr <= _thresh; ++_run_ptr) \
                                if (QSORT_LT (_run_ptr, _tmp_ptr))      \
                                        _tmp_ptr = _run_ptr;            \
                                                                        \
                        if (_tmp_ptr != _base)                          \
                                _QSORT_SWAP (QSORT_BASE, QSORT_BASE1, _tmp_ptr, _base, _hold); \
                                                                        \
                        /* Insertion sort, running from left-hand-side  \
                         * up to right-hand-side. */                    \
                                                                        \
                        _run_ptr = _base + 1;                           \
                        while (++_run_ptr <= _end_ptr) {                \
                                _tmp_ptr = _run_ptr - 1;                \
                                while (QSORT_LT (_run_ptr, _tmp_ptr))   \
                                        --_tmp_ptr;                     \
                                                                        \
                                ++_tmp_ptr;                             \
                                if (_tmp_ptr != _run_ptr) {             \
                                        QSORT_TYPE *_trav = _run_ptr + 1; \
                                        while (--_trav >= _run_ptr) {   \
                                                QSORT_TYPE *_hi; QSORT_TYPE *_lo; \
                                                _hold = *_trav;         \
                                                                        \
                                                for (_hi = _lo = _trav; --_lo >= _tmp_ptr; _hi = _lo) \
                                                        *_hi = *_lo;    \
                                                *_hi = _hold;           \
                                        }                               \
                                }                                       \
                        }                                               \
                }                                                       \
        } while(0)


#endif 
