/**
 * Simple matrix implementation
 */

#ifndef CGDL_MAT_H
#define CGDL_MAT_H

struct mat {
        int rows, cols;
        double *data;
};

#define MAT_AT(m, r, c) ((m)->data[(r) * (m)->cols + (c)])

struct mat* mat_new(int rows, int cols);
struct mat* mat_load(const char *fname);
void   mat_free(struct mat **m);
void   mat_set(struct mat *m, double v);
double mat_sum_col(const struct mat *m, int col);
double mat_sum_row(const struct mat *m, int row);
void   mat_print(const struct mat *m);

#endif
