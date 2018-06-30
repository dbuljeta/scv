
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

#define IMAGE_H 720
#define IMAGE_W 1280

// https://www.learnopencv.com/install-opencv3-on-ubuntu/
// g++ -std=c++11 test.cpp `pkg-config --libs --cflags opencv` -o test


static void setAlpha(Mat* img)
{
  cvtColor(*img, *img, COLOR_BGR2BGRA);
  for (int y = 0; y < (*img).rows; ++y)
  {
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
    
}
void fourPointTransform(Mat* img)
{
  
  int i,j ;
  Mat warped;
  Point2f src[4];
  Point2f dst[4];
  int maxHeight,maxWidth;

  //Points
  src[0] = Point2f(420, 380);
  src[1] = Point2f(870, 370);
  src[2] = Point2f(1260, 500);
  src[3] = Point2f(20, 515);

  maxWidth = 1080;
  maxHeight = 260;
  
  dst[0] = Point2f(0,0);
  dst[1] = Point2f(maxWidth - 1,0);
  dst[2] = Point2f(maxWidth - 1, maxHeight -1);
  dst[3] = Point2f(0, maxHeight -1);  
  Mat M (2, 4, CV_32FC1);
  M = getPerspectiveTransform(src, dst);
  
  for (i = 0; i< 4 ; i++)
  {
    // cout<<endl;
    for (j = 0; j< 2 ; j++)
    {
      // cout <<M.at<float>(i,j)<<" ";
      // cout <<"wads";
    }
  }
  Mat draw = (*img).clone();

  // resolution in warpPerspective transformation defines the ratio betwen elements, this resolution depends of number of charts.
  warpPerspective(draw, (*img), M, Size (1000, 250),INTER_LINEAR,0);

  //Draw region for warpPerspective
  // vector<Point> not_a_rect_shape;
  //   not_a_rect_shape.push_back(Point(420, 380));
  //   not_a_rect_shape.push_back(Point(870, 370));
  //   not_a_rect_shape.push_back(Point(1260, 500));
  //   not_a_rect_shape.push_back(Point(20, 515));
  // const Point* point = &not_a_rect_shape[0];
  // int n = 4;
  // polylines(draw, &point, &n, 1, true, Scalar(0, 255, 0), 3, 8);
  // imwrite("draw.jpg", draw);
  // imshow("fourPointsWarped",*img);
  // waitKey(0);

}
void mergeImagesFourPoints(Mat* stitched, Mat front, Mat right, Mat back, Mat left, Mat car)
{
  int i,j;

  //Resolution of every warped image is 1000x250/ 250x1000 depending of is it a side(250x1000) or front/back image(1000x250)

  //right image stitching on stitched image->all pixels
  for (j = 0; j<1000;j++)
  for(i = 0; i<250;i++)
  {
    (*stitched).at<cv::Vec3b>(j,i+405) = (right).at<cv::Vec3b>(j,i);
    //x offset on big (stitched) image for pasting right image pixels 
  }

  // left image stitching on stitched image->all pixels
  // yoffset = 155 
  for (j = 0; j<1000;j++)
  for(i = 0; i<250;i++)
  {
    (*stitched).at<cv::Vec3b>(j+155,i) = (left).at<cv::Vec3b>(j,i);
  }

  //front image stitching on stitched image->all pixels
  //valid x (width) pixels with whole height (240-900)
  for (j = 240; j<900;j++)
  for(i = 0; i<250;i++)
  { 
    (*stitched).at<cv::Vec3b>(i,j-240) = (front).at<cv::Vec3b>(i,j);
    //-240 saving on stitched image starts at zero pixel 
  }

  //back image stitching on stitched image->all pixels
  //valid x (width) pixels with whole height (110-770)
  for (j = 110; j<770;j++)
  for(i = 0; i<250;i++)
  {
      (*stitched).at<cv::Vec3b>(i+930,j-110) = (back).at<cv::Vec3b>(i,j);  
      //-110 saving on stitched image starts at zero pixel 
  }


  imwrite("stitched1.png",*stitched);
  imshow("stitched", *stitched);
  waitKey(0);
  
}


void mergeImages (Mat* stitched, Mat front, Mat right, Mat back, Mat left, Mat car)
{
  int i,j;

  //frontImage iterating and pasting pixels onto stitched image
  // for (i = 0; i<1000;i++)
  // for(j = 0; j<250;j++)
  // {
  //   cout <<"i,j = "<< i,j;
  //   cout << endl;
  //   (*stitched).at<cv::Vec4b>(i,j) = (front).at<cv::Vec4b>(i,j);
  // }
  // for (i = 0; i < 490; ++i)
  // {
  //   for (j = i; j < 1280 - i; ++j)
  //   {
  //     (*stitched).at<cv::Vec4b>(i,j) = (front).at<cv::Vec4b>(i,j);
  //   }
  // }

  // // //leftImage iterating and pasting pixels onto stitched image
  // for (i = 0; i < 490; ++i)
  // {
  //   for (j = i; j < 1280-i; ++j)
  //   {
  //     (*stitched).at<cv::Vec4b>(j,i) = (left).at<cv::Vec4b>(j,i);
  //   }
  // }
  
  // //backImageIterataion
  // for (i = 790; i < 1280; ++i)
  // {
  //   for (j = 1280 - i; j < i; ++j)
  //   {
  //     (*stitched).at<cv::Vec4b>(i,j) = (back).at<cv::Vec4b>(i - 560 ,j);
  //   }
  // }
  
  // //rightImageIteration
  //  for (i = 790; i < 1280; ++i)
  // {
  //   for (j = 1279-i; j < i; ++j)
  //   {
  //     (*stitched).at<cv::Vec4b>(j,i) = (right).at<cv::Vec4b>(j,i-560);
  //   }
  // }

  // imshow("stitched", *stitched);
  // imwrite("stitched.png",*stitched);
  // imwrite("left.png",left);
  // imwrite("right.png",right);
  // imwrite("front.png",front);
  // imwrite("back.png",back);
  
}


int main ()
{
 
  Mat left = imread("paintLinedPictures/leftPaint.bmp", IMREAD_COLOR);
  Mat leftUnd;
  Mat right = imread("paintLinedPictures/rightPaint.bmp", IMREAD_COLOR);
  Mat rightUnd;
  Mat front = imread("paintLinedPictures/frontPaint.bmp", IMREAD_COLOR);
  Mat frontUnd;
  Mat back = imread("paintLinedPictures/backPaint.bmp", IMREAD_COLOR);
  Mat backUnd;
  Mat car = imread ("blueCarResized.png", IMREAD_COLOR);
  Mat stitched(1200, 660, CV_8UC3, Scalar(0, 0, 0));  
  // setAlpha(&stitched);
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

  fourPointTransform(&leftUnd);  
  fourPointTransform(&rightUnd); 
  fourPointTransform(&frontUnd); 
  fourPointTransform(&backUnd); 

  rotate (leftUnd,leftUnd,ROTATE_90_COUNTERCLOCKWISE);
  rotate (rightUnd, rightUnd, ROTATE_90_CLOCKWISE);
  rotate(backUnd, backUnd, ROTATE_180);
  imwrite("leftUnd.png",leftUnd);
  imwrite("rightUnd.png",rightUnd);
  imwrite("frontUnd.png",frontUnd);
  imwrite("backUnd.png",backUnd);
  
  // setAlpha(&left);
  // setAlpha(&right);
  // setAlpha(&front);
  // setAlpha(&back);

  // mergeImages(&stitched, front, right, back, left, car);
  mergeImagesFourPoints(&stitched, frontUnd, rightUnd, backUnd, leftUnd, car);

  return 0;
}