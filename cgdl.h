/**
 * CGDL: Graph degree linkage implemented in C.
 *
 * @blackball
 */

#ifndef CGDL_H
#define CGDL_H

struct mat;
struct cgdl;

struct cgdl* cgdl_new(int kw, int kl);
void cgdl_free(struct cgdl **gdl);
int cgdl_init(struct cgdl *gdl, const struct mat *dm);
int cgdl_num(const struct cgdl *gdl);
int cgdl_labels(const struct cgdl *gdl, int *labels);
double cgdl_merge(struct cgdl *gdl);

#endif
