
#include "..\include\DF.h"
/*typedef vcl_vector< vil_image_view<unsigned char> > image_array;
vil_image_view<unsigned char> image; /// the image to extract to a DF
int numChannels; /// The size of the diustribution field
int channelWidth = 256/numChannels;
int spatialBlurSize; /// The amount of spatial blur desired
int colourBlurSize; /// The amount of blur applied to the channel

// make the DF
// create the storage space for the DF
image_array DF = initImageArray( width, height, arrLength);
/// fill the DF (not sure what this means, ie. fill with zeros or fill with images?)
/// after this process, we have a set of channels that indicate exactly where in
/// the source image each colour occurs

image_array SomeClass::initImageArray(int width, int height, int planes int arrLength)
{
    vcl_vector< vil_image_view<unsigned char> > image_vector;
    for (int i=0; i<arrLength; i++)
    {
        image_vector[i] = vil_image_view(width,height,planes);
    }
    return image_vector;
}
*/

// Default constructor
DistributionField::DistributionField()
{
}

DistributionField::DistributionField(const DistributionField& SuperF, int x, int y, int Width, int Height)
{

    if((x+Width >= SuperF.width)||
       (y+Height >= SuperF.height)||
       (x < 0)||(y < 0)
       ){
            stringstream Ex;
            Ex << "Attempt to Create a Subfield of Size ( " << Width << ", "
            << Height << " ) Failed, as it reaches Coordinates ( " << x+Width
            << ", " << y+Height << " ) On a Feild of Size ( " << SuperF.width << ", "
            << SuperF.height << " )\n";

            vcl_string ExStr = Ex.str();
            throw ExStr;
       }

    /*Transfer Parameters*/
    numChannels = SuperF.numChannels;
    channelWidth = SuperF.channelWidth;
    blurSpatial = SuperF.blurSpatial;
    blurColour = SuperF.blurColour;
    sdSpatial = SuperF.sdSpatial;
    sdColour = SuperF.sdColour;

    width = Width;
    height = Height;
    planes = SuperF.planes;

    origin = new unsigned char*[numChannels];
    di = new vcl_ptrdiff_t[numChannels];
    dj = new vcl_ptrdiff_t[numChannels];
    dp = new vcl_ptrdiff_t[numChannels];

    /*For each Channel, save a "sub-channel" using vil_crop*/
    for(int k = 0; k < numChannels; k++){

        /*dist_field.push_back(vil_crop(SuperF.dist_field[k],
                                      x, Width,
                                      y, Height));*/
        di[k] = SuperF.di[k];
        dj[k] = SuperF.dj[k];
        dp[k] = SuperF.dp[k];
        origin[k] = SuperF.origin[k] + x*di[k] + y*dj[k];
    }

}

DistributionField::DistributionField(const vil_image_view<unsigned char>& Input, DF_params& params)
{

    /*Following RAII*/
    init(Input, params);
}

DistributionField::DistributionField(const vil_image_view<unsigned char>& Input, DF_params& params,
                                    int x, int y, int width, int height)
{

    /*Following RAII*/
    vil_image_view<unsigned char> window = vil_crop(Input, x, width, y, height);
    init(window, params);
    createField(window);
}

// Default destructor
DistributionField::~DistributionField(){}

//
void DistributionField::init(const vil_image_view<unsigned char>& Input, DF_params& params)
{

    /* Set Parameters from Arguments*/
    numChannels = params.numChannels;
    channelWidth = 256/numChannels;
    blurSpatial = params.blurSpatial;
    blurColour = params.blurColour;
    sdSpatial = params.sdSpatial;
    sdColour = params.sdColour;

    width = Input.ni();
    height = Input.nj();
    planes = params.planes;

    /*Begin DF Creation */
    //vil_image_view<unsigned char> greyInput = grey(Input);
    //vil_image_view<unsigned char> newIn = Input;
    //createField(newIn);
    //createChannRep(Input);

}

//
void DistributionField::createField(vil_image_view<unsigned char>& Input)
{

    origin = new unsigned char*[numChannels];
    di = new vcl_ptrdiff_t[numChannels];
    dj = new vcl_ptrdiff_t[numChannels];
    dp = new vcl_ptrdiff_t[numChannels];

    /*Create All Channels - Blank*/
    for(int k = 0; k < numChannels; k++){

        vil_image_view<unsigned char> channel =
            vil_image_view<unsigned char>(width, height, planes, 1);
        channel.fill(0);

        dist_field.push_back(channel);
        origin[k] = dist_field[k].top_left_ptr();
        di[k] = dist_field[k].istep();
        dj[k] = dist_field[k].jstep();
        dp[k] = dist_field[k].planestep();
    }

    /*Pixel-byPixel loop*/
    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){
            for(int p = 0; p < planes; p++){
                /*"Set" this pixel in the approriate channel for all colours*/
                int channel = Input(i, j, p)/channelWidth;
                dist_field[channel](i, j, p) = 255;
            }
        }
    }

    /*Iterate Through Channels*/
    for(int i = 0; i < numChannels; i++)
    {

        /*Blur Channe using a parameter object - essentially a kernel*/
        /*vil_gauss_filter_5tap_params Kernel = vil_gauss_filter_5tap_params(blur_spatial);
        vil_gauss_filter_5tap(dist_field[i], dist_field[i], Kernel);*/

        vil_gauss_filter_2d(dist_field[i], dist_field[i], sdSpatial, blurSpatial);
    }

    /*Run colour blur*/
    colourBlur();

}

// Colour Blur routine
void DistributionField::colourBlur()
{

    /*Pixel-byPixel loop*/
    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){

            /*Create 1D psuedo-image representing all channels*/
            vil_image_view<unsigned char> colourLine =
                vil_image_view<unsigned char>(numChannels, 1, planes, 1);

            /*Fill the input psuedo-image from the DF*/
            for(int k = 0; k < numChannels; k++){
                for(int p = 0; p < planes; p++){
                    colourLine(k, 0, p) = dist_field[k](i, j, p);

                }
            }

            /*Blur*/
            vil_gauss_filter_1d(colourLine, colourLine, sdColour, blurColour);

            /*Copy Results from output psuedo-image back to the DF*/
            for(int k = 0; k < numChannels; k++){
                for(int p = 0; p < planes; p++){
                    dist_field[k](i, j, p) = colourLine(k, 0, p);

                }
            }

        }
    }



}

// Determine if the contents of two distribution fields are the same
float DistributionField::compare(DistributionField& inputDF) const
{

    if(inputDF != *this)
    {
        //throw 0;
        throw "Distribution Field Sizes Do Not Match";
    }


    float distance = 0;

    for(int channel = 0; channel < numChannels; channel++)
    {
        unsigned char* i0 = origin[channel];
        unsigned char* in_i0 = inputDF.origin[channel];
        for(int i = 0; i < width; i++)
        {
            unsigned char* j0 = i0;
            unsigned char* in_j0 = in_i0;
            for(int j = 0; j < height; j++)
            {

                unsigned char* p0 = j0;
                unsigned char* in_p0 = in_j0;

                float del = 0;
                for(int p = 0; p < planes; p++)
                {

                    del += pow(
                        *p0 - *in_p0, 2);

                    p0 += dp[channel];
                    in_p0 += inputDF.dp[channel];

                }
                distance += sqrt(del);

                j0 += dj[channel];
                in_j0 += inputDF.dj[channel];
            }

            i0 += di[channel];
            in_i0 += inputDF.di[channel];
        }
    }

    return distance;

}

//
void DistributionField::update(DistributionField& inputDF, float learning_rate)
{

    if(inputDF != *this)
    {
        //throw 0;
        throw "Distribution Field Sizes Do Not Match";
    }

    /*for(int channel = 0; channel < numChannels; channel++)
    {
        unsigned char* i0 = origin[channel];
        unsigned char* in_i0 = inputDF.origin[channel];
        for(int i = 0; i < width; i++)
        {
            unsigned char* j0 = i0;
            unsigned char* in_j0 = in_i0;
            for(int j = 0; j < height; j++)
            {
                unsigned char* p0 = j0;
                unsigned char* in_p0 = in_j0;
                for(int p = 0; p < planes; p++)
                {
                    float prev = (1 - learning_rate)*(*p0);
                    float Update = learning_rate*(*in_p0);

                    /* *p0dist_field[channel](i, j, p) = round(prev + Update);

                    p0 += dp[channel];
                    in_p0 += inputDF.dp[channel];
                }
            }

            j0 += dj[channel];
            in_j0 += inputDF.dj[channel];
        }

        i0 += di[channel];
        in_i0 += inputDF.di[channel];
    } */

    for(int channel = 0; channel < numChannels; channel++)
    {
        unsigned char* i0 = origin[channel];
        unsigned char* in_i0 = inputDF.origin[channel];
        for(int i = 0; i < width; i++)
        {
            unsigned char* j0 = i0;
            unsigned char* in_j0 = in_i0;
            for(int j = 0; j < height; j++)
            {

                unsigned char* p0 = j0;
                unsigned char* in_p0 = in_j0;

                float del = 0;
                for(int p = 0; p < planes; p++)
                {

                    float prev = (1 - learning_rate)*(*p0);
                    float Update = learning_rate*(*in_p0);

                    *p0 = round(prev + Update);

                    p0 += dp[channel];
                    in_p0 += inputDF.dp[channel];

                }

                j0 += dj[channel];
                in_j0 += inputDF.dj[channel];
            }

            i0 += di[channel];
            in_i0 += inputDF.di[channel];
        }
    }

}

/*
 * This method will grab a (width x height) subfield starting at (X, Y)
*/
DistributionField DistributionField::subfield(int X, int Y, int Width, int Height) const
{

    // /*Package Parameters */
    //int pos[2] = {X, Y};
    //int size[2] = {width, height};



    /*Return constructed sub-field based on smaller window of DF*/
    return DistributionField(*this, X, Y, Width, Height);
}

// Save the DF by saving each channel as a JPEG
void DistributionField::saveField()
{

    /*Iterate through channels*/
    for (int i = 0; i < numChannels; i++)
    {

        /*Use a string stream to convert int to stream*/
        stringstream conv;
        conv << i;
        vcl_string index;
        conv >> index;

        /*Save channel as jpeg*/
        vil_save(dist_field[i], vcl_string(vcl_string("Channel")+index+vcl_string(".jpeg")).c_str());
    }
}

// Convert a colour image to greyscale
vil_image_view<unsigned char> DistributionField::grey(const vil_image_view<unsigned char>& Input)
{
    float scale = sqrt(Input.nplanes())*sqrt(pow(255, 2));
    vil_image_view<unsigned char> Grey = vil_image_view<unsigned char>(width, height, 1, 1);

    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){

            float mag = 0;
            for(int p = 0; p < Input.nplanes(); p++){

                mag += pow(Input(i, j, p), 2);
            }

            Grey(i, j, 0) = (255/scale)*sqrt(mag);
        }
    }

    return Grey;
}

// Compare distribution field dimensions
bool DistributionField::operator!=(const DistributionField& inputDF)
{

    return !(width == inputDF.width&&
            height == inputDF.height&&
            planes == inputDF.planes&&
            numChannels == inputDF.numChannels);
}

// constructor, takes the following parameters which are used
// to define a Distribution Field and stores them:
// Number of channels, Blur_spatial, Blur_colour, SD_spatial, SD_colour
DF_params::DF_params(int Num_channels, int Blur_spatial, int Blur_colour, float SD_spatial, float SD_colour, int planes)
{
    numChannels = Num_channels;
    channelWidth = 256/numChannels;
    blurSpatial = Blur_spatial;
    blurColour = Blur_colour;
    sdSpatial = SD_spatial;
    sdColour = SD_colour;
    this->planes = planes;
}

DF_params::DF_params(){}

DF_params::~DF_params()
{
}

ChannelRep::ChannelRep(const vil_image_view<unsigned char>& Input, DF_params& params,
                                    int x, int y, int width, int height){

    vil_image_view<unsigned char> window = vil_crop(Input, x, width, y, height);
    init(window, params);
    createChannRep(window);
}

void ChannelRep::createChannRep(const vil_image_view<unsigned char>& Input){

    float** LUT = new float*[256];

    for(int i = 0; i < 256; i++){

        LUT[i] = new float[numChannels];
    }

    for(int i = 0; i < 256; i++){
        for(int j = 0; j < numChannels; j++){

            float dist = fabs((j + 1)*channelWidth/2 - i);

            if(dist <= channelWidth/2){
                LUT[i][j] = 0.75 - pow(dist/channelWidth, 2);
            }
            else if(dist <= 1.5*channelWidth){
                LUT[i][j] = pow((dist/channelWidth - 1.5), 2)/2;
            }
            else{
                LUT[i][j] = 0;
            }

        }
    }

    origin = new unsigned char*[numChannels];
    di = new vcl_ptrdiff_t[numChannels];
    dj = new vcl_ptrdiff_t[numChannels];
    dp = new vcl_ptrdiff_t[numChannels];

    for(int k = 0; k < numChannels; k++){

        vil_image_view<unsigned char> channel =
            vil_image_view<unsigned char>(width, height, planes, 1);
        channel.fill(0);

        dist_field.push_back(channel);
        origin[k] = dist_field[k].top_left_ptr();
        di[k] = dist_field[k].istep();
        dj[k] = dist_field[k].jstep();
        dp[k] = dist_field[k].planestep();
    }


    /*Pixel-byPixel loop*/
    for(int channel = 0; channel < numChannels; channel++){
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                for(int p = 0; p < planes; p++){
                    dist_field[channel](i, j, p) = 255*LUT[Input(i, j, p)][channel];
                }
            }
        }
    }

    /*Iterate Through Channels*/
    for(int i = 0; i < numChannels; i++)
    {

        /*Blur Channe using a parameter object - essentially a kernel*/
        /*vil_gauss_filter_5tap_params Kernel = vil_gauss_filter_5tap_params(blur_spatial);
        vil_gauss_filter_5tap(dist_field[i], dist_field[i], Kernel);*/

        vil_gauss_filter_2d(dist_field[i], dist_field[i], sdSpatial, blurSpatial);
    }

}






