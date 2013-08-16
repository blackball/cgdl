/**
 * CGDL: Graph Degree Linkage implemented in C.
 *
 * @blackball
 */

#include "sort.h" /* used in wmat_sort, sort array by another array */
#include "mat.h"
#include "cgdl.h"

/** idict structure **/
 
#define Itype int
 
#define IVEC_HEADER_SIZE    3
#define IVEC_HEADER_KEY_I   0
#define IVEC_HEADER_SIZE_I  1
#define IVEC_HEADER_ALLOC_I 2
 
#define IVEC_DEFAULT_SIZE 128
#define IVEC_DEFAULT_STEP 128
 
#define IVEC_KEY(v)     ((v)[IVEC_HEADER_KEY_I])
#define IVEC_SIZE(v)    ((v)[IVEC_HEADER_SIZE_I])
#define IVEC_ALLOC(v)   ((v)[IVEC_HEADER_ALLOC_I])
#define IVEC_DATAPTR(v) ((v) + IVEC_HEADER_SIZE))
#define IVEC_AT(v, i)   ((v)[IVEC_HEADER_SIZE + (i)])
#define IVEC_CLEAR(v)   ((v)[IVEC_HEADER_SIZE_I] = 0)

#define IVEC_NEW(v, sz)                                                 \
        do {                                                            \
                const int s = (sz) > IVEC_DEFAULT_SIZE ? (sz) : IVEC_DEFAULT_SIZE; \
                (v) = malloc(sizeof(Itype) * (IVEC_HEADER_SIZE + s));   \
                if ((v) != NULL) {                                      \
                        (v)[IVEC_HEADER_KEY_I] = -1;                    \
                        (v)[IVEC_HEADER_SIZE_I] = 0;                    \
                        (v)[IVEC_HEADER_ALLOC_I] = s;                   \
                }                                                       \
        } while (0)
 
#define IVEC_FREE(v)                            \
        do {                                    \
                free(v);                        \
                (v) = NULL;                     \
        } while (0)
 
 
#define IVEC_RESIZE(v, nsz)                                             \
        do {                                                            \
                if ( IVEC_ALLOC(v) < (nsz) ) {                          \
                        (v) = realloc((v), sizeof(Itype) * (nsz + IVEC_HEADER_SIZE)); \
                        if ((v) != NULL) {                              \
                                IVEC_ALLOC(v) = (nsz);                  \
                                if ((nsz) < IVEC_SIZE(v)) {             \
                                        IVEC_SIZE(v) = (nsz);           \
                                }                                       \
                        }                                               \
                }                                                       \
        } while(0)
 
 
#define IVEC_APPEND(v, val)                                             \
        do {                                                            \
                if ( IVEC_SIZE(v) >= IVEC_ALLOC(v) ) {                  \
                        IVEC_RESIZE(v, IVEC_SIZE(v) + IVEC_DEFAULT_STEP); \
                }                                                       \
                if ((v) != NULL) {                                      \
                        IVEC_AT(v, IVEC_SIZE(v)) = (val);               \
                        IVEC_SIZE(v) += 1;                              \
                }                                                       \
        } while (0)
 
 
/* merge v1 to v0 */
#define IVEC_MERGE(v0, v1)                                              \
                do {                                                    \
                        const int nsz = IVEC_SIZE(v0) + IVEC_SIZE(v1);  \
                        IVEC_RESIZE(v0, nsz);                           \
                        if (v0 != NULL) {                               \
                                memcpy( IVEC_DATAPTR(v0) + IVEC_SIZE(v0), IVEC_DATAPTR(v1), sizeof(Itype) * IVEC_SIZE(v1)); \
                                IVEC_SIZE(v0) += IVEC_SIZE(v1);         \
                        }                                               \
                } while (0)
 
 
#endif
 
 
struct idict {
        int size, alloc;
        int vnum; /* valid, non-null vectors, because we have a merge keep method ... */
        /* pvec[i] is a int array, and the pvec[i][0] is the key, pvec is the size of it. */
        int **pvec;
};
 
#define IDICT_DEFAULT_SIZE 128
#define IDICT_DEFAULT_STEP 128
 
static struct idict*
idict_new(int sz) {
        struct idict *dict = malloc(sizeof(*dict));
 
        if (dict == NULL) {
                return NULL;
        }
 
        sz = sz > 0 ? sz : IDICT_DEFAULT_SIZE;
 
        dict->pvec = malloc(sizeof(int *) * sz);
 
        if (dict->pvec == NULL) {
                free(dict);
                return NULL;
        }
 
        dict->vnum = 0;
 
        dict->size = 0;
        dict->alloc = sz;
        return dict;
}
 
static void
idict_free(struct idict **dict) {
        if (dict && (*dict)) {
                free( (*dict)->pvec );
                free( *dict );
                *dict = NULL;
        }
}
 
static int
idict_find(const struct idict *dict, int key) {
        int i;
 
        if (dict == NULL || dict->size <= 0) {
                return -1;
        }
 
        for (i = 0; i < dict->size; ++i) {
                if (dict->pvec[i] != NULL) {
                        if (key == IVEC_KEY(dict->pvec[i])) {
                                return i;
                        }
                }
        }
        return -1;
}
 
static int
idict_resize(struct idict *dict, int nsz) {
        if (dict->alloc >= nsz || nsz <= 0) {
                return 0;
        }
 
        /* this is a shrink, free tails */
        if (dict->size > nsz) {
                int i;
                for (i = nsz; i < dict->size; ++i) {
 
                        if (dict->pvec[i] != NULL) {
                                IVEC_FREE( dict->pvec[i] );
                                dict->vnum --;
                        }
                }
 
                dict->size = nsz;
        }
 
        dict->pvec = realloc(dict->pvec, sizeof(int*) * nsz);
 
        dict->alloc = nsz;
 
        return 0;
}
 
static void
idict_add(struct idict *dict, int key, int val) {
        int i = idict_find(dict, key);
 
        if (i == -1) {
                /* no key found, append it as a new one */
                int *ivec = NULL;
 
                if (dict->size >= dict->alloc) {
                        idict_resize(dict, dict->size + IDICT_DEFAULT_STEP);
                }
 
                IVEC_NEW(ivec, 0);
                IVEC_KEY(ivec) = key;
                IVEC_APPEND(ivec, val);
 
                dict->pvec[ dict->size ] = ivec;
                dict->size ++;
                dict->vnum ++;
        }
        else {
                /* find a key, append to the specific vector */
                IVEC_APPEND(dict->pvec[i], val);
        }
}
 
static int
idict_merge(struct idict *dict, int dstIdx, int srcIdx) {
        int i;
 
        if ( dict == NULL || dict->size <= dstIdx || dict->size <= srcIdx ||
             dict->pvec[dstIdx] == NULL || dict->pvec[srcIdx] == NULL) {
                return -1;
        }
 
        IVEC_MERGE(dict->pvec[dstIdx], dict->pvec[srcIdx]);
 
        IVEC_FREE(dict->pvec[srcIdx]);
 
        for (i = srcIdx; i < dict->size - 1; ++i) {
                dict->pvec[i] = dict->pvec[i+1];
        }
 
        dict->size --;
        dict->vnum --;
 
        return 0;
}
 
/* the normal process of merging will change all the index behind,
 * sometimes, I don't want this happen, so just keep the
 * srcIdx == NULL, and not move tails.
 * NOTE this won't decrease the number of pvec!!
 */
static int
idict_merge_keep(struct idict *dict, int dstIdx, int srcIdx) {
        int i;
 
        if ( dict == NULL || dict->size <= dstIdx || dict->size <= srcIdx ||
             dict->pvec[dstIdx] == NULL || dict->pvec[srcIdx] == NULL) {
                return -1;
        }
 
        IVEC_MERGE(dict->pvec[dstIdx], dict->pvec[srcIdx]);
 
        IVEC_FREE(dict->pvec[srcIdx]);
 
        dict->pvec[srcIdx] = NULL;
 
        dict->vnum --;
 
        return 0;
}
 
static void
idict_print(const struct idict *dict) {
        int i, j;
        if (dict == NULL) {
                return ;
        }
 
        for (i = 0; i < dict->size; ++i) {
                if (dict->pvec[i] != NULL) {
                        for (j = 0; j < IVEC_SIZE( dict->pvec[i] ); ++j) {
                                printf("%d, ", IVEC_AT(dict->pvec[i], j));
                        }
                        printf("\n");
                }
        }
}
 

/** iset structure **/
struct iset {
        int size, alloc;
        int *data;
};
 
struct iset*
iset_new(int sz) {
        struct iset *s = malloc(sizeof(*s));
        sz = sz > 0 ? sz : 64;
        s->data = malloc(sizeof(int) * sz);
        s->size = 0;
        s->alloc = sz;
        return s;
}
 
void
iset_free(struct iset **s) {
        if (s && (*s)) {
                free( (*s)->data );
                free( *s );
                *s = NULL;
        }
}
 
static inline int
iset_find(const struct iset *s, int val) {
        int i;
        for (i = 0; i < s->size; ++i) {
                if (s->data[i] == val) {
                        return i;
                }
        }
        return -1;
}
 
static inline void
iset_clear(struct iset *s) {
        s->size = 0;
}
 
static inline int
iset_insert(struct iset *s, int val) {
        int i = iset_find(s, val);
 
        if (i == -1) {
                if (s->size >= s->alloc) {
                        /* ! out of range */
                        return -1;
                }
                s->data[ s->size++ ] = val;
        }
}
 
static inline int
iset_min(const struct iset *s) {
        int i, m;
        if (s == NULL) {
                /* ! Exception */
                return -1;
        }
 
        m = s->data[0];
        for (i = 1; i < s->size; ++i) {
                if (s->data[i] < m) {
                        m = s->data[i];
                }
        }
        
        return m;
}

/** wmat structure **/
struct wmat {
        int rows, cols;
        int *i;
        double *w; 
};

static struct wmat*
wmat_new(int r, int c) {
        struct wmat *wm = (struct wmat*)malloc(sizeof(*wm));
        wm->i = (int *)malloc(sizeof(int) * r * c);
        wm->w = (double *)malloc(sizeof(double) * r * c);
        wm->rows = r;
        wm->cols = c;
        return wm;
}

static void
wmat_free(struct wmat **wm) {
        if (wm) {
                free( (*wm)->i );
                free( (*wm)->w );
                free( (*wm) );
                *wm = NULL;
        }
}

static void
wmat_square(struct wmat *wm) {
        const int cols = wm->cols;
        const int rows = wm->rows;
        double *w = wm->w;
        int i, j;
        
        for (i = 0; i < rows; ++i) {
                for (j = 0; j < cols; ++j) {
                        w[i * cols + j] *= w[i * cols + j];
                }
        }
}

double
wmat_sum(const struct wmat *wm) {
        const int cols = wm->cols;
        const int rows = wm->rows;
        double *w = wm->w;
        double sum = .0;
        int i, j;
        
        for (i = 0; i < rows; ++i) {
                for (j = 0; j < cols; ++j) {
                        sum += w[i * cols + j];
                }
        }
        
        return sum;
}        

#define WMAT_SORT_BY_I 0
#define WMAT_SORT_BY_D 1

static void
wmat_sort(struct wmat *wm, int flag) {
        int i;

#define WMAT_CMP(a, b) ((*a) < (*b))
       
        if (flag == WMAT_SORT_BY_I) {
                for (i = 0; i < wm->rows; ++i) {
                        double *dists = wm->d + i * wm->cols;
                        int *indices = wm->i + i * wm->cols;
                        _QSORT(int, indices, double, dists, wm->cols, WMAT_CMP);
                }
        }
        else if (flag == WMAT_SORT_BY_D) {
                for (i = 0; i < wm->rows; ++i) {
                        double *dists = wm->d + i * wm->cols;
                        int *indices = wm->i + i * wm->cols;
                        _QSORT(double, dists, int, indices, wm->cols, WMAT_CMP);
                }
        }
        else {
                /* !! error */
                ;
        }
                        
#endif WMAT_CMP
}

static int
wmat_normalize(struct wmat *wm, int k, double a) {
        int i;
        double sum = .0, sigma2_inv = .0;
        
        /* first square them */
        wmat_square(wm);

        sum = wmat_sum(wm);
        
        sigma2_inv = (double)(wm->rows * k) / (a * sum);

        for (i = 0; i < wm->rows * wm->cols; ++i) {
                wm->data[i] = -exp( wm->data[i] ) * sigma2_inv;
        }
        
        return 0;
}
static void
wmat_swap_label(int *labels, int n, int old, int new) {
        for (int i = 0; i < n; ++i) {
                if (labels[i] == old) {
                        labels[i] = new;
                }
        }
}

/* Link wm [mxk matrixs] to a disjoint set, store the label into labels 
 *
 * For every group:
 *     if no one was labeled, give a new label
 *     if there's only one label, give it all one label
 *     if there's multiple labels, give *them* all the smallest label
 * Done!
 *
 * k <= wm->cols
 */
static int
wmat_link(const struct wmat *wm, int k, int *labels) {
        int i, j, label_gen = 0;
        struct iset *label_set = NULL;

        if (wm == NULL || wm->cols < k || labels == NULL) {
                return -1;
        }

        label_set = iset_new(k);
        
        for (i = 0; i < N; ++i) {
                labels[i] = -1;
        }

        for (i = 0; i < wm->rows; ++i) {
                iset_clear(label_set);

                if (labels[i] >= 0) {
                        iset_insert(label_set, labels[i]);
                }
                
#define WMAT_I_AT(wm, r, c) ( (wm)->i[ (r) * (wm)->cols + (c)] )
                
                for (j = 0; j < k; ++j) {
                        int t = labels[ WMAT_I_AT(wm, i, j) ];
                        if (t >= 0) {
                                iset_insert(label_set, t);
                        }
                }

                if (label_set->size <= 0) {
                        int new_label = label_gen++;
 
                        for (j = 0; j < k; ++j) {
                                labels[i] = new_label;
                                labels[ WMAT_I_AT(wm, i, j) ] = new_label;
                        }
                }
                else {
                        int new_label = iset_min(label_set);
                        int m;
                        
                        for (j = 0; j < k; ++j) {
                                labels[i] = new_label;
                                labels[ WMAT_I_AT(wm, i, j) ] = new_label;
                        }
                        
                        for (m = 0; m < label_set->size; ++m) {
                                int old_label = label_set->data[m];
                                if (old_label != new_label) {
                                        swap_label(labels, N, old_label, new_label);
                                }
                        }
                }
#undef WMAT_I_AT
        }
}


static inline int
_wmat_bsearch(const int *indices, int n, int val) {
        int start = 0, end = n - 1, mid;

        while (start <= end) {
                mid = start + ((end - start) >> 1);
                if (val > indices[mid]) {
                        start = mid + 1;
                }
                else if (val < indices[mid]) {
                        end = mid - 1;
                }
                else {
                        return mid;
                }
        }
        return -1;
}

static inline double
wmat_get_d(const struct wmat *wm, int r, int c) {
        const int *indices = wm->i + r * wm->cols;

        int re = _wmat_bsearch(indices, wm->cols, c);

        if (re != -1) {
                return wm->d[r * wm->cols + re]; 
        }
        
        return 0.0;
}

/* get a mat from wmat by row indices and col indices */
static struct mat*
wmat_getm(const struct wmat *wm, const int *ri, int rn, const int *ci, int cn) {
        int i, j;
        struct mat *m = mat_new(rn, cn);

        if (m == NULL) {
                return NULL;
        }
        
        for (i = 0; i < rn; ++i) {
                for (j = 0; j < cn; ++j) {
                        MAT_AT(m, i, j) = wmat_get_d(wm, ri[i], ci[j]);
                }
        }
        
        return m;
}

/** fastpair routine implementation **/

struct fastpair {
        int npoints;
        int *points;
        int *where;
        int *neighbors;
        double *nbr_dist;

        /* external data, used by distance func */
        const struct idict *dict;
        const struct wmat  *wm;
};

#define MAX_DISTANCE DBL_MAX

/* cluster affinity */
static inline double
fastpair_distance(const struct fastpair *fp, int ia, int ib) {
        double dist = .0, ab = .0, ba = .0;

        struct mat *mab = NULL, *mba = NULL;
        const int i, *ia = NULL, *ib = NULL;

        ia = fp->dict->pvec[ia];
        ib = fp->dict->pvec[ib];
        
        if (ia == NULL || ib == NULL) {
                return MAX_DISTANCE;
        }

        mab = wmat_getm(fp->wm, IVEC_DATAPTR(ia), IVEC_SIZE(ia), IVEC_DATAPTR(ib), IVEC_SIZE(ib));
        mba = wmat_getm(fp->wm, IVEC_DATAPTR(ib), IVEC_SIZE(ib), IVEC_DATAPTR(ia), IVEC_SIZE(ia));

        for (i = 0; i < mab->cols; ++i) {
                ab += mat_sum_col(mab, i) * mat_sum_row(mba, i);
        }

        /* normalization */
        ab /= mab->cols * mab->cols;

        for (i = 0; i < mba->cols; ++i) {
                ba += mat_sum_col(mba, i) * mat_sum_row(mba, i);
        }

        ba /= mba->cols * mba->cols;

        mat_free(&mab);
        mat_free(&mba);

        /* because bigger affinity means closer, so, need to inverse it for fastpair */

        return 1.0 / (dist + DBL_EPSILON);
}

static void
fastpair_free(struct fastpair **fp) {
        if (fp && (*fp)) {
                free( (*fp)->points );
                free( (*fp)->where );
                free( (*fp)->neighbors);
                free( (*fp)->nbr_dist);
                *fp = NULL;
        }
}

static struct fastpair*
fastpair_new(const struct idict *dict, const struct wmat *wm) {
        int np = dict->rows, i, *p = NULL;
        struct fastpair *fp = malloc(sizeof(*fp));

        if (fp == NULL) {
                return NULL;
        }

        fp->npoints   = np;
        fp->points    = malloc(sizeof(int) * np);
        fp->where     = malloc(sizeof(int) * np);
        fp->neighbors = malloc(sizeof(int) * np);
        fp->nbr_dist  = malloc(sizeof(double) * np);

        if (!fp->points || !fp->where || !fp->neighbors || !fp->nbr_dist) {
                /* memory use out ?? */
                fastpair_free(&fp);
                return NULL;
        }
         
        fp->dict = dict;
        fp->wm = wm;
         
        for (i = 0; i < np; ++i) {
                fp->points[i] = i;
        }

        p = fp->points;

        while (np > 1) {
                int nbr = 1;
                double nbd = fastpair_distance(fp, p[0], p[1]);
                for (i = 2; i < np; ++i) {
                        double d = fastpair_distance(fp, p[0], p[i]);

                        if (d < nbd) {
                                nbr = i;
                                nbd = d;
                        }
                }
                /* add 1nn edge */
                fp->neighbors[ p[0] ] = p[ nbr ];
                fp->nbr_dist[ p[0] ] = nbd;
                p[ nbr ] = p[1];
                p[1] = fp->neighbors[ p[0] ];
                
                p ++;
                np --;
        }
         
        /* no more neighbors, terminate */
        fp->neighbors[ p[0] ] = p[0];
        fp->nbr_dist[ p[0] ] = MAX_DISTANCE;

        for (i = 0; i < fp->npoints; ++i) {
                fp->where[ fp->points[i] ] = i;
        }
         
        return fp;
}

/**
 * find the NN of a given point 
 */
static inline void
fastpair_findnn(struct fastpair *fp, int p) {
        int i, first_nbr;
        
        if (fp->npoints == 1) {
                fp->neighbors[p] = p;
                fp->nbr_dist[p] = MAX_DISTANCE;
                return ;
        }

        first_nbr = 0;

        if (p == fp->points[first_nbr]) {
                first_nbr = 1;
        }

        fp->neighbors[p] = fp->points[first_nbr];
        fp->nbr_dist[p] = fastpair_distance(fp, p, fp->neighbors[p]);

        for (i = first_nbr + 1; i < fp->npoints; ++i) {
                int q = fp->points[i];
                if (q != p) {
                        double d = fastpair_distance(fp, p, q);
                        if (d < fp->nbr_dist[p]) {
                                fp->nbr_dist[p] = d;
                                fp->neighbors[p] = q;
                        }
                }
        }
}

static inline void
fastpair_add(struct fastpair *fp, int p) {
        fastpair_findnn(fp, p);
        fp->points[ fp->where[p] = fp->npoints ++ ] = p; 
}

static inline void
fastpair_delete(struct fastpair *fp, int p) {
        int i, q = fp->where[p];

        fp->npoints --;

        fp->where[ fp->points[q] = fp->points[ fp->npoints ] ] = q;

        for (i = 0; i < fp->npoints; ++i) {
                if (fp->neighbors[ fp->points[i] ] == p) {
                        fastpair_findnn(fp, fp->points[i]);
                }
        }
}

/**
 * Find the closest pair (a, b), and store the closes distance in dist.
 *
 * if error happened, return -1, else 0
 */
int 
fastpair_find(const struct fastpair *fp, int *a, int *b, double *dist) {
        double d;
        int r, i;
        
        if ( fp->npoints < 2 ) {
                /* !! */
                return -1;
        }

        d = fp->nbr_dist[ fp->points[0] ];
        r = 0;

        for (i = 1; i < fp->npoints; ++i) {
                const double t = fp->nbr_dist[ fp->points[i] ];
                if ( t < d) {
                        d = t;
                        r = i;
                }
        }

        *a = fp->points[r];
        *b = fp->neighbors[ *a ];
        *dist = d;

        return 0;
}

/** cgdl API implementation **/

struct cgdl {
        int kw;
        int kl;
        double a;

        struct wmat *wm;
        struct idict *dict;
        struct fastpair *fp;
};


static inline void
_cgdl_heaptify(int *indices, double *values, int k) {
        int maxi = 0;
        double maxv = values[0];
        
        for (i = 1; i < k; ++i) {
                if (values[i] > maxv) {
                        maxi = i;
                        maxv = values[i];
                }
        }

        /* swap 0<->maxi */
        indicies[maxi] = indicies[0];
        indicies[0] = maxi;
        values[maxi] = values[0];
        values[0] = maxv;
}

static inline void
_cgdl_smallest_k(const double *data, int sz, int *indices, double *values, int k) {
        int i, maxi;
        double maxv;
        
        for (i = 0; i < k; ++i) {
                indices[i] = i;
                values[i] = data[i];
        }

        _cgdl_heaptify(indices, values, k);

        for (i = k; i < sz; ++i) {
                if (data[i] < values[0]) {
                        values[0] = data[i];
                        _cgdl_heaptify(indices, values, k);
                }
        }
}

/* construct KNN matrix from distance mat
 * using simplified min-heap.
 */
static struct wmat*
_cgdl_knn(const struct mat *dm, int k) {
        struct wmat *wm = NULL;
        int i;
        
        if (dm == 0 || (k <= 0 || k > dm->rows)) {
                return NULL;
        }

        wm = wmat_new(dm->rows, k);

        if (wm == NULL) {
                return NULL;
        }

        for (i = 0; i < dm->rows; ++i) {
                _cgdl_smallest_k(dm->data + i*dm->cols, dm->cols, wm->i + i*k, wm->d + i*k, k);
        }
        
        return wm;
}

/* Just one link operation.
 */
static struct idict*
_cgdl_link(const struct wmat *wm, int k) {
        struct idict *dict = NULL; 
        int i, j, err = 0;
        int *labels = NULL;

        labels = malloc(sizeof(int) * wm->rows);

        if (labels == NULL) {
                goto _DOOR;
        }

        err = wmat_link(wm, k, labels);

        if (err != 0) {
                /* !! Exception */
                goto _DOOR;
        }
        
        dict = idict_new(0);

        if (dict == NULL) {
                goto _DOOR;
        }

        /* construct idicit from labels */
        for (i = 0; i < wm->rows; ++i) {
                idict_add(dict, labels[i], i);
        }

 _DOOR:        
        if (labels) {
                free(labels);
        }
        
        return dict;
}

struct cgdl*
cgdl_new(void) {
        struct cgdl *gdl = malloc(sizeof(*gdl));

        if (gdl != NULL) {
                /* default values for GDL */
                gdl->kw = 20;
                gdl->kl = 4;
                gdl->a  = 1.0;

                gdl->wm = NULL;
                gdl->fp = NULL;
                gdl->dict = NULL;
        }
        return gdl;
}

void
cgdl_free(struct cgdl **gdl) {
        if (gdl && (*gdl)) {
                struct cgdl *t = *gdl;

                if (t->dict) {
                        idict_free(&(t->dict));
                }

                if (t->wm) {
                        wmat_free(&(t->wm));
                }
                
                if (t->fp) {
                        fastpair_free(&(t->fp));
                }

                free(t);

                *gdl = NULL;
        }
}

int
cgdl_init(struct cgdl *gdl, const struct mat *dm) {
        struct wmat *wm = NULL;
        struct idict *dict = NULL;
        struct fastpair *fp = NULL;
        
        wm = _cgdl_knn(dm, gdl->kw);

        if (wm == NULL) {
                goto _DOOR;
        }

        wmat_sort(wm, WMAT_SORT_BY_D);
        
        dict = _cgdl_link(wm, gdl->kl);

        if (dict == NULL) {
                goto _DOOR;
        }

        wmat_sort(wm, WMAT_SORT_BY_I);
        
        wmat_normalize(wm, gdl->kw, gdl->a);
        
        fp = fastpair_new(dict, wm);

        if (fp == NULL) {
                gotot _DOOR;
        }
        
        gdl->wm = wm;
        gdl->dict = dict;
        gdl->fp = fp;
        return 0;

 _DOOR:
        if (wm) {
                wmat_free(&wm);
        }

        if (idict) {
                idict_free(&dict);
        }

        if (fp) {
                fastpair_free(&fp);
        }
        
        return -1;
}

int
cgdl_num(const struct cgdl *gdl) {
        if (gdl == NULL || gdl->cv == NULL) {
                return -1;
        }
        return gdl->dict->vnum;
}

double
cgdl_merge(struct cgdl *gdl) {
        double aff = .0;

        int a, b, err = 0;

        err = fastpair_find(gdl->fp, &a, &b, &aff);

        if (err != 0) {
                /* !! Exception */
                return MAX_DISTANCE;
        }

        if (a > b) {
                int t = a;
                a = b;
                b = t;
        }

        idict_merge_keep(gdl->dict, a, b);

        fastpair_delete(gdl->fp, b);
        fastpair_update(gdl->fp, a);
        
        return aff;
}

int
cgdl_labels(const struct cgdl *gdl, int *labels) {
        int i, j, label;
        const struct idict *dict = gdl->dict;

        for (i = 0; i < gdl->wm->rows; ++i) {
                labels[i] = -1;
        }
        
        label = 0;
        for (i = 0; i < dict->size; ++i) {
                if (dict->pvec[i] != NULL) {
                        const int isize = IVEC_SIZE(dict->pvec[i]);
                        const int *indices = IVEC_DATAPTR(dict->pvec[i]);
                        for (j = 0; j < isize; ++j) {
                                labels[ indices[j] ] = label;
                        }
                        label++;
                }
        }

        return 0;
}