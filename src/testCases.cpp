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

        vcl_cout << "Configuration file test: ";
        dispPass(true == testParseTxt1());
        vcl_cout << "Configuration file test: ";
        dispPass(true == testParseTxt2());
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

    vcl_cout << "Testing DF Class" << vcl_endl;

    DF_params default_params1 = DF_params(numChannels, blurSpatial, blurColour, sdSpatial,
                                          sdColour, 1);
    DF_params default_params3 = DF_params(numChannels, blurSpatial, blurColour, sdSpatial,
                                          sdColour, 3);

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

    dispPass(saveFrame.testField(numChannels, width, height, 1));
    dispPass(tester.testField(numChannels, width, height, 1));

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

    dispPass(saveFrame.testField(numChannels, width, height, 3));
    dispPass(tester.testField(numChannels, width, height, 3));





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

#endif
