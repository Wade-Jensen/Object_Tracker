
#include "..\include\DF.h"
/*typedef vcl_vector< vil_image_view<unsigned char> > image_array;
Contains implementations of all methods for Distribution Field class
*/
extern int passes; // a global variable used only for counting number of passed test cases

// Default constructor - Leave plain
DistributionField::DistributionField()
{
}

//Copy Constructor - DON'T copy field itself, point to it
DistributionField::DistributionField(const DistributionField& SuperF)
{

    numChannels = SuperF.numChannels;
    channelWidth = SuperF.channelWidth;
    blurSpatial = SuperF.blurSpatial;
    blurColour = SuperF.blurColour;
    sdSpatial = SuperF.sdSpatial;
    sdColour = SuperF.sdColour;

    width = SuperF.width;
    height = SuperF.height;
    planes = SuperF.planes;

    di = SuperF.di;
    dj = SuperF.dj;
    dp = SuperF.dp;
    origin = SuperF.origin;

}

//Ranged Constructor, create DF (width x height) starting at (x, y)
DistributionField::DistributionField(const DistributionField& SuperF, int x, int y, int Width, int Height)
{

    //Catch out of range input and throw exception
    if((x+Width > SuperF.width)||
       (y+Height > SuperF.height)||
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

    /*Transfer All Parameters*/
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

    /*For each Channel, save the pointer and iterators*/
    /*We are creating, in essence, a "pointer" field*/
    for(int k = 0; k < numChannels; k++){
        di[k] = SuperF.di[k];
        dj[k] = SuperF.dj[k];
        dp[k] = SuperF.dp[k];
        origin[k] = SuperF.origin[k] + x*di[k] + y*dj[k];
    }

}

DistributionField::DistributionField(const vil_image_view<unsigned char>& Input, DF_params& params,
                                    int x, int y, int width, int height)
{

    /*Following RAII*/

    // Limit crop size so that fields near the border can stil be searched
    if(y < 0) y = 0;    //Lower Limit
    if(x < 0) x = 0;
    if(y + height >= Input.nj()) height = Input.nj() - y - 1;       //Upper Limit
    if(x + width >= Input.ni()) width = Input.ni() - x - 1;

    //Crop
    vil_image_view<unsigned char> window = vil_crop(Input, x, width, y, height);

    //If colour mode has not been selected, convert images to greyscale
    if(!params.colour)
        window = grey(window);

    //Initialise and Create
    init(window, params);
    createField(window);
}

// Default destructor
DistributionField::~DistributionField(){

    //Delete / manually deaccolcate heap memory
    delete origin;
    delete di;
    delete dj;
    delete dp;
}

//Init MEthod
void DistributionField::init(const vil_image_view<unsigned char>& Input, DF_params& params)
{

    /* Set Parameters from Param Argument*/
    numChannels = params.numChannels;
    channelWidth = 256/numChannels;
    blurSpatial = params.blurSpatial;
    blurColour = params.blurColour;
    sdSpatial = params.sdSpatial;
    sdColour = params.sdColour;

    width = Input.ni();
    height = Input.nj();
    planes = Input.nplanes();

    /*Private's are no ready for Creation method */

}

//Actually Field creation
void DistributionField::createField(vil_image_view<unsigned char>& Input)
{

    //Alocate pointer and iterator arrays
    origin = new unsigned char*[numChannels];
    di = new vcl_ptrdiff_t[numChannels];
    dj = new vcl_ptrdiff_t[numChannels];
    dp = new vcl_ptrdiff_t[numChannels];

    /*Create All Channels - Blank*/
    for(int k = 0; k < numChannels; k++){

        vil_image_view<unsigned char> channel =
            vil_image_view<unsigned char>(width, height, planes, 1);
        channel.fill(0);

        //Fill specific iterators
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

    //If the fields do not have matching dimenions, throw exception
    if(inputDF != *this)
    {
        throw "Distribution Field Sizes Do Not Match";
    }

    //Distance counter
    float distance = 0;

    //Iterate Throuch Channels
    for(int channel = 0; channel < numChannels; channel++)
    {
        unsigned char* i0 = origin[channel];
        unsigned char* in_i0 = inputDF.origin[channel];
        //Iterate Through colums
        for(int i = 0; i < width; i++)
        {
            unsigned char* j0 = i0;
            unsigned char* in_j0 = in_i0;
            //Iterate Through Rows
            for(int j = 0; j < height; j++)
            {

                unsigned char* p0 = j0;
                unsigned char* in_p0 = in_j0;

                //Delta of upcoming pixel
                float del = 0;
                //Iterate through colour planes
                for(int p = 0; p < planes; p++)
                {
                    //Build up cartesian distance
                    del += pow(
                        *p0 - *in_p0, 2);

                    p0 += dp[channel];
                    in_p0 += inputDF.dp[channel];

                }
                //Increase distance by contributing of pixel
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

//Update a DF, particularly the object model
void DistributionField::update(DistributionField& inputDF, float learning_rate)
{

    //If the fields do not have matching dimenions, throw exception
    if(inputDF != *this)
    {
        throw "Distribution Field Sizes Do Not Match";
    }

     //Iterate Throuch Channels
    for(int channel = 0; channel < numChannels; channel++)
    {
        unsigned char* i0 = origin[channel];
        unsigned char* in_i0 = inputDF.origin[channel];
        //Iterate Through columns
        for(int i = 0; i < width; i++)
        {
            unsigned char* j0 = i0;
            unsigned char* in_j0 = in_i0;
            //Iterate Through Rows
            for(int j = 0; j < height; j++)
            {

                unsigned char* p0 = j0;
                unsigned char* in_p0 = in_j0;

                //Iterate through colour planes
                for(int p = 0; p < planes; p++)
                {

                    //Calculate Contribution of Previous Pixel
                    float prev = (1 - learning_rate)*(*p0);
                    //Calculate Contribution of new pixel
                    float Update = learning_rate*(*in_p0);

                    //Fuse Contributions
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
    //Find maximum pixel value - pure white
    float scale = sqrt(Input.nplanes())*255;
    //Grey image to be created
    vil_image_view<unsigned char> Grey = vil_image_view<unsigned char>(Input.ni(), Input.nj(), 1, 1);

    //Iterate through al pixels
    for(int i = 0; i < Input.ni(); i++){
        for(int j = 0; j < Input.nj(); j++){

            float mag = 0;
            for(int p = 0; p < Input.nplanes(); p++){

                //Find magnitude of pixel as a 3-vector
                mag += pow(Input(i, j, p), 2);
            }

            //Scalle magnitude to 255, assign as greyscale value
            Grey(i, j, 0) = (255/scale)*sqrt(mag);
        }
    }

    //Return grey image
    return Grey;
}

// Compare distribution field dimensions
bool DistributionField::operator!=(const DistributionField& inputDF)
{

    //Compare of dimensions
    return !(width == inputDF.width&&
            height == inputDF.height&&
            planes == inputDF.planes&&
            numChannels == inputDF.numChannels);
}

// Constructor takes the following parameters which are used
// to define a Distribution Field and stores them:
// Number of channels, Blur_spatial, Blur_colour, SD_spatial, SD_colour
DF_params::DF_params(int Num_channels, int Blur_spatial, int Blur_colour, float SD_spatial,
                     float SD_colour, bool Colour)
{
    numChannels = Num_channels;
    channelWidth = 256/numChannels;
    blurSpatial = Blur_spatial;
    blurColour = Blur_colour;
    sdSpatial = SD_spatial;
    sdColour = SD_colour;
    colour = Colour;
}

//Default construcor - Leave empty
DF_params::DF_params(){}

//Destructor - Nothing needs manual deletion
DF_params::~DF_params()
{
}

//Channel Rep Constructor, same arguements as DF construcotr
ChannelRep::ChannelRep(const vil_image_view<unsigned char>& Input, DF_params& params,
                                    int x, int y, int width, int height){

    vil_image_view<unsigned char> window = vil_crop(Input, x, width, y, height);

    if(!params.colour)
        window = grey(window);

    init(window, params);
    createChannRep(window);
}

//CreateField method modified for create a Channel Representation
void ChannelRep::createChannRep(const vil_image_view<unsigned char>& Input){

    float** LUT = new float*[256];

    for(int i = 0; i < 256; i++){

        LUT[i] = new float[numChannels];
    }

    //Look Up Table Creation
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

    //Identical technique to that used above in CreateFied
    /* --------------------------------------------------*/
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

    /* --------------------------------------------------*/

}

bool DistributionField::testField(int tnumChannels, int twidth, int theight, int tplanes)
{
    assert(dist_field.size()==tnumChannels);
    vcl_cout << "Channels match; ";
	for (int i = 0; i<dist_field.size(); i++)
	{
		assert(dist_field[i].ni()==twidth); passes++;

		assert(dist_field[i].nj()==theight);

		assert(dist_field[i].nplanes()==tplanes);
	}
	vcl_cout << "Width matches; ";
	vcl_cout << "Height matches; ";
    vcl_cout << "Planes match; ";
	return true;
}






