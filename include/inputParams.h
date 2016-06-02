#ifndef INPUTPARAMS_H
#define INPUTPARAMS_H

#include <vul/vul_file_iterator.h>

#ifndef _VUL_FILE_
#define _VUL_FILE_
#include <vul/vul_file.h>
#endif
#ifndef _VIL_LOAD_
#define _VIL_LOAD_
#include <vil/vil_load.h>
#endif
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
        bool initInputParams(vcl_vector<vcl_string>, int, int, int, int, int, int, int, int, float, float, float, vcl_string);
        bool textParamsReader();

        /** Destructor */
        ~inputParams();
        /** Parameters */
        vcl_vector<vcl_string> filenames;
        int ipx, ipy, w, h, c, sb, bc,sd;
        float lr, sds, sdc;
        vcl_string odir;
    protected:


    private:
};

#endif // INPUTPARAMS_H
