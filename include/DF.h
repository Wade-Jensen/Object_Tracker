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

// Class for passing bulk parameters to a DF constructor
class DF_params
{

public:

    friend class DistributionField;

	// constructor, takes the following parameters which are used
	// to define a Distribution Field:
	// Number of channels, Blur_spatial, Blur_colour, SD_spatial, SD_colour
    DF_params(int, int, int, float, float, int);

	// Destructor
    ~DF_params();

private:

	// Distribution Field parameters
    int numChannels;
    int channelWidth;
    int blurSpatial;
    int blurColour;
    float sdSpatial;
    float sdColour;
    int planes;

    int width;
    int height;

};

// Distribution Field Class
class DistributionField
{

public:

    // Constructors & Destructor
    DistributionField();

	// Create using image and parameters
    DistributionField(const vil_image_view<unsigned char>&, DF_params&);
    DistributionField(const vil_image_view<unsigned char>&, DF_params&,
                                    int, int, int, int);

	// Create based on an existing distribution field and a bounding region
	// for the tracked object
    DistributionField(const DistributionField&, int x, int y, int width, int height);

    ~DistributionField();

    /*Member functions for use in DFT
      Compare returns raw difference, as described in algorithm breakdown
      Update updates the DF, also as described
      Subfield returns a sub-DF, contained within this
    */
    float compare(DistributionField&) const;
    void update(DistributionField&, float);
    DistributionField subfield(int, int, int ,int) const;

    // Save the DF by saving each channel as a JPEG
    void saveField();

	// Compare distribution field dimensions
    bool operator!=(const DistributionField&);

private:

    /* Init will create the DF
    *  Input Image, Channels, Spatial Blur Size, Colour Blur Size
    */
    void init(const vil_image_view<unsigned char>&, DF_params&);

    /*Create DF with Set Parameters*/
    void createField(vil_image_view<unsigned char>&);
    void createChannRep(const vil_image_view<unsigned char>&);

    /*Run Colour Blur routine*/
    void colourBlur();

	// Convert a colour image to greyscale
    vil_image_view<unsigned char> grey(const vil_image_view<unsigned char>&);

    /*Vector of Images for the DF*/
    vector< vil_image_view<unsigned char> > dist_field;
    unsigned char** origin;
    vcl_ptrdiff_t* di;
    vcl_ptrdiff_t* dj;
    vcl_ptrdiff_t* dp;

    /*Parameters*/
    int numChannels;
    int channelWidth;
    int blurSpatial;
    int blurColour;
    float sdSpatial;
    float sdColour;
    int planes;

    int width;
    int height;

};

class SubField : public DistributionField{



};

#endif // DF_H_INCLUDED
