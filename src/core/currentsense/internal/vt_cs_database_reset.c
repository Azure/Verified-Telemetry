/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_database.h"

VT_VOID cs_reset_db(VT_CURRENTSENSE_OBJECT* cs_object)
{
    //cs_object->fingerprintdb.template_type                                    = VT_CS_NON_REPEATING_SIGNATURE;
    cs_object->fingerprintdb.template.repeating_signatures.num_signatures     = 0;
    cs_object->fingerprintdb.template.repeating_signatures.lowest_sample_freq = VT_DATA_NOT_AVAILABLE;
    cs_object->fingerprintdb.template.repeating_signatures.offset_current     = VT_DATA_NOT_AVAILABLE;
    cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_on     = VT_DATA_NOT_AVAILABLE;
    cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_off    = VT_DATA_NOT_AVAILABLE;
}

VT_VOID cs_reset_Repeatability_Data_Array(VT_CURRENTSENSE_OBJECT* cs_object)
{
    cs_object->fingerprintdb.template.repeating_signatures.multisampling_num_signatures=0;
    cs_object->fingerprintdb.template.repeating_signatures.num_signatures=0;
    cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_on_off_repeatable_count=0;
    cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_on     = 0;
    cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_off    = 0;

    for (VT_UINT iter = 0; iter < VT_CS_MULTISAMPLING_MAX_SIGNATURES; iter++)
    {
    cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].sampling_freq = 0;
    cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].signature_freq = 0;
    for(int iter1=0;iter1<VT_CS_ACRFFT_MAX_PEAKS;iter1++){
        cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].betasig_freqs[iter1] = 0;
        cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].signature_freq_and_duty_cycle_repeatable_count[iter1] = 0;

    }
    cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].duty_cycle = 0;
    cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].relative_curr_draw = 0;

    cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].sampling_freq_repeatable_count = 0;
    
    cs_object->fingerprintdb.template.repeating_signatures.Multisampling_Signatures_With_Repeatability_Data_Array[iter].current_dif_repeatable_count = 0;
    }

    for (VT_UINT iter = 0; iter < VT_CS_MAX_SIGNATURES; iter++)
    {
    cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].sampling_freq = 0;
    cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].signature_freq = 0;
    cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].duty_cycle = 0;
    cs_object->fingerprintdb.template.repeating_signatures.signatures[iter].relative_curr_draw = 0;


    }

}