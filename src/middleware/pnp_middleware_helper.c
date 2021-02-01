/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "pnp_middleware_helper.h"

static const az_span iot_hub_property_reported = AZ_SPAN_LITERAL_FROM_STR("reported");
static const az_span iot_hub_property_desired_version = AZ_SPAN_LITERAL_FROM_STR("$version");
static const az_span component_property_label_name = AZ_SPAN_LITERAL_FROM_STR("__t");

// Check if the component name is in the model
static bool is_component_in_model(
    az_iot_pnp_client const* client,
    az_json_token const* component_name,
    az_span* out_component_name)
{
  int32_t index = 0;

  while (index < client->_internal.options.component_names_length)
  {
    if (az_json_token_is_text_equal(
            component_name, client->_internal.options.component_names[index]))
    {
      *out_component_name = client->_internal.options.component_names[index];
      return true;
    }

    index++;
  }

  return false;
}

// Move reader to the value of property name
static az_result json_child_token_move(az_json_reader* ref_jr, az_span property_name)
{
  az_result _az_result;
  do
  {
    if ((ref_jr->token.kind == AZ_JSON_TOKEN_PROPERTY_NAME)
        && az_json_token_is_text_equal(&(ref_jr->token), property_name))
    {
      _az_result = az_json_reader_next_token(ref_jr);
      if (az_result_failed(_az_result))   
      {                                   
        return _az_result;                
      }
      return AZ_OK;
    }
    else if (ref_jr->token.kind == AZ_JSON_TOKEN_BEGIN_OBJECT)
    {
      if (az_result_failed(az_json_reader_skip_children(ref_jr)))
      {
        return AZ_ERROR_UNEXPECTED_CHAR;
      }
    }
    else if (ref_jr->token.kind == AZ_JSON_TOKEN_END_OBJECT)
    {
      return AZ_ERROR_ITEM_NOT_FOUND;
    }
  } while (az_result_succeeded(az_json_reader_next_token(ref_jr)));

  return AZ_ERROR_ITEM_NOT_FOUND;
}

static az_result check_if_skippable_reported(
    az_json_reader* jr,
    az_iot_pnp_client_property_response_type response_type)
{
  az_result _az_result;
  // First time move
  if (jr->_internal.bit_stack._internal.current_depth == 0)
  {
    _az_result = az_json_reader_next_token(jr);
    if (az_result_failed(_az_result))   
    {                                   
      return _az_result;                
    } 

    if (jr->token.kind != AZ_JSON_TOKEN_BEGIN_OBJECT)
    {
      return AZ_ERROR_UNEXPECTED_CHAR;
    }

    _az_result = az_json_reader_next_token(jr);
    if (az_result_failed(_az_result))   
    {                                   
      return _az_result;                
    } 

    if (response_type == AZ_IOT_PNP_CLIENT_PROPERTY_RESPONSE_TYPE_GET)
    {
      _az_result = json_child_token_move(jr, iot_hub_property_reported);
      if (az_result_failed(_az_result))   
      {                                   
        return _az_result;                
      }   
      _az_result = az_json_reader_next_token(jr);
      if (az_result_failed(_az_result))   
      {                                   
        return _az_result;                
      } 
    }
  }
  while (true)
  {
    // Within the "root" or "component name" section
    if ((response_type == AZ_IOT_PNP_CLIENT_PROPERTY_RESPONSE_TYPE_DESIRED_PROPERTIES
         && jr->_internal.bit_stack._internal.current_depth == 1)
        || (response_type == AZ_IOT_PNP_CLIENT_PROPERTY_RESPONSE_TYPE_GET
            && jr->_internal.bit_stack._internal.current_depth == 2))
    {
      if ((az_json_token_is_text_equal(&jr->token, iot_hub_property_desired_version)))
      {
        // Skip version property name and property value
        _az_result = az_json_reader_next_token(jr);
        if (az_result_failed(_az_result))   
        {                                   
          return _az_result;                
        }
        _az_result = az_json_reader_next_token(jr);
        if (az_result_failed(_az_result))   
        {                                   
          return _az_result;                
        } 

        continue;
      }
      else
      {
        return AZ_OK;
      }
    }
    // Within the property value section
    else if (
        (response_type == AZ_IOT_PNP_CLIENT_PROPERTY_RESPONSE_TYPE_DESIRED_PROPERTIES
         && jr->_internal.bit_stack._internal.current_depth == 2)
        || (response_type == AZ_IOT_PNP_CLIENT_PROPERTY_RESPONSE_TYPE_GET
            && jr->_internal.bit_stack._internal.current_depth == 3))
    {
      if (az_json_token_is_text_equal(&jr->token, component_property_label_name))
      {
        // Skip label property name and property value
        _az_result = az_json_reader_next_token(jr);
        if (az_result_failed(_az_result))   
        {                                   
          return _az_result;                
        }
        _az_result = az_json_reader_next_token(jr);
        if (az_result_failed(_az_result))   
        {                                   
          return _az_result;                
        } 

        continue;
      }
      else
      {
        return AZ_OK;
      }
    }
    else
    {
      return AZ_OK;
    }
  }
}
static az_result az_iot_pnp_client_property_get_next_component_reported_property(
    az_iot_pnp_client const* client,
    az_json_reader* ref_json_reader,
    az_iot_pnp_client_property_response_type response_type,
    az_span* out_component_name,
    az_json_reader* out_property_name_and_value)
{

  (void)client;
  az_result _az_result;

  while (true)
  {
    _az_result = check_if_skippable_reported(ref_json_reader, response_type);
    if (az_result_failed(_az_result))   
    {                                   
      return _az_result;                
    } 

    if (ref_json_reader->token.kind == AZ_JSON_TOKEN_END_OBJECT)
    {
      // At the end of the "root component" or "component name". Done parsing.
      if ((response_type == AZ_IOT_PNP_CLIENT_PROPERTY_RESPONSE_TYPE_DESIRED_PROPERTIES
           && ref_json_reader->_internal.bit_stack._internal.current_depth == 0)
          || (response_type == AZ_IOT_PNP_CLIENT_PROPERTY_RESPONSE_TYPE_GET
              && ref_json_reader->_internal.bit_stack._internal.current_depth == 1))
      {
        return AZ_ERROR_IOT_END_OF_PROPERTIES;
      }

      _az_result = az_json_reader_next_token(ref_json_reader);
      if (az_result_failed(_az_result))   
      {                                   
        return _az_result;                
      }
      // Continue loop if at the end of the component
      continue;
    }

    break;
  }

  // Check if within the "root component" or "component name" section
  if ((response_type == AZ_IOT_PNP_CLIENT_PROPERTY_RESPONSE_TYPE_DESIRED_PROPERTIES
       && ref_json_reader->_internal.bit_stack._internal.current_depth == 1)
      || (response_type == AZ_IOT_PNP_CLIENT_PROPERTY_RESPONSE_TYPE_GET
          && ref_json_reader->_internal.bit_stack._internal.current_depth == 2))
  {
    if (is_component_in_model(client, &ref_json_reader->token, out_component_name))
    {
      _az_result = az_json_reader_next_token(ref_json_reader);
      if (az_result_failed(_az_result))   
      {                                   
        return _az_result;                
      }

      if (ref_json_reader->token.kind != AZ_JSON_TOKEN_BEGIN_OBJECT)
      {
        return AZ_ERROR_UNEXPECTED_CHAR;
      }

      _az_result = az_json_reader_next_token(ref_json_reader);
      if (az_result_failed(_az_result))   
      {                                   
        return _az_result;                
      }
      _az_result = check_if_skippable_reported(ref_json_reader, response_type);
      if (az_result_failed(_az_result))   
      {                                   
        return _az_result;                
      }
    }
    else
    {
      *out_component_name = AZ_SPAN_EMPTY;
    }
  }

  *out_property_name_and_value = *ref_json_reader;

  // Skip the property value array (if applicable) and move to next token
  _az_result = az_json_reader_skip_children(ref_json_reader);
  if (az_result_failed(_az_result))   
  {                                   
    return _az_result;                
  }
  _az_result = az_json_reader_next_token(ref_json_reader);
  if (az_result_failed(_az_result))   
  {                                   
    return _az_result;                
  }

  return AZ_OK;
}

UINT nx_azure_iot_pnp_client_reported_component_property_value_next(NX_AZURE_IOT_PNP_CLIENT *pnp_client_ptr,
                                                                   NX_AZURE_IOT_JSON_READER *reader_ptr, UINT message_type,
                                                                   const UCHAR **component_pptr, UINT *component_len_ptr,
                                                                   NX_AZURE_IOT_JSON_READER *name_value_reader_ptr)
{
az_iot_pnp_client_property_response_type type;
az_result core_result;

    if ((pnp_client_ptr == NX_NULL) ||
        (reader_ptr == NX_NULL) ||
        (component_pptr == NX_NULL) ||
        (component_len_ptr == NX_NULL) )
    {
        LogError(LogLiteralArgs("IoT PnP client reported component next property failed: INVALID POINTER"));
        return(NX_AZURE_IOT_INVALID_PARAMETER);
    }

    if ((message_type != NX_AZURE_IOT_PNP_PROPERTIES)||(message_type == NX_AZURE_IOT_PNP_DESIRED_PROPERTIES))
    {
        // LogError(LogLiteralArgs("Invalid message type passed"));
        return(NX_AZURE_IOT_INVALID_PARAMETER);
    }
    az_span component_name = az_span_create((uint8_t *)*component_pptr, *component_len_ptr);
    
    /* Copy reader control block but do not give NX_PACKET ownership  */
    *name_value_reader_ptr = *reader_ptr;
    name_value_reader_ptr -> packet_ptr = NX_NULL;
    type = (message_type == NX_AZURE_IOT_PNP_DESIRED_PROPERTIES) ? AZ_IOT_PNP_CLIENT_PROPERTY_RESPONSE_TYPE_DESIRED_PROPERTIES :
                AZ_IOT_PNP_CLIENT_PROPERTY_RESPONSE_TYPE_GET;

    core_result = az_iot_pnp_client_property_get_next_component_reported_property(&(pnp_client_ptr -> iot_pnp_client_core),
                                                                         &(reader_ptr -> json_reader),
                                                                         type, &component_name,
                                                                         &(name_value_reader_ptr -> json_reader));
    if (core_result == AZ_ERROR_IOT_END_OF_PROPERTIES)
    {
        return(NX_AZURE_IOT_NOT_FOUND);
    }
    else if (az_result_failed(core_result))
    {
        LogError(LogLiteralArgs("Failed to parse document with core error : %d"), core_result);
        return(NX_AZURE_IOT_SDK_CORE_ERROR);
    }

    *component_pptr = az_span_ptr(component_name);
    *component_len_ptr = (UINT)az_span_size(component_name);

    return(NX_AZURE_IOT_SUCCESS);
}