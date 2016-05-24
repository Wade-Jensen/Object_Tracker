#ifndef DFT_H
#define DFT_H

#ifndef _VCL_VECTOR_
#define _VCL_VECTOR_
#include <vcl_vector.h>
using std::vector;
#endif

#ifndef _VIL_IMAGE_
#define _VIL_IMAGE_
#include <vil/vil_image_view.h>
#endif

#ifndef _MAP_
#define _MAP_
#include <map>
using std::map;
#endif

#ifndef _VUL_FILE_
#define _VUL_FILE_
#include <vul/vul_file.h>
#endif

#include "../include/DF.h"

class DFT
{
public:
    DFT();
    ~DFT();
protected:
private:
    void trackObject( vcl_vector< vil_image_view<unsigned char> >& images);
    vector<vil_image_view<unsigned char> > getDistributionField();

    map<vcl_string,int> locateObject(const DistributionField& df, map<vcl_string,int> currentPosition, int maxSearchDist);


    void displayCurrentPosition ( vil_image_view<unsigned char>& image, map<vcl_string,int> currentPosition );
    void updateModel(vector< vil_image_view<unsigned char> > df, map<vcl_string,int> currentPosition);

    bool _firstFrame; // have we computed // the first frame?

    map<vcl_string,int> _currentPosition; // the current location and size of the object
    unsigned int _maxSearchDist; ///  max distance to travel in search
    //vector< vil_image_view<unsigned char> > _objectModel; /// a model of the object being tracked
    DistributionField _objectModel;
};

#endif // DFT_H
