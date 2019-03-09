#include "trackcurrentcalculator.h"
#include <QtCore/qmath.h>

TrackCurrentCalculator::TrackCurrentCalculator(){

}


// get max current depending of witdh, copper thickness, allowed temp rise and position of the layer
double TrackCurrentCalculator::currentFromTrackWidth(int width, double thickNess, int temperature, bool internal){
    double area = width * thickNess;
    double k;
    if  (internal )
        k = 0.024;
    else
        k = 0.048;
    double current = k * qPow(temperature,0.44) * qPow(area,0.725);
    return current;
}

