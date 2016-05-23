
#include "..\include\Version.h"
#include "..\include\DF.h"

#include <vil/vil_load.h>

#ifdef DF_TESTER


int main(){

    vil_image_view<unsigned char> test_image = vil_load("baboon.jpg");
    DF_params params = DF_params(8, 9, 3, 3, 1);
    DistributionField DF = DistributionField(test_image, params);

    DF.saveField();

    std::cout << DF.compare(DF) << "\n";

    DistributionField DF2 = DF.subfield(128, 128, 256, 256);
    DF2.saveField();

    try{
        std::cout << DF2.compare(DF);
    }
    catch(const char* e){

        //std::cout << "Distribution Field Sizes Do Not Match\n";
        std::cout << e << "\n";
    }

    return 0;
}


#endif
