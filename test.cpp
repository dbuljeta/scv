
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define IMAGE_H 720
#define IMAGE_W 1280

// https://www.learnopencv.com/install-opencv3-on-ubuntu/
// g++ -std=c++11 test.cpp `pkg-config --libs --cflags opencv` -o test


void setAlpha(Mat* img)
{
  cvtColor(*img, *img, COLOR_BGR2BGRA);
  for (int y = 0; y < (*img).rows; ++y)
    for (int x = 0; x < (*img).cols; ++x)
    {
        cv::Vec4b & pixel = (*img).at<cv::Vec4b>(y, x);
        // if pixel is black
        if (pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0)
        {
            // set alpha to zero:
            pixel[3] = 0;
        }
    }
}

void mergeImages (Mat* stitched, Mat front, Mat right, Mat back, Mat left, Mat car)
{
  int i,j;

  //frontImage iterating and pasting pixels onto stitched image
  for (i = 0; i < 490; ++i)
  {
    for (j = i; j < 1280 - i; ++j)
    {
      (*stitched).at<cv::Vec4b>(i,j) = (front).at<cv::Vec4b>(i,j);
    }
  }

  // //leftImage iterating and pasting pixels onto stitched image
  for (i = 0; i < 490; ++i)
  {
    for (j = i; j < 1280-i; ++j)
    {
      (*stitched).at<cv::Vec4b>(j,i) = (left).at<cv::Vec4b>(j,i);
    }
  }
  
  //backImageIterataion
  for (i = 790; i < 1280; ++i)
  {
    for (j = 1280 - i; j < i; ++j)
    {
      (*stitched).at<cv::Vec4b>(i,j) = (back).at<cv::Vec4b>(i - 560 ,j);
    }
  }
  
  //rightImageIteration
   for (i = 790; i < 1280; ++i)
  {
    for (j = 1279-i; j < i; ++j)
    {
      (*stitched).at<cv::Vec4b>(j,i) = (right).at<cv::Vec4b>(j,i-560);
    }
  }

  //Try to align pictures from each view
// //leftImage iterating and pasting pixels onto stitched image
    // for (i = 0; i < 490; ++i)
    // {
    //   for (j = i; j < 1280-i; ++j)
    //   {
    //     (*stitched).at<cv::Vec4b>(j,i+350) = (left).at<cv::Vec4b>(j,i);
    //   }
    // }
    // //rightImageIteration
    // for (i = 790; i < 1280; ++i)
    //   {
    //   for (j = 1279-i; j < i; ++j)
    //   {
    //     (*stitched).at<cv::Vec4b>(j,i-350) = (right).at<cv::Vec4b>(j,i-560);
    //   }
    // }
    // //backImageIterataion
    // for (i = 790; i < 1280; ++i)
    // {
    //   for (j = 1280 - i; j < i; ++j)
    //   {
    //     (*stitched).at<cv::Vec4b>(i-50,j) = (back).at<cv::Vec4b>(i - 560 ,j);
    //   }
    // }
    // //frontImage iterating and pasting pixels onto stitched image
    // for (i = 0; i < 490; ++i)
    // {
    //   for (j = i; j < 1280 - i; ++j)
    //   {
    //     (*stitched).at<cv::Vec4b>(i+100,j) = (front).at<cv::Vec4b>(i,j);
    //   }
    // }
//


  // imshow("stitched", *stitched);
  imwrite("stitched.png",*stitched);
  imwrite("left.png",left);
  imwrite("right.png",right);
  imwrite("front.png",front);
  imwrite("back.png",back);
  
}


int main ()
{
  Rect warpPerspCrop(0, 400, IMAGE_W, IMAGE_H - 400);
  Mat left = imread("paintLinedPictures/leftPaint.bmp", IMREAD_COLOR);

  Mat leftUnd;
  Mat right = imread("paintLinedPictures/rightPaint.bmp", IMREAD_COLOR);
  Mat rightUnd;
  Mat front = imread("paintLinedPictures/frontPaint.bmp", IMREAD_COLOR);
  Mat frontUnd;
  Mat back = imread("paintLinedPictures/backPaint.bmp", IMREAD_COLOR);
  Mat backUnd;
  Mat car = imread ("blueCarResized.png", IMREAD_COLOR);
  Mat stitched(1280, 1280, CV_8UC3, Scalar(0, 0, 0));  
  setAlpha(&stitched);
  //init undistort rectify params
  Matx33d P (335.4360116970886, 0.0, 638.3853408401494 ,
               0.0, 335.6314521829435, 403.2844174394132,
                0, 0, 1);

  Mat map_x, map_y;
  //
  Matx33d KPComputed;
  //Camera params
  Matx33d K (335.4360116970886, 0.0, 638.3853408401494 , 0.0, 335.6314521829435, 403.2844174394132,  0, 0, 1);
        
  Vec4d D (-0.010624391932770951,0.0692322261552681, -0.018577927478565195, 0.0006725877509963431);

  fisheye::estimateNewCameraMatrixForUndistortRectify(K, D, left.size(),Matx33d::eye(), KPComputed, 1);
  //M get perspectiveTransform
  Point2f src[4];
  Point2f dst[4];
  src[0] = Point2f( 9,IMAGE_H);
  src[1] = Point2f( 1276,IMAGE_H);
  src[2] = Point2f( 0, 0);
  src[3] = Point2f( IMAGE_W, 0); 
  dst[0] = Point2f( 373,IMAGE_H);
  dst[1] = Point2f( 920,IMAGE_H);
  dst[2] = Point2f( 0, 0);
  dst[3] = Point2f( IMAGE_W, 0); 

  Mat M (2, 4, CV_32FC1);
  M = getPerspectiveTransform(src, dst);
  //working warp perspective
  // 
  map_x.create( left.size(), CV_32FC1 );
  map_y.create( left.size(), CV_32FC1 );
  fisheye::initUndistortRectifyMap(K, D, Matx33d::eye(), KPComputed, left.size(), CV_32FC1, map_x, map_y);
  remap(left, leftUnd, map_x, map_y, INTER_LINEAR);
  remap(right, rightUnd, map_x, map_y, INTER_LINEAR);
  remap(front, frontUnd, map_x, map_y, INTER_LINEAR);
  remap(back, backUnd, map_x, map_y, INTER_LINEAR);

  leftUnd = leftUnd(warpPerspCrop);
  rightUnd = rightUnd(warpPerspCrop);
  frontUnd = frontUnd(warpPerspCrop);
  backUnd = backUnd(warpPerspCrop);

  warpPerspective(leftUnd,left,M,left.size()); 
  warpPerspective(rightUnd,right,M,left.size());
  rotate (right, right, ROTATE_90_CLOCKWISE);
  warpPerspective(frontUnd,front,M,left.size());
  rotate (left,left,ROTATE_90_COUNTERCLOCKWISE);
  warpPerspective(backUnd,back,M,front.size()); 
  rotate(back, back, ROTATE_180);
  

  setAlpha(&left);
  setAlpha(&right);
  setAlpha(&front);
  setAlpha(&back);
  mergeImages(&stitched, front, right, back, left, car);
  // imshow("left", left);
  // imshow("right", right);
  // imshow("front", front);
  // imshow("back", back);

  waitKey(0);
  return 0;
}