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
        assert(f);

        for (; i < n; ++i) {
                fprintf(f, "%d\n", labels[i]);
        }
        
        fclose(f);
}

static void
demo(void) {
        /* target cluster number */
        const int T = 4;
        int ret = 0;

        int *labels = NULL;
        struct cgdl_t *gdl = NULL;

        struct mat_t *m = mat_load("dm-1.txt");

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
                printf("The affinity of merged clusters is: %lf\n", aff);
        }

        labels = malloc(sizeof(labels[0]) * m->rows);

        if (labels == NULL) {
                goto _DOOR;
        }

        /* get the class labels */
        cgdl_labels(gdl, labels);

        save_labels("labels.txt", labels, m->rows);
        /* Then you get the labels, do whatever you what next  */
        _print_labels(labels, m->cols);

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


#if defined(USE_IMAGE)
#include "utils.h"
#endif

int
main(int argc, char *argv[]) {
#if defined(USE_IMAGE)
        extract_dm("draw.bmp", "dm.txt", "positions.txt");
#endif
        demo();
#if defined(USE_IMAGE)
        show_result_in_image("draw.bmp", "positions.txt", "labels.txt");
#endif
        return 0;
}

