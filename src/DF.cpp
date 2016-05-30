
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

    /*For each Channel, save a "sub-channel" using vil_crop*/
    for(int k = 0; k < numChannels; k++){

        dist_field.push_back(vil_crop(SuperF.dist_field[k],
                                      x, Width,
                                      y, Height));
    }

}

DistributionField::DistributionField(const vil_image_view<unsigned char>& Input, DF_params& params)
{

    /*Following RAII*/
    init(Input, params);
}

/*Default, does nothing*/
DistributionField::~DistributionField(){}

void DistributionField::init(const vil_image_view<unsigned char>& Input, DF_params& params)
{

    /* Set Parameters from Arguements*/
    numChannels = params.numChannels;
    channelWidth = 256/numChannels;
    blurSpatial = params.blurSpatial;
    blurColour = params.blurColour;
    sdSpatial = params.sdSpatial;
    sdColour = params.sdColour;

    width = Input.ni();
    height = Input.nj();
    planes = 1;

    /*Begin DF Creation */
    vil_image_view<unsigned char> greyInput = grey(Input);
    createField(greyInput);

}

void DistributionField::createField(vil_image_view<unsigned char>& Input)
{

    /*Create All Channels - Blank*/
    for(int k = 0; k < numChannels; k++){

        vil_image_view<unsigned char> channel =
            vil_image_view<unsigned char>(width, height, planes, 1);
        channel.fill(0);
        dist_field.push_back(channel);
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
        for(int i = 0; i < width; i++)
        {
            for(int j = 0; j < height; j++)
            {

                float del = 0;
                for(int p = 0; p < planes; p++)
                {

                    del += pow(
                        dist_field[channel](i, j, p)-inputDF.dist_field[channel](i, j, p), 2);
                }
                distance += sqrt(del);
            }
        }
    }

    return distance;

}

void DistributionField::update(DistributionField& inputDF, float learning_rate)
{

    if(inputDF != *this)
    {
        //throw 0;
        throw "Distribution Field Sizes Do Not Match";
    }

    for(int channel = 0; channel < numChannels; channel++)
    {
        for(int i = 0; i < width; i++)
        {
            for(int j = 0; j < height; j++)
            {
                for(int p = 0; p < planes; p++)
                {
                    int PIX = dist_field[channel](i, j, p);
                    float prev = (1 - learning_rate)*dist_field[channel](i, j, p);
                    float Update = learning_rate*inputDF.dist_field[channel](i, j, p);

                    dist_field[channel](i, j, p) = round(prev + Update);
                    PIX = dist_field[channel](i, j, p) = round(prev + Update);
                    PIX = dist_field[channel](i, j, p);
                    PIX = 0;
                }
            }
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

bool DistributionField::operator!=(const DistributionField& inputDF)
{

    return !(width == inputDF.width&&
            height == inputDF.height&&
            planes == inputDF.planes&&
            numChannels == inputDF.numChannels);
}

vector<vil_image_view<unsigned char> > DistributionField::getDistributionField()
{
    return dist_field;
}

DF_params::DF_params(int Num_channels, int Blur_spatial, int Blur_colour, float SD_spatial, float SD_colour)
{

    numChannels = Num_channels;
    channelWidth = 256/numChannels;
    blurSpatial = Blur_spatial;
    blurColour = Blur_colour;
    sdSpatial = SD_spatial;
    sdColour = SD_colour;
}

DF_params::~DF_params()
{
}







