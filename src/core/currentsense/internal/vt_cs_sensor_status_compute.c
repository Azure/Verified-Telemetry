/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_api.h"
#include "vt_cs_database.h"
#include "vt_cs_raw_signature_read.h"
#include "vt_cs_sensor_status.h"
#include "vt_cs_signature_features.h"
#include <math.h>
#include "vt_debug.h"

static VT_VOID cs_sensor_status_with_non_repeating_signature_template(VT_CURRENTSENSE_OBJECT* cs_object)
{
    VT_FLOAT raw_signature[VT_CS_SAMPLE_LENGTH] = {0};
    VT_FLOAT sampling_frequency                 = 0;
    VT_UINT num_datapoints                      = 0;
    VT_FLOAT avg_curr_on;
    VT_FLOAT avg_curr_on_saved;
    VT_FLOAT avg_curr_off;
    VT_FLOAT avg_curr_off_saved;

    VT_FLOAT avg_curr_drift = 0;

    if (cs_fetch_template_non_repeating_signature_average_current(cs_object, &avg_curr_on_saved, &avg_curr_off_saved) ==
        VT_SUCCESS)
    {
        if (cs_non_repeating_raw_signature_fetch_stored_current_measurement(
                cs_object, raw_signature, &sampling_frequency, &num_datapoints) == VT_SUCCESS)
        {

            if (cs_non_repeating_signature_average_current_compute(
                    cs_object, raw_signature, num_datapoints, &avg_curr_on, &avg_curr_off) == VT_SUCCESS)
            {
                avg_curr_drift = cs_non_repeating_signature_average_current_evaluate(
                    avg_curr_on, avg_curr_on_saved, avg_curr_off, avg_curr_off_saved);

                if (avg_curr_drift > VT_CS_MAX_AVG_CURR_DRIFT)
                {   VTLogDebugNoTag("\n non_repeating_signature -  VT_SIGNATURE_NOT_MATCHING \n");
                    //cs_object->sensor_status = VT_SIGNATURE_NOT_MATCHING;
                    cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_sensor_status=VT_SIGNATURE_NOT_MATCHING;
                    cs_object->sensor_drift  = avg_curr_drift;
                }
                else
                {   VTLogDebugNoTag("\n non_repeating_signature -  VT_SIGNATURE_MATCHING \n");
                    //cs_object->sensor_status = VT_SIGNATURE_MATCHING;
                    cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_sensor_status=VT_SIGNATURE_MATCHING;
                    cs_object->sensor_drift  = avg_curr_drift;
                }
                return;
            }
            //cs_object->sensor_status = VT_SIGNATURE_COMPUTE_FAIL;
            VTLogDebugNoTag("\n non_repeating_signature -  VT_SIGNATURE_COMPUTE_FAIL \n");
            cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_sensor_status=VT_SIGNATURE_COMPUTE_FAIL;
            cs_object->sensor_drift  = 100;
        }
        //cs_object->sensor_status = VT_SIGNATURE_COMPUTE_FAIL;
        VTLogDebugNoTag("\n non_repeating_signature -  VT_SIGNATURE_COMPUTE_FAIL \n");
        cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_sensor_status=VT_SIGNATURE_COMPUTE_FAIL;
        cs_object->sensor_drift  = 100;
    }

    //cs_object->sensor_status = VT_SIGNATURE_DB_EMPTY;
    VTLogDebugNoTag("\n non_repeating_signature -  VT_SIGNATURE_DB_EMPTY \n");
    cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_sensor_status=VT_SIGNATURE_DB_EMPTY;
    cs_object->sensor_drift  = 100;
}

static VT_VOID cs_sensor_status_with_repeating_signature_template(VT_CURRENTSENSE_OBJECT* cs_object)
{
    printf(" \n inside rep sig evaluate \n ");

    VT_FLOAT raw_signature[VT_CS_SAMPLE_LENGTH] = {0};

    VT_FLOAT lowest_sample_freq_saved;
    VT_FLOAT offset_current;
    VT_FLOAT offset_current_saved;

    VT_FLOAT sampling_frequency_saved;
    VT_FLOAT signature_frequency;
    VT_FLOAT duty_cycle;
    VT_FLOAT relative_current_draw;
    VT_FLOAT signature_frequency_saved;
    VT_FLOAT duty_cycle_saved;
    VT_FLOAT relative_current_draw_saved;

    VT_FLOAT offset_current_drift = 0;
    VT_FLOAT feature_vector_drift = 0;
    VT_FLOAT temp_feature_vector_drift = 0;
    VT_FLOAT temp_relative_current_drift = 0;
    VT_FLOAT temp_offset_current_drift = 0;
    

    VT_BOOL offset_current_unavailable = false;

    VT_UINT signatures_evaluated                  = 0;
    VT_BOOL signature_feature_vector_compute_fail = false;
    VT_BOOL signature_offset_current_compute_fail = false;

#if VT_LOG_LEVEL > 2
    VT_INT decimal=0;
    VT_FLOAT frac_float;
    VT_INT frac;
    #endif

    VT_BOOL sensor_status=true;
    VT_BOOL temp_sensor_status=true;
    VT_BOOL offset_curr_status=false;

    if (cs_fetch_template_repeating_signature_offset_current(cs_object, &lowest_sample_freq_saved, &offset_current_saved))
    {
        offset_current_unavailable = true;
        printf("\nerror\n");
    }
    if (!offset_current_unavailable)
    #if VT_LOG_LEVEL > 2
            decimal    = lowest_sample_freq_saved;
        frac_float = lowest_sample_freq_saved - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
    
    VTLogDebugNoTag("\nLOWEST TEMPLATE FREQ: %d.%04d : \n", decimal, frac);
    #endif
    {
        if (cs_repeating_raw_signature_fetch_stored_current_measurement(
                cs_object, raw_signature, lowest_sample_freq_saved, VT_CS_SAMPLE_LENGTH) == VT_SUCCESS)
        {   
            if (cs_repeating_signature_offset_current_compute(cs_object, raw_signature, VT_CS_SAMPLE_LENGTH, &offset_current) ==
                VT_SUCCESS)
            { 
                temp_offset_current_drift = cs_repeating_signature_offset_current_evaluate(offset_current, offset_current_saved);
                if((temp_offset_current_drift)<40){
                    offset_curr_status=true;
                    offset_current_drift=temp_offset_current_drift*(50/40);
                }
                else{
                    offset_curr_status=false;
                    offset_current_drift=temp_offset_current_drift*(50/40);
                }
            }
            else
            {
                signature_offset_current_compute_fail = true;
            }
        }
        else
        {
            signature_offset_current_compute_fail = true;
        }
    }
    sensor_status=sensor_status && offset_curr_status;
    for (VT_UINT iter = 0; iter < cs_object->fingerprintdb.template.repeating_signatures.num_signatures; iter++)

    {
        signature_feature_vector_compute_fail=false;
    temp_feature_vector_drift = 0;
        #if VT_LOG_LEVEL > 2
    VTLogDebugNoTag("\n sensor_status : %d",sensor_status);
    #endif
        if (cs_fetch_template_repeating_signature_feature_vector(cs_object,
                iter,
                &sampling_frequency_saved,
                &signature_frequency_saved,
                &duty_cycle_saved,
                &relative_current_draw_saved))
        {
            break;
        }

        if (cs_repeating_raw_signature_fetch_stored_current_measurement(
                cs_object, raw_signature, sampling_frequency_saved, VT_CS_SAMPLE_LENGTH))
        {
            signature_feature_vector_compute_fail = true;
            break;
        }

        #if VT_LOG_LEVEL > 2
        decimal    = sampling_frequency_saved;
        frac_float = sampling_frequency_saved - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebugNoTag("\ncalculating for %d.%04d : \n", decimal, frac);

         VTLogDebugNoTag("\nRAW SIG:\n");
   
for (VT_INT iter1 = 0; iter1 < VT_CS_SAMPLE_LENGTH; iter1++)
    {
        decimal    = raw_signature[iter1];
        frac_float = raw_signature[iter1] - (VT_FLOAT)decimal;
        frac       = fabsf(frac_float) * 10000;
        VTLogDebugNoTag("%d.%04d, ", decimal, frac);
    }
VTLogDebugNoTag("\n");
#endif
    
        if (cs_repeating_signature_feature_vector_compute(cs_object,
                raw_signature,
                VT_CS_SAMPLE_LENGTH,
                sampling_frequency_saved,
                &signature_frequency,
                &duty_cycle,
                &relative_current_draw))
        {
            signature_feature_vector_compute_fail = true;
           // break;
        }
    /*
    if (sampling_frequency_saved==VT_CS_ADC_MAX_SAMPLING_FREQ){
        if ((signature_feature_vector_compute_fail==false))
        {

            //temp_relative_current_drift=cs_repeating_signature_relative_current_evaluate(relative_current_draw,relative_current_draw_saved);

            //if (temp_relative_current_drift<60.0f){
                
            if(cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].signature_freq_and_duty_cycle_valid==true)
            {
                temp_feature_vector_drift = cs_repeating_signature_feature_vector_evaluate(signature_frequency,
                signature_frequency_saved,
                duty_cycle,
                duty_cycle_saved,
                relative_current_draw,
                relative_current_draw_saved);
            }
        }
                        if (signature_feature_vector_compute_fail==true){
                temp_feature_vector_drift =100;
            }
            //}

            // else{signature_feature_vector_compute_fail=true;}

            // }

            // if ((signature_feature_vector_compute_fail==true) || (temp_feature_vector_drift>50)){
            //     temp_sensor_status=false;
            //     temp_feature_vector_drift=100;
            //     }
            // else {
            //     temp_sensor_status=true;
                               
            // }
            if(cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].current_dif_valid==true)
            {
                temp_relative_current_drift=cs_repeating_signature_relative_current_evaluate(relative_current_draw,relative_current_draw_saved);
            }

            if((cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].signature_freq_and_duty_cycle_valid==true)&&
                    (cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].current_dif_valid==true))
            {   
                printf(" \n both signature_freq_and_duty_cycle_valid & current_dif_valid valid  \n");
                if((temp_feature_vector_drift<50.0f) || (temp_relative_current_drift<55.0f))
                {
                    temp_sensor_status=true;
                    feature_vector_drift=feature_vector_drift+temp_feature_vector_drift+temp_relative_current_drift;
                }
                else
                    temp_sensor_status=false;
            }
            else if(cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].signature_freq_and_duty_cycle_valid==true)
            {
                printf(" \n only signature_freq_and_duty_cycle_valid  \n");
                if(temp_feature_vector_drift<50.0f)
                {
                    temp_sensor_status=true;
                    feature_vector_drift=feature_vector_drift+temp_feature_vector_drift;
                }
                else
                    temp_sensor_status=false;
            }
            else if(cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].current_dif_valid==true)
            {
                printf(" \n only current_dif_valid  \n");
                if(temp_relative_current_drift<55.0f)
                {
                    temp_sensor_status=true;
                    feature_vector_drift=feature_vector_drift+temp_relative_current_drift;
                }
                else
                    temp_sensor_status=false;
            }
            else
                {
                    printf(" \n NONE valid  \n"); 
                    temp_sensor_status=false;
                }
            
            //            if ((temp_feature_vector_drift<50.0f) || (temp_relative_current_drift<55.0f)){
            //    
            //        #if VT_LOG_LEVEL > 2
            //        VTLogDebugNoTag("component correct");
            //        #endif
            //    temp_sensor_status=true;
            //}
            //else{
            //        #if VT_LOG_LEVEL > 2
            //        VTLogDebugNoTag("component faulty");
            //        #endif
            //    temp_sensor_status=false;                
            //}
            //feature_vector_drift=feature_vector_drift +temp_feature_vector_drift;
            
            sensor_status=sensor_status && temp_sensor_status;
            
        signatures_evaluated++;}

    else {
            */
            if ((signature_feature_vector_compute_fail==false))
            {

            if(cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].signature_freq_and_duty_cycle_valid==true)
            {

            temp_feature_vector_drift = cs_repeating_signature_feature_vector_evaluate(signature_frequency,
            signature_frequency_saved,
            duty_cycle,
            duty_cycle_saved,
            relative_current_draw,
            relative_current_draw_saved);
            }
            }
            if (signature_feature_vector_compute_fail==true){
                temp_feature_vector_drift =100;
            }

            if(cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].current_dif_valid==true)
            {
            temp_relative_current_drift=cs_repeating_signature_relative_current_evaluate(relative_current_draw,relative_current_draw_saved);
            }

            if((cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].signature_freq_and_duty_cycle_valid==true)&&
                    (cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].current_dif_valid==true))
            {   
                printf(" \n both signature_freq_and_duty_cycle_valid & current_dif_valid valid  \n");
                if((temp_feature_vector_drift<50.0f) && (temp_relative_current_drift<50.0f))
                {
                    temp_sensor_status=true;
                    feature_vector_drift=feature_vector_drift+(temp_feature_vector_drift+temp_relative_current_drift)/2;
                }
                else
                    temp_sensor_status=false;
            }
            else if(cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].signature_freq_and_duty_cycle_valid==true)
            {
                printf(" \n only signature_freq_and_duty_cycle_valid  \n");
                if(temp_feature_vector_drift<50.0f)
                {
                    temp_sensor_status=true;
                    feature_vector_drift=feature_vector_drift+temp_feature_vector_drift;
                }
                else
                    temp_sensor_status=false;
            }
            else if(cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].current_dif_valid==true)
            {
                printf(" \n only current_dif_valid  \n");
                if(temp_relative_current_drift<50.0f)
                {
                    temp_sensor_status=true;
                    feature_vector_drift=feature_vector_drift+temp_relative_current_drift;
                }
                else
                    temp_sensor_status=false;
            }
            else
                {
                    printf(" \n NONE valid  \n"); 
                    temp_sensor_status=false;
                }


            /*
            if ((temp_feature_vector_drift<50.0f) || (temp_relative_current_drift<40.0f)){
                
                    #if VT_LOG_LEVEL > 2
                    VTLogDebugNoTag("component correct");
                    #endif
                temp_sensor_status=true;
            }
            else{
                    #if VT_LOG_LEVEL > 2
                    VTLogDebugNoTag("component faulty");
                    #endif
                temp_sensor_status=false;
            }
            */
            sensor_status=sensor_status && temp_sensor_status;
            /*
            if(temp_feature_vector_drift<(temp_relative_current_drift*(50/30))){
                feature_vector_drift=feature_vector_drift +temp_feature_vector_drift;
            }
            else{
                feature_vector_drift=feature_vector_drift +(temp_relative_current_drift*(50/30));
            }
            */
            signatures_evaluated++;  
            
            //}

    



    }
    if (signatures_evaluated)
    {
        feature_vector_drift /= signatures_evaluated;
    }

    if (signature_offset_current_compute_fail)
    {
        //cs_object->sensor_status = VT_SIGNATURE_COMPUTE_FAIL;
        VTLogDebugNoTag("\n repeating_signature -  VT_SIGNATURE_COMPUTE_FAIL \n");
        cs_object->fingerprintdb.template.repeating_signatures.repeating_sensor_status=VT_SIGNATURE_COMPUTE_FAIL;
        cs_object->sensor_drift  = 100;
    }

    

    else if ((signatures_evaluated == 0) && offset_current_unavailable)
    {
        //cs_object->sensor_status = VT_SIGNATURE_DB_EMPTY;
        VTLogDebugNoTag("\n repeating_signature -  VT_SIGNATURE_DB_EMPTY \n");
        cs_object->fingerprintdb.template.repeating_signatures.repeating_sensor_status=VT_SIGNATURE_DB_EMPTY;
        cs_object->sensor_drift  = 100;
    }
        cs_object->fingerprintdb.template.repeating_signatures.repeating_sensor_status=!sensor_status;
        VTLogDebugNoTag("\n repeating_signature -  (!sensor_status), repeating_sensor_status : %d \n",cs_object->fingerprintdb.template.repeating_signatures.repeating_sensor_status);
        //cs_object->sensor_status = !sensor_status;
        
        if (signatures_evaluated && (!offset_current_unavailable))
        {
            cs_object->sensor_drift = (offset_current_drift + feature_vector_drift) / 2;
        }
        else if (signatures_evaluated)
        {
            cs_object->sensor_drift = feature_vector_drift;
        }
        else
        {
            cs_object->sensor_drift = offset_current_drift;
        }
    
  
    return;
}


VT_VOID average_status(VT_CURRENTSENSE_OBJECT* cs_object){

    VT_UINT sum=0;

    for(int i=VT_HISTORY_ARRAY_LENGTH-1;i>0;i--){
        cs_object->hist_array[i]=cs_object->hist_array[i-1];
            }
    cs_object->hist_array[0]=cs_object->sensor_status;

    printf("\nHIST ARRAY: ");

    for(int i=0;i<VT_HISTORY_ARRAY_LENGTH;i++){
        sum=sum+cs_object->hist_array[i];
        printf("%d",cs_object->hist_array[i]);
            }
printf("\n%d",sum);
    if(sum>=ceil((float)VT_HISTORY_ARRAY_LENGTH/2)){
        cs_object->sensor_status=true;
    }
    else{
        cs_object->sensor_status=false;
    }


    printf("\nFINAL STATUS:%d\n",cs_object->sensor_status);


}
VT_VOID cs_sensor_status(VT_CURRENTSENSE_OBJECT* cs_object)
{

        #if VT_LOG_LEVEL > 2
        VTLogDebugNoTag("\nNON REPEATING signature valid :  %s\n",(cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_valid?"true":"false"));
        #endif
        if(cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_valid==true)
            cs_sensor_status_with_non_repeating_signature_template(cs_object);


        #if VT_LOG_LEVEL > 2
        VTLogDebugNoTag("\nREPEATING signature valid : %s\n",(cs_object->fingerprintdb.template.repeating_signatures.repeating_valid?"true":"false"));
        #endif
        if(cs_object->fingerprintdb.template.repeating_signatures.repeating_valid==true)
            cs_sensor_status_with_repeating_signature_template(cs_object);

        if(cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_valid==true || cs_object->fingerprintdb.template.repeating_signatures.repeating_valid==true)
        {
            cs_object->fingerprintdb.template.repeating_signatures.repeating_sensor_status=!cs_object->fingerprintdb.template.repeating_signatures.repeating_sensor_status;

            if(cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_sensor_status){
                cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_sensor_status=0;
            }
            else{
                cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_sensor_status=1;
            }

            if(cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_valid==true && cs_object->fingerprintdb.template.repeating_signatures.repeating_valid==true){
                cs_object->sensor_status=
                cs_object->fingerprintdb.template.repeating_signatures.repeating_sensor_status | cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_sensor_status;

            }
            else{
            cs_object->sensor_status=
                cs_object->fingerprintdb.template.repeating_signatures.repeating_sensor_status|cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_sensor_status;
            }

            average_status(cs_object);
            cs_object->sensor_status=!cs_object->sensor_status;

            cs_object->fingerprintdb.template.repeating_signatures.repeating_sensor_status=!cs_object->fingerprintdb.template.repeating_signatures.repeating_sensor_status;

            if(cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_sensor_status){
                cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_sensor_status=0;
            }
            else{
                cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_sensor_status=1;
            }
        }
        else 
        {
            cs_object->sensor_status=VT_SIGNATURE_DB_EMPTY;
        }
        #if VT_LOG_LEVEL > 2        
        VTLogDebugNoTag("\n repeating_sensor_status : %s\n",(cs_object->fingerprintdb.template.repeating_signatures.repeating_sensor_status?"false":"true"));
        VTLogDebugNoTag("\n non_repeating_sensor_status : %s\n",(cs_object->fingerprintdb.template.non_repeating_signature.non_repeating_sensor_status?"false":"true"));
        VTLogDebugNoTag("\n cs_object->sensor_status : %s\n",(cs_object->sensor_status?"false":"true"));
        #endif
        //if either of them s true, output is true.
}
