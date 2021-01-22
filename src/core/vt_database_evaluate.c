/* Copyright (c) Microsoft Corporation. */

#include "vt_database.h"

int _vt_database_falltime_nearestindex_search(VT_DATABASE* database_ptr, int fall_time);
int _vt_database_pearsoncoefficient_falltimeindex_search(VT_DATABASE* database_ptr, int value);

int _vt_database_evaluate_nrmse(VT_DATABASE* database_ptr, uint32_t* fallcurvearray)
{
    if (!(database_ptr->_vt_total_fingerprints > 0))
        return -1;

    double nrmse[database_ptr->_vt_total_fingerprints];
    double min = 65355.00;
    int index;

    for (int i = 0; i < (database_ptr->_vt_total_fingerprints); i++)
        nrmse[i] = _vt_fingerprint_evaluate_nrmse(&(database_ptr->_vt_fingerprintdb[i][2]), fallcurvearray,100);


    for (int k = 0; k < (database_ptr->_vt_total_fingerprints); k++)
        if (nrmse[k] < min)
        {
            min = nrmse[k], index = k;
        }

    if (min < VT_REPEATABILITY_THRESHOLD && min>=0)
        return database_ptr->_vt_fingerprintdb[index][0];
    else
        return 0;
}

int _vt_database_evaluate_pearson_falltime(VT_DATABASE* database_ptr, int fall_time, double pearson_coefficient)
{
    if (database_ptr->_vt_total_falltime == 0)
        return -1;

    // Find index of the member nearest to the recieved Fall Time
    int nearest_index = _vt_database_falltime_nearestindex_search(database_ptr, fall_time);

    // Check if the variation from the nearest fall time is less than the threshold
    if ((((abs((float)(database_ptr->_vt_falltimedb[nearest_index][1] - fall_time))) /
            database_ptr->_vt_falltimedb[nearest_index][1])*100) < FALL_TIME_THRESHOLD)
    {

        // Find index of the existing label
        int index = _vt_database_pearsoncoefficient_falltimeindex_search(database_ptr, database_ptr->_vt_falltimedb[nearest_index][0]);

        // Check if the variation from the nearest Pearsdon Coefficient is less than the threshold
        if (((abs(database_ptr->_vt_pearson_coefficientdb[index][1] - pearson_coefficient)/
            database_ptr->_vt_pearson_coefficientdb[index][1]) * 100) < PEARSON_COEFFICIENT_THRESHOLD)
            return database_ptr->_vt_pearson_coefficientdb[index][0];

        return 0;
    }

    return 0;
}

int _vt_database_falltime_nearestindex_search(VT_DATABASE* database_ptr, int fall_time)
{
    int i;

    for (i = 0; (i < database_ptr->_vt_total_falltime) && (database_ptr->_vt_falltimedb[i][1] <= fall_time);i++);

    if (abs(fall_time - database_ptr->_vt_falltimedb[i][1]) <=
        abs(database_ptr->_vt_falltimedb[i - 1][1] - fall_time))
        return i;
    else
        return i - 1;
}

int _vt_database_pearsoncoefficient_falltimeindex_search(VT_DATABASE* database_ptr, int value)
{
    int i;
    for (i = 0; (i < database_ptr->_vt_total_pearson_coefficient) && (database_ptr->_vt_pearson_coefficientdb[i][0] != value);i++);

    return i;
}
