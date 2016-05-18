
#include "..\include\Version.h"
#include "..\include\DF.h"

#include <vil/vil_load.h>

#ifdef DF_TESTER


int main(){

    vil_image_view<unsigned char> test_image = vil_load("baboon.jpg");
    DistributionField DF;

    DF.init(test_image, 8, 3, 1);
    DF.saveField();

    return 0;
}


#endif
