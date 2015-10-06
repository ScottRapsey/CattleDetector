// CattleDetector.cpp : Defines the entry point for the console application.
//

#include <opencv2/dpm.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
//#include <VideoProcessor.h>

using namespace cv;
using namespace cv::dpm;
using namespace std;

//int main(int argc, char** argv)
//{
//	vector<string> models;
//	vector<string> names;
//	//names.push_back("car");
//	//models.push_back("car.xml");
//	names.push_back("cow");
//	models.push_back("G:\\Src\\Uncontrolled\\OpenCV\\Cattle\\CowDetector2\\Debug\\cow.xml");
//
//	cv::Ptr<DPMDetector> detector = DPMDetector::create(models, names);
//	cerr << "loaded " << models.size() << " models" << endl;
//
//	Mat frame = imread("G:\\Src\\Uncontrolled\\OpenCV\\Cattle\\CowDetector2\\Debug\\cow.png");
//	resize(frame, frame, Size(), 2, 2);
//	namedWindow("DPM Cascade Detection", 1);
//	// the color of the rectangle
//	Scalar color[3] = { Scalar(0, 255, 255),  Scalar(0, 255, 0),  Scalar(0, 0, 255) };
//
//
//	double t = (double)getTickCount();
//
//	// detection
//	vector<DPMDetector::ObjectDetection> ds;
//	detector->detect(frame.clone(), ds); // it somehow eats the image ;(
//
//	t = ((double)getTickCount() - t) / getTickFrequency();//elapsed time
//	cerr << "dpm took " << t << " seconds" << endl;
//	for (unsigned int i = 0; i < ds.size(); i++)
//	{
//		int id = ds[i].classID;
//		cerr << names[id] << "\t" << ds[i].score << "\t" << ds[i].rect << endl;
//		//if (ds[i].score < -0.95)
//		//    continue;
//		rectangle(frame, ds[i].rect, Scalar(0, 255, 255), 1);
//	}
//
//	imshow("DPM Cascade Detection", frame);
//	imwrite("cow_det.png", frame);
//
//	waitKey(0);
//
//	return 0;
//}
//

class VideoProcessor
{
public:

	//VideoProcessor()
	//{
	//}

	//virtual ~VideoProcessor()
	//{
	//}

	int ProcessVideo()
	{
		const string inputVideoPath = "D:\\Cattle Drone\\Video\\Checking Cattle with Drone\\Checking Cattle with Drone.mp4";
		const string outputVideoPath = "D:\\Cattle Drone\\Video\\Checking Cattle with Drone\\Checking Cattle with Drone_Processed.avi";

		vector<string> models;
		vector<string> names;
		//names.push_back("car");
		//models.push_back("car.xml");
		names.push_back("cow");
		models.push_back("cow.xml");

		cv::Ptr<DPMDetector> detector = DPMDetector::create(models, names);
		cout << "loaded " << models.size() << " models" << endl;


		//ap = cv2.VideoCapture("./out.mp4")
		//while not cap.isOpened() :
		//	cap = cv2.VideoCapture("./out.mp4")
		//	cv2.waitKey(1000)
		//	print "Wait for the header"


		VideoCapture inputVideo(inputVideoPath); // open video file
		if (!inputVideo.isOpened())  // check if we succeeded
			return -1;

		//Mat edges;
		//namedWindow("edges", 1);
		namedWindow("DPM Cascade Detection Before", 1);
		namedWindow("DPM Cascade Detection After", 1);


		int ex = static_cast<int>(inputVideo.get(CV_CAP_PROP_FOURCC));     // Get Codec Type- Int form
																		   // Transform from int to char via Bitwise operators
		char EXT[] = { (char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0 };

		cout << "Input codec type: " << EXT << endl;

		Size inputVideoSize = Size((int)inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
			(int)inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));
		double inputVideoFrameCount = inputVideo.get(CV_CAP_PROP_FRAME_COUNT);
		double inputVideoFPS = inputVideo.get(CV_CAP_PROP_FPS);

		cout << "Input frame resolution: Width=" << inputVideoSize.width << "  Height=" << inputVideoSize.height
			<< " of nr#: " << inputVideoFrameCount
			<< " FPS: " << inputVideoFPS
			<< endl;

		//VideoWriter outputVideo(outputVideoPath, -1, inputVideoFPS, inputVideoSize, true); //doesn't work, dvix.dll access violation
		//VideoWriter outputVideo(outputVideoPath, CV_FOURCC('M', 'P', '4', '2'), inputVideoFPS, inputVideoSize, true); //works, video 3 times larger than original
		//VideoWriter outputVideo(outputVideoPath, CV_FOURCC('D', 'I', 'V', '3'), inputVideoFPS, inputVideoSize, true); //works
		//VideoWriter outputVideo(outputVideoPath, CV_FOURCC('A', 'V', 'C', '1'), inputVideoFPS, inputVideoSize, true); //doesn't work, just doesn't open, no exception
		//VideoWriter outputVideo(outputVideoPath, CV_FOURCC('D', 'M', '4', 'V'), inputVideoFPS, inputVideoSize, true); //doesn't work, just doesn't open, no exception
		VideoWriter outputVideo(outputVideoPath, CV_FOURCC('F', 'M', 'P', '4'), inputVideoFPS, inputVideoSize, true); //doesn't work, just doesn't open, no exception

		if (!outputVideo.isOpened())
		{
			cout << "Could not open the output video for write: " << outputVideoPath << endl;
			return -1;
		}

		const int processEveryXFrames = 1;
		const int shrinkSize = 5;
		const double shrinkRatio = (1.0 / shrinkSize);

		bool x = detect();

		Mat originalFrame;
		Mat resizedFrame;
		int counter = 0;
		//cv:parallel_for_()
		for (;;)
		{
			counter++;

			//inputVideo >> frame; // get a new frame from camera
			bool didRead = inputVideo.read(originalFrame);
			
			if (didRead)
			{
				cout << "Read frame: " << counter << " of " << inputVideoFrameCount << endl;

				resize(originalFrame, resizedFrame, Size(), shrinkRatio, shrinkRatio);
				// the color of the rectangle
				//Scalar color[3] = { Scalar(0, 255, 255),  Scalar(0, 255, 0),  Scalar(0, 0, 255) };

				imshow("DPM Cascade Detection Before", resizedFrame);
				waitKey(1);


				if (counter % processEveryXFrames == 0) {
					cout << "Running detection" << endl;
					// detection
					vector<DPMDetector::ObjectDetection> ds;
					double t = (double)getTickCount();

					detector->detect(resizedFrame.clone(), ds); // it somehow eats the image ;(

					t = ((double)getTickCount() - t) / getTickFrequency();//elapsed time
					cout << "dpm took " << t << " seconds" << endl;
					for (unsigned int i = 0; i < ds.size(); i++)
					{
						cout << "COW DETECTED! COW DETECTED!" << counter << endl;

						int id = ds[i].classID;
						cout << names[id] << "\t" << ds[i].score << "\t" << ds[i].rect << endl;
						//if (ds[i].score < -0.95)
						//    continue;
						//rectangle rect =  ds[i].rect
						Rect blownupRect(ds[i].rect.x * shrinkSize, ds[i].rect.y * shrinkSize, ds[i].rect.width * shrinkSize, ds[i].rect.height * shrinkSize);
						rectangle(originalFrame, blownupRect, Scalar(0, 255, 255), 1);
					}
				}
				else
				{
					cout << "skiping detection" << endl;
				}

				imshow("DPM Cascade Detection After", originalFrame);
				waitKey(1);

				//resize(frame, frame, inputVideoSize);

				outputVideo << originalFrame;
				//outputVideo.write(originalFrame);

				resizedFrame.release();
				originalFrame.release();
				//imwrite("cow_det.png", frame);
				if (waitKey(30) >= 0) break;
			}
			else
			{
				break; //exit the for loop
			}
		}

		outputVideo.release();

		// the camera will be deinitialized automatically in VideoCapture destructor
		return 0;
		return 0;
	}


private:

	bool detect()
	{
		return true;
	}
};


int main(int argc, char** argv)
{
	VideoProcessor vidProc;
	vidProc.ProcessVideo();
}


//class CattleDetection
//{
//	CattleDetection()
//	{
//		cout << "Constructor" << endl;
//	}
//	int ProcessVideo()
//	{
//
//		const string inputVideoPath = "D:\\Cattle Drone\\Video\\Checking Cattle with Drone\\Checking Cattle with Drone.mp4";
//		const string outputVideoPath = "D:\\Cattle Drone\\Video\\Checking Cattle with Drone\\Checking Cattle with Drone_Processed.avi";
//
//		vector<string> models;
//		vector<string> names;
//		//names.push_back("car");
//		//models.push_back("car.xml");
//		names.push_back("cow");
//		models.push_back("G:\\Src\\Uncontrolled\\OpenCV\\Cattle\\CowDetector2\\Debug\\cow.xml");
//
//		cv::Ptr<DPMDetector> detector = DPMDetector::create(models, names);
//		cout << "loaded " << models.size() << " models" << endl;
//
//
//		//ap = cv2.VideoCapture("./out.mp4")
//			//while not cap.isOpened() :
//			//	cap = cv2.VideoCapture("./out.mp4")
//			//	cv2.waitKey(1000)
//			//	print "Wait for the header"
//
//
//		VideoCapture inputVideo(inputVideoPath); // open video file
//		if (!inputVideo.isOpened())  // check if we succeeded
//			return -1;
//
//		//Mat edges;
//		//namedWindow("edges", 1);
//		namedWindow("DPM Cascade Detection Before", 1);
//		namedWindow("DPM Cascade Detection After", 1);
//
//
//		int ex = static_cast<int>(inputVideo.get(CV_CAP_PROP_FOURCC));     // Get Codec Type- Int form
//																		   // Transform from int to char via Bitwise operators
//		char EXT[] = { (char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0 };
//
//		cout << "Input codec type: " << EXT << endl;
//
//		Size inputVideoSize = Size((int)inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
//			(int)inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));
//		double inputVideoFrameCount = inputVideo.get(CV_CAP_PROP_FRAME_COUNT);
//		double inputVideoFPS = inputVideo.get(CV_CAP_PROP_FPS);
//
//		cout << "Input frame resolution: Width=" << inputVideoSize.width << "  Height=" << inputVideoSize.height
//			<< " of nr#: " << inputVideoFrameCount
//			<< " FPS: " << inputVideoFPS
//			<< endl;
//
//		//VideoWriter outputVideo(outputVideoPath, -1, inputVideoFPS, inputVideoSize, true); //doesn't work, dvix.dll access violation
//		//VideoWriter outputVideo(outputVideoPath, CV_FOURCC('M', 'P', '4', '2'), inputVideoFPS, inputVideoSize, true); //works, video 3 times larger than original
//		//VideoWriter outputVideo(outputVideoPath, CV_FOURCC('D', 'I', 'V', '3'), inputVideoFPS, inputVideoSize, true); //works
//		//VideoWriter outputVideo(outputVideoPath, CV_FOURCC('A', 'V', 'C', '1'), inputVideoFPS, inputVideoSize, true); //doesn't work, just doesn't open, no exception
//		//VideoWriter outputVideo(outputVideoPath, CV_FOURCC('D', 'M', '4', 'V'), inputVideoFPS, inputVideoSize, true); //doesn't work, just doesn't open, no exception
//		VideoWriter outputVideo(outputVideoPath, CV_FOURCC('F', 'M', 'P', '4'), inputVideoFPS, inputVideoSize, true); //doesn't work, just doesn't open, no exception
//
//		if (!outputVideo.isOpened())
//		{
//			cout << "Could not open the output video for write: " << outputVideoPath << endl;
//			return -1;
//		}
//
//		const int processEveryXFrames = 1;
//		const int shrinkSize = 5;
//		const double shrinkRatio = (1.0 / shrinkSize);
//
//		bool x = detect();
//
//		Mat originalFrame;
//		Mat resizedFrame;
//		int counter = 0;
//		//cv:parallel_for_()
//		for (;;)
//		{
//			counter++;
//
//			//inputVideo >> frame; // get a new frame from camera
//			bool didRead = inputVideo.read(originalFrame);
//			if (didRead)
//			{
//				cout << "Read frame: " << counter << " of " << inputVideoFrameCount << endl;
//
//				resize(originalFrame, resizedFrame, Size(), shrinkRatio, shrinkRatio);
//				// the color of the rectangle
//				//Scalar color[3] = { Scalar(0, 255, 255),  Scalar(0, 255, 0),  Scalar(0, 0, 255) };
//
//				imshow("DPM Cascade Detection Before", resizedFrame);
//				waitKey(1);
//
//
//				if (counter % processEveryXFrames == 0) {
//					cout << "Running detection" << endl;
//					// detection
//					vector<DPMDetector::ObjectDetection> ds;
//					double t = (double)getTickCount();
//
//					detector->detect(resizedFrame.clone(), ds); // it somehow eats the image ;(
//
//					t = ((double)getTickCount() - t) / getTickFrequency();//elapsed time
//					cout << "dpm took " << t << " seconds" << endl;
//					for (unsigned int i = 0; i < ds.size(); i++)
//					{
//						cout << "COW DETECTED! COW DETECTED!" << counter << endl;
//
//						int id = ds[i].classID;
//						cout << names[id] << "\t" << ds[i].score << "\t" << ds[i].rect << endl;
//						//if (ds[i].score < -0.95)
//						//    continue;
//						//rectangle rect =  ds[i].rect
//						Rect blownupRect(ds[i].rect.x * shrinkSize, ds[i].rect.y * shrinkSize, ds[i].rect.width * shrinkSize, ds[i].rect.height * shrinkSize);
//						rectangle(originalFrame, blownupRect, Scalar(0, 255, 255), 1);
//					}
//				}
//				else
//				{
//					cout << "skiping detection" << endl;
//				}
//
//				imshow("DPM Cascade Detection After", originalFrame);
//				waitKey(1);
//
//				//resize(frame, frame, inputVideoSize);
//
//				outputVideo << originalFrame;
//				//outputVideo.write(originalFrame);
//
//				resizedFrame.release();
//				originalFrame.release();
//				//imwrite("cow_det.png", frame);
//				if (waitKey(30) >= 0) break;
//			}
//			else
//			{
//				break; //exit the for loop
//			}
//		}
//
//		outputVideo.release();
//
//		// the camera will be deinitialized automatically in VideoCapture destructor
//		return 0;
//	}
//
//	bool detect()
//	{
//		return true;
//	}
//};
