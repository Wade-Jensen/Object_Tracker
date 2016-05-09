#ifndef DFT_H
#define DFT_H

#ifndef _VCL_VECTOR_
#define _VCL_VECTOR_
#include <vcl_vector.h>
#endif

#ifndef _VIL_IMAGE_
#define _VIL_IMAGE_
#include <vil/vil_image_view.h>
#endif

class DFT
{
public:
    DFT();
    ~DFT();
protected:
private:
    void trackObject( vcl_vector< vil_image_view<unsigned char> >& images);
    //unknown type getDistributionField();
    //unknown type (same as currentPosition) locateObject( unknown type df frame, unknown type currentPosition);
    //void displayCurrentPosition ( vil_image_view<unsigned char>& image, // unknown type _currentPosition );
    bool _firstFrame; // have we computed // the first frame?
    // unknown type _currentPosition; // the current location and size of the object
    unsigned int _maxSearchDist; ///  max distance to travel in search
    // unknown type (likely object) _objectModel; /// a model of the object being tracked

};

#endif // DFT_H
