#include "..\include\inputParams.h"
#include "..\include\Version.h"
#include "..\include\DFT.h"


#ifdef RELEASE

#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vul/vul_file_iterator.h>

#ifndef _VUL_FILE_
#define _VUL_FILE_
#include <vul/vul_file.h>
#endif

#include <vul/vul_arg.h>

/*
 * Main program to run the Object Tracker algorithm.
 * This program will
 *  - take a list of images on the command line, specified as a path and a partial filename (i.e. *.png to get all png files)
 *  - take a set of parameters for the ViBe segmenter from the command line, if no value is given defaults will be used
 *  - will save a set of images showing the motion segmented output
 *  - will optionally compute performance metrics using a given ground truth image and index
 */
using namespace std;
bool textParamsReader(char seperator);

vcl_vector<vcl_string>  generateFileNames(vcl_string directory, vcl_string extension );
int main (int argc, char * argv[])
{
    inputParams params;// input Parameters

    vul_arg<vcl_string>
		arg_in_path("-path", "Path to Image Frames, e.g. C:/somefiles/"),
		arg_in_glob("-glob", "Image (Frame) Extension (png,jpg,bmp,tiff,jpeg), e.g. *.jpeg, this will get all jpeg's.");

	// now we have some integer arguments
	vul_arg<unsigned int> arg_ipx("-ipx", "Initial Position x .", -1),
                          arg_ipy("-ipy", "Initial Position y.", -1),
                          arg_w("-w", "Width.", -1),
                          arg_h("-h", "Heignt", -1),
                          arg_c("-c", "No Of Channels", -1),
                          arg_sb("-sb", "Spatial Blur", -1),
                          arg_bc("-bc", "Blur Colour", -1),
                          arg_sd("-sd", "Max Search Distance", -1),
                          arg_planes("-planes", "Number of colour planes in DF", -1);

    // float arguments
	vul_arg<float> arg_lr("-lr", "Learning Rate", -1),
                   arg_sds("-sds", "SD Spatial", -1),
                   arg_sdc("-sdc", "SD Colour", -1);

    // last string argument
    vul_arg<vcl_string> arg_odir("-odir", "Output Frames Storage Director", "Output");
	vul_arg_parse(argc, argv, true);

	if(((arg_in_path() == "") || (arg_in_glob() == ""))) // command line args are not passed perfectly
    {
        vcl_cout << "Not sufficient or incorrrect argument parameters detected..." <<vcl_endl;
        vcl_cout <<"Utilizing parameter text file method to obtain input parameters"<<endl;
        vcl_cout << "Checking parameter text file for parameters..." <<vcl_endl;
        bool parametersExtracted = params.parseTxt();
        if (parametersExtracted == false) // failed to capture all parameters
            {
            vcl_cout << "Text file doesn't contain valid parameters, exiting" <<vcl_endl;
            return 0;
            }
    }

    // Parsing a directory of images from command line argument
	else // command line arguments are passed correctly
    {
        vcl_vector<vcl_string> filenames;

        // create file list
        for (vul_file_iterator fn=(arg_in_path() + "/*" + arg_in_glob()); fn; ++fn)
        {
            if (!vul_file::is_directory(fn()))
            {
                filenames.push_back (fn());
            }
        }

        // check input files and for valid pixel positions
        if (filenames.size() == 0 || arg_ipy()<= 0|| arg_ipx()<=0 || arg_sd()<= 0 || arg_lr()<=0)
        {
            vcl_cout << "No input files at given path OR parameters are incorrect" << vcl_endl;
            vcl_cout << "Exiting from program"<< endl;
            return 0;
        }

        else // parameters given by command line Argument
        {
            // calling InputParams Class Constructor
            params.initInputParams(filenames,  arg_ipx(),  arg_ipy(),  arg_w(),  arg_h(),  arg_c(),  arg_sb(),  arg_bc(),  arg_sd(), arg_planes(), arg_lr(),  arg_sds(),  arg_sdc(), arg_odir());
            vcl_cout << "There are " << filenames.size() <<" frames in the selected directory"<< vcl_endl;
            vcl_cout << "Input Parameters Initialized By Command Line Arguments";
        }
    }

// delete &params;

// Uncomment to print statements checking correct read in of parameters:
/*
vcl_cout << "ipx:"<<params.ipx<<"; "<<x <<endl;
vcl_cout << "ipy:"<<params.ipy<<"; "<< y<<endl;
vcl_cout << "width:"<<params.w<<"; "<< width<<endl;
vcl_cout << "height:"<<params.h<<"; "<< height <<endl;
vcl_cout << "numChannels:"<<params.c<<"; "<<numChannels<<endl;
vcl_cout << "blurSpatial:"<<params.sb<<"; "<<blurSpatial <<endl;
vcl_cout << "blurColour:"<<params.bc<<"; "<< blurColour <<endl;
vcl_cout << "sdSpatial:"<<params.sds<<"; "<< sdSpatial <<endl;
vcl_cout << "sdColour:"<<params.sdc<<"; "<< sdColour <<endl;
vcl_cout << "maxSearchDist:"<<params.sd<<"; "<< maxSearchDist<<endl;
vcl_cout << "learningRate:"<<params.lr<<"; "<< learningRate<<endl;
*/

    vul_file vulStruct;

    // Print current working directory
    vcl_string dir = vulStruct.get_cwd();
    vcl_cout << dir << vcl_endl;

    // Top corner and size of object in first image to track
    int x = params.ipx;
    int y = params.ipy;
    int width = params.w;
    int height = params.h;

	// Distribution Field parameters
    int numChannels = params.c;
    int blurSpatial = params.sb;
    int blurColour = params.bc;
    float sdSpatial = params.sds;
    float sdColour = params.sdc;
    int planes = params.planes;

   	// Tracker parameters
    int maxSearchDist = params.sd;
    float learningRate = params.lr;

    //output Path
    vcl_string outputPath = params.odir;

    //loading images
    vcl_vector< vil_image_view<unsigned char> > images;
    vcl_vector<vcl_string> filenames = params.filenames;

    // filenames should now contain the names of all the files with our target extension
	// in the input directory, if we want to loop through them, we can now do
	for (int i = 0; i < filenames.size(); i++)
	{
        vcl_cout << filenames[i].c_str() << vcl_endl;
		// load the image file into the vector of images:
		images.push_back( vil_load(filenames[i].c_str()) );
	}

	// save the distribution field parameters
    DF_params default_params = DF_params(numChannels, blurSpatial, blurColour, sdSpatial,
                                          sdColour, planes);

    // The first frame is used to build the object model before we can track it
    static const DistributionField initFrame = DistributionField(images[0], default_params);

    DistributionField saveFrame = DistributionField(images[0], default_params);
    // output jpeg images of the distribution field
    saveFrame.saveField();

    // create the object model for tracking
    DFT DFTracker;
    DFTracker = DFT(initFrame, x, y, width, height, learningRate);

    // create output path if it does not exist
    if (outputPath.c_str() != "")
    {
        if (!vul_file::is_directory(outputPath.c_str()))
        {
            vul_file::make_directory(outputPath.c_str());
        }
    }

    // move the working directory to the output path, print to confirm
    vul_file::change_directory(outputPath.c_str());
    dir = vulStruct.get_cwd();
    vcl_cout << "Working directory is now: " << dir << vcl_endl;

    // This is the main loop that tracks the object through the image sequence
    try{
        for (int i=0; i<images.size(); i++)
        {
            vcl_cout << "Current frame is: "<< i << vcl_endl;

            // create distribution field for the current frame
            DistributionField dfFrame = DistributionField(images[i], default_params);

            // locate the object in the current frame. Use gradient descent search
            // to find the new object position
            map<vcl_string,int> currentPosition = DFTracker.locateObject( dfFrame, maxSearchDist );
            int x = currentPosition["x"];
            int y = currentPosition["y"];

            //  update the object model to incorporate new information
            DFTracker.updateModel(dfFrame);

            // display or print an image, ie. draw a bounding box around the object being tracked
            DFTracker.displayCurrentPosition (images[i], outputPath, i );

        }
    }
    catch(int bad_write[6]){
        std::cout << "Attempted Writing to pixel (" << bad_write[0] << "," <<
        bad_write[1] << "," << bad_write[2] << ")";
        std::cout << " Failed as frame is of size (" << bad_write[3] << "," <<
        bad_write[4] << "," << bad_write[5] << ")\n";
        std::cout << "Object Position at Throw Time: (" << bad_write[6] << ", "
        << bad_write[7] << ")\n";

        delete bad_write;
    }

/*
   //bool first_frame; // have we computed // the first frame?
    // unknown type current_position = initial_position /// objects current // position, initial // position is // provided by the
                                                        /// Needs to store a location and size
                                                        /// provided by the user
    vcl_vector< vil_image_view<unsigned char> > images; /// we track the object over a list of images provided by the user
                                                    /// this shall be a vector of vil_image_view's
    // unknown type object_model; // a model of the DF of the image (sequence?)
    unsigned int maxSearchDist;  ///  max distance to travel in search (user provided)












                                //vil_image_view<unsigned char> test= vil_load("Data/Sequence1/groundtruth.bmp");

                                //vcl_cout << "HelloWorld!" << vcl_endl;
                                //double test_var = test(100,100,0);
                                //vcl_cout <<  test_var << vcl_endl;

	/// specify all the arguments you expect/need
	/// group them according to type, i.e. all the strings first here, so we have vul_arg<vcl_string>
	/// for each command line argument, you have create a "vul_arg", this takes the following three arguments:
	///	  - the argument itself, this is what you expect to see on the command line when you call it
	///    - a descritpion of this arguement, i.e. what it's for
	///    - an optional default value.
	vul_arg<vcl_string>
		arg_in_path("-path", "Input path, i.e. C:/somefiles/"),
		arg_in_glob("-glob", "Input glob, i.e. *png, this will get all png's.");

	/// now we have some integer arguments
	vul_arg<unsigned> arg_number("-n", "A number.", 4),
		arg_number_again("-n2", "another number.", 4);

	/// finally, we have some floats
	vul_arg<float> arg_float("-f", "A float", 4.0);

	/// call vul_arg_parse(argc, argv) to parse the arguments, this will go through the command line string, look for all the specified argument string,
	/// and extract the provided values.
	vul_arg_parse(argc, argv);

	/// now we can start to access our arguments, to do it, simply call the created vul_arg object. i.e. arg_in_path() will return the value that was found after the path argument.
	/// So, the command line: "<program name> -path c:\nothing -glob * -n 6 -f 8" would yield the following:
	///		arg_in_path() 		-> c:\nothing
	///		arg_in_glob() 		-> *
	///		arg_number()  		-> 6
	///		arg_number_again()	-> 4 			(NOTE: default value is used as this paramter is not specified on the command line)
	///		arg_float()			-> 8.0
	///
	/// we can use this to check that required variables were provided
	/// if arg_in_path() or arg_in_glob() are empty strings, it means they weren't provided
	if (((arg_in_path() == "") || (arg_in_glob() == "")))
	{
		/// if we need these arguments to proceed, we can now exit and print the help as we quit. vul_arg_display_usage_and_exit() will display the available arguments
		/// alongside the help message specified when the vul_arg objects were created
		vul_arg_display_usage_and_exit();
	}


	/// Parsing a directory of images
	/// this is a list to store our filenames in
	vcl_vector<vcl_string> filenames;

	/// Get the directory we're going to search, and what we're going to search for. We'll take these from the vul_arg's we used above
	vcl_string directory = arg_in_path();
	vcl_string extension = arg_in_glob();

	/// loop through a directory using a vul_file_iterator, this will create a list of all files that match "directory + "/*" + extension", i.e. all files in the directory
	/// that have our target extension
	for (vul_file_iterator fn=(directory + "/*" + extension); fn; ++fn)
	{
		/// we can check to make sure that what we are looking at is a file and not a directory
		if (!vul_file::is_directory(fn()))
		{
			/// if it is a file, add it to our list of files
			filenames.push_back (fn());
		}
	}

	if (filenames.size() == 0)
    {
        vcl_cout << "No input files, exiting." << vcl_endl;
        return 0;
    }
*/

/*
    vil_image_view<unsigned char> anImage = vil_load(filenames[0].c_str());

    ViBe_Model Model;
    Model.Init(NUM_SAMPLES, RADIUS, MINSAMPLES, SUBSAMPLING, anImage.ni(), anImage.nj());

    Model.InitBackground(NUM_TRAINING_IMAGES, filenames);



	/// filenames now contain all of the files with our target extension in our directory, if we want to loop through them, we can now do
	for (int i = 0; i < filenames.size(); i++)
	{
		//vcl_cout << filenames[i].c_str() << vcl_endl;

		/// do something with filenames[i]
		/// if filenames[i] is an image, we might want to load it, so we could do:
		vil_image_view<unsigned char> srcImage = vil_load(filenames[i].c_str());
        vil_image_view<unsigned char> resultImage( srcImage.ni(), srcImage.nj(), 1);

        Model.Segment(srcImage, resultImage);

		vcl_stringstream outputFilename;
        outputFilename << "output/" << "BackgroundSegmentation_" << i << ".png";
        vil_save(resultImage, outputFilename.str().c_str());

		// we could now do other things with this file, such as run it through a motion segmentation algorithm
	}
	*/

}
#endif
