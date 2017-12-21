//
//  main.cpp
//  parallel
//
//  Created by Napatchol Thaipanich on 11/18/17.
//  Copyright © 2017 Napatchol Thaipanich. All rights reserved.
//
// 1.    Miss Kanjanaporn Sumitdech 5888178 Section 2
// 2.    Miss Napatchol Thaipanich 5888205 Section 2
// 3.    Mr. Arnuphap Yupuech 5888236 Section 2

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <mpi.h>

#define source "/Users/napatcholthaipanich/Documents/3rd/parallel/parallel/input.jpg"
#define result "/Users/napatcholthaipanich/Documents/3rd/parallel/parallel/output.jpg"

using namespace cv;

const int Mask[5][5] = {
    {1,4,7,4,1},
    {4,16,26,16,4},
    {7,26,41,26,7},
    {4,16,26,16,4},
    {1,4,7,4,1}
};
const int sumOfElementsInMask = 273;

Vec3b processPixel(int x, int y, cv::Mat * input, cv::Mat * output) {
    Point p = Point(x,y);
    Vec3b pixel = input->at<Vec3b>(p);
    
    for (int i = 0; i < 3; ++i) {
        int sum = 0;
        
        for (int j = -2; j <= 2; ++j) {
            for (int k = -2; k <= 2; ++k) {
                int x_idx = x+j;
                int y_idx = y+k;
                
                if (x_idx < 0 || x_idx > output->cols-1) {
                    x_idx = x;
                }
                
                if (y_idx < 0 || y_idx > output->rows-1) {
                    y_idx = y;
                }
                
                sum += input->at<Vec3b>(Point(x_idx,y_idx)).val[i] * Mask[j+2][k+2];
            }
        }
        int newValue = sum / sumOfElementsInMask;
        pixel.val[i] = newValue;
    }
    
    return pixel;
}

cv::Mat * GaussianBlur(cv::Mat * input){
    cv::Mat *in = input;
    cv::Mat *re = new Mat(in->rows, in->cols, CV_8UC3, 0.0);
    
    for (int i = 0; i < re->cols; ++i) {
        for (int j = 0; j < re->rows; ++j) {
            Point p = Point(i,j);
            Vec3b pixel = processPixel(i, j, in, re);
            re->at<Vec3b>(p) = pixel;
        }
    }
    return re;
}

int main(void) {
    double start, end;
    start = MPI_Wtime();
    // insert code here...
    int rank;
    int world;
    int Rows, Cols;
    int *numsend, *rowsPerPro, *displace;
    Mat in;
    Mat* re = new Mat();
    
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world);
    
    int slices = world -1;
    
    //std::cout << "OperCV Version " << CV_VERSION << " from "<< rank << std::endl;
    
    if (rank == 0) {
        
        if(world == 1) {
            fprintf(stderr, "%d should be running with at least 2 processes.\n", world);
            exit(EXIT_FAILURE);
        }
        
        in = imread(source, CV_LOAD_IMAGE_COLOR);
        Cols = in.cols;
        Rows = in.rows;
        re = new Mat(Rows, Cols, CV_8UC3);
        
        numsend = new int[slices];
        displace = new int[slices];
        rowsPerPro = new int[world];
        
        for (int i = 0; i < world; i++) {
            numsend[i] = displace[i] = 0;
        }
        
        rowsPerPro[0] = 0;
        int colSize = Rows / slices;
        
        for (int i = 1; i < world; i++) {
            rowsPerPro[i] = colSize;
            displace[i] = displace[i-1] + numsend[i-1];
            numsend[i] = colSize * Cols * 3;
        }
        
        if(slices > 1) {
            int lastSliceSize = Rows % (colSize * (slices - 1));
            
            if (lastSliceSize == 0) {
                lastSliceSize = colSize;
            }
            rowsPerPro[world - 1] = lastSliceSize;
            displace[world - 1] = displace[world - 2] + numsend[world - 2];
            numsend[world - 1] = lastSliceSize * Cols * 3;
        }
    }
    
    MPI_Scatter(rowsPerPro, 1, MPI_INT, &Rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&Cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    
    Mat* out = new Mat(Rows, Cols, CV_8UC3);
    MPI_Scatterv(in.data, numsend, displace, MPI_BYTE, out->data, Rows * Cols * 3, MPI_BYTE, 0, MPI_COMM_WORLD);
    
    if (1 <= rank && rank < world) {
        cv::Mat * gB = GaussianBlur(out);
        out = gB;
    }
    
    MPI_Gatherv(out->data, Rows * Cols * 3, MPI_BYTE,re->data, numsend, displace, MPI_BYTE, 0, MPI_COMM_WORLD);
    
    end = MPI_Wtime();
    
    if (rank == 0) {
        printf("Time: %f secs\n", end - start);
        imwrite(result, *re);
    }
    delete out;
    delete re;
    
    MPI_Finalize();
    return 0;
}
