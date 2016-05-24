#include "../include/DFT.h"

#include "..\include\DF.h"

#ifndef _VCL_VECTOR_
#define _VCL_VECTOR_
#include <vcl_vector.h>
#endif

DFT::DFT()
{
    //constructor
}

DFT::~DFT()
{
    //destructor
}

void DFT::trackObject( vcl_vector< vil_image_view<unsigned char> >& images )
{
    /// placeholders until we work out how to pass in these variables
    /*int numChannels;
    int spatialBlurSize;
    int colourBlurSize;*/

    DF_params default_params = DF_params(8, 3, 1, 1, 1);

    for (int i=0; i<images.size(); i++)
    {
        DistributionField dfObj;

        /// if this is the first frame, we need to build the model before we can track it
        if (_firstFrame)
        {
            /// create the model from the distribution field, and the current position

            /// setup and generate the distribution field


            dfObj = DistributionField(images[i], default_params);

            _firstFrame = false; /// not the first frame, so we can track the object
        }
        if (!_firstFrame)
        {
            /// return the distribution field of the current frame
            //vector<vil_image_view<unsigned char> > df = dfObj.getDistributionField();

            /// locate the object in the current frame. Use gradient descent search
            /// to find the new object position
            _currentPosition = locateObject( dfObj, _currentPosition, _maxSearchDist );

            ///  update the object model to incorporate new information
            //updateModel( df, _currentPosition);

            /// display or print an image, ie. draw a bounding box around the object being tracked
            displayCurrentPosition (images[i], _currentPosition );
        }

    }
}

map<vcl_string,int> DFT::locateObject(const DistributionField& df, map<vcl_string,int> initialPosition, int maxSearchDist)
{
    // TODO
    // update the currentPosition member variable to the new object position

    // df = distribution field of the current frame
    // objectModel = distribution field of the object being tracked
    // initialPosition = a map which includes the x,y, height and width of initial position of the objectModel
    // maxSearchDist = the maximum distance from the initial location we should search

    int numSearchLocations = 5;
    int searchLocationsX[] = {0,-1,0,1,0};
    int searchLocationsY[] = {0,0,-1,0,1};

    map<vcl_string,int> objectLocation = initialPosition;


    while(true)
    {
        // we need to work out where the best position is in the local neighbourhood
         // an arbitrarily large number
        int bestLocation = 0;
        int minDistance = 9999999;

        for (int i=0; i<numSearchLocations;i++)
        {
            // compare the model to the image at the current search location
            // note that we need to crop df so that it is the same size as the objectModel
            // Our crop takes a location, a width and a height

            int x = objectLocation["x"]+searchLocationsX[i];
            int y = objectLocation["y"]+searchLocationsY[i];
            int width = objectLocation["width"];
            int height = objectLocation["height"];

            //vector <vil_image_view<unsigned char> > croppedField = cropDF(df,x,y,height,width);
            DistributionField croppedField = df.subfield(x, y, width, height);

            int distance = _objectModel.compare(croppedField);
            //int distance;
            if (distance < minDistance)
            {
                minDistance = distance;
                bestLocation = i;
            }
        }

        // update the object location based on this iteration
        objectLocation["x"] += searchLocationsX[bestLocation];
        objectLocation["y"] += searchLocationsY[bestLocation];


        int searchDist = sqrt(
                              (objectLocation["x"]-initialPosition["x"])*(objectLocation["x"]-initialPosition["x"])
                              +
                              (objectLocation["y"]-initialPosition["y"])*(objectLocation["y"]-initialPosition["y"])
                              );

        // if the best_location is 0, i.e. no motion, then we have reached the end of the search, or
        // if we’ve gone maxSearchDist, we’ve reached the end of our search
        if ( (bestLocation != 0) & (searchDist < maxSearchDist) )
        {
            break;
        }
    }


    return objectLocation;
}

void DFT::displayCurrentPosition ( vil_image_view<unsigned char>& currentFrame, map<vcl_string,int> currentPosition )
{
    //TODO display the image somehow (probably draw a bounding box onto an output image
}

void DFT::updateModel(vector< vil_image_view<unsigned char> > df, map<vcl_string,int> currentPosition)
{
    // Update the object_model member variable
}



