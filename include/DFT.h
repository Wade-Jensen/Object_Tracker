#ifndef DFT_H
#define DFT_H

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
