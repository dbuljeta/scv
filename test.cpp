
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define IMAGE_H 720
#define IMAGE_W 1280

// https://www.learnopencv.com/install-opencv3-on-ubuntu/
// g++ -std=c++11 removeRedEyes.cpp `pkg-config --libs --cflags opencv` -o removeRedEyes

int main ()
{
  Rect warpPerspCrop(0, 400, IMAGE_W, IMAGE_H - 400);
  Mat left = imread("left.bmp", IMREAD_COLOR);
  Mat leftUnd;
  Mat leftWarped;
  Mat right = imread("right.bmp", IMREAD_COLOR);
  Mat rightUnd;
  Mat rightWarped;
  Mat front = imread("front.bmp", IMREAD_COLOR);
  Mat frontUnd;
  Mat frontWarped;
  Mat back = imread("back.bmp", IMREAD_COLOR);
  Mat backUnd;
  Mat backWarped;
  
  
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

  imshow("undistortedLeft", leftUnd);
  leftUnd = leftUnd(warpPerspCrop);
  rightUnd = rightUnd(warpPerspCrop);
  frontUnd = frontUnd(warpPerspCrop);
  backUnd = backUnd(warpPerspCrop);

  warpPerspective(leftUnd,leftWarped,M,left.size()); 
  warpPerspective(rightUnd,rightWarped,M,left.size()); 
  warpPerspective(frontUnd,frontWarped,M,left.size()); 
  warpPerspective(backUnd,backWarped,M,left.size()); 
  
  imshow("leftWarped", leftWarped);
  imshow("rightWarped", rightWarped);
  imshow("frontWarped", frontWarped);
  imshow("backWarped", backWarped);

  waitKey(0);
  return 0;
}
