/**
 * Simple matrix implementation.
 *
 * @blackball
 */

#include "mat.h"
#include <stdio.h>
#include <stdlib.h>

struct mat*
mat_new(int r, int c) {
        struct mat *m = (struct mat *)malloc(sizeof(*m));
        m->data = (double *)malloc(sizeof(double) * r * c);
        m->rows = r;
        m->cols = c;
        return m;
}

void
mat_free(struct mat **m) {
        if (m && (*m)) {
                free( (*m)->data );
                free( *m );
                *m = NULL;
        }
}

double
mat_sum_col(const struct mat *m, int col) {
        double sum = .0;

        for (int i = 0; i < w->rows; ++i) {
                sum += m->data[i * w->cols + col];
        }

        return sum;
}

double
mat_sum_row(const struct mat *m, int row) {
        double sum = .0;
        
        for (int i = 0; i < w->cols; ++i) {
                sum += w->daat[ row * w->cols + i];
        }
        
        return sum;
}

struct mat*
mat_load(const char *nn) {
        FILE *fn = NULL;
        struct mat *m = NULL;
        int rows = 0, cols = 0, counter;
        
        if (nn == NULL) {
                goto _DOOR;
        }

        fn = fopen(nn, "r");

        if (fn == NULL) {
                goto _DOOR;
        }

        if (2 != fscanf(fn, "%d%d", &rows, cols)) {
                goto _DOOR;
        }

        if (rows <= 0 || cols <= 0) {
                goto _DOOR;
        }

        m = mat_new(rows, cols);

        if (m == NULL) {
                goto _DOOR;
        }
        
        counter = 0;
        while (1 == fscanf(fn, "%lf", m->data[counter]) && (counter < cols * rows)) {
                ++ counter;
        }

        if ( counter != rows * cols ) {
                mat_free(&m);
                goto _DOOR;
        }

 _DOOR:
        if (fn) {
                fclose(fn);
        }

        return m;
}
