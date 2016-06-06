#include "../include/DFT.h"

#include "..\include\DF.h"

// Default Constructor
DFT::DFT()
{
}

// Initialise a DFT using the first frame of a series,
// the top left pixel location (x,y), width and height of the
// object in the image to be tracked
DFT::DFT(const vil_image_view<unsigned char>& initialFrame, DF_params& params, int x, int y,
         int width, int height, float learningRate, int maxSearchDist, bool extended)
{
    //Initialise Map
    _currentPosition["x"] = x;
    _currentPosition["y"] = y;
    _currentPosition["width"] = width;
    _currentPosition["height"] = height;
    _model_params = params;

    //Decided on DF or CR depending on whether this DFT is going to be extended
    if(!extended){

        _objectModel = new DistributionField(initialFrame, _model_params, x, y, width, height);
    }
    else
    {
        _objectModel = new ChannelRep(initialFrame, _model_params, x, y, width, height);
        _extended = true;   /* Extended only Parameters */
        _velocityX = 0;
        _velocityY = 0;
    }

    //Further Parameter setting
    _learningRate = learningRate;
    _maxSearchDist = maxSearchDist;
}

//Destructor
DFT::~DFT()
{

    //No manual deaccolation neccessary
}

//Return Current location of the object as a map
map<vcl_string,int> DFT::locateObject(void)
{
    return this->_currentPosition;
}

//Main method for object location, find object in passed frame
map<vcl_string,int> DFT::locateObject(const vil_image_view<unsigned char>& frame)
{

    //We are searching 9 neighboring pixels (including diagonals)
    int numSearchLocations = 9;
    int searchLocationsX[] = {-1, 0, 1, -1, 0, 1, -1, 0, 1};
    int searchLocationsY[] = {-1, -1, -1, 0, 0, 0, 1, 1, 1};

    //Make some locals for local use
    map<vcl_string,int> initialPosition = _currentPosition;
    map<vcl_string,int> objectLocation = _currentPosition;

    //More locals for use
    int x0 = objectLocation["x"];
    int y0 = objectLocation["y"];
    int width = objectLocation["width"];
    int height = objectLocation["height"];

    //Stil experimenting
    DistributionField searchField = DistributionField(frame, _model_params,
                                                      x0-_maxSearchDist,
                                                      y0-_maxSearchDist,
                                                      2*_maxSearchDist + width,
                                                      2*_maxSearchDist + height);
    //DistributionField croppedField;

    //Search must be manually broken by algorithm
    while(true)
    {
        // We need to work out where the best position is in the local neighbourhood
        // an arbitrarily large number
        int bestLocation = 0;
        int minDistance = 9999999;

        //Iterate of neighbor pixels
        for (int i=0; i<numSearchLocations;i++)
        {
            // Compare the model to the image at the current search location
            // note that we need to crop df so that it is the same size as the objectModel
            // Our crop takes a location, a width and a height

            //If we are in EDFT mode, project the location of the object
            if(_extended){
                project();
            }

            //Current search location (x, y)
            int xt = (objectLocation["x"])+searchLocationsX[i];
            int x = (objectLocation["x"] - x0 + _maxSearchDist - 1)+searchLocationsX[i];
            int yt = (objectLocation["y"])+searchLocationsY[i];
            int y = (objectLocation["y"] - y0 + _maxSearchDist - 1)+searchLocationsY[i];

            float distance = 0;
            //Catch Exceptions, which are common here
            try{

                //Create a object size DF at search location
                //Polymorphic pointer
                DistributionField* croppedField;
                if(!_extended)
                    //Get DF for non-extended
                    croppedField = new DistributionField(frame, _model_params, xt, yt, width, height);

                else
                    //Get CR for extended
                    croppedField = new ChannelRep(frame, _model_params, xt, yt, width, height);

                //Calculate "distance" between object model and search location
                //using compare method
                distance = _objectModel->compare(*croppedField);
                delete croppedField;
            }
            //Catch bad search location, make it's distance very large
            catch(vcl_string ex_msg){

                std::cout << ex_msg;
                distance = 100000000;
            }
            //Catch other thrown exceptions
            catch(const char* msg){

                std::cout << msg << "\n";
            }

            //Update best current distance if required
            if (distance < minDistance)
            {
                minDistance = distance;
                bestLocation = i;
            }
        }

        // Update the object location based on this iteration
        objectLocation["x"] += searchLocationsX[bestLocation];
        objectLocation["y"] += searchLocationsY[bestLocation];
        velocityUpdate(objectLocation["x"] - x0, objectLocation["y"] - y0);

        //Check Search Distance (Euclidean)
        float searchDist = sqrt(
                              (objectLocation["x"]-initialPosition["x"])*(objectLocation["x"]-initialPosition["x"])
                              +
                              (objectLocation["y"]-initialPosition["y"])*(objectLocation["y"]-initialPosition["y"])
                              );

        // Ff the best_location is 0, i.e. no motion, then we have reached the end of the search, or
        // If we’ve gone maxSearchDist, we’ve reached the end of our search
        if ( (bestLocation == 4) || (searchDist >= _maxSearchDist) )
        {
            break;
        }
    }

    _currentPosition = objectLocation;

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

    // Use a string stream to convert int to stream
    stringstream conv;
    conv << frameNum;
    vcl_string index;
    conv >> index;

    //Save Image as png
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

    //Grab new location of object
    //Polymorphic pointer
    DistributionField* croppedField;
    if(!_extended)
        //Get DF for non extended
        croppedField = new DistributionField(frame, _model_params, x, y, width, height);

    else
        //Get CR for extended
        croppedField = new ChannelRep(frame, _model_params, x, y, width, height);
    //Update using update method
    _objectModel->update(*croppedField, _learningRate);

}

//MIGHT NOT NEED
void DFT::updateModel(DistributionField& newModel){


    _objectModel->update(newModel, _learningRate);
}

//Update the object velocity (mean) using velocity from last iteration
void DFT::velocityUpdate(int dx, int dy){

    static int i = 0;

    _velocityX = (_velocityX*i + dx)/(i+1);
    _velocityY = (_velocityY*i + dy)/(i+1);
    i++;
}

void DFT::project(){

    //Project next location of object by adding mean velocities
    _currentPosition["x"] += _velocityX;
    _currentPosition["y"] += _velocityY;
}

//Avoid Writing off the edge of the image
void DFT::SafeWrite(vil_image_view<unsigned char>& frame, int i, int j, int p,
               unsigned char pix_val)
{

    //Check all dimensions
    if((i < frame.ni())&&(j < frame.nj())&&(p < frame.nplanes())){
            frame(i, j, p) = pix_val;
    }
    else{

        /*Throw and exception if the pixel does not exist */

        int x = _currentPosition["x"];
        int y = _currentPosition["y"];
        int *bad_write = new int[8] {i, j, p, frame.ni(), frame.nj(), frame.nplanes(), x, y};
        throw bad_write;
    }

}






