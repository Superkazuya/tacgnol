#include <stdio.h>
#include <math.h>
#include "cv.h"
#include "highgui.h"

#ifndef BLK_H
#define BLK_H 8
#endif
#ifndef BLK_W
#define BLK_W 8
#endif

enum {DCT_FORWARD, DCT_INVERSE};

extern void  cvBlkDCT(const IplImage*, IplImage*, int);
extern void  cvBlkIDCT(IplImage*, IplImage*);

extern void  cvBlkDCT_JPEG(const IplImage*, IplImage*, float);
extern void  cvBlkIDCT_JPEG(IplImage*, IplImage*, float);
extern float print_PSNR(const IplImage*, const IplImage*);
extern float print_VARIANCE(const IplImage*);
