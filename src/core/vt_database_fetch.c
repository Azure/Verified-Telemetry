/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <string.h>

#include "vt_database.h"

uint32_t vt_database_falltime_fetch(VT_DATABASE* database_ptr, uint8_t* index, uint32_t* fall_time, uint8_t* sensor_id)
{
    if (database_ptr == NULL)
    {
        return (VT_PTR_ERROR);
    }

    if (!(*index < database_ptr->_vt_total_falltime))
    {
        *index = -1;
        return (VT_SUCCESS);
    }

    *sensor_id  = database_ptr->_vt_falltimedb[*index][0];
    *fall_time = database_ptr->_vt_falltimedb[*index][1];
    (*index)++;
    return VT_SUCCESS;
}

uint32_t vt_database_pearsoncoefficient_fetch(
    VT_DATABASE* database_ptr, uint8_t* index, float* pearson_coefficient, uint8_t* sensor_id)
{
    if (database_ptr == NULL)
    {
        return (VT_PTR_ERROR);
    }

    if (!(*index < database_ptr->_vt_total_pearson_coefficient))
    {
        *index = -1;
        return (VT_SUCCESS);
    }

    *sensor_id            = database_ptr->_vt_pearson_coefficientdb[*index][0];
    *pearson_coefficient  = database_ptr->_vt_pearson_coefficientdb[*index][1];
    (*index)++;
    return VT_SUCCESS;
}

uint32_t _vt_database_check_pearson_falltime_availability(VT_DATABASE* database_ptr)
{
    if (database_ptr->_vt_total_falltime == 0)
    {
        return VT_ERROR;
    }
    return VT_SUCCESS;
}
