#include <iostream>
#include <thread>
#include <unistd.h>
#include <cmath>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "ranger_fusion.h"
#include "radar.h"
#include "sonar.h"

using std::cout;
using std::endl;
using std::cin;
using std::thread;
using std::ref;

//functions for console________________________________________________
void const getFixParameter (Ranger* ranger)
{
    cout<<"The model: "<<ranger->getModel()<<endl;
    cout<<"The max distance: "<< ranger->getMaxDistance()<<"m"<<endl;
    cout<<"The min distance: "<< ranger->getMinDistance()<<"m"<<endl;
    cout<<"The Data Rate:"<< ranger->getDataRate()<<"HZ"<<endl;

}

void const rangerInitiliseMessage(vector<Ranger*> ranger)
{
    for(int i=0;i<ranger.size();i++)
    {
        cout<<"Ranging sensor # "<<i+1<<" is initialised"<<endl;
        getFixParameter (ranger.at(i));
        cout<<endl;
    }
}

void setBaud(Ranger* ranger,const long* baud)
{
    ranger->setBaud(baud);
    if (ranger->ifSaneBaud())
    {
        cout<<"The baud entered, " <<ranger->getBaud() << ", is valid."<<endl;
    }
    else
    {
        cout<<"The baud entered is invalid. The default value 38400 will be used."<<endl;
    }

    cout<<endl;

}

void setPort(Ranger* ranger, const int *port)
{
    ranger->setPort(port);
    if (ranger->ifSanePort())
    {
        cout<<"The port entered, " <<ranger->getPort() << ", is valid."<<endl;
    }
    else
    {
        cout<<"The port entered is invalid. The default value 0 will be used."<<endl;
    }
    cout<<endl;
}

void setParametersRadar(Radar* radar)
{
    cout<<"Please enter the baud (38400 or 115200) for Rader"<<endl;
    static long baud;
    cin>>baud;
    setBaud(radar,&baud);

    cout<<"Please enter the USB port (0,1 or 2) for Rader"<<endl;
    static int port;
    cin>>port;
    setPort(radar,&port);

    cout<<"Please enter the field of view (20 or 40) deg for Rader"<<endl;
    static int field_of_view;
    cin>>field_of_view;
    radar->setFieldOfView(&field_of_view);
    if (radar->ifSaneFieldOfView())
    {
         cout<<"The field of view entered, " <<radar->getFieldOfView() << ", is valid."<<endl;
    }
    else
    {
        cout<<"The field of view entered is invalid. The default value 20 will be used."<<endl;
    }

    cout<<endl;
}

void setParametersSonar(Sonar* sonar)
{
    cout<<"Please enter the baud (38400 or 115200) for Sonar"<<endl;
    static long baud;
    cin>>baud;
    setBaud(sonar,&baud);
    cout<<"Please enter the USB port (0,1 or 2) for Sonar"<<endl;
    static int port;
    cin>>port;
    setPort(sonar,&port);
    cout<<endl;
}


void infoRadar(Radar* radar)
{
    getFixParameter (radar);
    cout<<"The Baud: "<<radar->getBaud()<<endl;
    cout<<"The USB Port: "<<radar->getPort()<<endl;
    cout<<"The Field of View: "<<radar->getFieldOfView()<<" deg"<<endl;
    cout<<"The Number of each Sample: "<<radar->getSampleSize()<<endl<<endl;
}

void infoSonar(Sonar* sonar)
{
    getFixParameter (sonar);
    cout<<"The Baud: "<<sonar->getBaud()<<endl;
    cout<<"The USB Port: "<<sonar->getPort()<<endl;
    cout<<"The Field of View: "<<sonar->getFieldOfView()<<" deg"<<endl;
    cout<<"The Number of each Sample: "<<sonar->getSampleSize()<<endl<<endl;
}


int setFusionMode()
{
    cout<<"Please select the sensor fusion modes:"<<endl<<"Enter 0 for min, 1 for max, or any other number for average"<<endl;
    cout<<"Min mode will select the minimum reading as fusion results"<<endl;
    cout<<"Max mode will select the maximum reading as fusion results"<<endl;
    cout<<"Average mode will use the mean value of readings as fusion results"<<endl;

    int mode;
    cin>>mode;

    if(mode==0)
    {
        cout<<"Min mode is selected."<<endl;
    }
    else if(mode==1)
    {
        cout<<"Max mode is selected."<<endl;
    }
    else
    {
        cout<<"Average mode is selected."<<endl;
    }

    cout<<endl;
    return mode;
}

int setFusionTiming()
{

    cout<<"Please select the fusion timing: "<<endl<<"Enter 0 for 5 HZ-timing, or any other number for Data-coming-timing"<<endl;
    cout<<"5 HZ-timing: Fusion will be provided in each 0.25s"<<endl;
    cout<<"Data-coming-timing: Fusion will be provided once a reading is received from a sensor."<<endl;
    int fusion_timing;

    cin>>fusion_timing;

    if(fusion_timing==0)
    {
        cout<<"Fusion will be provided in 5 HZ"<<endl;
    }
    else
    {
        cout<<"Fusion will be provided once a reading is received from a sensor."<<endl;
    }
        cout<<endl;
        return fusion_timing;

}

//Threading____________________________________________________________________
/*!
 *  \brief     Thread Functions Class, used in Main
 *  \details
 *  This class contains all members to be used in threading.\n
 *  \author    Jian Zhan
 *  \version   1.00
 *  \date      2017
 */
class ThreadFunctions
{
public:
    ThreadFunctions();//!< ThreadFunctions constructor \details initialise start time \sa start_time_
    /*!
     * \brief Data Producer
     * \details Function for thread of data generationg
     * \param ranger sensors
     */
    void rangerDataGeneration(Ranger* ranger);

    /*!
     * \brief Data Consumer
     * \details Function for thread of data fusion
     * \param ranger_fusion to fused data from all sensors
     * \param rangers containing actual renging sensor objects
     */
    void dataFusion(RangerFusion& ranger_fusion, vector<Ranger *> rangers);

private:

    std::mutex mu_;//!< Mutex lock
    std::condition_variable cond_var_;//!< Conditional variable
    unsigned long long int start_time_;//!< Start time [s] of threading since epsoch
    bool ready_flag_=false;//!< A boolean value telling whether the fusion can be started

};

ThreadFunctions::ThreadFunctions()
{
    start_time_=std::chrono::steady_clock::now().time_since_epoch().count()/1000000;// Unit ms
}

void ThreadFunctions::rangerDataGeneration(Ranger* ranger)
{
   while(1)
   {
       usleep((1.0/ranger->getDataRate())*1000000);
       std::unique_lock<std::mutex> locker(mu_);
       ranger->dataGeneration();
//       cout<<"Model "<<ranger->getModel()<<endl;
//       cout<<"current data is "<<ranger->getData().at(0)<<endl<<endl;
       locker.unlock();
       cond_var_.notify_all();
   }
}

void ThreadFunctions::dataFusion(RangerFusion& ranger_fusion,vector<Ranger*> rangers)
{
     cout<<"Gathering Data..."<<endl<<endl;
//    usleep(1000000); // Wait for the sensors for initilisation for 1 second
    int loop_number=1;

    // check if ready
    do{
        ready_flag_=true;
        for(int i=0;i<rangers.size();i++)
        {
            mu_.lock();
            ready_flag_=!rangers.at(i)->getDataPrevious().empty();
            mu_.unlock();
        }
    }while(!ready_flag_);


    while(1)
    {
        //what timing mode?
         if (ranger_fusion.getFusionTiming()==0)
         {
             usleep((1.0/5.0)*1000000);
         }
         std::unique_lock<std::mutex> locker(mu_);
         if(ranger_fusion.getFusionTiming()!=0)
         {
            cond_var_.wait(locker);
         }

        ranger_fusion.fuseData();
        vector<double> output=ranger_fusion.getFusedRangeData();
        cout<<"Sample #"<<loop_number<<endl;
        cout<<"Time lapsed from the start: "<<
              (double)(std::chrono::steady_clock::now().time_since_epoch().count()/1000000-start_time_)/1000
           <<" s"<<endl;
        for (int i=0;i<output.size();i++)
        {
            cout<<"Fused values: "<<output.at(i)<<" m"<<endl<<endl;
        }
        loop_number++;
    }
}

//Threads
void dataGenerationThread(ThreadFunctions& thread_functions,Ranger* ranger)
{
    thread_functions.rangerDataGeneration(ranger);
}

void fusionThread(ThreadFunctions& thread_functions,RangerFusion& ranger_fusion,vector<Ranger*> rangers)
{
   thread_functions.dataFusion(ranger_fusion,rangers);
}


//main function___________________________________________________
int main()
{
    //Initilised Sensors----------------------------------
        Radar radar_1;
        Sonar sonar_1;
        vector<Ranger*> rangers;
        rangers.push_back(&radar_1);
        rangers.push_back(&sonar_1);

        rangerInitiliseMessage(rangers);//print fix parameters

    //set configurable parameters----------------------------------

        setParametersRadar(&radar_1);
        setParametersSonar(&sonar_1);

        //information summary
        cout<<"Sensor information summary:"<<endl;
        cout<<"#1"<<endl;
        infoRadar(&radar_1);
        cout<<"#2"<<endl;
        infoSonar(&sonar_1);
        cout<<endl;

        int mode = setFusionMode();
        int fusion_timing=setFusionTiming();

        //Give the user a moment to read the messages,and ask the user to press Enter to proceed.
        cout<<"Please press Enter to start the sampling."<<endl;
        cin.get();
        cin.get();

        RangerFusion ranger_fusion(mode,fusion_timing);
        ranger_fusion.setRangers(rangers);
        ThreadFunctions thread_functions;

        //define thread   
       thread thread_radar_data(dataGenerationThread,ref(thread_functions),&radar_1);
       thread thread_sonar_data(dataGenerationThread,ref(thread_functions), &sonar_1);
       thread thread_data_fusion(fusionThread,ref(thread_functions),ref(ranger_fusion),rangers);

       //join
       thread_radar_data.join();
       thread_sonar_data.join();
       thread_data_fusion.join();

        return 0;

}
