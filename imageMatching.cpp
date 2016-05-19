/*
 * imageMatching.cpp
 *
 *  Created on: Jun 8, 2011
 *      Author: susan
 *
 *  This program extracts features from images and compares the images for similarity based on those
 *  features.  See the README for more details.
 */

#include "imageMatching.h"


int main( int argc, char** argv ) {
	// Log to file? and Log to console? are the inputs to this constructor
	OutputLogger* outputter = new OutputLogger(true, true);

	if ( (argc == 1) ) {
		std::cout << "option one got here" << std::endl;
		compareAllPictures(outputter);
	}
	else if ( strcmp(argv[1], "cycle") == 0 )  {
		currDirectory = argv[2];
		currExtension = argv[3];
		startPicture = atoi(argv[4]);
		numPictures = atoi(argv[5]);
		*outputter << "Comparing all pictures with all";
		outputter->writeLog();
		compareAllPictures(outputter);
	}
	else if ( strcmp(argv[1], "compare") == 0 ) {
		currDirectory = argv[2];
		currExtension = argv[3];
		compareSelected(outputter);
	}
	else if ( strcmp(argv[1], "collect") == 0 ) {
		currDirectory = argv[2];
		currExtension = argv[3];
		secondDirectory = argv[4];
		startPicture = atoi(argv[5]);
		numPictures = atoi(argv[6]);
		makeCollection(outputter);
	}
	else if ( strcmp(argv[1], "camera") == 0 ) {
			currDirectory = argv[2];
			currExtension = argv[3];
			secondDirectory = argv[4];
			startPicture = atoi(argv[5]);
			numPictures = atoi(argv[6]);
			makeCamera(outputter);
		}
}


/*
 * Reads in all the pictures in the currentDirectory, making a vector of ImageFeature pointers.
 * It then loops until the user says to quit.  In each loop, it gets a single frame from the
 * secondDirectory, and it compares that picture's features to the vector of features.  It keeps
 * the three best matches, assuming there are at least three with a similarity less than 300.0
 * It displays these three at the end.
 */
void makeCamera(OutputLogger* outputter) {
	bool quitTime = false;
	CvCapture* capture;
	capture = cvCreateCameraCapture(0);
	IplImage* image;
	IplImage* original;
	ImageFeatures* features;
	vector<ImageFeatures*> featureCollection;
	int pic2Num;

	*outputter << "Reading in image database";
	outputter->writeLog();
	for (int picNum = startPicture; (picNum < numPictures) && !quitTime; picNum += 1) {
		image = getFileByNumber(picNum, currDirectory);
//		cvShowImage("Image", image);
		*outputter  <<  "Image = " << picNum;
		outputter->writeLog();
		features = new ImageFeatures(image, outputter);
		featureCollection.push_back(features);
	}

	*outputter << "Length of collection..." << featureCollection.size();
	outputter->writeLog();

	double threshold = 300.0;
	vector<ImageFeatures*> bestMatches;
	vector<double> bestScores;
	vector<int> bestImageNums;

	while (!quitTime) {
		cout << "Enter number of picture to compare: ";
//		cin >> pic2Num;
		original= cvQueryFrame( capture );
		image = cvCreateImage(cvSize(320 ,240 ), original->depth, original->nChannels );
		char c = cvWaitKey(33);
		if( c == 27 ) break;
		cvResize(original, image);

		cvShowImage("Image", image);
		*outputter  <<  "Comparing image = Webcam"; //<< pic2Num;
		outputter->writeLog();
		features = new ImageFeatures(image, outputter);
		features->displayFeaturePics("Primary image", 100, 100);

		bestMatches.clear();
		bestScores.clear();
		bestImageNums.clear();
		cout <<"test1"<<endl;
		for (unsigned int i = 0; (i < featureCollection.size()) && !quitTime; i++) {
			cout <<"test2"<<endl;
			double simValue = features->evaluateSimilarity(featureCollection[i]);
			cout <<"test3"<<endl;
			if (simValue < threshold) {
				if (bestScores.size() < 3) {
					*outputter << "adding more good matches " << bestScores.size();
					outputter->writeLog();
					bestMatches.push_back(featureCollection[i]);
					bestScores.push_back(simValue);
					bestImageNums.push_back(i);
				}
				else if (bestScores.size() == 3) {
					int whichMax = -1;
					double maxBest = -1.0;
					*outputter << "Current scores = ";
					for (unsigned int j = 0; j < 3; j++) {
						*outputter << bestScores[j] << " ";
						if (bestScores[j] > maxBest) {
							maxBest = bestScores[j];
							whichMax = j;
						}
					}
					outputter->writeLog();
					*outputter << "Current max best = " << maxBest << "   Current simValue = " << simValue;
					outputter->writeLog();
					if (simValue < maxBest) {
						*outputter << "Changing " << whichMax << "to new value";
						outputter->writeLog();
						bestScores[whichMax] = simValue;
						bestMatches[whichMax] = featureCollection[i];
						bestImageNums[whichMax] = i;
					}
				}
				else {
					*outputter << "Should never get here... too many items in bestMatches! " << bestMatches.size();
					outputter->writeLog();
				}
			}
		}

		for (unsigned int j = 0; j < bestMatches.size(); j++) {
			*outputter << "Second image " << bestImageNums[j];
			outputter->writeLog();
			bestMatches[j]->displayFeaturePics("Second image features", 500, 100);
			*outputter << "Image matches with similarity value of " << bestScores[j];
			outputter->writeLog();
			cvWaitKey(0);

		}
		features = new ImageFeatures(image, outputter);
				featureCollection.push_back(features);
	} // while (! quitTime)

	// clean up the collection before quitting
	for (unsigned int i = 0; i < featureCollection.size(); i++) {
		delete featureCollection[i];
	}
}

void makeCollection(OutputLogger* outputter) {
	bool quitTime = false;
	IplImage* image;
	ImageFeatures* features;
	vector<ImageFeatures*> featureCollection;
	int pic2Num;

	*outputter << "Reading in image database";
	outputter->writeLog();
	for (int picNum = startPicture; (picNum < numPictures) && !quitTime; picNum += 1) {
		image = getFileByNumber(picNum, currDirectory);
//		cvShowImage("Image", image);
		*outputter  <<  "Image = " << picNum;
		outputter->writeLog();
		features = new ImageFeatures(image, outputter);
		featureCollection.push_back(features);
	}

	*outputter << "Length of collection..." << featureCollection.size();
	outputter->writeLog();

	double threshold = 300.0;
	vector<ImageFeatures*> bestMatches;
	vector<double> bestScores;
	vector<int> bestImageNums;

	while (!quitTime) {
		cout << "Enter number of picture to compare: ";
		cin >> pic2Num;
		if (pic2Num < 0) {
			break;
		}
		image = getFileByNumber(pic2Num, secondDirectory);
		cvShowImage("Image", image);
		*outputter  <<  "Comparing image = " << pic2Num;
		outputter->writeLog();
		features = new ImageFeatures(image, outputter);
		features->displayFeaturePics("Primary image", 100, 100);

		bestMatches.clear();
		bestScores.clear();
		bestImageNums.clear();
		for (unsigned int i = 0; (i < featureCollection.size()) && !quitTime; i++) {
			double simValue = features->evaluateSimilarity(featureCollection[i]);
			if (simValue < threshold) {
				if (bestScores.size() < 3) {
					*outputter << "adding more good matches " << bestScores.size();
					outputter->writeLog();
					bestMatches.push_back(featureCollection[i]);
					bestScores.push_back(simValue);
					bestImageNums.push_back(i);
				}
				else if (bestScores.size() == 3) {
					int whichMax = -1;
					double maxBest = -1.0;
					*outputter << "Current scores = ";
					for (unsigned int j = 0; j < 3; j++) {
						*outputter << bestScores[j] << " ";
						if (bestScores[j] > maxBest) {
							maxBest = bestScores[j];
							whichMax = j;
						}
					}
					outputter->writeLog();
					*outputter << "Current max best = " << maxBest << "   Current simValue = " << simValue;
					outputter->writeLog();
					if (simValue < maxBest) {
						*outputter << "Changing " << whichMax << "to new value";
						outputter->writeLog();
						bestScores[whichMax] = simValue;
						bestMatches[whichMax] = featureCollection[i];
						bestImageNums[whichMax] = i;
					}
				}
				else {
					*outputter << "Should never get here... too many items in bestMatches! " << bestMatches.size();
					outputter->writeLog();
				}
			}
		}

		for (unsigned int j = 0; j < bestMatches.size(); j++) {
			*outputter << "Second image " << bestImageNums[j];
			outputter->writeLog();
			bestMatches[j]->displayFeaturePics("Second image features", 500, 100);
			*outputter << "Image matches with similarity value of " << bestScores[j];
			outputter->writeLog();
			cvWaitKey(0);

		}
	} // while (! quitTime)

	// clean up the collection before quitting
	for (unsigned int i = 0; i < featureCollection.size(); i++) {
		delete featureCollection[i];
	}
}



/*
 * Loops until the user says to quit.  It asks the user for two image numbers, reads those images from the
 * currentDirectory, and compares them, displaying the results.
 */
void compareSelected(OutputLogger* outputter) {

	bool quitTime = false;
	int pic1num;
	int pic2num;
	IplImage* image1;
	IplImage* image2;
	ImageFeatures* features1;
	ImageFeatures* features2;
	cvNamedWindow("Image1", 1);
	cvNamedWindow("Image2", 1);

	while ( !quitTime ) {
		cout << "Enter number of first picture to compare: ";
		cin >> pic1num;
		if (pic1num < 0)
			break;
		cout << "Enter number of second picture to compare: ";
		cin >> pic2num;
		if (pic2num < 0)
			break;
		image1 = getFileByNumber(pic1num, currDirectory);
		cvShowImage("Image1", image1);
		*outputter  <<  "Image1 = " << pic1num;
		outputter->writeLog();
		features1 = new ImageFeatures(image1, outputter);
		features1->displayFeaturePics("Feature1", 100, 100);
		image2 = getFileByNumber(pic2num, currDirectory);
		cvShowImage("Image2", image2);
		*outputter  <<  "Image2 = " << pic2num;
		outputter->writeLog();
		features2 = new ImageFeatures(image2, outputter);
		features2->displayFeaturePics("Feature2  ", 400, 100);
		double simVal1 = features1->evaluateSimilarity(features2, 'v');
		*outputter << "Similarity = " << simVal1;
		outputter->writeLog();
	}
}



/*
 * It compares every image in currentDirectory to every other image there, and builds a matrix
 * of the resulting values.  It displays close matches for the user to see as it goes.
 */
void compareAllPictures(OutputLogger* outputter) {
	bool quitTime = false;

	IplImage* image1;
	IplImage* image2;
	ImageFeatures* features1;
	ImageFeatures* features2;

	cvNamedWindow("Image1", 1);
	cvNamedWindow("Image2", 1);

	double matchScores[numPictures][numPictures];
	for (int i = 0; i < numPictures; i++) {
		for (int j = 0; j < numPictures; j++) {
			matchScores[i][j] = 0.0;
		}
	}

	double threshold = 300.0;
	for (int picNum = startPicture; (picNum < numPictures) && !quitTime; picNum += 1) {
		std::cout << "getting next picture" << picNum << std::endl;
		image1 = getFileByNumber(picNum, currDirectory);
		cvShowImage("Image1", image1);
		*outputter  <<  "Image1 = " << picNum;
		outputter->writeLog();
		features1 = new ImageFeatures(image1, outputter);

		for (int pic2Num = startPicture; (pic2Num < numPictures)  && !quitTime; pic2Num += 1) {
			*outputter << "=======================================================" << endl;
			outputter->writeLog();
			// Read in file
			image2 = getFileByNumber(pic2Num, currDirectory);
			features2 = new ImageFeatures(image2, outputter);
			double simVal = features1->evaluateSimilarity(features2);
			matchScores[picNum][pic2Num] = simVal;
			*outputter << "Image " << picNum << "  matched image " << pic2Num << "  with score " << simVal;
			outputter->writeLog();

			if (simVal < threshold) {
				cvShowImage("Image2", image2);
				features1->displayFeaturePics("Image 1", 100, 100);
				features2->displayFeaturePics("Images 2", 400, 100);
				*outputter << "Image " << picNum << "  matched image " << pic2Num << "  with score " << simVal;
				outputter->writeLog();
				char c = cvWaitKey(0);
				if ( c == 'q' ) {
					quitTime = true;
				}
				else if (c == 'm') {
					threshold += 500;
				}
				else if (c == 'l') {
					threshold -= 500;
				}
			}
			cvReleaseImage(&image2);
 			delete features2;
		} // end inner for loop


		char c = cvWaitKey(00);
		if ( c == 'q' ) {
			quitTime = true;
		}
		cvReleaseImage(&image1);
		delete features1;
	}

	// Print out the table of match values
	*outputter <<  "     ";
	for ( int i = 0; i < numPictures; i++) {
		*outputter << setfill(' ') << setw(9) << i;
	}
	outputter->writeLog();


	for ( int i = 0; i < numPictures; i++ ) {
		*outputter << setfill(' ') << setw(9) << i;
		for ( int j = 0; j <= numPictures; j++ ) {
			*outputter << setfill(' ') << setw(9) << setiosflags(ios::fixed) << setprecision(3) << matchScores[i][j];
		}
		outputter->writeLog();
	}

	delete outputter;

}



/*
 * These three functions read in images based on their directory and file number, or write them out, if we want
 */
IplImage* getFileByNumber(int filenum, const char* directory) {
	const char * filename = makeFilename(filenum, directory);
	std::cout << "Filename = " << filename << std::endl;
	IplImage* image = cvLoadImage(filename, 1);
	return image;
}

void putFileByNumber(int filenum, IplImage* image) {
	const char * filename = makeFilename(filenum, newDirectory);
	cvSaveImage(filename, image);
}

const char* makeFilename(int fileNum, const char* directory) {
	stringstream nameMaker;
	nameMaker << directory << "foo" << setfill('0') << setw(4) << fileNum << currExtension;
	return nameMaker.str().c_str();
}





/*
 ***************************************************************************************
 * Dead code that might be useful someday below this point
 */


//			bool keeper = false;

//			if ( (fiveBestPics.size() < 5) || (simVal < fiveBestScores[4]) ) {
//				int i;
//				for (i = 0; i < fiveBestPics.size(); i++) {
//					if ( simVal < fiveBestScores[i] ) {
//						break;
//					}
//				}
//
//				vector<IplImage*>::iterator picIt = fiveBestPics.begin();
//				vector<double>::iterator scoreIt = fiveBestScores.begin();
//				vector<int>::iterator numIt = fiveBestNums.begin();
//
//				fiveBestPics.insert(picIt+i, image2);
//				fiveBestScores.insert(scoreIt+i, simVal);
//				fiveBestNums.insert(numIt+i, pic2Num);
//
//				keeper = true;
//				if ( fiveBestPics.size() == 6 ) {
//					cvReleaseImage(&(fiveBestPics[5]));
//					fiveBestPics.pop_back();
//					fiveBestScores.pop_back();
//					fiveBestNums.pop_back();
//				}
//			}
//			if ( simVal <= 0.5 ) {
//				cvShowImage("Image2", image2);
//				cout << "Image2 = " << pic2Num << endl;
//				cout << "SIMILARITY = " << simVal << endl;
//				features1->displayFeaturePics("features1");
//				features2->displayFeaturePics("features2");
//				char c = cvWaitKey(0);
//				if (c == 'q') {
//					quitTime = true;
//				}
//			}

//			char c = cvWaitKey(20);
//			if ( c == 'q' ) {
//				quitTime = true;
//			}
////			if (!keeper) {
//				cvReleaseImage(&image2);
//			}



//		for (unsigned int i = 0; i < fiveBestPics.size(); i++) {
//			cvShowImage("Image2", fiveBestPics[i]);
//			cout << "Image = " << fiveBestNums[i] << "  Score: " << fiveBestScores[i] << endl;
//			cvWaitKey(0);
//		}
