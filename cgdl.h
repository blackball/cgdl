/**
 * CGDL: Graph degree linkage implemented in C.
 *
 * @blackball
 */

#ifndef CGDL_H
#define CGDL_H

struct mat_t;
struct cgdl_t;

struct cgdl_t* cgdl_new(int kw, int kl);
void cgdl_free(struct cgdl_t **gdl);

/* initialize CGDL using distance matrix */
int cgdl_init(struct cgdl_t *gdl, const struct mat_t *dm);

/* merge the most closest two clusters, return affinity (their distance) */
double cgdl_merge(struct cgdl_t *gdl);

/* current cluster number */
int cgdl_num(const struct cgdl_t *gdl);

/* after clustering, get the cluster labels, [0, num-1] */
int cgdl_labels(const struct cgdl_t *gdl, int *labels);

#endif
