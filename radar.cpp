#include "radar.h"

using std::vector;

Radar::Radar():Ranger("RAD-001",10.0,0.2, 40,10)
{

}

void Radar::setFieldOfView(const int *field_of_view)
{
    field_of_view_=*field_of_view;
}


void Radar::dataGeneration()
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

bool Radar::ifSaneFieldOfView()
{
    if(field_of_view_==20||field_of_view_==40)
    {
        return true;
    }
    else
    {
        field_of_view_=20;
        return false;
    }
}
