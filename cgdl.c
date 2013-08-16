/**
 * Implementation of CGDL
 *
 * @blackball
 */

struct cgdl {
        int K;
        int A;
        struct wmat *wm; /* weight matrix */
        struct fastpair *fp;
        struct cluster_set *cset;
};

struct cgdl *
cgdl_new() {
        
}

int
cgdl_init(struct cgdl *cgdl, const struct mat *m) {
        /* caclulate knn matrix */

        /* calculate weight matrix */

        /* L link, disjoint knn graph to clusters */
        
}

int
cgdl_merge(struct cgdl *cgdl, int T) {

        /* find closest clusters and merge */

        while ( T < cgdl_cnum(cgdl) ) {
                int a, b;
                fastpair(&a, &b);
                fastpair_update(a);
                fastpair_delete(b);
                cgdl_merge_i(cgdl, a, b);
        }
        
}
