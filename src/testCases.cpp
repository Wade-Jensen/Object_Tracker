#include "..\include\UserInput.h"
#include "..\include\Version.h"
#include "..\include\DFT.h"


#ifdef _TEST_

#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vul/vul_arg.h>

/*
Test Cases
This will run tests detailed in the documentation for this project.
*/

using namespace std;

bool testParseCli1();
bool testParseCli2();
bool testParseTxt1();
bool testParseTxt2();
bool testLocateObj1();

int passes = 0; // count total passes
int fails = 0; // count total fails
// display total passes and fails at the end.

bool dispPass(bool pass)
{
    if (pass) {
	cout << "PASS" << endl;
	passes++;
	} else {
	cout << "FAIL" << endl;
	fails++;
	}
    return pass;
}

int main (int argc, char * argv[])
{
    {
        vcl_cout << "Testing UserInput Class" << endl;

        //vcl_cout << "Command line arguments test: " << vcl_endl<< vcl_endl;
        //dispPass(testParseCli1());
        //vcl_cout << "Command line arguments test: "<< vcl_endl<< vcl_endl;
        //dispPass(testParseCli2());

        vcl_cout << "Configuration file test: "<< vcl_endl<< vcl_endl;
        dispPass(testParseTxt1());
        vcl_cout << "Configuration file test: "<< vcl_endl<< vcl_endl;
        dispPass(testParseTxt2());

        //vcl_cout << "Gradient Descent Algorithm test 1:" << vcl_endl<< vcl_endl;
        //dispPass(testLocateObj1());
    }

    UserInput userInput;// input Parameters

    bool configFileRead = userInput.parseTxt("config.txt");
    if (!configFileRead)
    {
        vcl_cout << "Text file doesn't contain valid parameters, exiting" <<vcl_endl;
        return 0;
    }
    // return the input parameters in a structure with the const qualifier to avoid
    // needing individual getters
    const struct Params params = userInput.getParams();

    vul_file vulStruct;

    // Top corner and size of object in first image to track
    int x = params.initialX;
    int y = params.initialY;
    int width = params.width;
    int height = params.height;

	// Distribution Field parameters
    int numChannels = params.numChannels;
    int blurSpatial = params.blurSpatial;
    int blurColour = params.blurColour;
    float sdSpatial = params.sdSpatial;
    float sdColour = params.sdColour;
    int planes = params.planes;

   	// Tracker parameters
    int maxSearchDist = params.maxSearchDist;
    float learningRate = params.learningRate;

    // output Path
    vcl_string outputPath = params.outputDir;

    // loading images
    vcl_vector< vil_image_view<unsigned char> > images;
    vcl_vector<vcl_string> filenames = params.filenames;

    // load the image file into the vector of images:
    images.push_back( vil_load(filenames[0].c_str()) );

    vcl_cout << "Testing DF Class" << endl << "DistributionField(const vil_image_view<unsigned char>&, DF_params&)" << vcl_endl;

    DF_params default_params1 = DF_params(numChannels, blurSpatial, blurColour, sdSpatial,
                                          sdColour, false);
    DF_params default_params3 = DF_params(numChannels, blurSpatial, blurColour, sdSpatial,
                                          sdColour, true);

    DistributionField saveFrame = DistributionField(images[0], default_params1, x, y, width, height);
    ChannelRep tester = ChannelRep(images[0], default_params1, x, y, width, height);
    // output jpeg images of the distribution field
    vul_file::make_directory_path("GreyDT");
    vul_file::make_directory_path("GreyChannel");
    vul_file::change_directory("GreyDT");
    saveFrame.saveField();
    vul_file::change_directory("../GreyChannel");
    tester.saveField();
    vul_file::change_directory("..");

    dispPass(saveFrame.testField(numChannels, width, height, false));
    dispPass(tester.testField(numChannels, width, height, false));

    saveFrame = DistributionField(images[0], default_params3, x, y, width, height);
    tester = ChannelRep(images[0], default_params3, x, y, width, height);
    // output jpeg images of the distribution field
    vul_file::make_directory_path("ColourDT");
    vul_file::make_directory_path("ColourChannel");
    vul_file::change_directory("ColourDT");
    saveFrame.saveField();
    vul_file::change_directory("../ColourChannel");
    tester.saveField();
    // images are all saved for manual inspection

    dispPass(saveFrame.testField(numChannels, width, height, true));
    dispPass(tester.testField(numChannels, width, height, true));

    /*
    vcl_cout << "DistributionField DistributionField::subfield(int X, int Y, int Width, int Height) const" << endl;
    int tx = 10, ty = 10, twidth = 14, theight = 20;
    DistributionField newTester;
    newTester = tester.subfield(x,y,width,height);
    newTester.testField(numChannels,width,height,true);
    */




    vcl_cout << "TOTAL " << "PASSES: " << passes << endl;
	vcl_cout << "TOTAL " << "FAILS: " << fails << endl;
	vcl_cout << endl;

    return 0;

}






bool testParseCli1()
{
    // Stub out the command line with input values
    int argc;
    char* argv[30];
    for (int i=0;i<30;i++)
    {
        argv[i] = new char[20];
    }
    argc = 29;
    argv[0] = "objectTracker.exe";
    argv[1] = "-path";
    argv[2] = "Data/bolt";
    argv[3] = "-glob";
    argv[4] = "jpg";
    argv[5] = "-ipx";
    argv[6] = "336";
    argv[7] = "-ipy";
    argv[8] = "165";
    argv[9] = "-w";
    argv[10] = "25";
    argv[11] = "-h";
    argv[12] = "60";
    argv[13] = "-c";
    argv[14] = "8";
    argv[15] = "-sb";
    argv[16] = "4";
    argv[17] = "-bc";
    argv[18] = "1";
    argv[19] = "-sd";
    argv[20] = "30";
    argv[21] = "-planes";
    argv[22] = "3";
    argv[23] = "-lr";
    argv[24] = "0.05";
    argv[25] = "-sds";
    argv[26] = "1.0";
    argv[27] = "-sdc";
    argv[28] = "0.625";

    UserInput userInput;
    bool cliRead = userInput.parseCli(argc, argv);

    for (int i=0;i<30;i++)
    {
        delete[] argv[i];
    }

    if (cliRead)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool testParseCli2()
{
    // Stub out the command line with input values
    int argc;
    char* argv[30];
    for (int i=0;i<30;i++)
    {
        argv[i] = new char[20];
    }
    argc = 29;
    argv[0] = "objectTracker.exe";
    argv[1] = "-path";
    argv[2] = "6";
    argv[3] = "-glob";
    argv[4] = "jpg";
    argv[5] = "-ipx";
    argv[6] = "336";
    argv[7] = "-ipy";
    argv[8] = "165";
    argv[9] = "-w";
    argv[10] = "-20";
    argv[11] = "-h";
    argv[12] = "-1";
    argv[13] = "-c";
    argv[14] = "8";
    argv[15] = "-sb";
    argv[16] = "4";
    argv[17] = "-bc";
    argv[18] = "1";
    argv[19] = "-sd";
    argv[20] = "30";
    argv[21] = "-planes";
    argv[22] = "3";
    argv[23] = "-lr";
    argv[24] = "0.05";
    argv[25] = "-sds";
    argv[26] = "1.0";
    argv[27] = "-sdc";
    argv[28] = "0.625";

    UserInput userInput;
    bool cliRead = userInput.parseCli(argc, argv);

    for (int i=0;i<30;i++)
    {
        delete[] argv[i];
    }

    if (!cliRead)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool testParseTxt1()
{
    UserInput userInput;
    bool txtRead = userInput.parseTxt("test/validconfig.txt");

    return txtRead;
}

bool testParseTxt2()
{
    UserInput userInput;
    bool txtRead = userInput.parseTxt("test/fake.txt");

    return !txtRead;
}

bool testLocateObj1()
{
    UserInput userInput;
    userInput.parseTxt("test/validconfig.txt");
    Params params = userInput.getParams();

    vul_file vulStruct;

    // Print current working directory
    vcl_string dir = vulStruct.get_cwd();
    vcl_cout << dir << vcl_endl;

    // Top corner and size of object in first image to track
    int x = params.initialX;
    int y = params.initialY;
    int width = params.width;
    int height = params.height;

	// Distribution Field parameters
    int numChannels = params.numChannels;
    int blurSpatial = params.blurSpatial;
    int blurColour = params.blurColour;
    float sdSpatial = params.sdSpatial;
    float sdColour = params.sdColour;
    int planes = params.planes;

   	// Tracker parameters
    int maxSearchDist = params.maxSearchDist;
    float learningRate = params.learningRate;

    //output Path
    vcl_string outputPath = params.outputDir;

    //loading images
    vcl_vector< vil_image_view<unsigned char> > images;
    vcl_vector<vcl_string> filenames = params.filenames;

    vcl_cout << "x: " << x << vcl_endl;
    vcl_cout << "y: " << y << vcl_endl;
    vcl_cout << "width: "<< width<< vcl_endl;
    vcl_cout << "height: "<< height<< vcl_endl;
    vcl_cout << "numChannels : "<< numChannels << vcl_endl;
    vcl_cout << "blurSpatial : "<< blurSpatial << vcl_endl;
    vcl_cout << "sdSpatial : "<< sdSpatial << vcl_endl;
    vcl_cout << "sdColour : "<< sdColour << vcl_endl;
    vcl_cout << "planes : "<< planes << vcl_endl;
    vcl_cout << "maxSearchDist : "<< maxSearchDist << vcl_endl;
    vcl_cout << "learningRate : "<< learningRate << vcl_endl;
    vcl_cout << "outputPath : "<< outputPath << vcl_endl;


    // filenames should now contain the names of all the files with our target extension
	// in the input directory, if we want to loop through them, we can now do
	for (int i = 0; i < 2; i++)
	{
        vcl_cout << filenames[i].c_str() << vcl_endl;
		// load the image file into the vector of images:
		images.push_back( vil_load(filenames[i].c_str()) );
	}

	// save the distribution field parameters
    DF_params default_params = DF_params(numChannels, blurSpatial, blurColour, sdSpatial,
                                          sdColour, planes);


    // create the object model for tracking
    DFT DFTracker;
    DFTracker = DFT(images[0], default_params, x, y, width, height, learningRate, maxSearchDist, false);

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

    // This is the main loop that tracks the object through the image sequence
    for (int i=0; i<images.size(); i++)
    {
        vcl_cout << "Current frame is: "<< i << vcl_endl;

        // create distribution field for the current frame
        //DistributionField dfFrame = DistributionField(images[i], default_params);

        // locate the object in the current frame. Use gradient descent search
        // to find the new object position
        map<vcl_string,int> currentPosition = DFTracker.locateObject(images[i]);
        int x = currentPosition["x"];
        int y = currentPosition["y"];

        if (i=1)
        {
            std::ifstream gTruth;
            gTruth.open( "Data/bolt/groundtruth.txt", std::ifstream::in);
            char comma;
            int dummy;
            int xg, yg, w, h;

            gTruth >> xg;
            gTruth >> comma;
            gTruth >> yg;
            gTruth >> comma;
            gTruth >> w;
            gTruth >> comma;
            gTruth >> h;

            vcl_cout << xg << vcl_endl;
            vcl_cout << yg << vcl_endl;

            vcl_cout << x << vcl_endl;
            vcl_cout << y << vcl_endl;

            assert( abs(339-x) <= 3 );
            assert( abs(163-y) <= 3 );

        }
    }

    return true;
}

#endif
