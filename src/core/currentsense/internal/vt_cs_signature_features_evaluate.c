/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_signature_features.h"
#include "vt_debug.h"
#include <math.h>

VT_FLOAT cs_repeating_signature_feature_vector_evaluate(VT_FLOAT* signature_frequency_under_test,
    VT_FLOAT signature_frequency_saved,
    VT_FLOAT duty_cycle_under_test,
    VT_FLOAT duty_cycle_saved,
    VT_FLOAT relative_current_draw_under_test,
    VT_FLOAT relative_current_draw_saved,
    VT_FLOAT sec_sig_freq)
{

        int32_t decimal  = signature_frequency_saved;
    float frac_float = signature_frequency_saved - (float)decimal;
    int32_t frac     = frac_float * 10000;
    VTLogDebug("Saved sig freq: %d.%d \r\n", (int)decimal, (int)frac);

       decimal  = sec_sig_freq;
   frac_float = sec_sig_freq - (float)decimal;
  frac     = frac_float * 10000;
    VTLogDebug("Sec saved sig freq: %d.%d \r\n", (int)decimal, (int)frac);


           VT_FLOAT mindrift=100.0;
    VT_FLOAT tempdrift=0.0;


 //VT_FLOAT signature_drift = 0;
    //VT_FLOAT harmonic = 0;
    //VT_BOOL harmonic_found=0;

    //signature_drift += fabsf((signature_frequency_under_test - signature_frequency_saved) / signature_frequency_saved) * 100.0f;


    // if(signature_frequency_saved>signature_frequency_under_test){
    //     harmonic=signature_frequency_saved/signature_frequency_under_test;
    //     if((fabsf((harmonic)-(float)round(harmonic))<0.2f)||((harmonic>=1.4f)&&(harmonic<=1.6f))){
    //         harmonic_found=1;
    //     signature_drift += fabsf((signature_frequency_under_test - signature_frequency_saved/harmonic) / signature_frequency_saved) * 100.0f;
    //     }
    // }

    // if(signature_frequency_saved<signature_frequency_under_test){
    //     harmonic=signature_frequency_under_test/signature_frequency_saved;
    //     if((fabsf((harmonic)-(float)round(harmonic))<0.2f)||((harmonic>=1.4f)&&(harmonic<=1.6f))){
    //         harmonic_found=1;
    //     signature_drift += fabsf((signature_frequency_under_test/harmonic - signature_frequency_saved) / signature_frequency_saved) * 100.0f;
    //     }
    // }

    // if(harmonic_found==0)
    // {
    //     signature_drift += fabsf((signature_frequency_under_test - signature_frequency_saved) / signature_frequency_saved) * 100.0f;
    // }
//     if(signature_frequency_saved>5000){
//     VT_FLOAT sig_freq_1=signature_frequency_saved/6000;
//     VT_FLOAT sig_freq_2=fmod(signature_frequency_saved,6000);

//      decimal  = sig_freq_1;
//    frac_float = sig_freq_1 - (float)decimal;
//     frac     = frac_float * 10000;
//     VTLogDebug("F1: %d.%d \r\n", (int)decimal, (int)frac);

//          decimal  = sig_freq_2;
//    frac_float = sig_freq_2 - (float)decimal;
//     frac     = frac_float * 10000;
//     VTLogDebug("F2: %d.%d \r\n", (int)decimal, (int)frac);


 
//      for(int testiter=0;testiter<VT_CS_ACRFFT_MAX_PEAKS-1;testiter++){
//          tempdrift=fabsf((signature_frequency_under_test[testiter] - sig_freq_1) / sig_freq_1) * 100.0f;
//                if (tempdrift<mindrift){

//                    mindrift=tempdrift;

//                }  

//             }

//          for(int testiter=0;testiter<VT_CS_ACRFFT_MAX_PEAKS-1;testiter++){
//          tempdrift=fabsf((signature_frequency_under_test[testiter] - sig_freq_2) / sig_freq_2) * 100.0f;
//                if (tempdrift<mindrift){

//                    mindrift=tempdrift;

//                }  

//             }
            
//     }

if(sec_sig_freq!=0){

    printf("\nsec freq found\n");
         for(int testiter=0;testiter<VT_CS_ACRFFT_MAX_PEAKS-1;testiter++){
         tempdrift=fabsf((signature_frequency_under_test[testiter] - signature_frequency_saved) / signature_frequency_saved) * 100.0f;
               if (tempdrift<mindrift){

                   mindrift=tempdrift;

               }  

            }

         for(int testiter=0;testiter<VT_CS_ACRFFT_MAX_PEAKS-1;testiter++){
         tempdrift=fabsf((signature_frequency_under_test[testiter] - sec_sig_freq) / sec_sig_freq) * 100.0f;
               if (tempdrift<mindrift){
                printf("\nsec freq used\n");
                   mindrift=tempdrift;

               }  

            }
            
    }




    else{
          mindrift=100.0;
     tempdrift=0.0;

             for(int testiter=0;testiter<VT_CS_ACRFFT_MAX_PEAKS-1;testiter++){
         tempdrift=fabsf((signature_frequency_under_test[testiter] - signature_frequency_saved) / signature_frequency_saved) * 100.0f;
               if (tempdrift<mindrift){

                   mindrift=tempdrift;

               }  

            }


    }






    //signature_drift += fabsf((duty_cycle_under_test - duty_cycle_saved) / duty_cycle_saved) * 100.0f;
    //signature_drift +=
    //    fabsf((relative_current_draw_under_test - relative_current_draw_saved) / relative_current_draw_saved) * 100.0f;
    //signature_drift /= 3.0f;

#if VT_LOG_LEVEL > 2
     decimal  = mindrift;
     frac_float = mindrift - (float)decimal;
     frac     = frac_float * 10000;
    VTLogDebug("Deviation in Repeating Signature Feature Vector: %d.%d \r\n", (int)decimal, (int)frac);
#endif /* VT_LOG_LEVEL > 2 */
    return mindrift;
}

VT_FLOAT cs_repeating_signature_relative_current_evaluate(VT_FLOAT relative_current_under_test, VT_FLOAT relative_current_saved, VT_FLOAT average_current_under_test, VT_FLOAT average_current_saved)
{
    VT_FLOAT relative_signature_drift = 100;
    VT_FLOAT average_signature_drift = 100;
    if(relative_current_saved!=0){
    relative_signature_drift          = fabsf((relative_current_under_test - relative_current_saved) / relative_current_saved) * 100.0f;}
    if(average_current_saved!=0){
    average_signature_drift          = fabsf((average_current_under_test - average_current_saved) / average_current_saved) * 100.0f;}
#if VT_LOG_LEVEL > 2
    int32_t decimal  = relative_signature_drift;
    float frac_float = relative_signature_drift - (float)decimal;
    int32_t frac     = frac_float * 10000;
    VTLogDebug("Deviation in relative Repeating Signature Current: %d.%d \r\n", (int)decimal, (int)frac);
         decimal  = average_signature_drift;
  frac_float = average_signature_drift - (float)decimal;
  frac     = frac_float * 10000;
    VTLogDebug("Deviation in average Repeating Signature Current: %d.%d \r\n", (int)decimal, (int)frac);
#endif /* VT_LOG_LEVEL > 2 */

    if (relative_signature_drift<average_signature_drift){
        return relative_signature_drift;
    }
    else{
    return average_signature_drift;}
}

VT_FLOAT cs_repeating_signature_offset_current_evaluate(VT_FLOAT offset_current_under_test, VT_FLOAT offset_current_saved)
{
    VT_FLOAT signature_drift = 0;
    signature_drift          = fabsf((offset_current_under_test - offset_current_saved) / offset_current_saved) * 100.0f;
#if VT_LOG_LEVEL > 2
    int32_t decimal  = signature_drift;
    float frac_float = signature_drift - (float)decimal;
    int32_t frac     = frac_float * 10000;
    VTLogDebug("Deviation in Repeating Signature Offset Current: %d.%d \r\n", (int)decimal, (int)frac);
#endif /* VT_LOG_LEVEL > 2 */
    return signature_drift;
}

VT_FLOAT cs_non_repeating_signature_average_current_evaluate(
    VT_FLOAT avg_curr_on_under_test, VT_FLOAT avg_curr_on_saved, VT_FLOAT avg_curr_off_under_test, VT_FLOAT avg_curr_off_saved, VT_FLOAT* avg_curr_off_drift,VT_FLOAT* avg_curr_on_drift)
{

    VT_FLOAT signature_drift_on = 0.0f;
    VT_FLOAT signature_drift_off = 0.0f;
    VT_FLOAT signature_drift = 0.0f;
    signature_drift_on = fabsf(((avg_curr_on_under_test - avg_curr_on_saved)*100) / avg_curr_on_saved);
    *avg_curr_on_drift=signature_drift_on;
    signature_drift_off = fabsf(((avg_curr_off_under_test - avg_curr_off_saved)*100) / avg_curr_off_saved);
    *avg_curr_off_drift=signature_drift_off;
    signature_drift = (float)(((int)signature_drift_on+(int)signature_drift_off)/2);


#if VT_LOG_LEVEL > 2
    int32_t decimal  = (int32_t)signature_drift_on;
    float frac_float = signature_drift_on - (float)decimal;
    int32_t frac     = (int32_t)(frac_float * 10000);
    VTLogDebug("Deviation in Non-Repeating Signature Average ON Current Draw: %d.%d \r\n", (int)decimal, (int)frac);
      decimal  = (int32_t)signature_drift_off;
   frac_float = signature_drift_off - (float)decimal;
     frac     = (int32_t)(frac_float * 10000);
    VTLogDebug("Deviation in Non-Repeating Signature Average OFF Current Draw: %d.%d \r\n", (int)decimal, (int)frac);
#endif /* VT_LOG_LEVEL > 2 */
    return signature_drift;
}
