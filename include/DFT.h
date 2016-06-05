#ifndef DFT_H
#define DFT_H

// include a bunch of stuff for VXL
#include <vcl_vector.h>
#include <vector>
#include <vil/vil_image_view.h>
#include <map>
using std::map;

#ifndef _VUL_FILE_
#define _VUL_FILE_
#include <vul/vul_file.h>
#endif _VUL_FILE_

#include <vil/vil_save.h>
#include "../include/DF.h"

class DFT
{
public:
    // initialise a default DFT, no arguments
    DFT();

	// initialise a DFT using the first frame of a series,
	// the top left pixel location (x,y), width and height of the
	// object in the image to be tracked
    DFT(const vil_image_view<unsigned char>& initialFrame, DF_params& params,
        int x, int y, int width, int height, float learningRate, int maxSearchDist/*, possible default values for optional arguments in the constructor*/ );

	// destructor
    ~DFT();

    map<vcl_string,int> locateObject(const vil_image_view<unsigned char>&);

    map<vcl_string,int> locateObject(void);

	// Update the object model using the learning rate
    void updateModel(const vil_image_view<unsigned char> frame);

	// Draw a box around the current position of the object
	// and save the image to disk
	// Takes the current frame, a path to save into, and the frame number
    void displayCurrentPosition ( vil_image_view<unsigned char> image, vcl_string outputPath, int frameNum );

private:

    void SafeWrite(vil_image_view<unsigned char>&, int, int, int, unsigned char);

    //*Parameters*//
    DF_params _model_params;
    map<vcl_string,int> _currentPosition; // the current location and size of the object
    DistributionField _objectModel; // model of the object to be tracked
    float _learningRate; // rate at which to update the object model
    int _maxSearchDist; // the maximum cartesian distance of the object position from one frame to the next
};

#endif // DFT_H
