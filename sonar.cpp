#include "sonar.h"
using std::vector;

Sonar::Sonar():Ranger("SONX-001",6.0,0.2,90,3)
{
}

void Sonar::dataGeneration()
{
    data_previous_=data_;
    data_time_previous_=data_time_;
    data_.clear();

    for (int i=0;i<sample_size_;i++)
    {
        double  rand=getRand();
        if(rand>max_distance_)
        {
            data_.push_back(max_distance_);
        }
        else if(rand<min_distance_)
        {
            data_.push_back(min_distance_);
        }
        else
        {
            data_.push_back(rand);
        }
    }
}

//shall not be used for sonar
void Sonar::setFieldOfView(const int *field_of_view){};
