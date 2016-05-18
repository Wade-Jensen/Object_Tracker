#ifndef DF_H_INCLUDED
#define DF_H_INCLUDED

#include <vector>
#include <vil/vil_image_view.h>

template <class T> class DistributionField{

public:

private:

        vector<vil_image_view<T> > dist_field;
};

#endif // DF_H_INCLUDED
