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


using namespace std;
class inputParams
{
    public:
        /** Constructor */
        inputParams();
        bool initInputParams(vcl_vector<vcl_string>, int, int, int, int, int, int, int, int, int, float, float, float, vcl_string);
        bool parseCli(int argc, char * argv[]);
        bool parseTxt(vcl_string configFile);

        /** Destructor */
        ~inputParams();
        /** Parameters */
        vcl_vector<vcl_string> filenames;
        int ipx, ipy, w, h, c, sb, bc, sd, planes;
        float lr, sds, sdc;
        vcl_string odir;
    protected:


    private:
};

#endif // INPUTPARAMS_H
