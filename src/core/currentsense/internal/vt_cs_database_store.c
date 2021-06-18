/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_database.h"
#include "vt_debug.h"

VT_UINT cs_store_signature(VT_CURRENTSENSE_OBJECT* cs_object,
    VT_FLOAT sampling_frequency,
    VT_FLOAT signature_frequency,
    VT_FLOAT duty_cycle,
    VT_FLOAT current_draw)
{
    VT_UINT num_signatures = cs_object->fingerprintdb.num_signatures;
    if ((num_signatures + 1) >= VT_CS_MAX_SIGNATURES)
    {
        return VT_ERROR;
    }

    cs_object->fingerprintdb.db[num_signatures].sampling_freq = sampling_frequency;
    cs_object->fingerprintdb.db[num_signatures].signal_freq   = signature_frequency;
    cs_object->fingerprintdb.db[num_signatures].duty_cycle    = duty_cycle;
    cs_object->fingerprintdb.db[num_signatures].curr_diff     = current_draw;
    cs_object->fingerprintdb.num_signatures++;

    cs_object->db_updated = VT_DB_UPDATED;
    VTLogDebug("Number of Signatures stored in DB %d\r\n", cs_object->fingerprintdb.num_signatures);
    return VT_SUCCESS;
}

VT_UINT cs_update_average_current_draw(VT_CURRENTSENSE_OBJECT* cs_object, VT_FLOAT lowest_sample_freq, VT_FLOAT avg_curr)
{
    if (lowest_sample_freq > cs_object->fingerprintdb.lowest_sample_freq)
    {
        return VT_ERROR;
    }

    cs_object->fingerprintdb.lowest_sample_freq = lowest_sample_freq;
    cs_object->fingerprintdb.avg_curr           = avg_curr;

    cs_object->db_updated = VT_DB_UPDATED;
    VTLogDebug("Modified Average Current value stored in DB \r\n");
    return VT_SUCCESS;
}