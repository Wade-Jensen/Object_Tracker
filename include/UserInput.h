#ifndef INPUTPARAMS_H
#define INPUTPARAMS_H

#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vul/vul_arg.h>
#include <vil/vil_load.h>
#include "vcl_vector.h"
#include "vcl_string.h"
#include <fstream>
#include <string>
#include <stdlib.h>

// A simple structure to store DFT parameters
struct Params
{
        vcl_vector<vcl_string> filenames;
        vcl_string gTruth;
        int initialX;
        int initialY;
        int width;
        int height;
        int numChannels;
        int blurSpatial;
        int blurColour;
        int maxSearchDist;
        int planes;
        float learningRate;
        float sdSpatial;
        float sdColour;
        vcl_string outputDir;
        bool extend;
        bool color;
};


using namespace std;

// UserInput class handles command line and configuration file inputs
// for path names, filetypes, and DFT parameters
class UserInput
{
    public:
        /** Constructor */
        UserInput();

        void initInputParams(vcl_vector<vcl_string> filenames, vcl_string, int initialX, int initialY,
                             int width, int height, int numChannels, int blurSpatial,
                             int blurColour, int maxSearchDist, int planes, float learningRate,
                             float sdSpatial, float sdColour, vcl_string outputDir, bool, bool);
        bool parseCli(int argc, char * argv[]);
        bool parseTxt(vcl_string configFile);
        const struct Params getParams();
        /** Destructor */
        ~UserInput();
    private:

        /** Parameters */
        vcl_vector<vcl_string> _filenames;
        int _initialX;
        int _initialY;
        int _width;
        int _height;
        int _numChannels;
        int _blurSpatial;
        int _blurColour;
        int _maxSearchDist;
        int _planes;
        float _learningRate;
        float _sdSpatial;
        float _sdColour;
        vcl_string _outputDir;
        vcl_string _gTruth;
        bool _extend;
        bool _color;
};

#endif // INPUTPARAMS_H
