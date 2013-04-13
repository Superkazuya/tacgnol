#include "blockdct.h"

uchar Q_MASK[][BLK_H*BLK_W] = {{
1, 1, 1, 0, 0, 0, 0, 0, 
1, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0 
},{
1, 1, 1, 0, 0, 0, 0, 0, 
1, 1, 0, 0, 0, 0, 0, 0, 
1, 1, 0, 0, 0, 0, 0, 0, 
1, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0 
},{
1, 1, 1, 1, 1, 0, 0, 0, 
1, 1, 1, 1, 0, 0, 0, 0, 
1, 1, 1, 0, 0, 0, 0, 0, 
1, 1, 0, 0, 0, 0, 0, 0, 
1, 0, 0, 0, 0, 0, 0, 0, 
1, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0 
},{
1, 1, 1, 1, 1, 1, 1, 0, 
1, 1, 1, 1, 1, 1, 0, 0, 
1, 1, 1, 1, 1, 0, 0, 0, 
1, 1, 1, 1, 0, 0, 0, 0, 
1, 1, 1, 1, 0, 0, 0, 0, 
1, 1, 1, 0, 0, 0, 0, 0, 
1, 1, 0, 0, 0, 0, 0, 0, 
1, 0, 0, 0, 0, 0, 0, 0 
},{
   16, 11, 10, 16, 24, 40, 51, 61,
   12, 12, 14, 19, 26, 58, 60, 55,
   14, 13, 16, 24, 40, 57, 69, 56,
   14, 17, 22, 29, 51, 87, 80, 62,
   18, 22, 37, 56, 68, 109, 103, 77,
   24, 35, 55, 64, 81, 104, 113, 92,
   49, 64, 78, 87, 103, 121, 120, 101,
   72, 92, 95, 98, 112, 100, 103, 99
  }};

void
cvBlkDCT(const IplImage* IMG, IplImage* DCT, int mask)//mask = 0, 1, 2, 3
{
  int i, j;
  CvMat* qm = cvCreateMatHeader(BLK_H, BLK_W, CV_8UC1);
  cvInitMatHeader(qm, BLK_H, BLK_W, CV_8UC1, Q_MASK[mask], CV_AUTOSTEP);
  cvConvertScale(IMG, DCT, 1, 0);
  CvRect roi;

  for(i = 0; i*BLK_H+BLK_H <= IMG->height; i++)
    for(j = 0; j*BLK_W+BLK_W <= IMG->width; j++)
    {
      roi = cvRect(j*BLK_W, i*BLK_H, BLK_W, BLK_H);
      cvSetImageROI(DCT, roi);
      cvDCT(DCT, DCT, DCT_FORWARD);
      //cvCopy(DCT, DCT, qm); //masking
      cvMul(DCT, qm, DCT, 1);
    }

  cvResetImageROI(DCT);
  cvReleaseMat(&qm);
}

void
cvBlkIDCT(IplImage* DCT, IplImage* IMG) 
{
  int i, j;
  CvRect roi;
  IplImage* image = cvCloneImage(DCT);

  for(i = 0; i*BLK_H+BLK_H <= image->height; i++)
    for(j = 0; j*BLK_W+BLK_W <= image->width; j++)
    {
      roi = cvRect(j*BLK_W, i*BLK_H, BLK_W, BLK_H);
      cvSetImageROI(image,  roi);
      cvDCT(image, image, DCT_INVERSE);
    }

  cvResetImageROI(image);
  cvConvertScale(image, IMG, 1, 0);
  cvReleaseImage(&image);
}

void
cvBlkDCT_JPEG(const IplImage* IMG, IplImage* index, float scale)
  //index 32S
{
  int i, j;
  CvMat* qm = cvCreateMatHeader(BLK_H, BLK_W, CV_8UC1);
  cvInitMatHeader(qm, BLK_H, BLK_W, CV_8UC1, Q_MASK[4], CV_AUTOSTEP);
  IplImage* image = cvCreateImage(cvGetSize(IMG), IPL_DEPTH_32F, IMG->nChannels); //coefficients may be negative
  cvConvertScale(IMG, image, 1, 0);
  CvRect roi;

  for(i = 0; i*BLK_H+BLK_H <= IMG->height; i++)
    for(j = 0; j*BLK_W+BLK_W <= IMG->width; j++)
    {
      roi = cvRect(j*BLK_W, i*BLK_H, BLK_W, BLK_H);
      cvSetImageROI(image,    roi);
      cvSetImageROI(index,    roi);
      cvDCT(image, image, DCT_FORWARD);
      //quantization
      cvDiv(image, qm, index, 1/scale);
    }
  cvResetImageROI(index);
  cvReleaseImage(&image);
  cvReleaseMat(&qm);
}

void
cvBlkIDCT_JPEG(IplImage* index, IplImage* IMG, float scale)
{
  int i, j;
  CvRect roi;
  IplImage* DCT = cvCreateImage(cvGetSize(index), IPL_DEPTH_32F, index->nChannels); //coefficients may be negative
  CvMat*    qm	  = cvCreateMatHeader(BLK_H, BLK_W, CV_8UC1);
  cvInitMatHeader(qm, BLK_H, BLK_W, CV_8UC1, Q_MASK[4], CV_AUTOSTEP);

  for(i = 0; i*BLK_H+BLK_H <= index->height; i++)
    for(j = 0; j*BLK_W+BLK_W <= index->width; j++)
    {
      roi = cvRect(j*BLK_W, i*BLK_H, BLK_W, BLK_H);
      cvSetImageROI(index,  roi);
      cvSetImageROI(DCT,    roi);
      //dequantization
      cvMul(index, qm, DCT, scale);
      cvDCT(DCT, DCT, DCT_INVERSE);
    }

  cvResetImageROI(DCT);
  cvResetImageROI(index);
  cvConvertScale(DCT, IMG, 1, 0);
  cvReleaseMat(&qm);
  cvReleaseImage(&DCT);
}

float
print_PSNR(const IplImage* org, const IplImage* com)
{
  float psnr; 
  psnr = cvNorm(org, com, CV_L2, NULL);
  psnr *=psnr/(org->width*org->height);
  psnr = 10*log10((1<<16)/psnr);
  printf("PSNR: %f\n", psnr);
  return(psnr);
}

float 
print_VARIANCE(const IplImage* img)
{
  CvScalar sdv;
  float var;
  cvAvgSdv(img, NULL, &sdv, NULL);
  var = sdv.val[0];
  printf("Variance: %f\n", var*=var);
  return(var);
}
