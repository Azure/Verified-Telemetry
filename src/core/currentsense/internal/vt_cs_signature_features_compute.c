/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_signature_features.h"
#include "vt_debug.h"
#include <math.h>

#define VT_CS_BUFFER_COUNT_UP             0x01
#define VT_CS_BUFFER_COUNT_DOWN           0x02
#define VT_CS_FIND_MINMAX_SEED_MAX_VALUE  -1
#define VT_CS_FIND_MINMAX_SEED_MIN_VALUE  65535
#define VT_CS_LOW_STD_DEVIATION_THRESHOLD 1.0f
#define VT_CS_PEAK_DETECTOR_SEED_POINTS   2

#define TOPNPEAKS 3
#define VT_ACR_DEVIATION 1

// static VT_VOID autocorrelation(VT_FLOAT* raw_signature, VT_FLOAT* lag_array, VT_INT sample_length, VT_INT lag_length)
// {
//         #if VT_LOG_LEVEL > 2
//         VT_INT decimal;
//     VT_FLOAT frac_float;
//     VT_INT frac;

//             VT_INT decimal1;
//     VT_FLOAT frac_float1;
//     VT_INT frac1;
//     #endif
//     VT_FLOAT var_sum_average = 0;
//     for (VT_INT iter1 = 0; iter1 < sample_length; iter1++)
//     {
//         var_sum_average += raw_signature[iter1];
//     }
//     var_sum_average /= sample_length;
//     for (VT_INT iter1 = 0; iter1 < sample_length; iter1++)
//     {
//         raw_signature[iter1] -= var_sum_average;
//     }
//     for (VT_INT iter1 = 0; iter1 < lag_length; iter1++)
//     {
//         lag_array[iter1] = raw_signature[iter1];
//     }
//     VT_INT iter2 = 0;
//     VT_INT iter3 = 0;

//     printf("\n");
//             for (VT_INT iter = 0; iter < VT_CS_SAMPLE_LENGTH; iter++)
//     {
//         decimal    = raw_signature[iter];
//         frac_float = raw_signature[iter] - (VT_FLOAT)decimal;
//         frac       = fabsf(frac_float) * 10000;
//         VTLogDebugNoTag("%d.%04d, ", decimal, frac);
//     }
//     printf("\n");

//     for (iter3 = 0; iter3 < (sample_length - lag_length); iter3++)
//     {
//         var_sum_average = 0;
       



//         for (iter2 = 0; iter2 < lag_length; iter2++)
//         {   
//             var_sum_average += (raw_signature[(iter3 + iter2)] * lag_array[iter2]);

//         //     if(iter3<5){

//         // decimal    = raw_signature[(iter3 + iter2)];
//         // frac_float = raw_signature[(iter3 + iter2)] - (VT_FLOAT)decimal;
//         // frac       = fabsf(frac_float) * 10000;

//         //         decimal1    = lag_array[iter2];
//         // frac_float1 = lag_array[iter2] - (VT_FLOAT)decimal1;
//         // frac1       = fabsf(frac_float1) * 10000;


//         // VTLogDebugNoTag("%d.%04d * %d.%04d \n", decimal, frac,decimal1,frac1);}
//         }
//         raw_signature[iter3] = var_sum_average;
//     }
//     for (VT_INT iter1 = 1; iter1 < sample_length - lag_length; iter1++)
//     {
//         if (raw_signature[0])
//         {
//             raw_signature[iter1] /= raw_signature[0];
//         }
//     }
//     raw_signature[0] = 1;
// }

// static VT_UINT check_acr_peak_present(VT_FLOAT* raw_signature,
//     VT_UINT* index,
//     VT_UINT period,
//     VT_UINT* period_total,
//     VT_UINT* peaks,
//     VT_FLOAT minimum_correlation_for_peak)
// {   

//     VT_FLOAT maxval=raw_signature[*index];
//     VT_UINT maxvalindex=*index;

//     if(*index>=VT_CS_SAMPLE_LENGTH-VT_CS_AUTO_CORRELATION_LAG){
//         maxval=0;
//         for (VT_UINT iterx=*index-1;iterx<VT_CS_SAMPLE_LENGTH-VT_CS_AUTO_CORRELATION_LAG;iterx++){
//             if (raw_signature[iterx]>maxval){
//             maxval=raw_signature[iterx];
//             maxvalindex=iterx;
//             }

//         }

//     }
//     else{
//     for(VT_INT iter=-1; iter<2;iter++){ //make 2 a macro
//         if((raw_signature[(*index)+iter]>maxval) &&(((*index)+iter)!=0 ) && (fabs((double)iter)<period)) {
//             maxval=raw_signature[(*index)+iter];
//             maxvalindex=(*index)+iter;
//         }
//         }}

//     if (maxval>minimum_correlation_for_peak){
        
        
//         *peaks += 1;
//         *period_total += maxvalindex;
//         *index = maxvalindex + period; 
//         #if VT_LOG_LEVEL > 2
//         VTLogDebugNoTag("SUCCESS : peaks = %d, period_total = %d, index= %d \n",*peaks,*period_total,*index);
//         #endif
//         return VT_SUCCESS;}    


//     else

//     {
//                #if VT_LOG_LEVEL > 2
//         VTLogDebugNoTag("FAILURE : peaks = %d, period_total = %d, index= %d \n",*peaks,*period_total,*index);
//         #endif
    
//         return VT_ERROR;
//     }
// }

// static VT_UINT period_calculate(VT_FLOAT* raw_signature, VT_FLOAT* period)
// {
//     VT_FLOAT raw_signature_copy[VT_CS_SAMPLE_LENGTH] = {0};
//     VT_FLOAT lag_array[VT_CS_AUTO_CORRELATION_LAG]   = {0};
//     VT_UINT peaks                                    = 0;
//     VT_UINT index                                    = 0;
//     VT_UINT period_total                             = 0;
//     *period                                          = 0;

//     #if VT_LOG_LEVEL > 2
//         VT_INT decimal;
//     VT_FLOAT frac_float;
//     VT_INT frac;
//     #endif

//     for (VT_UINT iter = 0; iter < VT_CS_SAMPLE_LENGTH; iter++)
//     {
//         raw_signature_copy[iter] = raw_signature[iter];
//     }
//     autocorrelation(raw_signature_copy, lag_array, VT_CS_SAMPLE_LENGTH, VT_CS_AUTO_CORRELATION_LAG);
//     #if VT_LOG_LEVEL > 2
//         VTLogDebugNoTag("\nACR RAW: \n");
//     for (VT_INT iter = 0; iter < VT_CS_SAMPLE_LENGTH; iter++)
//     {
//         decimal    = raw_signature_copy[iter];
//         frac_float = raw_signature_copy[iter] - (VT_FLOAT)decimal;
//         frac       = fabsf(frac_float) * 10000;
//         VTLogDebugNoTag("%d.%04d, ", decimal, frac);
//     }
//     printf("\n");
//     #endif


//     for (VT_UINT iter = 2; iter < VT_CS_SAMPLE_LENGTH - VT_CS_AUTO_CORRELATION_LAG - 2; iter++) 
//     {
//         index = iter;
 
//         bool cond1=(raw_signature_copy[iter] > raw_signature_copy[iter - 1]) || (raw_signature_copy[iter] > raw_signature_copy[iter - 2]);
//         bool cond2=(raw_signature_copy[iter] > raw_signature_copy[iter + 1]) || (raw_signature_copy[iter] > raw_signature_copy[iter + 2]);
//         VT_UINT flag=(int)cond1+(int)cond2;

//         if (flag==2)

//         {

//             period_total = 0;
//             peaks        = 0;
//             while (index < VT_CS_SAMPLE_LENGTH - VT_CS_AUTO_CORRELATION_LAG + 1)  //make 2 a macro
            
//             {       
//                 #if VT_LOG_LEVEL > 2
//         VTLogDebugNoTag("ENTERED IN : peaks = %d, period_total = %d, index= %d, iter= %d \n",peaks,period_total,index,iter);
//         #endif
//                 if (check_acr_peak_present(raw_signature_copy, &index, iter, &period_total, &peaks, VT_CS_MIN_CORRELATION) ==
//                     VT_ERROR)

//                     break;
//                 }
//             }
//             if (index > VT_CS_SAMPLE_LENGTH - VT_CS_AUTO_CORRELATION_LAG)
//             {   
//                 *period = iter;
//                 break;
//             }
//         }
    

//     if (peaks < 2)
//     {
//         *period = 0;
//         return VT_ERROR;
//     }
//     else
//     {
//         *period = (((period_total * 2.0f) / (VT_FLOAT)peaks) - (VT_FLOAT)(2.0f * (*period))) / (VT_FLOAT)(peaks - 1);
//         return VT_SUCCESS;
//     }
// }
static VT_FLOAT autocovariance(VT_FLOAT* raw_sig, VT_UINT lag){

    

    VT_FLOAT covar[VT_CS_SAMPLE_LENGTH]={0};
    VT_FLOAT raw_sig_copy[VT_CS_SAMPLE_LENGTH]={0};
    VT_FLOAT covariance=0;
    VT_FLOAT mean=0;
    VT_FLOAT autocvr=0;

    for(int i=0;i<VT_CS_SAMPLE_LENGTH;i++){
        mean=mean+raw_sig[i];
    }
    mean=mean/VT_CS_SAMPLE_LENGTH;

    for(int i=0;i<VT_CS_SAMPLE_LENGTH;i++){
        raw_sig_copy[i]=raw_sig[i];
    }
    

    for(int i=lag;i<VT_CS_SAMPLE_LENGTH;i++){
   
        covariance = (raw_sig_copy[i]-mean) * (raw_sig_copy[i - lag]-mean);
        covar[i-lag]=covariance;
        

    }

    for(int i=0;i<VT_CS_SAMPLE_LENGTH;i++){
        autocvr=autocvr+covar[i];
    }


    autocvr=autocvr/(float)(VT_CS_SAMPLE_LENGTH);


   
    return autocvr;

}

static VT_VOID autocorrelation(VT_FLOAT* raw_signature, VT_FLOAT* acr_sig){

//VT_FLOAT acf[VT_CS_SAMPLE_LENGTH] = {0};

        #if VT_LOG_LEVEL > 2
    int decimal;
    VT_FLOAT frac_float;
    VT_INT frac;
    #endif

    VT_FLOAT div=0;
    //bool less;

for (int i=0;i<128;i++){

acr_sig[i]=autocovariance(raw_signature, i);
}
div=acr_sig[0];


// printf("\nACR FUNC:\n");
//     for (VT_INT iter = 0; iter < VT_CS_SAMPLE_LENGTH; iter++)
//     {
//         decimal    = acr_sig[iter];
//         frac_float = acr_sig[iter] - (VT_FLOAT)decimal;
//         frac       = fabsf(frac_float) * 10000;
//         VTLogDebugNoTag("%d.%04d, ", decimal, frac);
//     }
//     printf("\ndiv:");

//         decimal    = div;
//         frac_float = div - (VT_FLOAT)decimal;
//         frac       = fabsf(frac_float) * 10000;
//         VTLogDebugNoTag("%d.%04d, ", decimal, frac);

for (int i=0;i<VT_CS_SAMPLE_LENGTH;i++){

acr_sig[i]=(float)acr_sig[i]/(float)div;
}


    #if VT_LOG_LEVEL > 2
    printf("\nACR FUNC:\n");
    for (VT_INT iter = 0; iter < VT_CS_SAMPLE_LENGTH; iter++)
    {
        decimal    =acr_sig[iter];
        frac_float = acr_sig[iter] - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        if(acr_sig[iter]<0 && acr_sig[iter]>-1){
            VTLogDebugNoTag("-%d.%04d, ", decimal, frac);
        }
        else{
        VTLogDebugNoTag("%d.%04d, ", decimal, frac);}
    }
    printf("\n");
    #endif

}


static VT_UINT gettopNidx(VT_INT* topNidx,VT_FLOAT* acr_sig){
VT_INT idx=0;
VT_FLOAT max=-100;
VT_UINT found=0;
VT_UINT foundonce=0;

VT_FLOAT raw_sig[VT_CS_SAMPLE_LENGTH]={0};

    for(int i=0;i<VT_CS_SAMPLE_LENGTH;i++){
        raw_sig[i]=acr_sig[i];
    }

for(int i=0;i<TOPNPEAKS;i++){
    max=-100;
    found=0;
    for(int j=1;j<VT_CS_SAMPLE_LENGTH/2;j++){
        if (raw_sig[j]>max && raw_sig[j] > 0.2f && raw_sig[j]>raw_sig[j-1] && raw_sig[j]>raw_sig[j+1]){
            found=1;
            max=raw_sig[j];
            idx=j;
        }
            }
        
        if(found){
        //raw_sig[idx]=(raw_sig[idx-1]+raw_sig[idx+1])/2; 
        foundonce=1; 
        raw_sig[idx]=-100;
        // raw_sig[idx-1]=raw_sig[idx];
        topNidx[i]=idx;}


}
if (foundonce){return 1;}
else {return 0;}

}

static VT_UINT check_acr_peaks(VT_FLOAT* acr_sig,
    VT_UINT* index,
    VT_UINT period,
    VT_FLOAT* energy,
    VT_UINT* peaks,
    VT_UINT* totaljump,VT_UINT* totalhighpeaks)
{   

    if(period==*index){
        *energy=*energy+acr_sig[*index];
        *index=*index+period;
        *peaks=*peaks+1;
        *totaljump=*totaljump+period;
                #if VT_LOG_LEVEL > 2
        VTLogDebugNoTag("SUCCESSi : peaks = %d, energy = %d, index= %d \n",(int)*peaks,(int)*energy,(int)*index);
        #endif
       
        return 1;
    }

    VT_FLOAT maxval=acr_sig[*index];
    VT_UINT maxvalindex=*index;
    VT_UINT deviation=0;
    
    // if(*index>=VT_CS_SAMPLE_LENGTH-VT_CS_AUTO_CORRELATION_LAG){
    //     maxval=0;
    //     for (VT_UINT iterx=*index-1;iterx<VT_CS_SAMPLE_LENGTH-VT_CS_AUTO_CORRELATION_LAG;iterx++){
    //         if (raw_signature[iterx]>maxval){
    //         maxval=raw_signature[iterx];
    //         maxvalindex=iterx;
    //         }

    //     }

    // }
    if(*index<VT_CS_SAMPLE_LENGTH-1){
    for(VT_INT iter=-1; iter<2;iter++){ //make 2 a macro
        if((acr_sig[(*index)+iter]>maxval) &&(((*index)+iter)!=0 ) && (fabs((double)iter)<period)&& (((*index)+iter)<VT_CS_SAMPLE_LENGTH-1)) {
            maxval=acr_sig[(*index)+iter];
            maxvalindex=(*index)+iter;
            deviation=iter;
        }
        }}
    else {
        printf("\nseen till end! \n");
        return 0;}


    // if(*peaks<2 && maxval<0.2 ){
    //     printf("\nsmall second peak\n");
    //     return 0;
    // }
    if((maxval>0.2f) || (maxvalindex>64 && maxval>0.1f)){
        printf("\n high peak detected at %d\n",maxvalindex);
        *totalhighpeaks=*totalhighpeaks+1;
    }

    if (maxval>acr_sig[maxvalindex-1] && maxval>acr_sig[maxvalindex+1]){
                
        *peaks += 1;
        *totaljump=*totaljump+period+deviation;
        *index = maxvalindex + period; 
        if(*peaks<2){
        *energy=*energy+maxval;}
        #if VT_LOG_LEVEL > 2
        VTLogDebugNoTag("SUCCESS : peaks = %d, energy = %d, index= %d \n",(int)*peaks,(int)maxval,(int)maxvalindex);
        #endif
        return 1;}    


    else

    {
        #if VT_LOG_LEVEL > 2
        VTLogDebugNoTag("FAIL : peaks = %d, energy = %d, index= %d \n",(int)*peaks,(int)maxval,(int)maxvalindex);
        #endif
    
        return 0;
    }
}

static VT_UINT analyze(VT_INT start,VT_FLOAT* acr_sig, VT_FLOAT* period, VT_FLOAT* energy,VT_UINT* peaks_){
    if(start==-1){
        return 0;
    }

VT_UINT index=start;
VT_FLOAT energy_=0;
VT_UINT peaks=0;
VT_UINT totaljump=0;
VT_UINT period_=start;
VT_UINT totalhighpeaks=0;

    #if VT_LOG_LEVEL > 2
    VT_INT decimal;
    VT_FLOAT frac_float;
    VT_INT frac;
    #endif

printf("\nanalyzing start point %d\n",start);
while(check_acr_peaks(acr_sig,&index,period_,&energy_,&peaks,&totaljump,&totalhighpeaks)==1){

}
*period=(float)totaljump/(float)peaks;
*energy=energy_/2;
*peaks_=peaks;
        #if VT_LOG_LEVEL > 2
        printf("\nFINAL ANALYSIS: start= %d, peaks= %d ",start,peaks);
        decimal    =*energy;
        frac_float = *energy - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        if(*energy<0 && *energy>-1){
            VTLogDebugNoTag("energy = %d.%04d, ", decimal, frac);
        }
        else{
        VTLogDebugNoTag("energy = %d.%04d, ", decimal, frac);}

        decimal    =*period;
        frac_float = *period - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        if(*period<0 && *period>-1){
            VTLogDebugNoTag("period = %d.%04d, ", decimal, frac);
        }
        else{
        VTLogDebugNoTag("period = %d.%04d, \n", decimal, frac);}
        #endif


if((peaks<((int)(VT_CS_SAMPLE_LENGTH/start))-3) || peaks<2){
    printf("less peaks - NOT TAKEN\n");
    return 0;
}
if(totalhighpeaks<1){
    printf("\nless high peaks - NOT TAKEN");
    return 0;
}

// printf("\nFINAL ANALYSIS: start= %d, peaks= %d ",start,peaks);
//         decimal    =*energy;
//         frac_float = *energy - (VT_FLOAT)decimal;
//         frac       = fabsf(frac_float) * 10000;
//         if(*energy<0 && *energy>-1){
//             VTLogDebugNoTag("energy = %d.%04d, ", decimal, frac);
//         }
//         else{
//         VTLogDebugNoTag("energy = %d.%04d, ", decimal, frac);}

//         decimal    =*period;
//         frac_float = *period - (VT_FLOAT)decimal;
//         frac       = fabsf(frac_float) * 10000;
//         if(*period<0 && *period>-1){
//             VTLogDebugNoTag("period = %d.%04d, ", decimal, frac);
//         }
//         else{
//         VTLogDebugNoTag("period = %d.%04d, \n", decimal, frac);}

return 1;
}


static VT_UINT period_calculate(VT_FLOAT* raw_signature, VT_FLOAT* period)
{
VT_FLOAT acr_signal[VT_CS_SAMPLE_LENGTH] = {0};
    //    VT_INT decimal;
    //VT_FLOAT frac_float;
    //VT_INT frac;

autocorrelation(raw_signature, acr_signal);

//VT_FLOAT acr_signal_copy[VT_CS_SAMPLE_LENGTH]={0};
VT_INT topNidx[TOPNPEAKS]={0};
VT_FLOAT period_=0;
VT_FLOAT energy=0;
//VT_FLOAT maxenergy=0;
VT_UINT maxpeaks=0;
VT_UINT result=0;
VT_UINT resulttemp;
VT_UINT peaks=0;

for(int i=0;i<TOPNPEAKS;i++){
    topNidx[i]=-1;
}


if(!gettopNidx(topNidx,acr_signal)){
    return VT_ERROR;
}

for(int i=0;i<TOPNPEAKS;i++){
    printf(" %d ",topNidx[i]);
}
//.
for(int i=0;i<TOPNPEAKS;i++){
    //printf("\nseeing start point %d\n",i);
    energy=0;
    period_=0;
    resulttemp=analyze(topNidx[i],acr_signal,&period_,&energy,&peaks);
result= result || resulttemp;
if(peaks>maxpeaks && resulttemp){
    maxpeaks=peaks;
    *period=period_;
    //maxenergy=energy;
}




}

if(result){
    return VT_SUCCESS;
}
else {return VT_ERROR;}

}

static VT_FLOAT average_calculate(
    VT_FLOAT* raw_signature, VT_UINT sample_length, VT_UINT start_point, VT_UINT count_up_count_down)
{
    VT_FLOAT avg_curr = 0;
    if (count_up_count_down == VT_CS_BUFFER_COUNT_UP)
    {
        for (VT_UINT iter = 0; iter < sample_length; iter++)
        {
            avg_curr += raw_signature[iter];
        }
    }
    else
    {
        for (VT_UINT iter = start_point; iter > start_point - sample_length; iter--)
        {
            avg_curr += raw_signature[iter];
        }
    }

    if (sample_length)
    {
        avg_curr /= sample_length;
    }
    return avg_curr;
}

static VT_FLOAT std_dev(VT_FLOAT* raw_signature, VT_UINT sample_length, VT_UINT start_point, VT_UINT count_up_count_down)
{
    VT_FLOAT mean               = average_calculate(raw_signature, sample_length, start_point, count_up_count_down);
    VT_FLOAT standard_deviation = 0.0f;
    VT_UINT i;
    if (count_up_count_down == VT_CS_BUFFER_COUNT_UP)
    {
        for (i = 0; i < sample_length; i++)
        {
            standard_deviation += powf(raw_signature[i] - mean, 2);
        }
    }
    else
    {
        for (i = start_point; i > start_point - sample_length; i--)
        {
            standard_deviation += powf(raw_signature[i] - mean, 2);
        }
    }
    return (float)sqrt(standard_deviation / sample_length);
}

static VT_UINT min_value(VT_FLOAT* raw_signature, VT_UINT sample_length, VT_FLOAT* min_value, VT_BOOL* datapoint_visited)
{
    VT_FLOAT current_min_value     = VT_CS_FIND_MINMAX_SEED_MIN_VALUE;
    VT_UINT current_min_value_iter = 0;
    for (VT_UINT iter = 0; iter < sample_length; iter++)
    {
        if (datapoint_visited[iter] == true)
        {
            continue;
        }
        if (raw_signature[iter] <= current_min_value)
        {
            current_min_value      = raw_signature[iter];
            current_min_value_iter = iter;
        }
    }

    datapoint_visited[current_min_value_iter] = true;
    *min_value                                = current_min_value;
    return current_min_value_iter;
}

static VT_UINT max_value(VT_FLOAT* raw_signature, VT_UINT sample_length, VT_FLOAT* max_value, VT_BOOL* datapoint_visited)
{
    VT_FLOAT current_max_value     = VT_CS_FIND_MINMAX_SEED_MAX_VALUE;
    VT_UINT current_max_value_iter = 0;
    for (VT_UINT iter = 0; iter < sample_length; iter++)
    {
        if (datapoint_visited[iter] == true)
        {
            continue;
        }
        if (raw_signature[iter] > current_max_value)
        {
            current_max_value      = raw_signature[iter];
            current_max_value_iter = iter;
        }
    }

    datapoint_visited[current_max_value_iter] = true;
    *max_value                                = current_max_value;
    return current_max_value_iter;
}

static VT_VOID binary_state_current_compute(VT_FLOAT* raw_signature,
    VT_UINT* category_array,
    VT_UINT sample_length,
    VT_FLOAT* curr_draw_active,
    VT_FLOAT* curr_draw_standby,
    VT_UINT* datapoints_active,
    VT_UINT* datapoints_standby)
{
#if VT_LOG_LEVEL > 2
    VT_INT decimal;
    VT_FLOAT frac_float;
    VT_INT frac;
#endif /* VT_LOG_LEVEL > 2 */
        #if VT_LOG_LEVEL > 2
    VTLogDebugNoTag("RAW SIG in BINARY COMPUTE : \n");
    for (VT_INT iter = 0; iter < VT_CS_SAMPLE_LENGTH; iter++)
    {
        decimal    = raw_signature[iter];
        frac_float = raw_signature[iter] - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebugNoTag("%d.%04d, ", decimal, frac);
    }
    #endif


    VT_FLOAT state_array[VT_CS_SAMPLE_LENGTH]   = {0};
    bool datapoint_visited[VT_CS_SAMPLE_LENGTH] = {0};
    VT_UINT low_state_count                     = 0;
    VT_UINT high_state_count                    = 0;
    VT_FLOAT z_score_low                        = 0;
    VT_FLOAT z_score_high                       = 0;
    VT_FLOAT average_low_diff_increase          = 0;
    VT_FLOAT average_high_diff_increase         = 0;
    VT_FLOAT min_max_value                      = 0;
    VT_UINT num_seedpoints =
        sample_length > (2 * VT_CS_PEAK_DETECTOR_SEED_POINTS) ? VT_CS_PEAK_DETECTOR_SEED_POINTS : (sample_length / 2);
    VT_UINT num_seedpoints_added = 0;

    VT_BOOL valid_seedpoints     = true;

    *curr_draw_active  = 0;
    *datapoints_active = 0;

    *curr_draw_standby  = 0;
    *datapoints_standby = 0;

    VT_UINT return_iter=0;

    while (num_seedpoints_added < num_seedpoints)
    {   
        
        return_iter=min_value(raw_signature, sample_length, &state_array[low_state_count], datapoint_visited);
        category_array[return_iter]=0;
        return_iter=max_value(raw_signature, sample_length, &state_array[sample_length - 1 - high_state_count], datapoint_visited);
        category_array[return_iter]=1;



        if (low_state_count && high_state_count)
        {
            if ((state_array[low_state_count] != state_array[low_state_count - 1]))
            {
                num_seedpoints_added++;
            }

            if((state_array[sample_length - 1 - high_state_count] != state_array[sample_length - 1 - high_state_count + 1]))
            {
                num_seedpoints_added++;
            }
        }

        if (state_array[low_state_count] == state_array[sample_length - 1 - high_state_count])
        {
            valid_seedpoints = false;
            break;
        }

        low_state_count++;
        high_state_count++;


    }

    if (valid_seedpoints == false)
    { 
        low_state_count  = sample_length;
        high_state_count = 0;
        for (VT_UINT iter = 0; iter < sample_length; iter++)
        {
            state_array[iter] = raw_signature[iter];
        }
    }

    if (valid_seedpoints && (sample_length - (low_state_count + high_state_count)))
    {
        while (true)
        {
            if (low_state_count > high_state_count)
            {
                return_iter=max_value(raw_signature, sample_length, &min_max_value, datapoint_visited);
            }
            else
            {
                return_iter=min_value(raw_signature, sample_length, &min_max_value, datapoint_visited);
            }

            z_score_low = fabsf(min_max_value - average_calculate(state_array, low_state_count, 0, VT_CS_BUFFER_COUNT_UP)) /
                          std_dev(state_array, low_state_count, 0, VT_CS_BUFFER_COUNT_UP);

            z_score_high = fabsf(min_max_value -
                                 average_calculate(state_array, high_state_count, sample_length - 1, VT_CS_BUFFER_COUNT_DOWN)) /
                           std_dev(state_array, high_state_count, sample_length - 1, VT_CS_BUFFER_COUNT_DOWN);

            average_low_diff_increase = fabsf(
                (((average_calculate(state_array, low_state_count, 0, VT_CS_BUFFER_COUNT_UP) * low_state_count) + min_max_value) /
                    (low_state_count + 1)) -
                average_calculate(state_array, high_state_count, sample_length - 1, VT_CS_BUFFER_COUNT_DOWN));

            average_high_diff_increase =
                fabsf((((average_calculate(state_array, high_state_count, sample_length - 1, VT_CS_BUFFER_COUNT_DOWN) *
                            high_state_count) +
                           min_max_value) /
                          (high_state_count + 1)) -
                      average_calculate(state_array, low_state_count, 0, VT_CS_BUFFER_COUNT_UP));

            if (fabsf(z_score_high - z_score_low) < VT_CS_LOW_STD_DEVIATION_THRESHOLD)
            {
                if (average_high_diff_increase > average_low_diff_increase)
                {
                    state_array[(sample_length - 1) - high_state_count] = min_max_value;
                    category_array[return_iter]=1;
                    high_state_count++;
                }
                else
                {
                    state_array[low_state_count] = min_max_value;
                    category_array[return_iter]=0;
                    low_state_count++;
                }
            }
            else
            {
                if (z_score_high < z_score_low)
                {
                    state_array[(sample_length - 1) - high_state_count] = min_max_value;
                    category_array[return_iter]=1;
                    high_state_count++;
                }
                else
                {
                    state_array[low_state_count] = min_max_value;
                    category_array[return_iter]=0;
                    low_state_count++;
                }
            }

            if (low_state_count + high_state_count == sample_length)
            {
                break;
            }
        }
    }
 
    if (high_state_count)
    {
        *curr_draw_active = average_calculate(state_array, high_state_count, sample_length - 1, VT_CS_BUFFER_COUNT_DOWN);
    }
    *datapoints_active = high_state_count;

    if (low_state_count)
    {
        *curr_draw_standby = average_calculate(state_array, low_state_count, 0, VT_CS_BUFFER_COUNT_UP);
    }
    *datapoints_standby = low_state_count;



}

VT_UINT cs_repeating_signature_feature_vector_compute(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT* raw_signature,
    VT_UINT raw_signature_length,
    VT_FLOAT sampling_frequency,
    VT_FLOAT* signature_frequency,
    VT_FLOAT* duty_cycle,
    VT_FLOAT* relative_current_draw)
{
    VT_FLOAT signature_period_datapoints = 1;
    VT_FLOAT curr_draw_active            = 0;
    VT_FLOAT curr_draw_standby           = 0;
    VT_UINT datapoints_active            = 1;
    VT_UINT datapoints_standby           = 1;

    VT_UINT category_array[raw_signature_length];

#if VT_LOG_LEVEL > 2
    VT_INT decimal;
    VT_FLOAT frac_float;
    VT_INT frac;
#endif /* VT_LOG_LEVEL > 2 */

    #if VT_LOG_LEVEL > 2
    VTLogDebugNoTag("RAW SIG : \n");
    for (VT_INT iter = 0; iter < VT_CS_SAMPLE_LENGTH; iter++)
    {
        decimal    = raw_signature[iter];
        frac_float = raw_signature[iter] - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebugNoTag("%d.%04d, ", decimal, frac);
    }
    #endif

    binary_state_current_compute(
            raw_signature, category_array,raw_signature_length, &curr_draw_active, &curr_draw_standby, &datapoints_active, &datapoints_standby);

     #if VT_LOG_LEVEL > 2
    VTLogDebugNoTag("\nactive points : \n");
    for (VT_INT iter = 0; iter < VT_CS_SAMPLE_LENGTH; iter++)
    {

        VTLogDebugNoTag("%d, ", category_array[iter]);
    }
    printf("\n");
    #endif
    
    

#if VT_LOG_LEVEL > 2
        VT_FLOAT curr_draw_relative           = curr_draw_active-curr_draw_standby;
  VTLogDebugNoTag("\nprinting currents:\n");
          decimal    = curr_draw_standby;
        frac_float = curr_draw_standby - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebugNoTag("STANDBY : %d.%04d, ", decimal, frac);

        decimal    = curr_draw_active;
        frac_float = curr_draw_active - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebugNoTag("ACTIVE : %d.%04d, ", decimal, frac);

                decimal    = curr_draw_relative;
        frac_float = curr_draw_relative - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebugNoTag("RELATIVE : %d.%04d \n", decimal, frac);
#endif

    VT_FLOAT raw_signature_copy[VT_CS_SAMPLE_LENGTH]={0};
    for (VT_UINT iter = 0; iter < VT_CS_SAMPLE_LENGTH; iter++)
    {
        raw_signature_copy[iter] = raw_signature[iter];
    }


if (sampling_frequency==VT_CS_ADC_MAX_SAMPLING_FREQ){
    #if VT_LOG_LEVEL > 2
    VTLogDebugNoTag("BEFORE NORM : \n");
    for (VT_INT iter = 0; iter < VT_CS_SAMPLE_LENGTH; iter++)
    {
        decimal    = raw_signature_copy[iter];
        frac_float = raw_signature_copy[iter] - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebugNoTag("%d.%04d, ", decimal, frac);
    }
    #endif
    
    if(curr_draw_active/curr_draw_standby>1.8f){
            #if VT_LOG_LEVEL > 2
    VTLogDebugNoTag("\nDID NORMALIZE\n");
    #endif
        VT_FLOAT offset=curr_draw_active-curr_draw_standby;
        for(VT_UINT iter=0;iter<128;iter++){
            if(category_array[iter]==0){
                raw_signature_copy[iter]=raw_signature_copy[iter]+offset;

            }

        }  
        binary_state_current_compute(
            raw_signature_copy, category_array,raw_signature_length, &curr_draw_active, &curr_draw_standby, &datapoints_active, &datapoints_standby);
                #if VT_LOG_LEVEL > 2
        printf("AFTER NORM : \n");
    for (VT_INT iter = 0; iter < VT_CS_SAMPLE_LENGTH; iter++)
    {
        decimal    = raw_signature_copy[iter];
        frac_float = raw_signature_copy[iter] - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebugNoTag("%d.%04d, ", decimal, frac);
    }
    printf("\n");
    #endif

    }

}

*relative_current_draw = curr_draw_active - curr_draw_standby;

    if (period_calculate(raw_signature_copy, &signature_period_datapoints) == VT_SUCCESS)
    {
        if (signature_period_datapoints)
        {
            *signature_frequency = sampling_frequency / signature_period_datapoints;
        }

        if (datapoints_standby || datapoints_active)
        {
            *duty_cycle = (VT_FLOAT)datapoints_active / (VT_FLOAT)(datapoints_standby + datapoints_active);
        }
        

#if VT_LOG_LEVEL > 2
        VTLogDebug(" period_calculate = VT_SUCCESS\r\n");
        decimal    = *signature_frequency;
        frac_float = *signature_frequency - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebug("Repeating Signature Frequency = %d.%04d\r\n", decimal, frac);
#endif /* VT_LOG_LEVEL > 2 */

#if VT_LOG_LEVEL > 2
        decimal    = *relative_current_draw;
        frac_float = *relative_current_draw - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebug("Repeating Signature Relative Curr Draw = %d.%04d\r\n", decimal, frac);
#endif /* VT_LOG_LEVEL > 2 */

#if VT_LOG_LEVEL > 2
        decimal    = *duty_cycle;
        frac_float = *duty_cycle - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebug("Repeating Signature Duty Cycle = %d.%04d\r\n", decimal, frac);
#endif /* VT_LOG_LEVEL > 2 */

        return VT_SUCCESS;
    }
    else
    {
        *signature_frequency=0;
        *duty_cycle=0;

    #if VT_LOG_LEVEL > 2
            VTLogDebug(" period_calculate = VT_NOT_SUCCESS\r\n");
            decimal    = *signature_frequency;
            frac_float = *signature_frequency - (VT_FLOAT)decimal;
            frac       = fabsf(frac_float) * 10000;
            VTLogDebug("Repeating Signature Frequency = %d.%04d\r\n", decimal, frac);
    #endif /* VT_LOG_LEVEL > 2 */

    #if VT_LOG_LEVEL > 2
            decimal    = *relative_current_draw;
            frac_float = *relative_current_draw - (VT_FLOAT)decimal;
            frac       = fabsf(frac_float) * 10000;
            VTLogDebug("Repeating Signature Relative Curr Draw = %d.%04d\r\n", decimal, frac);
    #endif /* VT_LOG_LEVEL > 2 */

    #if VT_LOG_LEVEL > 2
            decimal    = *duty_cycle;
            frac_float = *duty_cycle - (VT_FLOAT)decimal;
            frac       = fabsf(frac_float) * 10000;
            VTLogDebug("Repeating Signature Duty Cycle = %d.%04d\r\n", decimal, frac);
    #endif /* VT_LOG_LEVEL > 2 */

            return VT_SUCCESS;

    }

    VTLogDebug("Error in computing feature vectors for repeating signature\r\n");

    return VT_ERROR;
}

VT_UINT cs_repeating_signature_offset_current_compute(
    VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT* raw_signature, VT_UINT raw_signature_length, VT_FLOAT* offset_current)
{
    VT_FLOAT curr_draw_active  = 0;
    VT_FLOAT curr_draw_standby = 0;
    VT_UINT datapoints_active  = 1;
    VT_UINT datapoints_standby = 1;
    VT_UINT category_array[raw_signature_length];
    // VT_INT decimal;
    // VT_FLOAT frac_float;
    // VT_INT frac;
#if VT_LOG_LEVEL > 2
    VT_INT decimal;
    VT_FLOAT frac_float;
    VT_INT frac;

VTLogDebugNoTag("\nOFFSET CURRENT RAW SIG:\n");
for (VT_INT iter1 = 0; iter1 < VT_CS_SAMPLE_LENGTH; iter1++)
    {
        decimal    = raw_signature[iter1];
        frac_float = raw_signature[iter1] - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebugNoTag("%d.%04d, ", decimal, frac);
    }
VTLogDebugNoTag("\n");
#endif /* VT_LOG_LEVEL > 2 */

    binary_state_current_compute(
        raw_signature, category_array,raw_signature_length, &curr_draw_active, &curr_draw_standby, &datapoints_active, &datapoints_standby);

    *offset_current = curr_draw_standby;

#if VT_LOG_LEVEL > 2
    decimal    = curr_draw_active;
    frac_float = curr_draw_active - (VT_FLOAT)decimal;
    frac       = fabsf(frac_float) * 10000;
    VTLogDebug("curr_draw_active = %d.%04d\r\n", decimal, frac);
#endif /* VT_LOG_LEVEL > 2 */
#if VT_LOG_LEVEL > 2
    decimal    = curr_draw_standby;
    frac_float = curr_draw_standby - (VT_FLOAT)decimal;
    frac       = fabsf(frac_float) * 10000;
    VTLogDebug("curr_draw_standby = %d.%04d\r\n", decimal, frac);
#endif /* VT_LOG_LEVEL > 2 */
    VTLogDebug("datapoints_active = %d\r\n", datapoints_active);
    VTLogDebug("datapoints_standby = %d\r\n", datapoints_standby);

#if VT_LOG_LEVEL > 2
    decimal    = *offset_current;
    frac_float = *offset_current - (VT_FLOAT)decimal;
    frac       = fabsf(frac_float) * 10000;
    VTLogDebug("Repeating Signature Offset Current Draw = %d.%04d\r\n", decimal, frac);
#endif /* VT_LOG_LEVEL > 2 */
    return VT_SUCCESS;
}

VT_UINT cs_non_repeating_signature_average_current_compute(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT* raw_signature,
    VT_UINT raw_signature_length,
    VT_FLOAT* avg_curr_on,
    VT_FLOAT* avg_curr_off)
{
    VT_FLOAT curr_draw_active  = 0;
    VT_FLOAT curr_draw_standby = 0;
    VT_UINT datapoints_active  = 1;
    VT_UINT datapoints_standby = 1;

    VT_UINT category_array[raw_signature_length];

#if VT_LOG_LEVEL > 2
    VT_INT decimal;
    VT_FLOAT frac_float;
    VT_INT frac;
#endif /* VT_LOG_LEVEL > 2 */

    binary_state_current_compute(
        raw_signature, category_array,raw_signature_length, &curr_draw_active, &curr_draw_standby, &datapoints_active, &datapoints_standby);

    // VTLogDebug("non repeating Raw: \r\n");

    //         for (VT_INT iter = 0; iter < VT_CS_SAMPLE_LENGTH ; iter++)
    //         {
    //             decimal    = raw_signature[iter];
    //             frac_float = raw_signature[iter] - (VT_FLOAT)decimal;
    //             frac       = fabsf(frac_float) * 10000;
    //             VTLogDebugNoTag("%d.%04d, ", decimal, frac);
    //         }



    *avg_curr_on  = curr_draw_active;
    *avg_curr_off = curr_draw_standby;

#if VT_LOG_LEVEL > 2
    decimal    = *avg_curr_on;
    frac_float = *avg_curr_on - (VT_FLOAT)decimal;
    frac       = fabsf(frac_float) * 10000;
    VTLogDebug("Non-Repeating Signature Average ON Current Draw = %d.%04d\r\n", decimal, frac);
#endif /* VT_LOG_LEVEL > 2 */
#if VT_LOG_LEVEL > 2
    decimal    = *avg_curr_off;
    frac_float = *avg_curr_off - (VT_FLOAT)decimal;
    frac       = fabsf(frac_float) * 10000;
    VTLogDebug("Non-Repeating Signature Average OFF Current Draw = %d.%04d\r\n", decimal, frac);
#endif /* VT_LOG_LEVEL > 2 */
    return VT_SUCCESS;
}
