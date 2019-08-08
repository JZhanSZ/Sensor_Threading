#ifndef RANGER_H
#define RANGER_H

#include <string>
#include <time.h>
#include <vector>
#include <random>
#include <iostream>
#include <chrono>

// *  \ingroup   ac_shapre Shape

/*!

 *  \brief     Rangering sensors base class
 *  \details
 *  This class is the base class for all ranging sensors.\n
 *  \author    Jian Zhan
 *  \version   1.00
 *  \date      2017
 */

class Ranger
{
public:
    //! Set the baud to be used by the sensor
    /**

    @param    baud to be used by the sensor
    \sa baud_, getBaud()
    */
    void setBaud(const long *baud);

    //! Set the USB Port to be used by the sensor
    /**

    @param    port to be used by the sensor
    \sa port_, getPort()
    */
    void setPort(const int *port);

    //! Set the number of data each sampling
    /**

    @param    sample_size of the sensor
    \sa sample_size_
    */
    void setSampleSize(const int* sample_size);


    //get methods
    //! Returns sensor's model
    /**
      \return The sensor's model
      \sa model_
    */
    std::string getModel();

    //! Returns sensor's maximum detectable distance [m]
    /**
      \return The sensor's maximum detectable distance [m]
      \sa max_distance_
    */
    double getMaxDistance();

    //! Returns sensor's minimum detectable distance [m]
    /**
    \return The sensor's minimum detectable distance [m]
    \sa min_distance_
    */
    double getMinDistance();

    //! Returns sensor's data rate [HZ]
    /*!
      \return sensor's data rate [HZ]
      \sa data_rate_
    */
    int getDataRate();

    //! Returns Baud used by the sensor
    /*!
      \return Baud used by the sensor
      \sa baud_, setBaud();
    */
    long getBaud();

    //! Returns USB Port used by the sensor
    /*!
      \return USB Port used by the sensor
      \sa port_, setPort();
    */
    int getPort();

    //! Returns sensor's field of view [deg]
    /*!
      \return sensor's field of view [deg]
      \sa field_of_view_, setFieldOfView();
    */
    int getFieldOfView();

    //! Returns sensor's sample size
    /*!
      \return sensor's sample size
      \sa sample_size_

    */
    int getSampleSize();

    //! Returns sensor's sample of data
    /*!
      \return sensor's sample of data
      \sa data_
    */
    std::vector<double> getData();

    //! Returns sensor's last sample of data
    /*!
      \return sensor's last sample of data
      \sa data_previous_
    */
    std::vector<double> getDataPrevious();

    //! Returns sensor's the time since epoch at sampling of data
    /*!
      \return sensor's the time since epoch at sampling of data
      \sa data_time_
    */
    unsigned long long int getDataTime();

    //! Returns sensor's the time since epoch at last sampling of data
    /*!
      \return sensor's the time since epoch at last sampling of data
       \sa data_time_previous_
    */
    unsigned long long int getDataTimePrevious();

    //ifsane methodsfor checking whether iputs are sane
    //! Exam whether the baud is set sanely, and if it is not, set to default value of 38400
    /*!
       \return whether the baud is sane
    */
   bool ifSaneBaud();

   //! Exam whether the USB port is set sanely, and if it is not, set to default value of 0
   /*!
      \return whether the USB port is sane
   */
   bool ifSanePort();

   //random number generator
   //! Generates random distance
   /**


   */
    double getRand();


    //virtual methods
    //! Virtual Funcion
    /**
        Generate data sample according to sensor's sample size
        \sa getRand()
    */
    virtual void dataGeneration()=0;
    //! Virtual Funcion
    /**
        Set the sensor's field of view
        @param    field of view of sensor
        \sa field_of_view_, getFieldOfView()
    */
    virtual void setFieldOfView(const int *field_of_view)=0;

private:
    Ranger();//shall not be used

protected:
    //shall never be called excepted derived classes
    //! Ranger Constructor
    /**
      \brief Set fixed and default sensor parametres
        @param model of the sensor
        @param max_distance that the sensor can detect [m]
        @param min_distance that the sensor can detect [m]
        @param field_of_view of the sensor [deg]
        @param data_rate of the sensor [HZ]

    */
    Ranger(const std::string &model,double max_distance, double min_distance,int field_of_view, int data_rate);

    //common sensor parameters
    std::string model_; //!< Model of the sensor
    long baud_;//!< Baud used by the sensor
    int port_;//!< USB Port used by the sensor
    int field_of_view_;//!< Field of view of the sensor
    int sample_size_;//!< The number of reading the sensor provides each sampling
    int data_rate_;//!< Data rate of the sensor, unit: HZ
    double max_distance_;//!< Maximum distance the the sensor can detect, unit: metre
    double min_distance_;//!< Minimum distance the the sensor can detect, unit: metre
    unsigned long long int data_time_;//!< The time past epoch when a sample is generated
    unsigned long long int data_time_previous_;//!< The time past since epoch when last sample was generated

    //store sampling data;
    std::vector<double> data_;//!< The sample of reading
    std::vector<double> data_previous_;//!< Last sample of reading


};

#endif // RANGER_H
