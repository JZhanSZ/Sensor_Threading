#ifndef SONAR_H
#define SONAR_H

#include "ranger.h"

/*!

 *  \brief     Sonar class, inherited from Ranger class
 *  \details
 *  This class is a rangering sensor class for sonar model SONX-001.\n
 *  \author    Jian Zhan
 *  \version   1.00
 *  \date      2017
 */
class Sonar: public Ranger
{
public:
    Sonar();
    /**
        \brief Generate data sample according to sensor's sample size
        \sa getRand()
    */
    void dataGeneration();//get random sampling data

private:
    /**
       \brief shall never be used by Sonar class
    */
    void setFieldOfView(const int *field_of_view);//shall not be used for sonar
};

#endif // SONAR_H
