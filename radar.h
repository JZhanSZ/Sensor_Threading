#ifndef RADAR_H
#define RADAR_H

#include "ranger.h"
#include <cstdlib>

/*!

 *  \brief     Radar class, inherited from Ranger class
 *  \details
 *  This class is the rangering sensor class for radar model RAD-001.\n
 *  \author    Jian Zhan
 *  \version   1.00
 *  \date      2017
 */
class Radar: public Ranger
{
public:

    Radar();

    //set methods
    /**
        \brief Set the sensor's field of view
        @param    field of view of sensor
        \sa field_of_view_, getFieldOfView()
    */
    void setFieldOfView(const int *field_of_view);

    //get methods
    /**
        \brief Generate data sample according to sensor's sample size
        \sa getRand()
    */


    void dataGeneration();//get random sampling data

    //ifsane methods
    //! Exam whether the field of view is set sanely, and if it is not, set to default value of 20
    /*!
       \return whether the field of view is sane
    */
    bool ifSaneFieldOfView();



private:

};



#endif // RADAR_H
