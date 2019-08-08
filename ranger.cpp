#include "ranger.h"

using namespace std;
Ranger::Ranger(const std::string &model,double max_distance,
               double min_distance,int field_of_view,int data_rate)
    :model_(model),max_distance_(max_distance),min_distance_(min_distance)
    ,field_of_view_(field_of_view),baud_(38400),port_(0),sample_size_(1)
    ,data_rate_(data_rate), data_time_(0),data_previous_(0)
{
}

//set functions-----------------------------
void Ranger::setBaud(const long *baud)
{
    baud_=*baud;
}

void Ranger::setPort(const int *port)
{
    port_=*port;
}

void Ranger::setSampleSize(const int* sample_size)
{
    sample_size_= *sample_size;
}


//get functions-----------------------------
std::string Ranger::getModel()
{
    return model_;
}

double Ranger::getMaxDistance()
{
    return max_distance_;
}

double Ranger::getMinDistance()
{
    return min_distance_;
}

int Ranger::getDataRate()
{
    return data_rate_;
}

long Ranger::getBaud()
{
    return baud_;
}

int Ranger::getPort()
{
    return port_;
}

int Ranger::getFieldOfView()
{
    return field_of_view_;
}

int Ranger::getSampleSize()
{
    return sample_size_;
}

std::vector<double> Ranger::getData()
{
    return data_;
}

unsigned long long int Ranger::getDataTime()
{
    return data_time_;
}

std::vector<double> Ranger::getDataPrevious()
{
    return data_previous_;
}

unsigned long long int Ranger::getDataTimePrevious()
{
    return data_time_previous_;
}


//if sane functions-------------------------
bool Ranger::ifSaneBaud()
{
    if(baud_==38400||baud_==115200)
    {
        return true;
    }
    else
    {
        baud_=38400;
        return false;
    }
}

bool Ranger::ifSanePort()
{
    if(port_==0||port_==1||port_==2)
    {
        return true;
    }
    else
    {
        port_=0;
        return false;
    }
}

//random number generating methods
double Ranger::getRand()
{

    data_time_=std::chrono::steady_clock::now().time_since_epoch().count();
    srand(data_time_);
    data_time_/=1000000;
    std::default_random_engine generater(rand());
    std::normal_distribution<double> n_distribution(0.0,0.1);

    double r=6+(4*sin(2*M_PI*0.01*data_time_))+n_distribution(generater);
    return r;
}



