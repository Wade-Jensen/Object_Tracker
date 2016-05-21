#include "../include/DFT.h"

#include "..\include\DF.h"

#ifndef _VCL_VECTOR_
#define _VCL_VECTOR_
#include <vcl_vector.h>
#endif

DFT::DFT()
{
    //ctor
}

DFT::~DFT()
{
    //dtor
}

void DFT::trackObject( vcl_vector< vil_image_view<unsigned char> >& images )
{
    /// placeholders until we work out how to pass in these variables
    int numChannels;
    int spatialBlurSize;
    int colourBlurSize;


    for (int i=0; i<images.size(); i++)
    {
        DistributionField dfObj;
        dfObj = DistributionField();

        /// if this is the first frame, we need to build the model before we can track it
        if (_firstFrame)
        {
            /// create the model from the distribution field, and the current position

            /// setup and generate the distribution field


            dfObj.init(images[i],numChannels,spatialBlurSize,colourBlurSize);

            _firstFrame = false; /// not the first frame, so we can track the object
        }
        if (!_firstFrame)
        {
            /// return the distribution field of the current frame
            vector<vil_image_view<unsigned char> > df = dfObj.getDistributionField();

            /// locate the object in the current frame. Use gradient descent search
            /// to find the new object position
            //locateObject( df, _currentPosition, _objectModel, _maxSearchDist );

            ///  update the object model to incorporate new information
            updateModel( df, _currentPosition);

            /// display or print an image, ie. draw a bounding box around the object being tracked
            displayCurrentPosition (images[i], _currentPosition );
        }

    }
}
                                                                                                      /// I THINK THIS IS THE RIGHT TYPE (NOT SURE)
void DFT::locateObject( vector< vil_image_view<unsigned char> > df, map<vcl_string,int> currentPosition, vector<vil_image_view<unsigned char> > objectModel, int maxSearchDist)
{
    // TODO
    // update the currentPosition member variable to the new object position
}

void DFT::displayCurrentPosition ( vil_image_view<unsigned char>& currentFrame, map<vcl_string,int> currentPosition )
{
    //TODO display the image somehow (probably draw a bounding box onto an output image
}

void DFT::updateModel(vector< vil_image_view<unsigned char> > df, map<vcl_string,int> currentPosition)
{
    // Update the object_model member variable
}



