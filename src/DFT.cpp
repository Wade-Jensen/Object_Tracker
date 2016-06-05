#include "../include/DFT.h"

#include "..\include\DF.h"

// initialise a default DFT, no arguments
DFT::DFT()
{
    //constructor
}

// initialise a DFT using the first frame of a series,
// the top left pixel location (x,y), width and height of the
// object in the image to be tracked
DFT::DFT(const vil_image_view<unsigned char>& initialFrame, DF_params& params, int x, int y,
         int width, int height, float learningRate, int maxSearchDist /*default value = 0.05*/)
: _learningRate(learningRate) // initialiser list
{
    _currentPosition["x"] = x;
    _currentPosition["y"] = y;
    _currentPosition["width"] = width;
    _currentPosition["height"] = height;
    _model_params = params;
    //_objectModel = initialFrameDF.subfield(x, y, width, height);
    _objectModel = DistributionField(initialFrame, _model_params, x, y, width, height);
    _learningRate = learningRate;
    _maxSearchDist = maxSearchDist;
}

//destructor
DFT::~DFT()
{

}

map<vcl_string,int> DFT::locateObject(void)
{
    return this->_currentPosition;
}

map<vcl_string,int> DFT::locateObject(const vil_image_view<unsigned char>& frame)
{
    // TODO
    // update the currentPosition member variable to the new object position

    // df = distribution field of the current frame
    // objectModel = distribution field of the object being tracked
    // initialPosition = a map which includes the x,y, height and width of initial position of the objectModel
    // maxSearchDist = the maximum distance from the initial location we should search

    int numSearchLocations = 9;
    int searchLocationsX[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
    int searchLocationsY[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};

    /*int numSearchLocations = 5;
    int searchLocationsX[] = {0, 1, 0, -1, 0};
    int searchLocationsY[] = {0, 0, -1, 0, 1};*/

    map<vcl_string,int> initialPosition = _currentPosition;
    map<vcl_string,int> objectLocation = _currentPosition;

    int x0 = objectLocation["x"];
    int y0 = objectLocation["y"];
    int width = objectLocation["width"];
    int height = objectLocation["height"];

    DistributionField searchField = DistributionField(frame, _model_params,
                                                      x0-_maxSearchDist,
                                                      y0-_maxSearchDist,
                                                      2*_maxSearchDist + width,
                                                      2*_maxSearchDist + height);

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

            //int x = (objectLocation["x"])+searchLocationsX[i];
            int x = (objectLocation["x"] - x0 + _maxSearchDist)+searchLocationsX[i];
            //int y = (objectLocation["y"])+searchLocationsY[i];
            int y = (objectLocation["y"] - y0 + _maxSearchDist)+searchLocationsY[i];

            //vector <vil_image_view<unsigned char> > croppedField = cropDF(df,x,y,height,width);

            float distance = 0;
            try{
                //DistributionField croppedField = df.subfield(x, y, width, height);
                /*DistributionField croppedField = DistributionField(frame, _model_params,
                                                                   x, y, width, height);*/

                DistributionField croppedField = searchField.subfield(x, y, width, height);
                //
                //croppedField.saveField();
                //
                //croppedField.saveField();
                distance = _objectModel.compare(croppedField);
            }
            catch(vcl_string ex_msg){

                std::cout << ex_msg;
                distance = 100000000;
            }

            //vcl_cout << "Distance at (" << x << ", " << y << ") is: " << distance << vcl_endl;
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

        /*vcl_cout << "best location is: " << bestLocation << vcl_endl;*/
        //vcl_cout << "search distance is: " << searchDist << vcl_endl;
        //vcl_cout << "maxSearchDist is: " << maxSearchDist << vcl_endl;

        // if the best_location is 0, i.e. no motion, then we have reached the end of the search, or
        // if we�ve gone maxSearchDist, we�ve reached the end of our search
        if ( (bestLocation == 4) || (searchDist >= _maxSearchDist) )
        {
            break;
        }
    }

    _currentPosition = objectLocation;
    //vcl_cout << "Object Location: (" << _currentPosition["x"]  << ", " << _currentPosition["y"] << ")" << vcl_endl;

    return _currentPosition;
}

// Draw a bounding box around the current position of the object
// and save the image to disk
// Takes the current frame, a path to save into, and the frame number
void DFT::displayCurrentPosition ( vil_image_view<unsigned char> currentFrame, vcl_string outputPath, int frameNum )
{
    int x = _currentPosition["x"];
    int y = _currentPosition["y"];
    int width = _currentPosition["width"];
    int height = _currentPosition["height"];

    // turn the top line of the bounding box to a black line
    for (int i = x; i< x+width+1; i++)
    {
        for (int p = 0; p< currentFrame.nplanes(); p++)
        {
            //currentFrame(i,y,p) = 0;
            SafeWrite(currentFrame, i, y, p, 0);
        }
    }
    // turn the bottom line of the bounding box to a black line
    for (int i = x; i< x+width+1; i++)
    {
        for (int p = 0; p< currentFrame.nplanes(); p++)
        {
            //currentFrame(i, y+height, p) = 0;
            SafeWrite(currentFrame, i, y+height, p, 0);
        }
    }
    // turn the left line of the bounding box to a black line
    for (int i = y+height; i > y; i--)
    {
        for (int p = 0; p< currentFrame.nplanes(); p++)
        {
            //currentFrame(x, i, p) = 0;
            SafeWrite(currentFrame, x, i, p, 0);
        }
    }
    // turn the right line of the bounding box to a black line
    for (int i = y+height; i > y; i--)
    {
        for (int p = 0; p < currentFrame.nplanes(); p++)
        {
            //currentFrame(x+width, i, p) = 0;
            SafeWrite(currentFrame, x+width, i, p, 0);
        }
    }

    // Save the image

    // Use a string stream to convert int to stream
    stringstream conv;
    conv << frameNum;
    vcl_string index;
    conv >> index;

    //Save channel as png
    vil_save(currentFrame, vcl_string(vcl_string("frame")+index+vcl_string(".png")).c_str());
}

// Update the object model using the learning rate
void DFT::updateModel(const vil_image_view<unsigned char> frame)
{
    // Update the object_model member variable
    int x = _currentPosition["x"];
    int y = _currentPosition["y"];
    int width = _currentPosition["width"];
    int height = _currentPosition["height"];
    // get a cropped copy of the distribution field at the new object position
    DistributionField dfCropped = DistributionField(frame, _model_params, x, y, width, height);
    _objectModel.update(dfCropped, _learningRate);

}

void DFT::SafeWrite(vil_image_view<unsigned char>& frame, int i, int j, int p,
               unsigned char pix_val)
{

    if((i < frame.ni())&&(j < frame.nj())&&(p < frame.nplanes())){
            frame(i, j, p) = pix_val;
    }
    else{

        int x = _currentPosition["x"];
        int y = _currentPosition["y"];
        int *bad_write = new int[8] {i, j, p, frame.ni(), frame.nj(), frame.nplanes(), x, y};
        throw bad_write;
    }

}






