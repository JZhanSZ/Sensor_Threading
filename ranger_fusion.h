#ifndef RANGER_FUSION_H
#define RANGER_FUSION_H


#include <unistd.h>
#include <vector>
#include "ranger.h"
using std::vector;

// This is a forward declaration of the Ranger class (google it)
// It tells the RangerFusionInterface that such a class exists
// but does not specify its details
class Ranger;


/*!

 *  \brief     Data fusion class
 *  \details
 *  This class is to fuse the data from ranging sensors.\n
 *  \author    Jian Zhan
 *  \version   1.00
 *  \date      2017
 */
class RangerFusion//public RangerFusionInterface
{
public:

    /*!
     * \brief RangerFusion constructor
     * \param mode of data fusion: max, min or average
     * \param fusion_timing : 5HZ or at a reading arrives
     * \sa mode_, fusion_timing_
     */
    RangerFusion(int mode,int fusion_timing);

    //! Accept container of rangers
    /*!
     * \param rangers container containing rangers
     * \sa rangers_
     */
    void setRangers(vector<Ranger*> rangers);

    //! Returns a container of fused data from rangers
    /*!
     * \return the raw data received from sensors
     * \sa fused_range_data_
     */
    vector<double> getFusedRangeData();

    //! Returns raw/unfused data from rangers
    /*!
     * \return a container of raw/unfused data from rangers
     * \sa raw_range_data_, getRawRangeData()
     */
    vector<vector<double> > getRawRangeData();



    /*!
     * \brief Enter point of data fusion
     * \details Start fusiong data, colloection of  setRawRangeData(), setAngles() and setFuseRangeData();
     * \sa setRawRangeData(), setAngles(),setFuseRangeData();
     */
    void fuseData();

    /*!
     * \brief Return fusion timing method index
     * \return timing method index
     * \sa fusion_timing_
     */
    int getFusionTiming();

private:
     int mode_;//!< Fusion mode index
     /*!<
      * \details 0 for min mode, 1 for max mode, other for average mode.\n
      * Min mode will select the minimum reading as fusion results .\n
        Max mode will select the maximum reading as fusion results .\n
         Average mode will use the mean value of readings as fusion results
      */
     int fusion_timing_;//!< Fusion timing index. \details 0 for fuse in 5HZ, other for fuse when received a data

     vector<Ranger*> rangers_;//!< Container of ranging sensor vector
     vector<double> fused_range_data_;//!< Fused sampling data
     vector<vector<double> > raw_range_data_;//!< Unfused sampling data
     /*!< This is a vector of vector.
         //The first layer is the order of sensors,
         //while the second layer stores actually sampling data.
      */



     vector<int> view_angles_;//!< Start and end angales of view corresponding to sensors
     /*!<
     //This is the start and end angles of each sensors
     //Calculated using Field of view, and assumed every sensors are places symmetrically about the centre.
     //Centre is 90 degree and angle increase counterclockwisely.
     */
     //Each even numbers are start angles of sensors, while the following odd number is end angles

     //private methods
     char setFusionBase();//!<Find the densest data sample among rangers


     void setAngles();//!< Set the view angles \sa view_angles_

     double fuseMax(vector<double> *fusing_values);//!<Compute the max value of fuse data
     double fuseMin(vector<double> *fusing_values);//!<Compute the min value of fuse data
     double fuseAverage(vector<double> *fusing_values);//!<Compute average value of fuse data

     //! linearly extrapolate data from sensors
     /*!
        Perform linear extrapolation using current data sampale as well as last data sample
     */
     vector<double> linearExtrapolation(int sensor_number);

     //! Handle the bountray case
     /*!
     //The data in the fusing_values will be compared with its max range and
         //readings from other sensors. If a reading is hiting its max boundary and there is
         //another sensor's reading from the same location is larger than the it, it will be
         //abandoned. The reason is that there is likely an object outside of its range, but can be
         //by another sensor with longer range, and therefore this data shall not be consindered
         //in the fusion.
     */
     void boundaryCaseHandling(vector<double> *fusing_values);

     //! Compute fused data
     /*!
      Data fusions are conducted in this method.

     Firstly,the densest sampling data set amoung all sensors is found
     using setFusionBase() methods.

     Secondly, the distribution, in terms of anlges,
     of the densest sampling data set is calculated based on its field of view and
     number of samples.

     Thridly, all samples of all sensors will be scaned in order.
     Each time, a vector ,called fusing_values, is used to store data from all sensors in only one location.

     Fourthly, max boundary condition will be handled using setAngles() method.

     Finally, according to the mode selected by the users, the samples will
     be fused with their maximum value, their minimum value or their average value.
     */

     void setFuseRangeData();
     //! Extrct raw data from rangers
     /*!
     \sa raw_range_data_, getRawRangeData()
     */
     void setRawRangeData();


     RangerFusion();//shall not be used
};

#endif // RANGER_FUSION_H
