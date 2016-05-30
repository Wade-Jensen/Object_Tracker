#ifndef DF_H_INCLUDED
#define DF_H_INCLUDED

#include <vector>
#include <sstream>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vil/vil_crop.h>

using std::vector;
using std::stringstream;

/*
 * Class for passing bulk parameters to a DF constructor
*/
class DF_params
{

public:

    /*These are all obvious*/
    friend class DistributionField;
    DF_params(int, int, int, float, float);
    ~DF_params();

private:

    int numChannels;
    int channelWidth;
    int blurSpatial;
    int blurColour;
    float sdSpatial;
    float sdColour;

    int width;
    int height;


};

class DistributionField
{

public:

    /* Constructor & Destructor*/
    DistributionField();
    DistributionField(const vil_image_view<unsigned char>&, DF_params&);
    DistributionField(const DistributionField&, int x, int y, int width, int height);
    ~DistributionField();

    /*Methods for use in DFT
     * Compare returns raw difference, as described in algorithm breakdown
     * Update updates the DF, also as described
     * Subfield returns a sub-DF, contained within this
    */
    float compare(DistributionField&) const;
    void update(DistributionField&, float);
    DistributionField subfield(int, int, int ,int) const;

    /* Save the DF by saving each channel as a JPEG*/
    void saveField();

    vector<vil_image_view<unsigned char> > getDistributionField();

    bool operator!=(const DistributionField&);

private:

    /* Init will create the DF
    *  Input Image, Channels, Spatial Blur Size, Colour Blur Size
    */
    void init(const vil_image_view<unsigned char>&, DF_params&);
    /*Create DF with Set Parameters*/
    void createField(vil_image_view<unsigned char>&); /// NEED THIS TO RETURN SOMETHING
    /*Run Colour Clur routine*/
    void colourBlur();
    vil_image_view<unsigned char> grey(const vil_image_view<unsigned char>&);

    /*Vector of Images fors the DF*/
    vector< vil_image_view<unsigned char> > dist_field;

    /*Parameters*/
    int numChannels;
    int channelWidth;
    int blurSpatial;
    int blurColour;
    float sdSpatial;
    float sdColour;

    int width;
    int height;
    int planes;
};

#endif // DF_H_INCLUDED
