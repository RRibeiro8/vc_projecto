#include "opencv2/opencv.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <list>
#include <stdio.h>

#include "PupilDetector.cpp"
#include "object/Face.cpp"
#include "math/RectUtils.cpp"

using namespace std;
using namespace cv;

//Files path
String path = "data/";

//Cascades files
String face_cascade_name = "haarcascade_face.xml";
String eyes_cascade_name = "haarcascade_eye.xml";
String nose_cascade_name = "haarcascade_nose.xml";
String smile_cascade_name = "haarcascade_smile.xml";

//Cascade classifiers
CascadeClassifier face_cascade, eyes_cascade, nose_cascade, smile_cascade;

//Faces in the scene
list<Face*> face_list;

//Callback for mouse events
void mouseEvents(int event, int x, int y, int flags, void *param)
{
	if(event == EVENT_LBUTTONDOWN)
	{
		//TODO <MOUSE INTERACTION>
	}
}

//Main
int main(int argc, const char** argv)
{
	//Load the cascades
	if(!face_cascade.load(path + face_cascade_name))
	{
		printf("Error loading face cascade\n");
		return -1;
	}
	if(!eyes_cascade.load(path + eyes_cascade_name))
	{
		printf("Error loading eye cascade\n");
		return -1;
	}
	if(!nose_cascade.load(path + nose_cascade_name))
	{
		printf("Error loading nose cascade\n");
		return -1;
	}
	if(!smile_cascade.load(path + smile_cascade_name))
	{
		printf("Error loading smile cascade\n");
		return -1;
	}

	//Video capture
	VideoCapture capture;
	if(!capture.open(0))
	{
		printf("Unable to open video capture\n");
		return -1;
	}

	//Configure window
	namedWindow("FaceTracker", CV_WINDOW_NORMAL);
	setMouseCallback("FaceTracker", mouseEvents, nullptr);

	//Camera frame
	Mat frame;

	while(true)
	{
		//Read the current frame
		capture >> frame;
		if(frame.empty())
		{
			return -1;
		}

		Mat gray;
		cvtColor(frame, gray, CV_BGR2GRAY);

		vector<Rect> faces;

		//Detect faces
		face_cascade.detectMultiScale(gray, faces, 1.5, 2, CV_HAAR_SCALE_IMAGE);
		for(unsigned int i = 0; i < faces.size(); i++)
		{
			Face* face = nullptr;
			int found = 0;

			//Check if face already exists
			for(list<Face*>::iterator f = face_list.begin(); f != face_list.end(); f++)
			{
				if(RectUtils::overlaps(faces[i], (*f)->box) && (*f)->ttl > 0)
				{
					if(found > 0)
					{
						//Distance of actual face
						float dx_a = (face->box.x - faces[i].x);
						float dy_a = (face->box.y - faces[i].y);
						float dist_a = dx_a * dx_a + dy_a * dy_a;
						
						//Distance to new candidate
						float dx_b = (faces[i].x - (*f)->box.x);
						float dy_b = (faces[i].y - (*f)->box.y);
						float dist_b =  dx_b * dx_b + dy_b * dy_b;
						
						//If new candidate closer than actual face use new candidate
						if(dist_b < dist_a)
						{
							face = (*f);
						}
					}
					else
					{
						face = (*f);
					}
					
					found++;
				}
			}

			//If face was not found add new one
			if(found == 0)
			{
				face = new Face();
			}

			//Counter for elements found inside the face
			int elements = 0;

			//Set face box
			face->setBox(faces[i]);

			Rect upper_rect = Rect(faces[i].x, faces[i].y, faces[i].width, faces[i].height * 0.6);
			Mat upper = gray(upper_rect);

			//Detect eyes
			vector<Rect> eyes;
			eyes_cascade.detectMultiScale(upper, eyes, 1.1, 3, CV_HAAR_SCALE_IMAGE);
			if(eyes.size() >= 2)
			{
				for(unsigned int j = 0; j < 2; j++)
				{
					//Determine eye circle
					Point center(upper_rect.x + eyes[j].x + eyes[j].width * 0.5, upper_rect.y + eyes[j].y + eyes[j].height * 0.5);
					int radius = cvRound((eyes[j].width + eyes[j].height) * 0.25);

					//Detect eye pupil
					Mat eye = upper(eyes[i]);
					Point pupil = PupilDetector::findEyeCenter(eye);
					pupil.x += faces[i].x + eyes[j].x;
					pupil.y += faces[i].y + eyes[j].y;

					//Determine with side is this eye
					for(unsigned int k = 0; k < 2; k++)
					{
						if(k != j)
						{
							//Right eye
							if(eyes[j].x < eyes[k].x)
							{
								face->setRightEye(center, radius, pupil);
							}
							//Left eye
							else
							{
								face->setLeftEye(center, radius, pupil);
							}
							break;
						}
					}

					elements++;
				}
			}

			Rect middle_rect = Rect(faces[i].x, faces[i].y + faces[i].height * 0.35, faces[i].width, faces[i].height * 0.45);
			Mat middle = gray(middle_rect);

			//Detect nose
			vector<Rect> noses;
			nose_cascade.detectMultiScale(middle, noses, 1.3, 2, CV_HAAR_SCALE_IMAGE);

			if(noses.size() > 0)
			{
				noses[0].x += middle_rect.x;
				noses[0].y += middle_rect.y;

				elements++;

				face->setNoseBox(noses[0]);
			}

			Rect lower_rect = Rect(faces[i].x, faces[i].y + faces[i].height * 0.6, faces[i].width, faces[i].height * 0.4);
			Mat lower = gray(lower_rect);

			//Detect smile
			vector<Rect> smiles;
			smile_cascade.detectMultiScale(lower, smiles, 1.5, 2, CV_HAAR_SCALE_IMAGE);

			if(smiles.size() > 0)
			{
				smiles[0].x += lower_rect.x;
				smiles[0].y += lower_rect.y;

				elements++;

				face->setMouthBox(smiles[0]);
			}

			//If its a new face add it to the face list
			if(found == 0 && elements > 0)
			{
				face_list.push_back(face);
			}
		}

		//Draw faces
		for(list<Face*>::iterator f = face_list.begin(); f != face_list.end(); f++)
		{
			if(!(*f)->update())
			{
				delete *f;
				face_list.remove(*f);
			}
			else
			{
				(*f)->draw(frame);
			}
		}

		//Display frame
		imshow("FaceTracker", frame);

		//Get keyboard input
		int keyboard = waitKey(16);
		if(keyboard == 27)
		{
			break;
		}
	}

	return 0;
}

