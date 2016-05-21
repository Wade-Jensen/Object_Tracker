
#include "..\include\Version.h"
#include "..\include\DF.h"

#include <vil/vil_load.h>

#ifdef DF_TESTER


int main(){

    vil_image_view<unsigned char> test_image = vil_load("baboon.jpg");
    DF_params params = DF_params(8, 3, 1, 1, 1);
    DistributionField DF = DistributionField(test_image, params);

    DF.saveField();

    std::cout << DF.compare(DF);

    return 0;
}


#endif
