#include "ranger_fusion.h"



RangerFusion::RangerFusion(int mode,int fusion_timing):mode_(mode),fusion_timing_(fusion_timing)
{
}

// Accepts container of rangers as per requirement C2 of assignment 2
void RangerFusion::setRangers(vector<Ranger*> rangers)
{
    rangers_.clear();
    rangers_=rangers;


}

int RangerFusion::getFusionTiming()
{
    return fusion_timing_;
}

void RangerFusion::fuseData()
{
    setRawRangeData();
    setAngles();
    setFuseRangeData();
}

 // Returns a container of fused range readings as per C4 assignment 2
vector<double> RangerFusion::getFusedRangeData()
{
    return fused_range_data_;
}

 // Returns a container of raw/unfused range readings as per C5 assignment 2
vector<vector<double> > RangerFusion::getRawRangeData()
{
   return raw_range_data_;
}

//internal set methods----------------------------------------
void  RangerFusion::setAngles()
{
     view_angles_.clear();
     for (int i=0;i<rangers_.size();i++)
     {
       view_angles_.push_back(90-rangers_.at(i)->getFieldOfView()/2);//start angle
       view_angles_.push_back(90+rangers_.at(i)->getFieldOfView()/2);//end angle
     }
}

void RangerFusion::setRawRangeData()
{

     raw_range_data_.clear();


     for(int i=0;i<rangers_.size();i++)
     {

         raw_range_data_.push_back(linearExtrapolation(i));
     }

}


//fusion method----------------------------------
void RangerFusion::setFuseRangeData()
{
    fused_range_data_.clear();
    char base_index=setFusionBase();

    //find all the angles-----------------------------
    int angles[fused_range_data_.size()];
    angles[0]=view_angles_.at(base_index*2);
    int interval=0;
    if (fused_range_data_.size()!=0&&fused_range_data_.size()!=1)
    {
        interval=rangers_.at(base_index)->getFieldOfView()/(fused_range_data_.size()-1);
    }

    for (int i=1;i<fused_range_data_.size();i++)
    {
        angles[i]  = angles[i-1] +interval;
    }

    //Store readings from all sensor with repect to one location is store in a sensor
    static vector<double> fusing_values;
    for (int i=0;i<fused_range_data_.size();i++)
    {
        fusing_values.clear();

        for (int j=0;j<rangers_.size();j++)
        {
            if(j==base_index)
            {
                fusing_values.push_back(raw_range_data_.at(j).at(i));
            }
            else if (angles[i]>=view_angles_.at(j*2) && angles[i]<=view_angles_.at(j*2+1) )
            {
                fusing_values.push_back(raw_range_data_.at(j).front());
            }
        }

        //erase a value if it is hiting the max boundary, while a greater reading is obtained by other sensors
        boundaryCaseHandling(&fusing_values);

        //start fusing--------------------------------------
        //max fusion mode
        if (mode_==0)
        {
            fused_range_data_.at(i)=fuseMin(&fusing_values);
        }
        //min fusion mode
        else if (mode_==1)
        {
            fused_range_data_.at(i)=fuseMax(&fusing_values);
        }

        //average fusion mode
        else
        {
            fused_range_data_.at(i)=fuseAverage(&fusing_values);
        }
    }
}

//Functions used in fusion---------------------------------------
//find the densest data--------------------------------
char RangerFusion::setFusionBase()
{
    int max_size=0;
    char index_max_size=0;


    for (int i=0;i<rangers_.size();i++)
    {
        if (raw_range_data_.at(i).size()>max_size)
        {
            max_size=raw_range_data_.at(i).size();
            index_max_size=i;
        }
    }
    fused_range_data_=raw_range_data_.at(index_max_size);
    return index_max_size;
}


void RangerFusion::boundaryCaseHandling(vector<double> *fusing_values)
{
    for(int i=0;i<fusing_values->size();i++)
    {
        //check if hit the maximum boundary
        if(fusing_values->at(i) >=rangers_.at(i)->getMaxDistance())
        {
            for(int j=0;j<fusing_values->size();j++)
            {
                if (j!=i && fusing_values->at(i)<fusing_values->at(j))
                {
                    fusing_values->erase(fusing_values->begin()+i);
                    i--;
                    break;
                }
            }
        }
    }
}

//Fusion Methods------------------------------------------------
double RangerFusion::fuseMax(vector<double> *fusing_values)
{
    double fusedData=fusing_values->front();
    for(int i=1;i<fusing_values->size();i++)
    {
        if (fusedData<fusing_values->at(i))
        {
            fusedData=fusing_values->at(i);
        }
    }
    return  fusedData;
}

double RangerFusion::fuseMin(vector<double> *fusing_values)
{
    double fusedData=fusing_values->front();
    for(int i=1;i<fusing_values->size();i++)
    {
        if (fusedData>fusing_values->at(i))
        {
            fusedData=fusing_values->at(i);
        }
    }
    return  fusedData;
}

double RangerFusion::fuseAverage(vector<double> *fusing_values)
{
    double fusedData=fusing_values->front();
    for(int i=1;i<fusing_values->size();i++)
    {
        fusedData +=fusing_values->at(i);
    }
    return  fusedData/fusing_values->size();
}



vector<double> RangerFusion::linearExtrapolation(int sensor_number)
{

    vector<double> result_vector;
    vector<double> current_data = rangers_.at(sensor_number)->getData();
    vector<double> previous_data= rangers_.at(sensor_number)->getDataPrevious();
    unsigned long long int data_time = rangers_.at(sensor_number)->getDataTime();
    unsigned long long int data_time_previous=rangers_.at(sensor_number)->getDataTimePrevious();

    for (int j=0;j<current_data.size();j++)
    {
       double current_data_single =current_data.at(j);
       double previous_data_single =previous_data.at(j);

       // y1=k*x1+b; y2=k*x2+b
       // k=(y2-y1)/(x2-x1)
       // b=y1-k*x1
       // y3 = t3*k+b

       double k=(current_data_single-previous_data_single)/(data_time-data_time_previous);
       int now_time=std::chrono::steady_clock::now().time_since_epoch().count()/1000000;
       double intercept=current_data_single-k*data_time;
       double result = now_time*k
               +intercept;

//            std::cout<<"Model "<<rangers_.at(sensor_number)->getModel()<<std::endl;
//            std::cout<<"Previous Data: "<<previous_data_single<<std::endl;
//            std::cout<<"Previous Time: "<<data_time_previous<<std::endl;
//            std::cout<<"Current Data: "<<current_data_single<<std::endl;
//            std::cout<<"Current Time: "<<data_time<<std::endl;
//            std::cout<<"k = "<<k<<std::endl;
//            std::cout<<"intercept = "<<intercept<<std::endl;
//            std::cout<<"The result is: "<<result<<std::endl;
//            std::cout<<"Time now is: "<<now_time<<std::endl;

       //clip into the sensor's range
       if(result>rangers_.at(sensor_number)->getMaxDistance())
       {
           result=rangers_.at(sensor_number)->getMaxDistance();
       }
       else if(result<rangers_.at(sensor_number)->getMinDistance())
       {
           result=rangers_.at(sensor_number)->getMinDistance();
       }
       result_vector.push_back(result);

    }
    return result_vector;

}
