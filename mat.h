/**
 * Simple matrix implementation
 */

#ifndef CGDL_MAT_H
#define CGDL_MAT_H

struct mat_t {
        int rows, cols;
        double *data;
};

#define MAT_AT(m, i, j) ((m)->data[(i) * (m)->cols + (j)])

struct mat_t* mat_new(int rows, int cols);
struct mat_t* mat_load(const char *fname);
void   mat_free(struct mat_t **m);
void   mat_set(struct mat_t *m, double v);
double mat_sum_col(const struct mat_t *m, int col);
double mat_sum_row(const struct mat_t *m, int row);
void   mat_print(const struct mat_t *m);

#endif
