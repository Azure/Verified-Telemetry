/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_database.h"

static int _vt_database_falltime_nearestindex_search(VT_DATABASE* database_ptr, uint32_t fall_time);
static int _vt_database_pearsoncoefficient_falltimeindex_search(VT_DATABASE* database_ptr, int value);

int _vt_database_evaluate_nrmse(VT_DATABASE* database_ptr, uint32_t* fallcurvearray)
{
    if (!(database_ptr->_vt_total_fingerprints > 0))
    {
        return -1;
    }

    float nrmse[98];
    float min = 65355.00;
    int index;

    for (uint32_t i = 0; i < (database_ptr->_vt_total_fingerprints); i++)
        nrmse[i] = _vt_fingerprint_evaluate_nrmse(&(database_ptr->_vt_fingerprintdb[i][2]), fallcurvearray, 100);

    for (uint32_t k = 0; k < (database_ptr->_vt_total_fingerprints); k++)
        if (nrmse[k] < min)
        {
            min = nrmse[k], index = k;
        }

    if (min < VT_REPEATABILITY_THRESHOLD && min >= 0)
    {
        return database_ptr->_vt_fingerprintdb[index][0];
    }
    else
    {
        return 0;
    }
}

int _vt_database_check_pearson_falltime_availability(VT_DATABASE* database_ptr)
{
    if (database_ptr->_vt_total_falltime == 0)
    {
        return VT_ERROR;
    }
    return VT_SUCCESS;
}

int _vt_database_evaluate_pearson_falltime(VT_DATABASE* database_ptr, int fall_time, float pearson_coefficient)
{
    if (database_ptr->_vt_total_falltime == 0)
    {
        return -1;
    }

    // Find index of the member nearest to the recieved Fall Time
    int nearest_index = _vt_database_falltime_nearestindex_search(database_ptr, fall_time);

    // Check if the variation from the nearest fall time is less than the
    // threshold
    if ((((abs((float)(database_ptr->_vt_falltimedb[nearest_index][1] - fall_time))) /
             database_ptr->_vt_falltimedb[nearest_index][1]) *
            100) < FALL_TIME_THRESHOLD)
    {

        // Find index of the existing label
        int index = _vt_database_pearsoncoefficient_falltimeindex_search(
            database_ptr, database_ptr->_vt_falltimedb[nearest_index][0]);

        // Check if the variation from the nearest Pearsdon Coefficient is less than
        // the threshold
        if (((abs(database_ptr->_vt_pearson_coefficientdb[index][1] - pearson_coefficient) /
                 database_ptr->_vt_pearson_coefficientdb[index][1]) *
                100) < PEARSON_COEFFICIENT_THRESHOLD)
        {
            return database_ptr->_vt_pearson_coefficientdb[index][0];
        }

        return 0;
    }

    return 0;
}

static int _vt_database_falltime_nearestindex_search(VT_DATABASE* database_ptr, uint32_t fall_time)
{
    uint32_t i;

    for (i = 0; (i < database_ptr->_vt_total_falltime) && (database_ptr->_vt_falltimedb[i][1] <= fall_time); i++)
        ;

    if (abs(fall_time - database_ptr->_vt_falltimedb[i][1]) <= abs(database_ptr->_vt_falltimedb[i - 1][1] - fall_time))
    {
        return i;
    }
    else
    {
        return i - 1;
    }
}

static int _vt_database_pearsoncoefficient_falltimeindex_search(VT_DATABASE* database_ptr, int value)
{
    uint32_t i;
    for (i = 0;
         (i < database_ptr->_vt_total_pearson_coefficient) && (database_ptr->_vt_pearson_coefficientdb[i][0] != value);
         i++)
        ;

    return i;
}
