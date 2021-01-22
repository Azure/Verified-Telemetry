/* Copyright (c) Microsoft Corporation. */

#include "vt_database.h"

uint vt_database_fingerprint_fetch(VT_DATABASE* database_ptr, int* index, uint32_t* fallcurvearray, int* sensorid)
{
    if (database_ptr == NULL)
        return (VT_PTR_ERROR);
    if(!(*index < (int)(database_ptr->_vt_total_fingerprints)))
    {
        *index = -1;
        return (VT_SUCCESS);
    }
    *sensorid = *(*(database_ptr->_vt_fingerprintdb + *index) + 0);
    memcpy(fallcurvearray,(*(database_ptr->_vt_fingerprintdb + *index) + 1),(100 * sizeof((*(database_ptr->_vt_fingerprintdb + *index) + 1))));
    (*index)++;
    return VT_SUCCESS;
}

uint vt_database_falltime_fetch(VT_DATABASE* database_ptr, int* index, int* fall_time, int* sensorid)
{
    if (database_ptr == NULL)
        return (VT_PTR_ERROR);
    if(!(*index < (int)(database_ptr->_vt_total_falltime)))
    {
        *index = -1;
        return (VT_SUCCESS);
    }
    *sensorid = database_ptr->_vt_falltimedb[*index][0];
    *fall_time = database_ptr->_vt_falltimedb[*index][1];
    (*index)++;
    return VT_SUCCESS;
}

uint vt_database_pearsoncoefficient_fetch(VT_DATABASE* database_ptr, int* index, double* pearson_coefficient, int* sensorid)
{
    if (database_ptr == NULL)
        return (VT_PTR_ERROR);

    if(!(*index < (int)(database_ptr->_vt_total_pearson_coefficient)))
    {
        *index = -1;
        return (VT_SUCCESS);
    }
    *sensorid = (int)(database_ptr->_vt_pearson_coefficientdb[*index][0]);
    *pearson_coefficient = database_ptr->_vt_pearson_coefficientdb[*index][1];
    (*index)++;
    return VT_SUCCESS;
}