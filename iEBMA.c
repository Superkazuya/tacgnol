#include "iEBMA.h"

void
iebma(IplImage* anchor, IplImage* target, IplImage* pred_img, CvMat* motion_vec, int range)
{
  int i, j, k, l;
  int H = anchor->height;
  int W = anchor->width;
  int top, left;
  CvRect roi, mmse_roi;
  for(i = 0; i*BLK_H+BLK_H <= H; i++)
    for(j = 0; j*BLK_W+BLK_W <= W; j++)
    {
      float mse;
      float mmse = 4*255;
      roi = cvRect(j*BLK_W, i*BLK_H, BLK_W, BLK_H);
      cvSetImageROI(anchor, roi);
      cvSetImageROI(pred_img, roi);
      for (k = -range; k <= range; k++) 
      {
	top = i*BLK_H + k;
	if(top+BLK_H > H)
	  break; //break from k loop
	if(top < 0)
	{
	  k -= top+1;
	  continue;
	}
	for (l = -range; l <= range; l++) 
	{
	  left = j*BLK_W + l;
	  if(left+BLK_W > W)
	    break;
	  if(left < 0)
	  {
	    l -= left+1;
	    continue;
	  }
	  roi = cvRect(left, top, BLK_W, BLK_H);
	  cvSetImageROI(target, roi);
	  mse = cvNorm(target, anchor, CV_L2, NULL);
	  if(mse < mmse)
	  {
	    mmse = mse;
	    mmse_roi = roi;
	    if(mmse < 0.0001)
	      break;
	  }
	}
	if(mmse < 0.0001)
	  break;
      } //traversed the search window
      cvSetImageROI(target, mmse_roi);
      cvCopy(target, pred_img, NULL);
      if(motion_vec)
      {
	CvScalar mv;
	mv.val[0] = mmse_roi.x-roi.x;
	mv.val[1] = mmse_roi.y-roi.y;
	cvSet2D(motion_vec, i, j, mv);
      }

    }
  cvResetImageROI(anchor);
  cvResetImageROI(pred_img);
  cvResetImageROI(target);
}
