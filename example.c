/**
 * A simple usgae demo for CGDL.
 *
 * @blackball
 */

#include "mat.h"
#include "cgdl.h"
#include <stdio.h>

static void
demo(void) {
        /* target cluster number */
        const int T = 10;
        int ret = 0;

        int *labels = NULL;
        struct mat *m = mat_load("");


        if (m == NULL) {
                /* load distance mat failed! */
                return ;
        }

        struct cgdl *gdl = cgdl_new();

        if (gdl == NULL) {
                return ;
        }

        /* get weight matrix and initial clusters */
        ret = cgdl_init(gdl, m);

        if (ret != 0) {
                goto _DOOR;
        }

        while ( cgdl_num(gdl) > T ) {
                double aff = cgdl_merge(gdl);
                printf("The affinity of merged clusters is: %lf\n", aff);
        }

        labels = malloc(sizeof(labels[0]) * m->cols);

        if (labels == NULL) {
                goto _DOOR;
        }

        /* get the class labels */
        cgdl_labels(gdl, labels);

        /* The you get the labels, do whatever you what next  */

 _DOOR:
        if (m) {
                mat_free(&m);
        }

        if (gdl) {
                cgdl_free(&gdl);
        }

        if (labels) {
                free(labels);
        }
}

int
main(int argc, char *argv[]) {
        demo();
        return 0;
}

