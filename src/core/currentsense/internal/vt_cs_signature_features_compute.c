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

static VT_VOID autocorrelation(VT_FLOAT* raw_signature, VT_FLOAT* lag_array, VT_INT sample_length, VT_INT lag_length)
{
    VT_FLOAT var_sum_average = 0;
    for (VT_INT iter1 = 0; iter1 < sample_length; iter1++)
    {
        var_sum_average += raw_signature[iter1];
    }
    var_sum_average /= sample_length;
    for (VT_INT iter1 = 0; iter1 < sample_length; iter1++)
    {
        raw_signature[iter1] -= var_sum_average;
    }
    for (VT_INT iter1 = 0; iter1 < lag_length; iter1++)
    {
        lag_array[iter1] = raw_signature[iter1];
    }
    VT_INT iter2 = 0;
    VT_INT iter3 = 0;
    for (iter3 = 0; iter3 < (sample_length - lag_length); iter3++)
    {
        var_sum_average = 0;
        for (iter2 = 0; iter2 < lag_length; iter2++)
        {
            var_sum_average += (raw_signature[(iter3 + iter2)] * lag_array[iter2]);
        }
        raw_signature[iter3] = var_sum_average;
    }
    for (VT_INT iter1 = 1; iter1 < sample_length - lag_length; iter1++)
    {
        if (raw_signature[0])
        {
            raw_signature[iter1] /= raw_signature[0];
        }
    }
    raw_signature[0] = 1;
}

static VT_UINT check_acr_peak_present(VT_FLOAT* raw_signature,
    VT_UINT* index,
    VT_UINT period,
    VT_UINT* period_total,
    VT_UINT* peaks,
    VT_FLOAT minimum_correlation_for_peak)
<<<<<<< HEAD
{   

    VT_FLOAT maxval=raw_signature[*index];
    VT_UINT maxvalindex=*index;

    if(*index>=VT_CS_SAMPLE_LENGTH-VT_CS_AUTO_CORRELATION_LAG){
        maxval=0;
        for (VT_UINT iterx=*index-2;iterx<VT_CS_SAMPLE_LENGTH-VT_CS_AUTO_CORRELATION_LAG;iterx++){
            if (raw_signature[iterx]>maxval){
            maxval=raw_signature[iterx];
            maxvalindex=iterx;
            }

        }

    }
    else{
    for(VT_INT iter=-1; iter<2;iter++){ //make 2 a macro
=======
{   // !!! CHANGE

    VT_FLOAT maxval=raw_signature[*index];
    VT_UINT maxvalindex=*index;
    for(VT_INT iter=-2; iter<3;iter++){
>>>>>>> c08847240ee5e21382d04c27a17b53f52cb801f3
        if((raw_signature[(*index)+iter]>maxval) &&(((*index)+iter)!=0 ) && (fabs((double)iter)<period)) {
            maxval=raw_signature[(*index)+iter];
            maxvalindex=(*index)+iter;
        }
<<<<<<< HEAD
        }}

    if (maxval>minimum_correlation_for_peak){
        
        
        *peaks += 1;
        *period_total += maxvalindex;
        *index = maxvalindex + period; 
        #if VT_LOG_LEVEL > 2
        //VTLogDebugNoTag("SUCCESS : peaks = %d, period_total = %d, index= %d \n",*peaks,*period_total,*index);
        #endif
        return VT_SUCCESS;}    


    else

    {
               #if VT_LOG_LEVEL > 2
        //VTLogDebugNoTag("FAILURE : peaks = %d, period_total = %d, index= %d \n",*peaks,*period_total,*index);
        #endif
    
=======
        }
    if (maxval>minimum_correlation_for_peak){
        
        *peaks += 1;
        *period_total += maxvalindex;
        *index = maxvalindex + period; 
        //printf("SUCCESS : peaks = %d, period_total = %d, index= %d \n",*peaks,*period_total,*index);
        return VT_SUCCESS;}    



    // if (raw_signature[*index] > minimum_correlation_for_peak)
    // {   printf("SUCCESS : peaks = %d, period_total = %d, index= %d \n",*peaks,*period_total,*index);
    //     *peaks += 1;
    //     *period_total += *index;
    //     *index = *index + period;
        
    //     return VT_SUCCESS;
    // }
    // else if (raw_signature[*index + 1] > minimum_correlation_for_peak)
    // {   printf("SUCCESS : peaks = %d, period_total = %d, index= %d \n",*peaks,*period_total,*index);
    //     *peaks += 1;
    //     *period_total += *index;
    //     *index = *index + period + 1;
    //     return VT_SUCCESS;
    // }
    // else if (raw_signature[*index - 1] > minimum_correlation_for_peak)
    // {   printf("SUCCESS : peaks = %d, period_total = %d, index= %d \n",*peaks,*period_total,*index);
    //     *peaks += 1;
    //     *period_total += *index;
    //     *index = *index + period - 1;
    //     return VT_SUCCESS;
    //}
    else
    {//printf("FAILURE : peaks = %d, period_total = %d, index= %d \n",*peaks,*period_total,*index);
>>>>>>> c08847240ee5e21382d04c27a17b53f52cb801f3
        return VT_ERROR;
    }
}

static VT_UINT period_calculate(VT_FLOAT* raw_signature, VT_FLOAT* period)
{
    VT_FLOAT raw_signature_copy[VT_CS_SAMPLE_LENGTH] = {0};
    VT_FLOAT lag_array[VT_CS_AUTO_CORRELATION_LAG]   = {0};
    VT_UINT peaks                                    = 0;
    VT_UINT index                                    = 0;
    VT_UINT period_total                             = 0;
    *period                                          = 0;

    #if VT_LOG_LEVEL > 2
        VT_INT decimal;
    VT_FLOAT frac_float;
    VT_INT frac;
    #endif

    for (VT_UINT iter = 0; iter < VT_CS_SAMPLE_LENGTH; iter++)
    {
        raw_signature_copy[iter] = raw_signature[iter];
    }
    autocorrelation(raw_signature_copy, lag_array, VT_CS_SAMPLE_LENGTH, VT_CS_AUTO_CORRELATION_LAG);
<<<<<<< HEAD
    #if VT_LOG_LEVEL > 2
        VTLogDebugNoTag("\nACR RAW: \n");
    for (VT_INT iter = 0; iter < VT_CS_SAMPLE_LENGTH; iter++)
    {
        decimal    = raw_signature_copy[iter];
        frac_float = raw_signature_copy[iter] - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebugNoTag("%d.%04d, ", decimal, frac);
    }
    printf("\n");
    #endif
=======
    // printf("ACR RAW: \n");
    // for (VT_INT iter = 0; iter < VT_CS_SAMPLE_LENGTH; iter++)
    // {
    //     decimal    = raw_signature_copy[iter];
    //     frac_float = raw_signature_copy[iter] - (VT_FLOAT)decimal;
    //     frac       = fabsf(frac_float) * 10000;
    //     VTLogDebugNoTag("%d.%04d, ", decimal, frac);
    // }
    // printf("\n");
    // !!! CHANGE

    //VT_UINT master_peaks=0;
    //VT_UINT master_period_total=0;
>>>>>>> c08847240ee5e21382d04c27a17b53f52cb801f3


    for (VT_UINT iter = 2; iter < VT_CS_SAMPLE_LENGTH - VT_CS_AUTO_CORRELATION_LAG - 2; iter++) 
    {
        index = iter;
<<<<<<< HEAD
 
        bool cond1=(raw_signature_copy[iter] > raw_signature_copy[iter - 1]) || (raw_signature_copy[iter] > raw_signature_copy[iter - 2]);
        bool cond2=(raw_signature_copy[iter] > raw_signature_copy[iter + 1]) || (raw_signature_copy[iter] > raw_signature_copy[iter + 2]);
=======
        // if (((raw_signature_copy[iter] > raw_signature_copy[iter - 1]) ||
        //         (raw_signature_copy[iter] > raw_signature_copy[iter - 2])) &&
        //     ((raw_signature_copy[iter] > raw_signature_copy[iter + 1]) ||
        //         (raw_signature_copy[iter] > raw_signature_copy[iter + 2])))
        //printf("\n Seeing : %d\n",iter);
        bool cond1=raw_signature_copy[iter] > raw_signature_copy[iter - 1];
        bool cond2=raw_signature_copy[iter] > raw_signature_copy[iter + 1];
>>>>>>> c08847240ee5e21382d04c27a17b53f52cb801f3
        VT_UINT flag=(int)cond1+(int)cond2;

        if (flag==2)

<<<<<<< HEAD
        {

            period_total = 0;
            peaks        = 0;
            while (index < VT_CS_SAMPLE_LENGTH - VT_CS_AUTO_CORRELATION_LAG + 2)  //make 2 a macro
            
            {       
                #if VT_LOG_LEVEL > 2
        //VTLogDebugNoTag("ENTERED IN : peaks = %d, period_total = %d, index= %d, iter= %d \n",peaks,period_total,index,iter);
        #endif
                if (check_acr_peak_present(raw_signature_copy, &index, iter, &period_total, &peaks, VT_CS_MIN_CORRELATION) ==
                    VT_ERROR)

=======
        {//printf("\n***ENTERED at %d because of ",iter );
        // decimal    = raw_signature_copy[iter];
        // frac_float = raw_signature_copy[iter] - (VT_FLOAT)decimal;
        // frac       = fabsf(frac_float) * 10000;
        // printf("%d.%04d, ***\n", decimal, frac);

            period_total = 0;
            peaks        = 0;
            while (index < VT_CS_SAMPLE_LENGTH - VT_CS_AUTO_CORRELATION_LAG)
            {   //printf("ENTERED IN : peaks = %d, period_total = %d, index= %d, iter= %d \n",peaks,period_total,index,iter);
                if (check_acr_peak_present(raw_signature_copy, &index, iter, &period_total, &peaks, VT_CS_MIN_CORRELATION) ==
                    VT_ERROR)
                //{   if (peaks>master_peaks){
                //    master_peaks=peaks;
                //    master_period_total=period_total;
               // }
>>>>>>> c08847240ee5e21382d04c27a17b53f52cb801f3
                    break;
                }
            }
            if (index > VT_CS_SAMPLE_LENGTH - VT_CS_AUTO_CORRELATION_LAG)
<<<<<<< HEAD
            {   
=======
            {   //master_peaks=peaks;
>>>>>>> c08847240ee5e21382d04c27a17b53f52cb801f3
                *period = iter;
                break;
            }
        }
    

    if (peaks < 2)
    {
        *period = 0;
        return VT_ERROR;
    }
    else
    {
        *period = (((period_total * 2.0f) / (VT_FLOAT)peaks) - (VT_FLOAT)(2.0f * (*period))) / (VT_FLOAT)(peaks - 1);
        return VT_SUCCESS;
    }
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
<<<<<<< HEAD

=======
    VT_BOOL seedpoint_low_added=false;
    VT_BOOL seedpoint_high_added=false;
>>>>>>> c08847240ee5e21382d04c27a17b53f52cb801f3
    VT_BOOL valid_seedpoints     = true;

    *curr_draw_active  = 0;
    *datapoints_active = 0;

    *curr_draw_standby  = 0;
    *datapoints_standby = 0;

    VT_UINT return_iter=0;

    while (num_seedpoints_added < num_seedpoints)
<<<<<<< HEAD
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
=======
    {   //both ,max and min fucntions share the same datapoint_visited varaible 
        //each while iteration would give us one point in state_Aray each from min and max value
        //and each while iteration would give us one point in datapoints_visited each from min and max value
        min_value(raw_signature, sample_length, &state_array[low_state_count], datapoint_visited);
        max_value(raw_signature, sample_length, &state_array[sample_length - 1 - high_state_count], datapoint_visited);
        //printf("iter - %d \n",low_state_count);
        //printf("low_state_count - %d \n",(int)(state_array[low_state_count]*100));
        //printf("high_state_count - %d \n",(int)(state_array[sample_length - 1 - high_state_count]*100));

        if (low_state_count && high_state_count)
        {
            if ((state_array[low_state_count] != state_array[low_state_count - 1])&&(!seedpoint_low_added))
            {//   printf("low done");
                //seedpoint_low_added=true;
                num_seedpoints_added++;
            }

            if((state_array[sample_length - 1 - high_state_count] != state_array[sample_length - 1 - high_state_count + 1])&&(!seedpoint_high_added))
            {//   printf("high done");
                //seedpoint_high_added=true;
>>>>>>> c08847240ee5e21382d04c27a17b53f52cb801f3
                num_seedpoints_added++;
            }
        }

        if (state_array[low_state_count] == state_array[sample_length - 1 - high_state_count])
        {//   printf(" \n false \n");
            valid_seedpoints = false;
            break;
        }

        low_state_count++;
        high_state_count++;

<<<<<<< HEAD
=======
        //if((low_state_count>((sample_length/2)-1))||(high_state_count>((sample_length/2)-1)))
        //    break;
>>>>>>> c08847240ee5e21382d04c27a17b53f52cb801f3

    }

    if (valid_seedpoints == false)
<<<<<<< HEAD
    { 
=======
    {   //printf("not valid");
>>>>>>> c08847240ee5e21382d04c27a17b53f52cb801f3
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
<<<<<<< HEAD
 
=======
    /*
    printf("state_array \n");
    for(int iter=0;iter<128;iter++)
    {
    printf(" %d,",(int)(state_array[iter]*100));
    }
    printf("\n state_array end \n");

    printf("high_state_count %d \n,",high_state_count);
    printf("low_state_count %d \n,",low_state_count);
*/
    //i think state_array is suppose to be a sorted array, incressing from lowest value and 
    //high_state_count contain the number of digits from end of array that are in "high state"
    //low_state_count contain the number of digits from start of array that are in "low state"
>>>>>>> c08847240ee5e21382d04c27a17b53f52cb801f3
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

<<<<<<< HEAD

=======
    //printf("curr_draw_active %d \n,",(int)((*curr_draw_active)*100));
    //printf("curr_draw_standby %d \n,",(int)((*curr_draw_standby)*100));
>>>>>>> c08847240ee5e21382d04c27a17b53f52cb801f3

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

    binary_state_current_compute(
            raw_signature, category_array,raw_signature_length, &curr_draw_active, &curr_draw_standby, &datapoints_active, &datapoints_standby);

 
    

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
// printf("\nOFFSET CURRENT RAW SIG:\n");
// for (VT_INT iter1 = 0; iter1 < VT_CS_SAMPLE_LENGTH; iter1++)
//     {
//         decimal    = raw_signature[iter1];
//         frac_float = raw_signature[iter1] - (VT_FLOAT)decimal;
//         frac       = fabsf(frac_float) * 10000;
//         printf("%d.%04d, ", decimal, frac);
//     }
// printf("\n");

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



    VTLogDebug("non repeating Raw: \r\n");

            for (VT_INT iter = 0; iter < VT_CS_SAMPLE_LENGTH ; iter++)
            {
                decimal    = raw_signature[iter];
                frac_float = raw_signature[iter] - (VT_FLOAT)decimal;
                frac       = fabsf(frac_float) * 10000;
                VTLogDebugNoTag("%d.%04d, ", decimal, frac);
            }

    printf("curr_draw_active- %d \n",(int)(curr_draw_active*1000));
    printf("curr_draw_standby- %d \n",(int)(curr_draw_standby*1000));
    printf("datapoints_active- %d \n",(int)(datapoints_active*1000));
    printf("datapoints_standby- %d \n",(int)(datapoints_standby*1000));


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