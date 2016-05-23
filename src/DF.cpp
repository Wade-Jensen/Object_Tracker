
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

DistributionField::DistributionField(){
}

DistributionField::DistributionField(DistributionField& SuperF, int pos[2], int size[2]){

    /*Transfer Parameters*/
    num_channels = SuperF.num_channels;
    channel_width = SuperF.channel_width;
    blur_spatial = SuperF.blur_spatial;
    blur_colour = SuperF.blur_colour;
    sd_spatial = SuperF.sd_spatial;
    sd_colour = SuperF.sd_colour;

    width = size[0];
    height = size[1];

    /*For each Channel, save a "sub-channel" using vil_crop*/
    for(int k = 0; k < num_channels; k++){

        dist_field.push_back(vil_crop(SuperF.dist_field[k],
                                      pos[0], size[0],
                                      pos[1], size[1]));
    }

}

DistributionField::DistributionField(vil_image_view<unsigned char>& Input, DF_params& params){

    /*Following RAII*/
    init(Input, params);
}

/*Default, does nothing*/
DistributionField::~DistributionField(){}

void DistributionField::init(vil_image_view<unsigned char>& Input, DF_params& params){

    /* Set Parameters from Arguements*/
    num_channels = params.num_channels;
    channel_width = 256/num_channels;
    blur_spatial = params.blur_spatial;
    blur_colour = params.blur_colour;
    sd_spatial = params.sd_spatial;
    sd_colour = params.sd_colour;

    width = Input.ni();
    height = Input.nj();

    /*Begin DF Creation */
    createField(Input);

}

void DistributionField::createField(vil_image_view<unsigned char>& Input){

    /*Create All Channels - Blank*/
    for(int k = 0; k < num_channels; k++){

        dist_field.push_back(vil_image_view<unsigned char>(width, height, 3, 1));
    }

    /*Pixel-byPixel loop*/
    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){

                /*"Set" this pixel in the approriate channel for Red*/
                int R_channel = Input(i, j, 0)/channel_width;
                dist_field[R_channel](i, j, 0) = 255;

                /*For Blue*/
                int G_channel = Input(i, j, 1)/channel_width;
                dist_field[G_channel](i, j, 1) = 255;

                /*For Green*/
                int B_channel = Input(i, j, 2)/channel_width;
                dist_field[B_channel](i, j, 2) = 255;
        }
    }

    /*Iterate Through Channels*/
    for(int i = 0; i < num_channels; i++){

        /*Blur Channe using a parameter object - essentially a kernel*/
        /*vil_gauss_filter_5tap_params Kernel = vil_gauss_filter_5tap_params(blur_spatial);
        vil_gauss_filter_5tap(dist_field[i], dist_field[i], Kernel);*/

        vil_gauss_filter_2d(dist_field[i], dist_field[i], sd_spatial, blur_spatial);
    }

    /*Run colour blur*/
    colourBlur();

}

void DistributionField::colourBlur(){

    /*Pixel-byPixel loop*/
    for(int i = 0; i < width; i++){
        for(int j = 0; j < height; j++){

            /*Create 1D psuedo-image representing all channels*/
            vil_image_view<unsigned char> colourIn =
                vil_image_view<unsigned char>(num_channels, 1, 3, 1);

            vil_image_view<unsigned char> colourOut =
                vil_image_view<unsigned char>(num_channels, 1, 3, 1);

            /*Fill the input psuedo-image from the DF*/
            for(int k = 0; k < num_channels; k++){

                    unsigned char x = dist_field[k](i, j, 0);
                    colourIn(k, 0, 0) = x;
                    colourIn(k, 0, 1) = dist_field[k](i, j, 1);
                    colourIn(k, 0, 2) = dist_field[k](i, j, 2);
            }

            /*Blur*/
            vil_gauss_filter_1d(colourIn, colourOut, sd_colour, blur_colour);

            /*Copy Results from output psuedo-image back to the DF*/
            for(int k = 0; k < num_channels; k++){

                    dist_field[k](i, j, 0) = colourOut(k, 0, 0);
                    dist_field[k](i, j, 1) = colourOut(k, 0, 1);
                    dist_field[k](i, j, 2) = colourOut(k, 0, 2);
            }

        }
    }



}

int DistributionField::compare(DistributionField& inputDF){

    float distance = 0;

    for(int channel = 0; channel < num_channels; channel++){
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){

                float dR = pow(
                    dist_field[channel](i, j, 0)-inputDF.dist_field[channel](i, j, 0), 2);
                float dG = pow(
                    dist_field[channel](i, j, 1)-inputDF.dist_field[channel](i, j, 1), 2);
                float dB = pow(
                    dist_field[channel](i, j, 2)-inputDF.dist_field[channel](i, j, 2), 2);

                distance += sqrt(dR + dG + dB);
            }
        }
    }

    return distance;

}

void DistributionField::update(DistributionField& inputDF, float learning_rate){

    for(int channel = 0; channel < num_channels; channel++){
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){

                unsigned char R = (1 - learning_rate)*dist_field[channel](i, j, 0)
                                    + learning_rate*inputDF.dist_field[channel](i, j, 0);
                unsigned char G = (1 - learning_rate)*dist_field[channel](i, j, 1)
                                    + learning_rate*inputDF.dist_field[channel](i, j, 1);
                unsigned char B = (1 - learning_rate)*dist_field[channel](i, j, 2)
                                    + learning_rate*inputDF.dist_field[channel](i, j, 2);

                dist_field[channel](i, j, 0) =  R;
                dist_field[channel](i, j, 1) =  G;
                dist_field[channel](i, j, 2) =  B;
            }
        }
    }

}

/*
 * This method will grab a width x height subfield starting at (X, Y)
*/
DistributionField DistributionField::subfield(int X, int Y, int width, int height){

    /*Package Parameters */
    int pos[2] = {X, Y};
    int size[2] = {width, height};

    /*Return constructed sub-field*/
    return DistributionField(*this, pos, size);
}

void DistributionField::saveField(){

    /*Iterate through channels*/
    for(int i = 0; i < num_channels; i++){

        /*Use a string stream to convert int to stream*/
        stringstream conv;
        conv << i;
        vcl_string index;
        conv >> index;

        /*Save channel as jpeg*/
        vil_save(dist_field[i], vcl_string(vcl_string("Channel")+index+vcl_string(".jpeg")).c_str());
    }
}

vector<vil_image_view<unsigned char> > DistributionField::getDistributionField()
{
    return dist_field;
}

DF_params::DF_params(int Num_channels, int Blur_spatial, int Blur_colour, float SD_spatial, float SD_colour){

    num_channels = Num_channels;
    channel_width = 256/num_channels;
    blur_spatial = Blur_spatial;
    blur_colour = Blur_colour;
    sd_spatial = sd_spatial;
    sd_colour = sd_colour;
}

DF_params::~DF_params(){}







