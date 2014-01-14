/**
 * A simple usgae demo for CGDL.
 *
 * @blackball
 */

#include "mat.h"
#include "cgdl.h"
#include <stdio.h>
#include <stdlib.h>

#if defined (NO_PUTS)
#define PUTS(s)
#else
#define PUTS(s) puts(s)
#endif

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
        
        struct mat_t *m = 0;
        
        PUTS("Loading distance matrix...");
        m = mat_load(dmname);

        if (m == NULL) {
                /* load distance mat failed! */
                return ;
        }

        gdl = cgdl_new(4, 1);

        if (gdl == NULL) {
                return ;
        }

        /* get weight matrix and initial clusters */
        PUTS("Initializing GDL...");
        ret = cgdl_init(gdl, m);

        if (ret != 0) {
                goto _DOOR;
        }

        PUTS("Clustering...");
        while ( cgdl_num(gdl) > T ) {
                double aff = cgdl_merge(gdl);
                printf("The affinity of merged clusters is: %lf, %d\n", aff, cgdl_num(gdl));
        }

        PUTS("Finished!");
        
        labels = malloc(sizeof(labels[0]) * m->rows);

        if (labels == NULL) {
                goto _DOOR;
        }
        
        /* get the class labels */
        cgdl_labels(gdl, labels);

        PUTS("Save labels...");
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

/* I simply draw some clusters in a black background image
 * and use the foreground coordinates (x, y) as features. And use
 * Euclidean distance to calculate the distance matrix. Then perform
 * *GDL* on the distance matrix, and draw the clusters using different
 * colors in a same size image.
 *
 * @NOTE You may need to tune the *GDL* paramemters in the demo(...) function
 * to get a good results. 
 */

#define USE_IMAGE

#if defined(USE_IMAGE)
#include "utils.h"
#endif

int
main(int argc, char *argv[]) {
        const char *imgname = "./data/clusters.bmp";
        const char *dmname = "./data/dm.txt";
        const char *posname = "./data/positions.txt";
        const char *lname = "./data/labels.txt";
        
#if defined(USE_IMAGE)
        PUTS("Prepareing the distance matrix...");
        extract_dm(imgname, dmname, posname);
#endif
        demo(dmname, lname);
        
#if defined(USE_IMAGE)
        show_result_in_image(imgname, posname, lname);
#endif
        return 0;
}
