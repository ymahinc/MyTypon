#ifndef TRACKCURRENTCALCULATOR_H
#define TRACKCURRENTCALCULATOR_H


class TrackCurrentCalculator
{
public:
    TrackCurrentCalculator();

    static double currentFromTrackWidth(int width, double thickNess, int temperature = 10, bool internal = false);
};

#endif // TRACKCURRENTCALCULATOR_H
