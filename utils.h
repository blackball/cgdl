/**
 * Extract distance matrix from an image.
 * This was just used for testing.
 */

#ifndef UTILS_H
#define UTILS_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <math.h> // for sqrt
#include <assert.h>

static struct {
        int size, alloc;
        CvPoint *pts;
} s_ptsvec;

static void
ptsvec_init() {
        s_ptsvec.pts = malloc(sizeof(s_ptsvec.pts[0]) * 1000);
        assert(s_ptsvec.pts);
        s_ptsvec.size = 0;
        s_ptsvec.alloc = 1000;
}

static void
ptsvec_append(CvPoint pt) {
        if (!(s_ptsvec.size < s_ptsvec.alloc)) {
                s_ptsvec.pts = realloc(s_ptsvec.pts, sizeof(s_ptsvec.pts[0]) * (s_ptsvec.alloc + 1000));
                s_ptsvec.alloc += 1000;
                assert(s_ptsvec.pts);
        }
        
        s_ptsvec.pts[ s_ptsvec.size++ ] = pt;
}

static void
ptsvec_release() {
        free(s_ptsvec.pts);
        s_ptsvec.size = 0;
        s_ptsvec.alloc = 0;
}

static int
ptsvec_size() {
        return s_ptsvec.size;
}

static double 
pts_dist(const CvPoint p0, const CvPoint p1) {
        return sqrt((double)((p0.x - p1.x) * (p0.x - p1.x) + (p0.y - p1.y) * (p0.y - p1.y)));
}

static void
ptsvec_wtf(const char *dmname, const char *pixname) {
        FILE *f = fopen(pixname, "w");
        const int size = s_ptsvec.size;
        double *dm = 0;
        int i = 0, j = 0;
        
        assert(f);

        for (; i < size; ++i) {
                fprintf(f, "%d %d\n", s_ptsvec.pts[i].x, s_ptsvec.pts[i].y);
        }

        fclose(f);
        
        dm = malloc(sizeof(dm[0]) * size * size);
        assert(dm);

        for (i = 0; i < size; ++i) {
                for (j = 0; j < size; ++j) {
                        dm[i * size + j] = pts_dist(s_ptsvec.pts[i], s_ptsvec.pts[j]);
                }
        }
        
        f = fopen(dmname, "w");
        assert(f);

        fprintf(f, "%d %d\n", size, size);
        for (i = 0; i < size; ++i) {
                for (j = 0; j < size; ++j) {
                        fprintf(f, "%lf ", dm[i * size +j]);
                }
                fprintf(f, "\n");
        }
        
        fclose(f);
        free(dm);
}

#define is_foreground(p) (p != 0)

/* Extract the foreground pixel's location to form a distance matrix.
 * This will generate two files: the distance matrix file, the foreground
 * pixels file.
 */
static int
extract_dm(const char *imgname, const char *dmname, const char *pixname) {
        IplImage *img = cvLoadImage(imgname, 0);
        int i, j, ws;
        unsigned char *data;
        assert(img);
        
        ptsvec_init();
        ws = img->widthStep;
        data = img->imageData;
        for (i = 0; i < img->height; ++i) {
                for (j = 0; j < img->width; ++j) {
                        if ( is_foreground(data[i*ws + j]) ) {
                                ptsvec_append(cvPoint(j, i));
                        }
                }
        }

        assert( ptsvec_size() > 0 );

        ptsvec_wtf(dmname, pixname);
        
        ptsvec_release();
        cvReleaseImage(&img);
        return 0;
}

#ifndef ABS
#define ABS(v) ((v) > 0 ? (v) : -(v))
#endif

static CvScalar s_colors[16] = {
        {255, 255, 255, 0},
        {255, 0, 255, 0},
        {255, 255, 0, 0},
        {0, 255, 255, 0},
        {0, 0, 255, 0},
        {255, 0, 0, 0},
        {0, 255, 0, 0},
        {50, 50, 255, 0},
        {255, 50, 255, 0},
        {20, 255, 80, 0},
        {100, 100, 255, 0},
        {90, 255, 0, 0},
        {33, 99, 15, 0},
        {200, 90, 0, 0},
        {99, 0, 200, 0},
        {100, 25, 85, 0},
};

static CvScalar
colors_get(int i) {
        return s_colors[ i & 0x0f ];
}

static void
show_result_in_image(const char *imgname, const char *posname, const char *labelname) {
        IplImage *img = cvLoadImage(imgname, 1); // load color image
        IplImage *draw = cvCreateImage(cvGetSize(img), 8, 3);
        FILE *f;
        int i, j;
        int *labels;
        assert(img && draw);

        f = fopen(posname, "r");
        assert(f);
        
        ptsvec_init();
        
        while (2 == fscanf(f, "%d %d", &i, &j)) {
                ptsvec_append(cvPoint(i, j));
        }

        fclose(f);
        
        labels = malloc(sizeof(labels[0]) * ptsvec_size());
        assert(labels);
        
        f = fopen(labelname, "r");
        assert(f);

        j = ptsvec_size();
        for (i = 0; i < j; ++i) {
                if (1 != fscanf(f, "%d", &labels[i])) {
                        assert(0);
                }
        }

        cvSetZero(draw);
        // show result
        j = ptsvec_size();
        for (i = 0; i < j; ++i) {
                cvCircle(draw, s_ptsvec.pts[i], 2, colors_get(labels[i]), 2, CV_AA, 0);
        }

        cvNamedWindow("original", 1);
        cvNamedWindow("result", 1);
        cvShowImage("original", img);
        cvShowImage("result", draw);
        cvWaitKey(0);
        cvDestroyWindow("original");
        cvDestroyWindow("result");
        
        cvSaveImage("./data/result.bmp", draw, 0);

        free(labels);
        fclose(f);
        ptsvec_release();
        cvReleaseImage(&img);
        cvReleaseImage(&draw);
}

#endif
