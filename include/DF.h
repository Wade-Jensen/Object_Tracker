#ifndef DF_H_INCLUDED
#define DF_H_INCLUDED

#include <vector>
#include <sstream>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/algo/vil_gauss_filter.h>

using std::vector;
using std::stringstream;

class DF_params{

public:

    friend class DistributionField;
    DF_params(int, int, int, float, float);
    ~DF_params();

private:

    int num_channels;
    int channel_width;
    int blur_spatial;
    int blur_colour;
    float sd_spatial;
    float sd_colour;

    int width;
    int height;


};

class DistributionField{

public:

    /* Constructor & Destructor*/
    DistributionField(vil_image_view<unsigned char>&, DF_params&);
    ~DistributionField();

    int compare(DistributionField&);
    void update(DistributionField&, float);

    /* Save the DF by saving each channel as a JPEG*/
    void saveField();


private:

    /* Init will create the DF
    *  Input Image, Channels, Spatial Blur Size, Colour Blur Size
    */
    void init(vil_image_view<unsigned char>&, DF_params&);
    /*Create DF with Set Parameters*/
    void createField(vil_image_view<unsigned char>&);
    /*Run Colour Clur routine*/
    void colourBlur();

    /*Vector of Images fors the DF*/
    vector< vil_image_view<unsigned char> > dist_field;

    /*Parameters*/
    int num_channels;
    int channel_width;
    int blur_spatial;
    int blur_colour;
    float sd_spatial;
    float sd_colour;

    int width;
    int height;
};

#endif // DF_H_INCLUDED
