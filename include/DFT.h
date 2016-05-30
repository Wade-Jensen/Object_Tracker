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
    // create a method for initialising the DFT with what we need it to have
    DFT();
    DFT(const DistributionField& initialFrameDF , int x, int y, int width, int height, float /*, possible default values for optional arguments in the consturctor*/ );
    ~DFT();

    map<vcl_string,int> locateObject(const DistributionField& df, int maxSearchDist);
    map<vcl_string,int> locateObject(void);
    void updateModel( DistributionField& currentFrame );
    void displayCurrentPosition ( vil_image_view<unsigned char> image, vcl_string outputPath, int frameNum );

protected:
private:
    void trackObject( vcl_vector< vil_image_view<unsigned char> >& images);
    void SafeWrite(vil_image_view<unsigned char>&, int, int, int, unsigned char);
    vector<vil_image_view<unsigned char> > getDistributionField();



    bool _firstFrame; // have we computed // the first frame?

    map<vcl_string,int> _currentPosition; // the current location and size of the object
    unsigned int _maxSearchDist; ///  max distance to travel in search
    //vector< vil_image_view<unsigned char> > _objectModel; /// a model of the object being tracked
    DistributionField _objectModel;

    vcl_string _outputPath;
    float _learningRate;
};

#endif // DFT_H
