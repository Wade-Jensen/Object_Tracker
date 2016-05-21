#ifndef DF_H_INCLUDED
#define DF_H_INCLUDED

#include <vector>
#include <sstream>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/algo/vil_gauss_filter.h>

using std::vector;
using std::stringstream;

class DistributionField{

public:

    /* Constructor & Destructor*/
    DistributionField();
    ~DistributionField();

    /* Init will create the DF
    *  Input Image, Channels, Spatial Blur Size, Colour Blur Size
    */
    void init(vil_image_view<unsigned char>&, int, int, int);
    /* Save the DF by saving each channel as a JPEG*/
    void saveField();

    vector<vil_image_view<unsigned char> > getDistributionField();

private:

    /*Create DF with Set Parameters*/
    void createField(vil_image_view<unsigned char>&); /// NEED THIS TO RETURN SOMETHING
    /*Run Colour Clur routine*/
    void colourBlur();

    /*Vector of Images fors the DF*/
    vector< vil_image_view<unsigned char> > dist_field;

    /*Parameters*/
    int num_channels;
    int channel_width;
    int blur_spatial;
    int blur_colour;

    int width;
    int height;
};

#endif // DF_H_INCLUDED
