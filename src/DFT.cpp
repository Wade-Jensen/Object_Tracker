#include "../include/DFT.h"

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
    for (int i=0; i<images.size(); i++)
    {
        /// get a distribution field of the current frame
        //unknown type df = getDistributionField(images[i]);
        /// if this is the first frame, we need to build the model before we can track it
        if (_firstFrame)
        {
            /// create the model from the distribution field, and the current position
            //unknown type objectModel = buildDistributionField( //unknown type df, // unknown type current_position );
            // the model is initialised, so set first frame var to false
            _firstFrame = false; /// not the first frame, so we can track the object
        }
        else
        {
            /// locate the object in the current frame. Use gradient descent search
            /// to find the new object position
            //_currentPosition = locateObject( unknown type df frame, // unknown type _currentPosition, _objectModel, _maxSearchDist );
        }
        //  update the object model to incorporate new information
        //_objectModel = updateModel( //unknown type df, // unknown type current_position);

        /// display or print an image, ie. draw a bounding box around the object being tracked
        //displayCurrentPosition (images[i], // unknown type _currentPosition );
    }
}



//unknown type DFT::getDistributionField( vil_image_view<unsigned char>& image)
//{

//}



