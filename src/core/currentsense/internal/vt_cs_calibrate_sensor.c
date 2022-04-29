/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_api.h"
#include "vt_cs_calibrate.h"
#include "vt_cs_database.h"
#include "vt_cs_raw_signature_read.h"
#include "vt_cs_signature_features.h"
#include "vt_debug.h"
#include <math.h>


VT_UINT Num_Sampling_Freq_In_Repeatability_Data_Array(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT sampling_frequency,
    VT_FLOAT signature_frequency,
    VT_FLOAT duty_cycle,
    VT_FLOAT relative_current_draw)
{
    //check if this for loop is needed or not.
    // for(VT_UINT iter=0;iter<cs_object->fingerprintdb.template.repeating_signatures.multisampling_num_signatures;iter++){
    //     if(((abs(cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].signature_freq-signature_frequency))/cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].signature_freq)<0.15){
    //         VTLogDebug("Similar frequency already stored");
    //         return VT_SUCCESS;


    // }}

    //VT_UINT Num_Sampling_Freq_In_Repeatability_Data_Array = cs_object->fingerprintdb.template.repeating_signatures.Num_Sampling_Freq_In_Repeatability_Data_Array;
    VT_BOOL Sampling_Freq_Found=false;
    VT_UINT multisampling_num_signatures = cs_object->fingerprintdb.template.repeating_signatures.multisampling_num_signatures;
    //bool harmonic_found=false;
    VT_FLOAT harmonic = 0;

    for (VT_UINT iter = 0; iter < multisampling_num_signatures; iter++)
    {

        float stored_sampling_frequency=cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].sampling_freq;
        float stored_signature_frequency=cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].signature_freq;
        //float stored_duty_cycle=cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].duty_cycle;
        float stored_relative_current_draw=cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].relative_curr_draw;
        //float stored_signature_frequency=cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].signature_freq;
        //float stored_duty_cycle=cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].duty_cycle;
        //float stored_relative_current_draw=cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].relative_curr_draw;

    //when ever we would compare any freq with 5000, this would go in else, as that case is not needed.
    if(((stored_sampling_frequency==5000.0f)&&(sampling_frequency==5000.0f))||((stored_sampling_frequency!=5000.0f)&&(sampling_frequency!=5000.0f)))
    {

        if( (stored_sampling_frequency>(sampling_frequency-(stored_sampling_frequency*(VT_CS_REP_SAMPLING_FREQUENCY_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/(float)100))))&&
        (stored_sampling_frequency<(sampling_frequency+(stored_sampling_frequency*(VT_CS_REP_SAMPLING_FREQUENCY_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/(float)100))))   )
        {   printf("sampling_freq_repeatable_count++ \n");
            cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].sampling_freq_repeatable_count++;
            Sampling_Freq_Found=true;
            
            if(stored_signature_frequency>signature_frequency){
                harmonic=stored_signature_frequency/signature_frequency;
                if((fabsf((harmonic)-(float)round(harmonic))<0.2f)||((harmonic>=1.4f)&&(harmonic<=1.6f))){
                //harmonic_found=true;
                signature_frequency=signature_frequency*harmonic;
                }
            }
            if(stored_signature_frequency<signature_frequency){
                harmonic=signature_frequency/stored_signature_frequency;
                if((fabsf((harmonic)-(float)round(harmonic))<0.2f)||((harmonic>=1.4f)&&(harmonic<=1.6f))){
                //harmonic_found=true;
                signature_frequency=signature_frequency/harmonic;
                }
            }


            if( (stored_signature_frequency>(signature_frequency-(stored_signature_frequency*(VT_CS_REP_SIGNATURE_FREQUENCY_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/(float)100))))&&
            (stored_signature_frequency<(signature_frequency+(stored_signature_frequency*(VT_CS_REP_SIGNATURE_FREQUENCY_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/(float)100))))   )
            {
                //if( (stored_duty_cycle>(duty_cycle-(stored_duty_cycle*(VT_CS_REP_DUTY_CYCLE_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/(float)100))))&&
                //    (stored_duty_cycle<(duty_cycle+(stored_duty_cycle*(VT_CS_REP_DUTY_CYCLE_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/(float)100))))   )
                {   printf("signature_freq_and_duty_cycle_repeatable_count++ \n");
                cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].signature_freq_and_duty_cycle_repeatable_count++;
                }
                //else
                //    VTLogDebug("\n duty_cycle not reliable \n");
            }
            else
                {VTLogDebug("\n  signature_frequency not reliable \n");}
            if( (stored_relative_current_draw>(relative_current_draw-(stored_relative_current_draw*(VT_CS_REP_RELATIVE_CURR_DRAW_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/(float)100))))&&
                    (stored_relative_current_draw<(relative_current_draw+(stored_relative_current_draw*(VT_CS_REP_RELATIVE_CURR_DRAW_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/(float)100))))   )
            {   printf("current_dif_repeatable_count++ \n");
                cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].current_dif_repeatable_count++;
            }
            else
                {VTLogDebug("\n  relative_current_draw not reliable \n");}
            
            break;
        }
        else    
            {VTLogDebug("\n  sampling_frequency not reliable \n");}
    }
    else 
        {VTLogDebug("\n  5000 & 4999 case ignored \n");}

    }

    if(Sampling_Freq_Found==false)
    {

    //cs_object->fingerprintdb.template.repeating_signatures.Signature_Repeatability_Data_Array[Num_Sampling_Freq_In_Repeatability_Data_Array].sampling_freq_found=sampling_frequency;
    //cs_object->fingerprintdb.template.repeating_signatures.Signature_Repeatability_Data_Array[Num_Sampling_Freq_In_Repeatability_Data_Array].sampling_freq_repeatable_count=1;
    //cs_object->fingerprintdb.template.repeating_signatures.Signature_Repeatability_Data_Array[Num_Sampling_Freq_In_Repeatability_Data_Array].signature_freq_and_duty_cycle_repeatable_count=1;
    //cs_object->fingerprintdb.template.repeating_signatures.Signature_Repeatability_Data_Array[Num_Sampling_Freq_In_Repeatability_Data_Array].current_dif_repeatable_count=1;
    //add a check here to see if num_sig is out of bounds
    //dont use the signatures[num_signatures] to store the possible freq, make a seperate buffer to hold them
    // and transfer them once, reliable freq are filtered 
    cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[multisampling_num_signatures].sampling_freq = sampling_frequency;
    cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[multisampling_num_signatures].signature_freq = signature_frequency;
    cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[multisampling_num_signatures].duty_cycle = duty_cycle;
    cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[multisampling_num_signatures].relative_curr_draw = relative_current_draw;

    cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[multisampling_num_signatures].sampling_freq_repeatable_count = 1;
    cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[multisampling_num_signatures].current_dif_repeatable_count = 1;

    if((signature_frequency==0)||(duty_cycle==0))
        cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[multisampling_num_signatures].signature_freq_and_duty_cycle_repeatable_count = 0;
    else 
        cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[multisampling_num_signatures].signature_freq_and_duty_cycle_repeatable_count = 1;


    cs_object->fingerprintdb.template.repeating_signatures.multisampling_num_signatures++;

    }

    VTLogDebug("Reliable Signature updated in Signature_Reliable_Across_Calibrations \r\n");
    return VT_SUCCESS;
}

VT_UINT cs_signature_reliability_test_across_calibrations(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT sampling_frequency,
    VT_FLOAT signature_frequency,
    VT_FLOAT duty_cycle,
    VT_FLOAT relative_current_draw,
    VT_BOOL *Signature_Reliable_Across_Calibrations)
{
    VT_UINT num_signatures = cs_object->fingerprintdb.template.repeating_signatures.num_signatures;
    if ((num_signatures + 1) >= VT_CS_MAX_SIGNATURES)
    {
        return VT_ERROR;
    }

    // for(VT_UINT iter=0;iter<cs_object->fingerprintdb.template.repeating_signatures.num_signatures){
    //     if(((abs(cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].signature_freq-signature_frequency))/cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].signature_freq)<0.1){
    //         VTLogDebug("Similar frequency already stored");
    //         return VT_SUCCESS;


    // }

        for (VT_UINT iter = 0; iter < num_signatures; iter++)
    {
                //all these ranges should be exitable using a define
        float stored_sampling_frequency=cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].sampling_freq;
        float stored_signature_frequency=cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].signature_freq;
        float stored_duty_cycle=cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].duty_cycle;
        float stored_relative_current_draw=cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].relative_curr_draw;

    if( (stored_sampling_frequency>(sampling_frequency-(stored_sampling_frequency*(VT_CS_REP_SAMPLING_FREQUENCY_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/(float)100))))&&
        (stored_sampling_frequency<(sampling_frequency+(stored_sampling_frequency*(VT_CS_REP_SAMPLING_FREQUENCY_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/(float)100))))   )
        {
        if( (stored_signature_frequency>(signature_frequency-(stored_signature_frequency*(VT_CS_REP_SIGNATURE_FREQUENCY_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/(float)100))))&&
            (stored_signature_frequency<(signature_frequency+(stored_signature_frequency*(VT_CS_REP_SIGNATURE_FREQUENCY_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/(float)100))))   )
            {
            if( (stored_duty_cycle>(duty_cycle-(stored_duty_cycle*(VT_CS_REP_DUTY_CYCLE_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/(float)100))))&&
                (stored_duty_cycle<(duty_cycle+(stored_duty_cycle*(VT_CS_REP_DUTY_CYCLE_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/(float)100))))   )
                {
                if( (stored_relative_current_draw>(relative_current_draw-(stored_relative_current_draw*(VT_CS_REP_RELATIVE_CURR_DRAW_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/(float)100))))&&
                    (stored_relative_current_draw<(relative_current_draw+(stored_relative_current_draw*(VT_CS_REP_RELATIVE_CURR_DRAW_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/(float)100))))   )
                    {
                        Signature_Reliable_Across_Calibrations[iter]=true;
                        //break;
                    }
                    else 
                        printf(" \n relative_curr_draw not reliable \n");
                }
                else 
                    printf(" \n duty_cycle not reliable \n");
            }
            else 
                printf(" \n signature_frequency not reliable \n");
        }
        else 
            {
                printf(" \n sampling_frequency not reliable \n");
                printf(" stored : %d  & current : %d \n",(int)stored_sampling_frequency,(int)sampling_frequency);
            }
/*
    if(fabs(cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].sampling_freq-sampling_frequency)>(cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].sampling_freq*(VT_CS_REP_SAMPLING_FREQUENCY_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/100)))
        {
        if(fabs(cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].signature_freq-signature_frequency)>(cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].signature_freq*(VT_CS_REP_SAMPLING_FREQUENCY_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/100)))
            {
            if(fabs(cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].duty_cycle-duty_cycle)>(cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].duty_cycle*(VT_CS_REP_SAMPLING_FREQUENCY_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/100)))
                {
                if(fabs(cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].relative_curr_draw-relative_current_draw)>(cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].relative_curr_draw*(VT_CS_REP_SAMPLING_FREQUENCY_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/100)))
                    {
                        Signature_Reliable_Across_Calibrations[iter]=true;
                        //break;
                    }
                    else 
                        printf(" \n relative_curr_draw not reliable \n");
                }
                else 
                    printf(" \n duty_cycle not reliable \n");
            }
            else 
                printf(" \n signature_frequency not reliable \n");
        }
        else 
            printf(" \n sampling_frequency not reliable \n");

*/
    }

    VTLogDebug("Reliable Signature updated in Signature_Reliable_Across_Calibrations \r\n");
    return VT_SUCCESS;
}
/*
VT_UINT cs_remove_unreliable_signatures(VT_CURRENTSENSE_OBJECT* cs_object,
                                            VT_BOOL *Signature_Reliable_Across_Calibrations)
{
        for (VT_UINT iter = 0; iter < cs_object->fingerprintdb.template.repeating_signatures.num_signatures; iter++)
    {
        if(Signature_Reliable_Across_Calibrations[iter]==false)
        {
            for (VT_UINT iter2 = iter+1; iter2 < cs_object->fingerprintdb.template.repeating_signatures.num_signatures; iter2++)
            {
                cs_object->fingerprintdb.template.repeating_signatures.signatures[iter2-1].sampling_freq = cs_object->fingerprintdb.template.repeating_signatures.signatures[iter2].sampling_freq;
                cs_object->fingerprintdb.template.repeating_signatures.signatures[iter2-1].signature_freq = cs_object->fingerprintdb.template.repeating_signatures.signatures[iter2].signature_freq;
                cs_object->fingerprintdb.template.repeating_signatures.signatures[iter2-1].duty_cycle = cs_object->fingerprintdb.template.repeating_signatures.signatures[iter2].duty_cycle;
                cs_object->fingerprintdb.template.repeating_signatures.signatures[iter2-1].relative_curr_draw = cs_object->fingerprintdb.template.repeating_signatures.signatures[iter2].relative_curr_draw;
            }
            cs_object->fingerprintdb.template.repeating_signatures.num_signatures--;
        }
    }

        for (VT_UINT iter3 = cs_object->fingerprintdb.template.repeating_signatures.num_signatures; iter3 < VT_CS_MAX_SIGNATURES; iter3++)
    {
        cs_object->fingerprintdb.template.repeating_signatures.signatures[iter3].sampling_freq=0;
        cs_object->fingerprintdb.template.repeating_signatures.signatures[iter3].signature_freq=0;
        cs_object->fingerprintdb.template.repeating_signatures.signatures[iter3].duty_cycle=0;
        cs_object->fingerprintdb.template.repeating_signatures.signatures[iter3].relative_curr_draw=0;
    }

    if(cs_object->fingerprintdb.template.repeating_signatures.num_signatures==0)
        cs_object->fingerprintdb.template.repeating_signatures.repeating_valid=false;

}
*/

static VT_UINT cs_calibrate_repeating_signature_template(VT_CURRENTSENSE_OBJECT* cs_object)
{
    VT_FLOAT raw_signature[VT_CS_SAMPLE_LENGTH]            = {0};
    VT_FLOAT top_N_frequencies[VT_CS_MAX_TEST_FREQUENCIES] = {0};
    VT_FLOAT lowest_sample_freq                            = VT_CS_ADC_MAX_SAMPLING_FREQ;
    VT_FLOAT valid_lowest_frequency =VT_CS_ADC_MAX_SAMPLING_FREQ;

    cs_calibrate_repeating_signatures_compute_collection_settings(cs_object, top_N_frequencies, &lowest_sample_freq);

    VT_UINT8 characteristic_frequencies_found = 0;
    VT_FLOAT signal_freq;
    VT_FLOAT relative_curr_draw;
    VT_FLOAT duty_cycle;
    VT_FLOAT offset_current;

    //VT_BOOL Signature_Reliable_Across_Calibrations[VT_CS_MAX_SIGNATURES]={false};

#if VT_LOG_LEVEL > 2
    VT_INT decimal;
    VT_FLOAT frac_float;
    VT_INT frac;

#endif
    //....
    
    for (VT_UINT iterf = 0; iterf < VT_CS_MAX_TEST_FREQUENCIES; iterf++)
    { 
        if (top_N_frequencies[iterf] == 0)
        {
            continue;
        }
        for (VT_UINT i = iterf+1; i < VT_CS_MAX_TEST_FREQUENCIES; i++){
        if(((((fabs(top_N_frequencies[iterf]-top_N_frequencies[i]))/(double)top_N_frequencies[iterf])<0.1)&&(top_N_frequencies[i]!=5000.0f)&&(top_N_frequencies[iterf]!=5000.0f))||
                ((top_N_frequencies[iterf]==5000.0f) && (top_N_frequencies[i]==5000.0f)))
                {
            top_N_frequencies[i]=0;

        }}}
        
        #if VT_LOG_LEVEL > 2
        
        decimal    = lowest_sample_freq;
        frac_float = lowest_sample_freq - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebugNoTag("\nOFFSET CURRENT SAMPLING FREQ: %d.%04d : \n", decimal, frac);
        #endif

    for (VT_UINT iter = 0; iter < VT_CS_MAX_TEST_FREQUENCIES; iter++)
    { 
        if (characteristic_frequencies_found == VT_CS_MAX_SIGNATURES)
        {
            break;
        }

        if (top_N_frequencies[iter] == 0)
        {
            continue;
        }

        #if VT_LOG_LEVEL > 2
        
        decimal    = top_N_frequencies[iter];
        frac_float = top_N_frequencies[iter] - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebugNoTag("\ntrying for %d.%04d : \n", decimal, frac);
        #endif

        if (cs_repeating_raw_signature_fetch_extrapolated_current_measurement_for_calibration(
                cs_object, raw_signature, top_N_frequencies[iter], VT_CS_SAMPLE_LENGTH))
        {
            VTLogDebug("Error in fetching extrapolated raw signature! \r\n");
            continue;
        }

        

        if (cs_repeating_signature_feature_vector_compute(cs_object,
                raw_signature,
                VT_CS_SAMPLE_LENGTH,
                top_N_frequencies[iter],
                &signal_freq,
                &duty_cycle,
                &relative_curr_draw))
        {
            VTLogDebug("Error in computing feature vector! \r\n");
            continue;
        }
    /*
        if (characteristic_frequencies_found == 0) 
        {   //here we would compare with the prev calbration, and se if the freq holds
            if(cs_object->Initial_Calibration_Done==false)
            {
            cs_reset_db(cs_object);
            VTLogDebug("DB Reset! \r\n");
            }
        }
    
        if(cs_object->Initial_Calibration_Done==false)
        {   cs_object->fingerprintdb.template.repeating_signatures.repeating_valid=true;
            if (cs_store_repeating_signature_feature_vector(
                    cs_object, top_N_frequencies[iter], signal_freq, duty_cycle, relative_curr_draw))
            {  
                VTLogDebug("Error in storing feature vector! \r\n");
                break;
            }
        }
        else 
        {
            if (cs_signature_reliability_test_across_calibrations(
                    cs_object, top_N_frequencies[iter], signal_freq, duty_cycle, relative_curr_draw,Signature_Reliable_Across_Calibrations))
            {
                VTLogDebug("Error in computing signature reliability! \r\n");
                break;
            }
        }
    */

        if (characteristic_frequencies_found == 0) 
        {   //here we would compare with the prev calbration, and se if the freq holds
            if(cs_object->Initial_Calibration_Done==false)
            {
            cs_reset_Repeatability_Data_Array(cs_object);
            VTLogDebug("DB Reset! \r\n");
            }
        }

        Num_Sampling_Freq_In_Repeatability_Data_Array(
                    cs_object, top_N_frequencies[iter], signal_freq, duty_cycle, relative_curr_draw);

        if (top_N_frequencies[iter]<valid_lowest_frequency){
            valid_lowest_frequency=top_N_frequencies[iter];
        }
        VTLogDebug("Feature vector stored successfully! \r\n");
        characteristic_frequencies_found++;
    }

    //if(cs_object->Initial_Calibration_Done==true)
    //    cs_remove_unreliable_signatures(cs_object,Signature_Reliable_Across_Calibrations);

    //if(lowest_sample_freq<cs_object->fingerprintdb.template.repeating_signatures.lowest_sample_freq)
    //    cs_object->fingerprintdb.template.repeating_signatures.lowest_sample_freq=lowest_sample_freq;

    
    #if VT_LOG_LEVEL > 2
    VTLogDebugNoTag("Template type : %d",cs_object->fingerprintdb.template_type);
    decimal    = valid_lowest_frequency;
        frac_float = valid_lowest_frequency - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebugNoTag("\nVALID LOWESRT FREQ:  %d.%04d : \n", decimal, frac);
    #endif

    if (characteristic_frequencies_found == 0)
    {
        return VT_ERROR;
    }


   




    if (cs_repeating_raw_signature_fetch_extrapolated_current_measurement_for_calibration(
            cs_object, raw_signature, lowest_sample_freq, VT_CS_SAMPLE_LENGTH))
    {
        return VT_ERROR;
    }

    if (cs_repeating_signature_offset_current_compute(cs_object, raw_signature, VT_CS_SAMPLE_LENGTH, &offset_current))
    {
        VTLogDebug("Error in computing offset current feature! \r\n");
        return VT_ERROR;
    }

    cs_update_repeating_signature_offset_current_draw(cs_object, lowest_sample_freq, offset_current);
    VTLogDebug("Offset current feature stored! \r\n");
    cs_object->template_confidence_metric = 100;

    return VT_SUCCESS;
}

static VT_UINT cs_recalibrate_repeating_signature_template(VT_CURRENTSENSE_OBJECT* cs_object)
{
    VT_FLOAT raw_signature[VT_CS_SAMPLE_LENGTH]            = {0};
    VT_FLOAT top_N_frequencies[VT_CS_MAX_TEST_FREQUENCIES] = {0};
    VT_FLOAT lowest_sample_freq                            = VT_CS_ADC_MAX_SAMPLING_FREQ;
    cs_calibrate_repeating_signatures_compute_collection_settings(cs_object, top_N_frequencies, &lowest_sample_freq);

    VT_UINT8 characteristic_frequencies_found = 0;
    VT_FLOAT signal_freq;
    VT_FLOAT relative_curr_draw;
    VT_FLOAT duty_cycle;
    VT_FLOAT offset_current;

    for (VT_UINT iter = 0; iter < VT_CS_MAX_TEST_FREQUENCIES; iter++)
    {
        if (characteristic_frequencies_found == VT_CS_MAX_SIGNATURES)
        {
            break;
        }

        if (top_N_frequencies[iter] == 0)
        {
            continue;
        }

        
        if (cs_repeating_raw_signature_fetch_extrapolated_current_measurement_for_calibration(
                cs_object, raw_signature, top_N_frequencies[iter], VT_CS_SAMPLE_LENGTH))
        {
            continue;
        }
        
        if (cs_repeating_signature_feature_vector_compute(cs_object,
                raw_signature,
                VT_CS_SAMPLE_LENGTH,
                top_N_frequencies[iter],
                &signal_freq,
                &duty_cycle,
                &relative_curr_draw))
        {
            VTLogDebug("Error in computing feature vector! \r\n");
            continue;
        }

        if (cs_store_repeating_signature_feature_vector(
                cs_object, top_N_frequencies[iter], signal_freq, duty_cycle, relative_curr_draw))
        {
            break;
        }
        characteristic_frequencies_found++;
    }

   

    if (characteristic_frequencies_found == 0)
    {
        return VT_ERROR;
    }

    
    if (cs_repeating_raw_signature_fetch_extrapolated_current_measurement_for_calibration(
            cs_object, raw_signature, lowest_sample_freq, VT_CS_SAMPLE_LENGTH))
    {
        return VT_ERROR;
    }

    if (cs_repeating_signature_offset_current_compute(cs_object, raw_signature, VT_CS_SAMPLE_LENGTH, &offset_current))
    {
        return VT_ERROR;
    }

    cs_update_repeating_signature_offset_current_draw(cs_object, lowest_sample_freq, offset_current);
    cs_object->template_confidence_metric = 100;

    return VT_SUCCESS;
}

VT_UINT cs_non_repeating_signature_reliability_test_across_calibrations(
    VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT avg_curr_on, VT_FLOAT avg_curr_off)
{
    //cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_on  = avg_curr_on;
    //cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_off = avg_curr_off;

    float avg_curr_on_stored=cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_on;
    float avg_curr_off_stored=cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_off;

    if( (avg_curr_on_stored>(avg_curr_on-(avg_curr_on_stored*(VT_CS_NONREP_AVG_ON_CURR_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/(float)100))))&&
        (avg_curr_on_stored<(avg_curr_on+(avg_curr_on_stored*(VT_CS_NONREP_AVG_ON_CURR_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/(float)100))))   )
        {
        if( (avg_curr_off_stored>(avg_curr_off-(avg_curr_off_stored*(VT_CS_NONREP_AVG_OFF_CURR_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/(float)100))))&&
            (avg_curr_off_stored<(avg_curr_off+(avg_curr_off_stored*(VT_CS_NONREP_AVG_OFF_CURR_DIF_ACROSS_CALIBRATIONS_PERCENT_THRESHOLD/(float)100))))   )
            {
                cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_on_off_repeatable_count++;
            }
        else 
            {VTLogDebug(" avg_curr_off not reliable \r\n");}
        }
    else
        {VTLogDebug(" avg_curr_on not reliable \r\n");}

    VTLogDebug("Modified Average Current for Non-Repeating Signatures value stored in DB \r\n");
    return VT_SUCCESS;
}

static VT_UINT cs_calibrate_non_repeating_signature_template(VT_CURRENTSENSE_OBJECT* cs_object)
{
    VT_FLOAT raw_signature[VT_CS_SAMPLE_LENGTH] = {0};
    VT_FLOAT sampling_frequency                 = 0;
    VT_UINT num_datapoints                      = 0;
    VT_FLOAT avg_curr_on                        = 0;
    VT_FLOAT avg_curr_off                       = 0;

#if VT_LOG_LEVEL > 2
    // VT_INT decimal;
    // VT_FLOAT frac_float;
    // VT_INT frac;
#endif /* VT_LOG_LEVEL > 2 */

    if (cs_non_repeating_raw_signature_fetch_stored_current_measurement(
            cs_object, raw_signature, &sampling_frequency, &num_datapoints) == VT_SUCCESS)
    {
    // VTLogDebug("non repeating Raw: \r\n");

    //         for (VT_INT iter = 0; iter < VT_CS_SAMPLE_LENGTH ; iter++)
    //         {
    //             decimal    = raw_signature[iter];
    //             frac_float = raw_signature[iter] - (VT_FLOAT)decimal;
    //             frac       = fabsf(frac_float) * 10000;
    //             VTLogDebugNoTag("%d.%04d, ", decimal, frac);
    //         }

        if (cs_non_repeating_signature_average_current_compute(
                cs_object, raw_signature, num_datapoints, &avg_curr_on, &avg_curr_off))
        {
            return VT_ERROR;
        }
    }

    //cs_reset_db(cs_object);
    
    if(cs_object->Initial_Calibration_Done==false)
    {
        cs_update_non_repeating_signature_average_current_draw(cs_object, avg_curr_on, avg_curr_off);
        //cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_valid=true;
        cs_object->Initial_Calibration_Done=true;
        cs_object->template_confidence_metric = 100;
    }
    else 
    {
        cs_non_repeating_signature_reliability_test_across_calibrations(cs_object, avg_curr_on, avg_curr_off);
    }
    

    cs_object->Calibration_Done_Count++;

    return VT_SUCCESS;
}

static VT_UINT cs_recalibrate_non_repeating_signature_template(VT_CURRENTSENSE_OBJECT* cs_object)
{
    VT_FLOAT raw_signature[VT_CS_SAMPLE_LENGTH] = {0};
    VT_FLOAT sampling_frequency                 = 0;
    VT_UINT num_datapoints                      = 0;
    VT_FLOAT avg_curr_on                        = 0;
    VT_FLOAT avg_curr_off                       = 0;

    if (cs_non_repeating_raw_signature_fetch_stored_current_measurement(
            cs_object, raw_signature, &sampling_frequency, &num_datapoints) == VT_SUCCESS)
    {
        if (cs_non_repeating_signature_average_current_compute(
                cs_object, raw_signature, num_datapoints, &avg_curr_on, &avg_curr_off))
        {
            return VT_ERROR;
        }
    }

    cs_update_non_repeating_signature_average_current_draw(cs_object, avg_curr_on, avg_curr_off);
    cs_object->template_confidence_metric = 100;

    return VT_SUCCESS;
}

void print_float(float input)
{

#if VT_LOG_LEVEL > 2
    VT_INT decimal;
    VT_FLOAT frac_float;
    VT_INT frac;
        
decimal    = input;
frac_float = input - (VT_FLOAT)decimal;
frac       = fabsf(frac_float) * 10000;
VTLogDebugNoTag(" : %d.%04d \n", decimal, frac);
#endif

}


static uint16_t cs_store_repeating_signature_feature_vector_after_multicalibration(VT_CURRENTSENSE_OBJECT* cs_object)
{
    
    VT_UINT multisampling_num_signatures = cs_object->fingerprintdb.template.repeating_signatures.multisampling_num_signatures;
    for (VT_UINT iter = 0; iter < multisampling_num_signatures; iter++)
    {
        VT_UINT Max_sampling_freq_repeatable_count=0;
        VT_UINT Max_sampling_freq_repeatable_count_iter=0;
        for (VT_UINT iter2 = 0; iter2 < multisampling_num_signatures; iter2++)
        {

        VT_UINT sampling_freq_repeatable_count=cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter2].sampling_freq_repeatable_count;

    //cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].signature_freq_and_duty_cycle_repeatable_count;
    //cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].current_dif_repeatable_count; 
        
        if(sampling_freq_repeatable_count>=MIN_MULTICALIBRATION_MATCHING_COUNT)
        {
            if(sampling_freq_repeatable_count>Max_sampling_freq_repeatable_count)
                {
                    Max_sampling_freq_repeatable_count=sampling_freq_repeatable_count;
                    Max_sampling_freq_repeatable_count_iter=iter;
                }
        }

        }

        VT_UINT num_signatures = cs_object->fingerprintdb.template.repeating_signatures.num_signatures;

        if(cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[Max_sampling_freq_repeatable_count_iter].signature_freq_and_duty_cycle_repeatable_count>=MIN_MULTICALIBRATION_MATCHING_COUNT)
        {
            cs_object->fingerprintdb.template.repeating_signatures.signatures[num_signatures].sampling_freq=
            cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[Max_sampling_freq_repeatable_count_iter].sampling_freq;
            cs_object->fingerprintdb.template.repeating_signatures.signatures[num_signatures].signature_freq=
            cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[Max_sampling_freq_repeatable_count_iter].signature_freq;
            cs_object->fingerprintdb.template.repeating_signatures.signatures[num_signatures].duty_cycle=
            cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[Max_sampling_freq_repeatable_count_iter].duty_cycle;
            cs_object->fingerprintdb.template.repeating_signatures.signatures[num_signatures].signature_freq_and_duty_cycle_valid=true;
        }
        else
        {
            cs_object->fingerprintdb.template.repeating_signatures.signatures[num_signatures].signature_freq=0;
            cs_object->fingerprintdb.template.repeating_signatures.signatures[num_signatures].duty_cycle=0;
            cs_object->fingerprintdb.template.repeating_signatures.signatures[num_signatures].signature_freq_and_duty_cycle_valid=false;            
        }
        if(cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[Max_sampling_freq_repeatable_count_iter].current_dif_repeatable_count>=MIN_MULTICALIBRATION_MATCHING_COUNT)
        {
            cs_object->fingerprintdb.template.repeating_signatures.signatures[num_signatures].sampling_freq=
            cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[Max_sampling_freq_repeatable_count_iter].sampling_freq;
            cs_object->fingerprintdb.template.repeating_signatures.signatures[num_signatures].relative_curr_draw=
            cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[Max_sampling_freq_repeatable_count_iter].relative_curr_draw;
            cs_object->fingerprintdb.template.repeating_signatures.signatures[num_signatures].current_dif_valid=true;
        }
        else
        {
            cs_object->fingerprintdb.template.repeating_signatures.signatures[num_signatures].relative_curr_draw=0;
            cs_object->fingerprintdb.template.repeating_signatures.signatures[num_signatures].current_dif_valid=false;
        }
        if(cs_object->fingerprintdb.template.repeating_signatures.signatures[num_signatures].signature_freq_and_duty_cycle_valid ||
           cs_object->fingerprintdb.template.repeating_signatures.signatures[num_signatures].current_dif_valid)
        {
            cs_object->fingerprintdb.template.repeating_signatures.num_signatures++;
        }

    cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[Max_sampling_freq_repeatable_count_iter].sampling_freq=
        -cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[Max_sampling_freq_repeatable_count_iter].sampling_freq;

    }

    if(cs_object->fingerprintdb.template.repeating_signatures.num_signatures>0)
        {cs_object->fingerprintdb.template.repeating_signatures.repeating_valid=true;}
    else 
        {
        cs_object->fingerprintdb.template.repeating_signatures.repeating_valid=false;
        cs_object->fingerprintdb.template.repeating_signatures.repeating_sensor_status=true;
        }


    return VT_SUCCESS;

}

static uint16_t cs_check_non_repeating_signature_after_multicalibration(VT_CURRENTSENSE_OBJECT* cs_object)
{

    if(cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_on_off_repeatable_count>=MIN_MULTICALIBRATION_MATCHING_COUNT)
        {cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_valid=true;}
    else 
        {
        cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_valid=false;
        cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_sensor_status=true;
        }


        return VT_SUCCESS;
}

void print_db(VT_CURRENTSENSE_OBJECT* cs_object)
{


                    VT_UINT multisampling_num_signatures = cs_object->fingerprintdb.template.repeating_signatures.multisampling_num_signatures;

                    for (VT_UINT iter = 0; iter < multisampling_num_signatures; iter++)
                    {
                    printf("\n sampling_freq ");
                    print_float(cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].sampling_freq);
                    printf("\n signature_freq ");
                    print_float(cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].signature_freq);
                    printf("\n duty_cycle ");
                    print_float(cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].duty_cycle);
                    printf("\n relative_curr_draw ");
                    print_float(cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].relative_curr_draw);

                    printf("\n sampling_freq_repeatable_count : %d \n",cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].sampling_freq_repeatable_count);
                    printf("\n signature_freq_and_duty_cycle_repeatable_count : %d \n",cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].signature_freq_and_duty_cycle_repeatable_count);
                    printf("\n current_dif_repeatable_count : %d \n",cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].current_dif_repeatable_count);
                    
                    printf("\n \n \n");
                    }

                    printf("\n avg_curr_on ");
                    print_float(cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_on);
                    printf("\n avg_curr_off ");
                    print_float(cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_off);
                    printf("\n sampling_freq_repeatable_count : %d \n",cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_on_off_repeatable_count);

                    printf("\n \n \n");

                    VT_UINT num_signatures = cs_object->fingerprintdb.template.repeating_signatures.num_signatures;

                    for (VT_UINT iter = 0; iter < num_signatures; iter++)
                    {
                    printf("\n sampling_freq signatures");
                    print_float(cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].sampling_freq);
                    printf("\n signature_freq signatures");
                    print_float(cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].signature_freq);
                    printf("\n duty_cycle signatures");
                    print_float(cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].duty_cycle);
                    printf("\n relative_curr_draw signatures");
                    print_float(cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].relative_curr_draw);
                    printf("\n signature_freq_and_duty_cycle_valid : %d \n",cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].signature_freq_and_duty_cycle_valid);
                    printf("\n current_dif_valid : %d \n",cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].current_dif_valid);

                    }



}

VT_VOID cs_calibrate_sensor(VT_CURRENTSENSE_OBJECT* cs_object)
{
    if (cs_calibrate_repeating_signature_template(cs_object))
    { 
        VTLogInfo("Error calibrating for repeating \r\n");
    }
    if (cs_calibrate_non_repeating_signature_template(cs_object))
    {
        VTLogInfo("Error calibrating for non-repeating \r\n");
        return;
    }

    if(cs_object->Calibration_Done_Count>=MULTICALIBRATION_COUNT)//greater than equal to ?
        {
            cs_store_repeating_signature_feature_vector_after_multicalibration(cs_object);
            cs_check_non_repeating_signature_after_multicalibration(cs_object);
            #if VT_LOG_LEVEL > 2
            print_db(cs_object);
            #endif
            cs_object->mode = VT_MODE_RUNTIME_EVALUATE;
        }

    if( (cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_valid==true)
                &&(cs_object->fingerprintdb.template.repeating_signatures.repeating_valid==true)    )
        cs_object->fingerprintdb.template_type=VT_CS_BOTH_REPEATING_NONREPEATING_SIGNATURE_VALID;

    else if( (cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_valid==false)
                &&(cs_object->fingerprintdb.template.repeating_signatures.repeating_valid==true)    )
        cs_object->fingerprintdb.template_type=VT_CS_ONLY_REPEATING_SIGNATURE_VALID;

    else if( (cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_valid==true)
                &&(cs_object->fingerprintdb.template.repeating_signatures.repeating_valid==false)    )
        cs_object->fingerprintdb.template_type=VT_CS_ONLY_NONREPEATING_SIGNATURE_VALID;

    else if( (cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_valid==false)
                &&(cs_object->fingerprintdb.template.repeating_signatures.repeating_valid==false)    )
        cs_object->fingerprintdb.template_type=VT_CS_NO_SIGNATURE_VALID;

    VTLogInfo("Successfully calibrated sensor\r\n");
    return;
}



VT_VOID cs_recalibrate_sensor(VT_CURRENTSENSE_OBJECT* cs_object)
{
    if (cs_recalibrate_repeating_signature_template(cs_object))
    {
        if (cs_recalibrate_non_repeating_signature_template(cs_object))
        {
            VTLogInfo("Error re-calibrating sensor\r\n");
            return;
        }
    }
    VTLogInfo("Successfully re-calibrated sensor\r\n");
    return;
}