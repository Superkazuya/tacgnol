#include <stdlib.h>
#include "blockdct.h"
#include "iEBMA.h"

#ifndef CHANNEL_NUM
#define CHANNEL_NUM 1
#endif

#define MASK 2


int
main(int argc, const char *argv[])
{
  int ret_val = EXIT_FAILURE;
  /*Error Handling*/
  if(argc < 3)
    goto NO_ENOUGH_ARG;
  IplImage* img_target	= cvLoadImage(argv[1], 0); //Load the image as is
  IplImage* img_anchor	= cvLoadImage(argv[2], 0); //Load the image as is
  IplImage* img_pred	= cvCreateImage(cvGetSize(img_target), img_target->depth, img_target->nChannels);
  IplImage* img_diff	= cvCreateImage(cvGetSize(img_target), IPL_DEPTH_32F, img_target->nChannels);
  IplImage* img_index	= cvCreateImage(cvGetSize(img_target), IPL_DEPTH_32S, img_target->nChannels);
  //skip cropping
  /*Error Handling*/
  if(!img_target || !img_anchor)
    goto WRONG_IMAGE;

  iebma(img_anchor, img_target, img_pred, NULL, 16);
  print_PSNR(img_pred, img_anchor);

    if(!cvSaveImage("predicted.png", img_pred, 0))
      printf("Error occur while saving image.");
  cvNamedWindow ("Tacgonl", CV_WINDOW_AUTOSIZE);
  cvShowImage	  ("Tacgonl", img_pred);
  cvWaitKey(0);

  //printf("CHANN_NO: %d,\nIS UCHAR: %d\n\n", img_anchor->nChannels, img_anchor->depth == IPL_DEPTH_8U);
  //1
  printf("Direct Method: \n");
  print_VARIANCE(img_anchor);
  cvBlkDCT_JPEG(img_anchor, img_index, 1);
  printf("Non-zero Coefficient: %d\n\n", cvCountNonZero(img_index));

  printf("Diff Method: \n");
  cvSub(img_anchor, img_target, img_diff, NULL);
  print_VARIANCE(img_diff);
  cvBlkDCT_JPEG(img_diff, img_index, 1);
  printf("Non-zero Coefficient: %d\n\n", cvCountNonZero(img_index));

  printf("EBMA Residual Method: \n");
  cvSub(img_anchor, img_pred, img_diff, NULL);
  print_VARIANCE(img_diff);
  cvBlkDCT_JPEG(img_diff, img_index, 1);
  printf("Non-zero Coefficient: %d\n", cvCountNonZero(img_index));

 
  ret_val = EXIT_SUCCESS;

WRONG_IMAGE:
  cvReleaseImage(&img_index);
  cvReleaseImage(&img_diff);
  cvReleaseImage(&img_pred);
  cvReleaseImage(&img_anchor);
  cvReleaseImage(&img_target);
NO_ENOUGH_ARG:
  exit(ret_val);
}
