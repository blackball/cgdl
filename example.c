/**
 * A simple usgae demo for CGDL.
 *
 * @blackball
 */

#include "mat.h"
#include "cgdl.h"
#include <stdio.h>
#include <stdlib.h>

static void
_print_labels(const int *labels, int n) {
        int i = 0;
        for (; i < n; ++i) {
                printf("%d ", labels[i]);
        }
        printf("\n");
}

static void
save_labels(const char *name, const int *labels, int n) {
        FILE *f = fopen(name, "w");
        int i = 0;

        for (; i < n; ++i) {
                fprintf(f, "%d\n", labels[i]);
        }
        
        fclose(f);
}

static void
demo(const char *dmname, const char *labelname) {
        /* target cluster number */
        const int T = 5;
        int ret = 0;

        int *labels = NULL;
        struct cgdl_t *gdl = NULL;

        struct mat_t *m = mat_load(dmname);

        if (m == NULL) {
                /* load distance mat failed! */
                return ;
        }

        gdl = cgdl_new(4, 1);

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
                printf("The affinity of merged clusters is: %lf, %d\n", aff, cgdl_num(gdl));
        }

        labels = malloc(sizeof(labels[0]) * m->rows);

        if (labels == NULL) {
                goto _DOOR;
        }

        /* get the class labels */
        cgdl_labels(gdl, labels);

        save_labels(labelname, labels, m->rows);
        /* Then you get the labels, do whatever you what next  */
        //_print_labels(labels, m->cols);

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

#define USE_IMAGE

#if defined(USE_IMAGE)
#include "utils.h"
#endif

int
main(int argc, char *argv[]) {
#if defined(USE_IMAGE)
        extract_dm("draw4.bmp", "dm.txt", "positions.txt");
#endif
        demo("dm.txt", "labels.txt");
#if defined(USE_IMAGE)
        show_result_in_image("draw4.bmp", "positions.txt", "labels.txt");
#endif
        return 0;
}

