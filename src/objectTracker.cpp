#include "..\include\UserInput.h"
#include "..\include\Version.h"
#include "..\include\DFT.h"


#ifdef RELEASE

#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vul/vul_arg.h>

//#include <array>

/*
 * Main program to run the Object Tracker algorithm.
 * This program will
 *  - take a list of images on the command line, specified as a path and a partial filename (i.e. *.png to get all png files)
 *  - take a set of parameters for the ViBe segmenter from the command line, if no value is given defaults will be used
 *  - will save a set of images showing the motion segmented output
 *  - will optionally compute performance metrics using a given ground truth image and index
 */
//using namespace std;

//A not good Sorting Function I wrote - Pls Don't Mark This :'(
void crap_sort(int* array, int size){

    vector<int> buffer;

    int sorting = 4;

    while(sorting){
        int smallest = INFINITY;
        int index = 0;

        for (int i = 0; i < size; i++){
            if(array[i] < smallest){
                smallest = array[i];
                index = i;
            }
        }

        buffer.push_back(array[index]);
        array[index] = INFINITY;
        sorting--;
    }

    for(int i = 0; i < size; i++){
        array[i] = buffer[i];
    }

}

int main (int argc, char * argv[])
{
    UserInput userInput;// input Parameters

    bool cliRead = userInput.parseCli(argc, argv);
    if (!cliRead)
    {
		vcl_cout << "Use config file to get parameters (Y/N)? ";
		vcl_string goToText;
		vcl_cin >> goToText;
		vcl_cout << endl;
		if (goToText == "y" || goToText == "Y")
		{
			vcl_cout <<"Utilizing parameter text file method to obtain input parameters"<<endl;
			vcl_cout << "Checking parameter text file for parameters..." <<vcl_endl;
			bool configFileRead = userInput.parseTxt("config.txt");
			if (!configFileRead)
			{
				vcl_cout << "Text file doesn't contain valid parameters, exiting" <<vcl_endl;
				return 0;
			}
		} else {return 0;}
    }
    // return the input parameters in a structure with the const qualifier to avoid
    // needing individual getters
    const struct Params params = userInput.getParams();

    vul_file vulStruct;

    // Print current working directory
    vcl_string dir = vulStruct.get_cwd();
    vcl_cout << dir << vcl_endl;

    // Top corner and size of object in first image to track
    int x = params.initialX;
    int y = params.initialY;
    int width = params.width;
    int height = params.height;

   	// Tracker parameters
    int maxSearchDist = params.maxSearchDist;
    float learningRate = params.learningRate;

    //output Path
    vcl_string outputPath = params.outputDir;

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

    std::ifstream gTruth;
    gTruth.open(params.gTruth.c_str(), std::ifstream::in);
    char comma;
    int dummy;

	// save the distribution field parameters
    DF_params default_params = DF_params(params.numChannels, params.blurSpatial, params.blurColour,
                                         params.sdSpatial, params.sdColour, params.color);

    // Create the object model for tracking
    DFT DFTracker;
    DFTracker = DFT(images[0], default_params, x, y, width, height,
                    learningRate, maxSearchDist, params.extend);

    // create output path if it does not exist
    if (outputPath.c_str() != "")
    {
        if (!vul_file::is_directory(outputPath.c_str()))
        {
			vcl_cout << "Output path does not exist" << vcl_endl;
            vul_file::make_directory_path(outputPath.c_str());
            vcl_cout << "Output path made" << vcl_endl;
        }
    }

    // move the working directory to the output path, print to confirm
    vul_file::change_directory(outputPath.c_str());
    dir = vulStruct.get_cwd();
    vcl_cout << "Working directory is now: " << dir << vcl_endl;

    float meanDist = 0;

    // This is the main loop that tracks the object through the image sequence
    // Handle Exceptions withing loop

    float overlap = 0;

    try{

        //Iterate through images
        for (int i=0; i<images.size(); i++)
        {
            vcl_cout << "Current frame is: "<< i << vcl_endl;

            // create distribution field for the current frame
            //DistributionField dfFrame = DistributionField(images[i], default_params);

            // Locate the object in the current frame. Use gradient descent search
            // to find the new object position
            map<vcl_string,int> currentPosition = DFTracker.locateObject(images[i]);
            int x = currentPosition["x"];
            int y = currentPosition["y"];
            int width = currentPosition["width"];
            int height = currentPosition["height"];

            //Grab ground truth parameters
            float xg, yg, w, h;
            gTruth >> xg >> comma >> yg >> comma >> w >> comma >> h;

            //Find centroid from groung truth
            int xc = xg + w/2.0;
            int yc = yg + h/2.0;

            //Compare the object location centroid
            float dist = sqrt(
                              pow((x+width/2.0) - xc, 2) +
                              pow((y+height/2.0) - yc, 2));
            meanDist = (meanDist*i + dist)/(i+1);

            // Average overlap calculation
            //Find Rectangle lengths and sort
            int horiz[4] = {xg, xg+w, x, x+width};
            crap_sort(horiz, 4);
            int vert[4] = {yg+h, yg, y+height, y};
            crap_sort(vert, 4);
            // left, bottom, right, top
            int intersect[4] = {horiz[1], vert[1], horiz[2], vert[2]};
            //Calculate Interestion area
            int intArea = (intersect[3] - intersect[1])*(intersect[2]-intersect[0]);
            //Calculate Union Area
            int TotalArea = (height) * (width) + (h) * (w) - intArea;
            //Increase total overlap
            overlap += (float)intArea/TotalArea;


            //  update the object model to incorporate new information
            DFTracker.updateModel(images[i]);

            // Print an image, ie. draw a bounding box around the object being tracked
            DFTracker.displayCurrentPosition (images[i], outputPath, i );
        }

        //At the end of the algorithm, wrtie accuracy to console for user
        std::cout << "\n\n Mean Distance to Centroid: ";
        std::cout << meanDist << "\n\n";
        std::cout << "\n\n Mean Overlap: ";
        std::cout << overlap/filenames.size() << "\n\n";
    }
    //A bad write means the algorith gone off-screen, end it and display message for user
    catch(int bad_write[6]){
        std::cout << "Attempted Writing to pixel (" << bad_write[0] << "," <<
        bad_write[1] << "," << bad_write[2] << ")";
        std::cout << " Failed as frame is of size (" << bad_write[3] << "," <<
        bad_write[4] << "," << bad_write[5] << ")\n";
        std::cout << "Object Position at Throw Time: (" << bad_write[6] << ", "
        << bad_write[7] << ")\n";

        delete bad_write;
    }
}
#endif
