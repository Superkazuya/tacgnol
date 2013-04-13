#include <stdlib.h>
#include <stdio.h>
#include "blockdct.h"

#define CHANNEL_NUM 1
#define QSCALE_NUM  6
#define SAVE 1

int
main(int argc, char** argv)
{
  int ret_val = EXIT_FAILURE;
  /*Error Handling*/
  if(argc < 2)
    goto NO_ENOUGH_ARG;
  IplImage* image	= cvLoadImage(argv[1], 0); //Load the image as is

  /*Error Handling*/
  if(!image|| image->nChannels != CHANNEL_NUM || image->width < BLK_W || image->height < BLK_H)
    goto WRONG_IMAGE;

  //Crop
  cvSetImageROI(image, cvRect(0,0,image->width/BLK_W*BLK_W,image->height/BLK_H*BLK_H));
  IplImage* img	    = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, image->nChannels);
  IplImage* img_out = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, image->nChannels);
  cvCopy(image, img, NULL);
  IplImage* dct_img = cvCreateImage(cvGetSize(image), IPL_DEPTH_32F, image->nChannels);
  IplImage* index = cvCreateImage(cvGetSize(image), IPL_DEPTH_32S, image->nChannels);

  float scale[] = {0.5, 1, 2, 4, 8, 16};
  int mask_no;    
  char filename[13]; //8.3
  for (mask_no = 0; mask_no < 4; mask_no++) 
  {
    cvBlkDCT(img, dct_img, mask_no);
    cvBlkIDCT(dct_img, img_out);

    print_PSNR(img, img_out);
#if SAVE
    if(!sprintf(filename, "Masked%d.png", mask_no) ||
    !cvSaveImage(filename, img_out, 0))
      printf("Error occur while saving image.");
#endif
    cvNamedWindow ("Tacgonl", CV_WINDOW_AUTOSIZE);
    cvShowImage	  ("Tacgonl", img_out);
    cvWaitKey(0);
  }

  for (mask_no = 0; mask_no < QSCALE_NUM; mask_no++) 
  {
    cvBlkDCT_JPEG(img, index, scale[mask_no]);
    cvBlkIDCT_JPEG(index, img_out, scale[mask_no]);
    print_PSNR(img, img_out);
#if SAVE
    if(!sprintf(filename, "Quantized%f.png", scale[mask_no]) ||
    !cvSaveImage(filename, img_out, 0))
      printf("Error occur while saving image.");
#endif
    cvNamedWindow ("Tacgonl", CV_WINDOW_AUTOSIZE);
    cvShowImage	  ("Tacgonl", img_out);
    cvWaitKey(0);
  }

  ret_val = EXIT_SUCCESS;

  cvReleaseImage(&index);
  cvReleaseImage(&dct_img);
  cvReleaseImage(&img_out);
  cvReleaseImage(&img);
WRONG_IMAGE:
  cvReleaseImage(&image);
NO_ENOUGH_ARG:
  exit(ret_val);
}
