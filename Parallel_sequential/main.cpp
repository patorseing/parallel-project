//
//  main.cpp
//  Parallel_sequential
//
//  Created by Napatchol Thaipanich on 11/21/17.
//  Copyright © 2017 Napatchol Thaipanich. All rights reserved.
//
// 1.    Miss Kanjanaporn Sumitdech 5888178 Section 2
// 2.    Miss Napatchol Thaipanich 5888205 Section 2
// 3.    Mr. Arnuphap Yupuech 5888236 Section 2


#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <time.h>

#define source "/Users/napatcholthaipanich/Documents/3rd/Parallel_sequential/Parallel_sequential/input.jpg"
#define result "/Users/napatcholthaipanich/Documents/3rd/Parallel_sequential/Parallel_sequential/output.jpg"

using namespace std;

int main(void) {
    double total;
    double Milli, Seconds, Minutes;
    clock_t start, stop;
    
    start = clock() * CLK_TCK;      //start the timer
    IplImage* img = cvLoadImage(source);
    cvShowImage("Example-In", img);
    IplImage* out = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
    cvSmooth(img, out, CV_GAUSSIAN, 5, 5);
    cvSaveImage(result, out);
    stop = clock() * CLK_TCK;        //stop the timer
    
    total = stop - start;        //the number of ticks from Begin to End
    Milli = total/1000;     //milliseconds from Begin to End
    Seconds = Milli/1000;   //seconds from Begin to End
    Minutes = Seconds/60;   //minutes from Begin to End
    
    
    if(Seconds < 1)
        printf("Time: %f ms\n", Milli );
    else if(Seconds == 1)
        printf("It took  1 second.");
    else if(Seconds > 1 && Seconds < 60)
        printf("Time: %f secs\n", Seconds );
    else if(Seconds >= 60)
        printf("Time: %f mins\n", Minutes );
    return 0;
}
