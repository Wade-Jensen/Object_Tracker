#include "../include/DFT.h"

#include "..\include\DF.h"

DFT::DFT()
{
    //constructor
}

DFT::DFT(const DistributionField& initialFrameDF , int x, int y, int width, int height /*, possible default values for optional arguments in the consturctor*/ )
{
    _currentPosition["x"] = x;
    _currentPosition["y"] = y;
    _currentPosition["width"] = width;
    _currentPosition["height"] = height;

    _objectModel = initialFrameDF.subfield(x, y, width, height);
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

    int numChannels = 8;
    int blurSpatial = 3;
    int blurColour = 1;
    float sdSpatial = 1;
    float sdColour = 1;

    vcl_string outputPath = "output";

    DF_params default_params = DF_params(numChannels, blurSpatial, blurColour, sdSpatial, sdColour);

    for (int i=0; i<images.size(); i++)
    {
        DistributionField dfFrame;

        /// if this is the first frame, we need to build the model before we can track it
        if (_firstFrame)
        {
            int x = _currentPosition["x"];
            int y = _currentPosition["y"];

            int width = _currentPosition["width"];
            int height = _currentPosition["height"];

            /// create the model from the distribution field, and the current position
            /// setup and generate the distribution field for the whole frame, then crop it to just the object
            _objectModel = DistributionField(images[i], default_params);
            _objectModel.subfield(x, y, width, height);

            _firstFrame = false; /// not the first frame, so we can track the object
        }
        if (!_firstFrame)
        {
            /// locate the object in the current frame. Use gradient descent search
            /// to find the new object position
            _currentPosition = locateObject( dfFrame, _maxSearchDist );

            int x = _currentPosition["x"];
            int y = _currentPosition["y"];

            int width = _currentPosition["width"];
            int height = _currentPosition["height"];

            // get a cropped copy of the distribution field at the new object position
            DistributionField dfCropped = dfFrame.subfield(x,y,width,height);

            ///  update the object model to incorporate new information
            updateModel(dfCropped);

            /// display or print an image, ie. draw a bounding box around the object being tracked
            displayCurrentPosition (images[i], outputPath, i );
        }

    }
}

map<vcl_string,int> DFT::locateObject(void)
{
    return this->_currentPosition;
}

map<vcl_string,int> DFT::locateObject(const DistributionField& df, int maxSearchDist)
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

    map<vcl_string,int> initialPosition = _currentPosition;
    map<vcl_string,int> objectLocation = _currentPosition;


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
            //
            croppedField.saveField();
            //
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

        vcl_cout << bestLocation << vcl_endl;
        vcl_cout << searchDist << vcl_endl;
        vcl_cout << maxSearchDist << vcl_endl;

        // if the best_location is 0, i.e. no motion, then we have reached the end of the search, or
        // if we’ve gone maxSearchDist, we’ve reached the end of our search
        if ( (bestLocation != 0) & (searchDist < maxSearchDist) )
        {
            break;
        }
    }

    _currentPosition = objectLocation;
    return _currentPosition;
}

void DFT::displayCurrentPosition ( vil_image_view<unsigned char> currentFrame, vcl_string outputPath, int frameNum )
{
    //TODO display the image somehow (probably draw a bounding box onto an output image

    int x = _currentPosition["x"];
    int y = _currentPosition["y"];
    int width = _currentPosition["width"];
    int height = _currentPosition["height"];

    // turn the top line of the bounding box to a black line
    for (int i = x; i< x+width+1; i++)
    {
        for (int p = 0; p< currentFrame.nplanes(); p++)
        {
            currentFrame(i,y,p) = 0;
        }
    }
    // turn the bottom line of the bounding box to a black line
    for (int i = x; i< x+width; i++)
    {
        for (int p = 0; p< currentFrame.nplanes(); p++)
        {
            currentFrame(i,y-height,p) = 0;
        }
    }
    // turn the left line of the bounding box to a black line
    for (int i = y-height; i< y+1; i++)
    {
        for (int p = 0; p< currentFrame.nplanes(); p++)
        {
            currentFrame(x,i,p) = 0;
        }
    }
    // turn the bottom line of the bounding box to a black line
    for (int i = y-height; i< y+1; i++)
    {
        for (int p = 0; p < currentFrame.nplanes(); p++)
        {
            currentFrame(x+width,i,p) = 0;
        }
    }

    // Save the image

    /*Use a string stream to convert int to stream*/
    stringstream conv;
    conv << frameNum;
    vcl_string index;
    conv >> index;

    /*Save channel as jpeg*/
    vil_save(currentFrame, vcl_string(vcl_string("frame")+index+vcl_string(".jpeg")).c_str());
}

void DFT::updateModel(DistributionField dfFrame)
{
    // Update the object_model member variable

    int x = _currentPosition["x"];
    int y = _currentPosition["y"];
    int width = _currentPosition["width"];
    int height = _currentPosition["height"];
    // get a cropped copy of the distribution field at the new object position
    DistributionField dfCropped = dfFrame.subfield(x,y,width,height);
    _objectModel.update(dfCropped, _learningRate);
}






