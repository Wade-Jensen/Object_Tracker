#ifndef _VIL_IMAGE_
#define _VIL_IMAGE_
#include <vil/vil_image_view.h>
#endif

#ifndef _VIL_LOAD_
#define _VIL_LOAD_
#include <vil/vil_load.h>
#endif

#ifndef _VIL_SAVE_
#define _VIL_SAVE_
#include <vil/vil_save.h>
#endif

#include <vul/vul_file_iterator.h>

#ifndef _VUL_FILE_
#define _VUL_FILE_
#include <vul/vul_file.h>
#endif

#include <vul/vul_arg.h>
#include "../include/DF.h"
#include "../include/inputParams.h"
#include <fstream>
#include <string>

//#ifndef _CSTDINT_
//#define _CSTDINT_
//#include <cstdint>
//#endif

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
   //bool first_frame; // have we computed // the first frame?
    // unknown type current_position = initial_position /// objects current // position, initial // position is // provided by the
                                                        /// Needs to store a location and size
                                                        /// provided by the user
    vcl_vector< vil_image_view<unsigned char> > images; /// we track the object over a list of images provided by the user
                                                    /// this shall be a vector of vil_image_view's
    // unknown type object_model; // a model of the DF of the image (sequence?)
   /// specify all the arguments you expect/need
	/// group them according to type, i.e. all the strings first here, so we have vul_arg<vcl_string>
	/// for each command line argument, you have create a "vul_arg", this takes the following three arguments:
	///	  - the argument itself, this is what you expect to see on the command line when you call it
	///    - a descritpion of this arguement, i.e. what it's for
	///    - an optional default value.
	vul_arg<vcl_string>
		arg_in_path("-path", "Path to Image Frames, e.g. C:/somefiles/"),
		arg_in_glob("-glob", "Image (Frame) Extension (png,jpg,bmp,tiff,jpeg), e.g. *.jpeg, this will get all jpeg's.");

	// now we have some integer arguments
	vul_arg<unsigned> arg_ipx("-ipx", "Initial Position x .", -1),
		arg_ipy("-ipy", "Initial Position y.", -1),
        arg_sd("-sd", "Maximum Search Distance", -1);
	vul_arg<float> arg_lr("-lr", "Learning Rate", -1);
	vul_arg_parse(argc, argv, true);



	if(((arg_in_path() == "") || (arg_in_glob() == ""))) {// command line args are not passed perfectly
        //vcl_cout << "Parmeter Error. Check provided Parameters" <<vcl_endl;
        vcl_cout << "Not sufficient or incorrrect argument parameters detected..." <<vcl_endl;
        //vul_arg_display_usage_and_exit();
        vcl_cout <<"Utilizing Parameter Text File Method to Obtain input Parameters"<<endl;
        vcl_cout << "Checking Parameter Text File for Parameters..." <<vcl_endl;
        //vul_arg_display_usage_and_exit();
       bool parametersExtracted=params.textParamsReader();
     if ( parametersExtracted==false)// failed to capture all parameters
        {
            vcl_cout << "Text File Doesn't Contain Valid Parameters, Exiting After Checking Text File Parameters" <<vcl_endl;
            vcl_cout << "Text File Location: ../../Params/Params.txt" <<vcl_endl;
            //vul_arg_display_usage_and_exit();
            return 0;
        }


	}
		// Parsing a directory of images from command line argument

	else{ // command line arguments are passed correctly
         vcl_vector<vcl_string> filenames;
        for (vul_file_iterator fn=(arg_in_path() + "/*" + arg_in_glob()); fn; ++fn)
            {
            if (!vul_file::is_directory(fn()))
            {
                filenames.push_back (fn());
            }
        }
	if (filenames.size() == 0 || arg_ipy()<= 0|| arg_ipx()<=0 || arg_sd()<= 0 || arg_lr()<=0)
    {
        vcl_cout << "No input files at given path OR Parameters are Incorrect" << vcl_endl;
        vcl_cout <<  "Exiting form Program"<< endl;
        return 0;
    }
    else{// every thing is file thorugh command line Argument
    // calling InputParams Class Constructor
     params.initInputParams(filenames,arg_ipx(),arg_ipy(),arg_sd(),arg_lr());
    vcl_cout << "There are " << filenames.size() <<" frames in the selected directory"<< vcl_endl;
    vcl_cout << "Input Parameters Initialized By Command Line Arguments";
    // vcl_cout << "Initial Position x for DFT = "<< arg_ipx() <<vcl_endl;
    // vcl_cout << "Initial Position y for DFT = "<< arg_ipy() <<vcl_endl;
    // vcl_cout << "Maximum Search Distance for DFT = "<< arg_sd() <<vcl_endl;
    // vcl_cout << "Learning Rate for DFT = "<< arg_lr() <<vcl_endl;
    // unsigned int maxSearchDist=arg_sd();  ///  max distance to travel in search (user provided)


    }

  }
vcl_cout << "ipx:"<<params.ipx<<endl;
vcl_cout << "ipy:"<<params.ipy<<endl;
vcl_cout << "sd:"<<params.sd<<endl;
vcl_cout << "lr:"<<params.lr<<endl;
  delete &params;

  return 1;
}


