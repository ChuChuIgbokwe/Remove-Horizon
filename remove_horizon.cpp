#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <iostream>



using namespace cv;
using namespace std;

Mat equalizeIntensity(const Mat& inputImage)
{
    if(inputImage.channels() >= 3)
    {
        Mat ycrcb;
        cvtColor(inputImage,ycrcb,CV_BGR2YCrCb);

        vector<Mat> channels;
        split(ycrcb,channels);

        equalizeHist(channels[0], channels[0]);

        Mat result;
        merge(channels,ycrcb);
        cvtColor(ycrcb,result,CV_YCrCb2BGR);

        return result;
    }
    else
    {   Mat equalize;
        equalizeHist(inputImage, equalize); //equalize the histogram
        return equalize;
    }
}

Mat gaussian_blur(const Mat& inputImage){
    Mat blurred;
        for (int i=1; i<31; i=i+2)
       {
          // Gaussian smoothing
          GaussianBlur( inputImage, blurred, Size( i, i ), 0, 0 );
        }
    return blurred;
}



int main(int argc, char** argv )
    {
    // VideoCapture cap("field_trees.mp4"); // open the video file for reading
    // VideoCapture cap("dynamic_test.mp4"); // open the video file for reading
    // VideoCapture cap("falcon_landing.mp4"); // open the video file for reading
        VideoCapture cap(argv[1]);
    if ( !cap.isOpened() )  // if not success, exit program
    {
         cout << "Cannot open the video file" << endl;
         return -1;
    }

    //create window
    namedWindow("Output",CV_WINDOW_AUTOSIZE);

    //Create trackbar to change threshold value
    int thresh_value = 150;
    createTrackbar("Threshold Value", "Output", &thresh_value, 255);

    //Create trackbar for gaussian blurring
    int option = 0;
    createTrackbar("0 : Multiple Gaussian blurs (Slow) \n1 : One gaussian blurs(Fast)", "Output", &option,1);
    //create trackbar for THRESH_OTSU
    int choice = 0;
    createTrackbar("0 : THRESH_BINARY\n1 : THRESH_BINARY| CV_THRESH_OTSU", "Output", &choice,1);

    //create trackbar for background
    int colour_option = 0;
    createTrackbar("0 :Turn Sky Black\n1 :Turn Sky White", "Output", &colour_option,1);


    while (1)
    {
        Mat frame;

        bool bSuccess = cap.read(frame); // read a new frame from video

         if (!bSuccess) //if not success, break loop
        {
                 cout << "Cannot read the frame from video file" << endl;
                 break;
        }

        Mat gray, equ, res, blur,dst,thresh,eroded,dilated,kernel;
        //Equalize histogram of the frame
        equ = equalizeIntensity(frame);
        if (option == 0){
            blur = gaussian_blur(equ);
        }
        else{
           GaussianBlur( equ, blur, Size( 5, 5 ), 0, 0 );
        }
        //convert to grayscale
        cvtColor(blur,gray, CV_BGR2GRAY);
        //Threshold Image
        if (choice == 0){
            threshold(gray,thresh,thresh_value,255,THRESH_BINARY);
        }
        else{
           threshold(gray,thresh,thresh_value,255,THRESH_BINARY| CV_THRESH_OTSU);
        }

        kernel = getStructuringElement(0,Size(5,5) ,Point(3,3));
        //Morphological opening
        erode(thresh,eroded,kernel);
        dilate(eroded,dilated,kernel);

        //Find the contours.
        vector<vector<Point> > contours; // Vector for storing contour
        Mat contourOutput = dilated.clone();
        findContours( contourOutput, contours, CV_RETR_TREE, CV_CHAIN_APPROX_NONE );

        //Draw the contours
        Mat contourImage(frame.size(), CV_8UC3, Scalar(0,0,0));
        Scalar colours;
        if (colour_option == 0){
            colours = cv::Scalar(0, 0, 0);
        }
        else{
            colours = cv::Scalar(255, 255, 255);
        }

        for (size_t idx = 0; idx < contours.size(); idx++) {
            drawContours(frame, contours, idx, colours,-1);
        }

        imshow("Output", frame);

       if (waitKey(30) == 27) //wait for 'esc' key press for 30 ms. If 'esc' key is pressed, break loop
        {
                cout << "esc key is pressed by user" << endl;
                break;
        }
    }
    return 0;
}

