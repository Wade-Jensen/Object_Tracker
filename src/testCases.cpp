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

int main (int argc, char * argv[])
{
    UserInput userInput;// input Parameters

    bool cliRead = userInput.parseCli(argc, argv);
    if (!cliRead)
    {
        vcl_cout <<"Utilizing parameter text file method to obtain input parameters"<<endl;
        vcl_cout << "Checking parameter text file for parameters..." <<vcl_endl;
        bool configFileRead = userInput.parseTxt("config.txt");
        if (!configFileRead)
        {
            vcl_cout << "Text file doesn't contain valid parameters, exiting" <<vcl_endl;
            return 0;
        }
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

    vcl_cout << filenames[0].c_str() << vcl_endl;
    // load the image file into the vector of images:
    images.push_back( vil_load(filenames[0].c_str()) );

    bool success;
    int numPassed = 0;
    int numTests = 0;

    vcl_cout << "Running test script: " << vcl_endl;

    bool test1 = testParseCli1(); numPassed += test1; numTests++;
    bool test2 = testParseCli2(); numPassed += test2; numTests++;
    bool test3 = testParseTxt1(); numPassed += test3; numTests++;
    bool test4 = testParseTxt2(); numPassed += test4; numTests++;
    bool test5 = testParseTxt3(); numPassed += test5; numTests++;
    bool test6 = testParseTxt4(); numPassed += test6; numTests++;

    vcl_cout << vcl_endl;
    vcl_cout << "Test1: "; if (test1) {vcl_cout << "Passed"<<vcl_endl;} else if (!test1) {vcl_cout << "Failed"<<vcl_endl;}
    vcl_cout << "Test2: "; if (test2) {vcl_cout << "Passed"<<vcl_endl;} else if (!test2) {vcl_cout << "Failed"<<vcl_endl;}
    vcl_cout << "Test3: "; if (test3) {vcl_cout << "Passed"<<vcl_endl;} else if (!test3) {vcl_cout << "Failed"<<vcl_endl;}
    vcl_cout << "Test4: "; if (test4) {vcl_cout << "Passed"<<vcl_endl;} else if (!test4) {vcl_cout << "Failed"<<vcl_endl;}
    vcl_cout << "Test5: "; if (test5) {vcl_cout << "Passed"<<vcl_endl;} else if (!test5) {vcl_cout << "Failed"<<vcl_endl;}
    vcl_cout << "Test6: "; if (test6) {vcl_cout << "Passed"<<vcl_endl;} else if (!test6) {vcl_cout << "Failed"<<vcl_endl;}


    vcl_cout << numPassed << " of " << numTests << " Tests Passed." <<vcl_endl;;
    vcl_cout << "Test script complete" << vcl_endl;

    // Test 7 - first six tests are done by testBat
	// save the distribution field parameters
    vcl_cout << "TEST CASE 7" << vcl_endl;

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

    vcl_cout << "TEST CASE 7: PASSED"<< vcl_endl;

    saveFrame.testField(numChannels, width, height, 1);
    tester.testField(numChannels, width, height, 1);

    vcl_cout << vcl_endl << "TEST CASE 8" << vcl_endl;

    saveFrame = DistributionField(images[0], default_params3, x, y, width, height);
    tester = ChannelRep(images[0], default_params3, x, y, width, height);
    // output jpeg images of the distribution field
    vul_file::make_directory_path("ColourDT");
    vul_file::make_directory_path("ColourChannel");
    vul_file::change_directory("ColourDT");
    saveFrame.saveField();
    vul_file::change_directory("../ColourChannel");
    tester.saveField();

    vcl_cout << "TEST CASE 8: PASSED" << vcl_endl;

    saveFrame.testField(numChannels, width, height, 3);
    tester.testField(numChannels, width, height, 3);

    vcl_cout << "TEST CASE 9" << vcl_endl;

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
    delete argv;

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
    delete argv;

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

/*
    // create the object model for tracking
    DFT DFTracker;
    DFTracker = DFT(images[0], default_params, x, y, width, height, learningRate, maxSearchDist, false);

    // create output path if it does not exist
    if (outputPath.c_str() != "")
    {
        if (!vul_file::is_directory(outputPath.c_str()))
        {
            vul_file::make_directory_path(outputPath.c_str());
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
            //DistributionField dfFrame = DistributionField(images[i], default_params);

            // locate the object in the current frame. Use gradient descent search
            // to find the new object position
            map<vcl_string,int> currentPosition = DFTracker.locateObject(images[i]);
            int x = currentPosition["x"];
            int y = currentPosition["y"];

            //  update the object model to incorporate new information
            DFTracker.updateModel(images[i]);

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
}
*/

#endif
