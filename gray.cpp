#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <time.h>
#include <fstream>
#include <vector>
using namespace	std;
using namespace	cv;

struct vec3
{
	float x;
	float y;
	float z;
	int num;
};

void PrintMatrix(CvMat *Matrix,int Rows,int Cols)
{
    for(int i=0;i<Rows;i++)
    {
        for(int j=0;j<Cols;j++)
        {
            printf("%f ",cvGet2D(Matrix,i,j).val[0]);
        }
        printf("\n");
    }
}

int main()
{
	
	
	Mat	I1,I2,I3,I4,I5,I6;
	char *imagename1 = "pic1.bmp";
	char *imagename2 = "pic2.bmp";
	char *imagename3 = "pic3.bmp";
	char *imagename4 = "pic4.bmp";
	char *imagename5 = "pic5.bmp";
	char *imagename6 = "pic6.bmp";
	I1 = imread(imagename1, 0);
	I2 = imread(imagename2, 0);
	I3 = imread(imagename3, 0);
	I4 = imread(imagename4, 0);
	I5 = imread(imagename5, 0);
	I6 = imread(imagename6, 0);
	//I6 = imread(imagename6, CV_LOAD_IMAGE_COLOR);

	//float S_Array[]={238,235,2360,298,65,2480,-202,225,2240,-252,115,2310,18,45,2270,-22,295,2230};	//bunny
	float S_Array[]={0,0,150,22,29,156,-46,38,163,-25,13,160,12,10,151,35,5,168};		//teapot


	CvMat *SourceVector=cvCreateMat(6,3,CV_32FC1);					//light souce vector matrix S
    cvSetData(SourceVector,S_Array,SourceVector->step);
	//PrintMatrix(SourceVector,SourceVector->rows,SourceVector->cols);


	CvMat *TransposeSourceVector=cvCreateMat(3,6,CV_32FC1);			//S^T
	cvTranspose(SourceVector,TransposeSourceVector);

	//PrintMatrix(TransposeSourceVector,TransposeSourceVector->rows,TransposeSourceVector->cols);



	CvMat *MulSource_1=cvCreateMat(3,3,CV_32FC1);
	cvMatMul(TransposeSourceVector,SourceVector,MulSource_1);		//S^T * S
	//PrintMatrix(MulSource_1,MulSource_1->rows,MulSource_1->cols);

	CvMat *InverseMulSource_1=cvCreateMat(3,3,CV_32FC1);			//[S^T * S] -1
	cvInvert(MulSource_1,InverseMulSource_1,CV_LU);
	//PrintMatrix(InverseMulSource_1,InverseMulSource_1->rows,InverseMulSource_1->cols);


	CvMat *MulSource_2=cvCreateMat(3,6,CV_32FC1);					// [S^T * S] -1 * S^T
	cvMatMul(InverseMulSource_1,TransposeSourceVector,MulSource_2);
	//PrintMatrix(MulSource_2,MulSource_2->rows,MulSource_2->cols);
	//CvMat *TransposeMatrix=cvCreateMat(3,3,CV_32FC1);
    //CvMat *InverseMatrix=cvCreateMat(3,3,CV_32FC1);
	//CvMat *ResultMatrix=cvCreateMat(3,3,CV_32FC1);
    //cvSetData(Matrix1,S_Array,Matrix1->step);
	/*
    printf("\nThe Transpose Matrix is:\n");
    cvTranspose(Matrix1,TransposeMatrix);
    PrintMatrix(TransposeMatrix,TransposeMatrix->rows,TransposeMatrix->cols);

    printf("\nThe Invert Matrix is:\n");
    cvInvert(Matrix1,InverseMatrix,CV_LU);
    PrintMatrix(InverseMatrix,InverseMatrix->rows,InverseMatrix->cols);

	printf("\nThe Multiply Result Matrix is:\n");
	cvMatMul(Matrix1,InverseMatrix,ResultMatrix);
	PrintMatrix(ResultMatrix,ResultMatrix->rows,ResultMatrix->cols);
    system("pause");

	*/
	ofstream outfile("model.obj");
	IplImage* NormalColorMap=cvCreateImage(cvSize(I1.cols,I1.rows),IPL_DEPTH_8U,3);	//normal color map
	
	CvMat *IlluminationVector=cvCreateMat(6,1,CV_32FC1);		//I
	CvMat *BVector=cvCreateMat(3,1,CV_32FC1);		//B

	float I_Array[6];
	vector<vector<vec3>> BMap;
	vector<vector<vec3>> NormalMap;
	vector<vector<float>> PMap;
	vector<vector<float>> QMap;
	vector<vector<float>> HightMap;
	BMap.resize(I1.rows);
	PMap.resize(I1.rows);
	QMap.resize(I1.rows);
	HightMap.resize(I1.rows);
	NormalMap.resize(I1.rows);
	for(int i=0;i<I1.rows;i++){
		BMap[i].resize(I1.cols);
		PMap[i].resize(I1.cols);
		QMap[i].resize(I1.cols);
		HightMap[i].resize(I1.cols);
		NormalMap[i].resize(I1.cols);
		for(int j=0;j<I1.cols;j++){
			//int Gray = I1.at<uchar>(i,j);
			I_Array[0]=I1.at<uchar>(i,j);
			I_Array[1]=I2.at<uchar>(i,j);
			I_Array[2]=I3.at<uchar>(i,j);
			I_Array[3]=I4.at<uchar>(i,j);
			I_Array[4]=I5.at<uchar>(i,j);
			I_Array[5]=I6.at<uchar>(i,j);
			cvSetData(IlluminationVector,I_Array,IlluminationVector->step);
			//PrintMatrix(IlluminationVector,IlluminationVector->rows,IlluminationVector->cols);

			cvMatMul(MulSource_2,IlluminationVector,BVector);
			//PrintMatrix(BVector,BVector->rows,BVector->cols);

			//printf("(%f,%f,%f) ",cvGet2D(BVector,0,0).val[0],cvGet2D(BVector,1,0).val[0],cvGet2D(BVector,2,0).val[0]);
			BMap[i][j].x=BVector->data.fl[0];
			BMap[i][j].y=BVector->data.fl[1];
			BMap[i][j].z=BVector->data.fl[2];
			//printf("(%f,%f,%f), ",BMap[i][j].x,BMap[i][j].y,BMap[i][j].z);



			float ALBEDO= sqrt((BMap[i][j].x*BMap[i][j].x)+(BMap[i][j].y*BMap[i][j].y)+(BMap[i][j].z*BMap[i][j].z));
			//outfile << ALBEDO << endl;
			if(ALBEDO == 0.0 ){
				NormalMap[i][j].x=0;
				NormalMap[i][j].y=0;
				NormalMap[i][j].z=0;
			}
			else{
				NormalMap[i][j].x=BMap[i][j].x/ALBEDO;
				NormalMap[i][j].y=BMap[i][j].y/ALBEDO;
				NormalMap[i][j].z=BMap[i][j].z/ALBEDO;
			}
			//outfile << "("<<NormalMap[i][j].x << ","<< NormalMap[i][j].y << ","<< NormalMap[i][j].z << ")"<< endl;

			CvScalar s;
			s.val[0] = int(NormalMap[i][j].x*128+128);	
			s.val[1] = int(NormalMap[i][j].y*128+128);
			s.val[2] = int(NormalMap[i][j].z*128+128);
			//cvSet2D(NormalColorMap,j,i,s);
			cvSet2D(NormalColorMap,i,j,s);
			//outfile << "("<<s.val[0] << ","<< s.val[1] << ","<< s.val[2] << ")"<< endl;
			if(NormalMap[i][j].z!=0){
				PMap[i][j]= -(NormalMap[i][j].x/NormalMap[i][j].z);
				QMap[i][j]= -(NormalMap[i][j].y/NormalMap[i][j].z);
			}
			else{
				PMap[i][j]=0;
				QMap[i][j]=0;
			}
			if(j==0){
				if(i==0)
					HightMap[0][0]=0;
				else
					HightMap[i][j]=HightMap[i-1][j]+QMap[i][j];			
			}
		}
		
	}


	for(int i=0;i<I1.rows;i++){
		for(int j=1;j<I1.cols;j++){
			HightMap[i][j]=HightMap[i][j-1]+PMap[i][j];
		}
	}



	for(int i=0;i<I1.rows;i++){
		for(int j=0;j<I1.cols;j++){
			if((NormalMap[i][j].x!=0.0)||(NormalMap[i][j].y!=0.0)||(NormalMap[i][j].z!=0.0)){
				outfile << "v "<< i << " "<< j << " "<< HightMap[i][j] << endl;
			}
		
		}
	}

	int node=0;
	for(int i=0;i<I1.rows;i++){
		for(int j=0;j<I1.cols;j++){
			if((NormalMap[i][j].x!=0.0)||(NormalMap[i][j].y!=0.0)||(NormalMap[i][j].z!=0.0)){
				NormalMap[i][j].num=++node;
				outfile << "vn "<< NormalMap[i][j].x << " "<< NormalMap[i][j].y << " "<< NormalMap[i][j].z << endl;
			}
			else{
				NormalMap[i][j].num=0;
			}
		
		}
	}
	//printf("*****************     %d",node);
	for(int i=0;i<I1.rows;i++){
		for(int j=0;j<I1.cols;j++){
			if(NormalMap[i][j].num!=0){
				if(j!=0 && NormalMap[i+1][j-1].num!=0 && NormalMap[i+1][j].num!=0){
					outfile << "f "<< NormalMap[i][j].num << "//" << NormalMap[i][j].num;
					outfile << " " << NormalMap[i+1][j-1].num << "//" << NormalMap[i+1][j-1].num;
					outfile << " " << NormalMap[i+1][j].num << "//" << NormalMap[i+1][j].num <<	endl;
				}
				if(j<I1.cols-1 && NormalMap[i][j+1].num!=0){
					if(NormalMap[i+1][j].num!=0){
						outfile << "f "<< NormalMap[i][j].num << "//" << NormalMap[i][j].num;
						outfile << " " << NormalMap[i+1][j].num << "//" << NormalMap[i+1][j].num;
						outfile << " " << NormalMap[i][j+1].num << "//" << NormalMap[i][j+1].num <<	endl;
					}
					else if(NormalMap[i+1][j+1].num!=0){
						outfile << "f "<< NormalMap[i][j].num << "//" << NormalMap[i][j].num;
						outfile << " " << NormalMap[i+1][j+1].num << "//" << NormalMap[i+1][j+1].num;
						outfile << " " << NormalMap[i][j+1].num << "//" << NormalMap[i][j+1].num <<	endl;
					}
				}
			}
		
		}
	}


	//cvShowImage("Map",NormalColorMap);
	cvSaveImage("output.jpg",NormalColorMap);
	

	/*
	ofstream outfile("setting.txt");
	outfile << "image name : " << imagename1 << endl;
	outfile << "size : " << I1.rows << " X " << I1.cols << endl;
	for(int i=0;i<I1.rows;i++){
		for(int j=0;j<I1.cols;j++){
			int Gray = I1.at<uchar>(i,j);
				//I1.at<Vec3b>(j, i)[2];
			//int G1 = input.at<Vec3b>(j, i)[1];
			//int B1 = input.at<Vec3b>(j, i)[0];
			outfile << Gray << " ";
		}
		outfile << endl;
	}
	outfile.close();
	*/
	
	/*
	ofstream outfile("setting.txt");
	for(int i=0;i<BMap.size();i++){
		for(int j=0;j<BMap[i].size();j++){
			
			outfile << "(" << BMap[i][j].x << "," << BMap[i][j].y << "," << BMap[i][j].z << ")" << " ";
		}
		outfile << endl;
	}*/
	outfile.close();
	return 0;
}
