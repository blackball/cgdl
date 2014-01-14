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
int cgdl_init(struct cgdl_t *gdl, const struct mat_t *dm);
int cgdl_num(const struct cgdl_t *gdl);
int cgdl_labels(const struct cgdl_t *gdl, int *labels);
double cgdl_merge(struct cgdl_t *gdl);

#endif
