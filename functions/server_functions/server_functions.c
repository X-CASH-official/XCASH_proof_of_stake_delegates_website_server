#include <stdio.h>
#include <stdlib.h>
#include <mongoc/mongoc.h>
#include <bson/bson.h>
#include <microhttpd.h>

#include "define_macro_functions.h"
#include "define_macros.h"
#include "structures.h"
#include "variables.h"

#include "define_macros_functions.h"
#include "blockchain_functions.h"
#include "database_functions.h"
#include "network_daemon_functions.h"
#include "server_functions.h"
#include "string_functions.h"

/*
-----------------------------------------------------------------------------------------------------------
Functions
-----------------------------------------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------------------------------------
Name: read_file_server
Description: Reads a file for the server
Parameters:
   cls - Callback value
   position - Position to set the file pointer to
   data - Holds the file fata
   file_size - The size of the file
Return: The number of bytes read from the file
-----------------------------------------------------------------------------------------------------------
*/

ssize_t read_file_server(void *cls, size_t position , char* data, size_t file_size)
{
  FILE* file = cls;
  // set the pointer to the begining of the file
  fseek(file, position, SEEK_SET);
  // read the file into the buffer
  const ssize_t file_data = fread(data, sizeof(char), file_size, file);
  return file_data;
}



/*
-----------------------------------------------------------------------------------------------------------
Name: callback_reset
Description: resets the file read by the server
Parameters:
   cls - Callback value
-----------------------------------------------------------------------------------------------------------
*/

void callback_reset(void *cls)
{
  // close the file
  FILE* file = cls;
  fclose(file);
}



/*
-----------------------------------------------------------------------------------------------------------
Name: post_result
Description: Gets the data from the post request
Parameters:
   connection_info_cls - Callback value
   post_type - The post data item type
   item - The post data item
   file_name - file name
   content_type - Content type
   transfer_encoding - The post data item encoding
   data - The post data item value
   position - The position of the data
   size - The post data value size
Return: 0 if an error has occured, 1 if successfull
-----------------------------------------------------------------------------------------------------------
*/

int post_result(void *connection_info_cls, enum MHD_ValueKind post_type, const char* item,const char* file_name, const char* content_type,const char* transfer_encoding, const char* data, uint64_t position, size_t size)
{
  // Variables
  struct connection_info_struct *connection_info = connection_info_cls;
  char* result = malloc(POST_DATA_MAXIMUM_ITEM_SIZE);
  int settings;

  // define macros
  #define GET_DATA(item_data) \
  sprintf(result, "%s", data); \
  decode_html(result); \
  connection_info->item[connection_info->count] = item_data; \
  connection_info->value[connection_info->count] = result; \
  connection_info->count = connection_info->count + 1; \
  settings = MHD_YES;

  // error check
  if (size > 0 && size <= POST_DATA_MAXIMUM_ITEM_SIZE)
  {
    if (strncmp(item,"public_address",BUFFER_SIZE) == 0)
    {
      GET_DATA("public_address");
    }
    else if (strncmp(item,"delegate_name",BUFFER_SIZE) == 0)
    {
      GET_DATA("delegate_name");
    }
    else if (strncmp(item,"IP_address",BUFFER_SIZE) == 0)
    {
      GET_DATA("IP_address");
    }
    else if (strncmp(item,"pool_mode",BUFFER_SIZE) == 0)
    {
     GET_DATA("pool_mode");
    }
    else if (strncmp(item,"password",BUFFER_SIZE) == 0)
    {
      GET_DATA("password");
    }
    else if (strncmp(item,"session",BUFFER_SIZE) == 0)
    {
      GET_DATA("session");
    }
    else if (strncmp(item,"about",BUFFER_SIZE) == 0)
    {
      GET_DATA("about");
    }
    else if (strncmp(item,"website",BUFFER_SIZE) == 0)
    {
      GET_DATA("website");
    }
    else if (strncmp(item,"team",BUFFER_SIZE) == 0)
    {
      GET_DATA("team");
    }
    else if (strncmp(item,"fee_structure",BUFFER_SIZE) == 0)
    {
      GET_DATA("fee_structure");
    }
    else if (strncmp(item,"server_settings",BUFFER_SIZE) == 0)
    {
      GET_DATA("server_settings");
    }
  }
  else
  {
    settings = MHD_NO;
  } 
  return settings;

  #undef GET_DATA
}



/*
-----------------------------------------------------------------------------------------------------------
Name: post_request_completed
Description: Callback for when the post request is done
Parameters:
   cls - Callback value
   connection - The server connection
   settings - Settings
   request - Request status
Return: 0 if an error has occured, 1 if successfull
-----------------------------------------------------------------------------------------------------------
*/

void post_request_completed(void* cls, struct MHD_Connection* connection,void** settings, enum MHD_RequestTerminationCode request)
{
  // Variables
  struct connection_info_struct *connection_info = *settings;

  if (connection_info == NULL)
  {
    return;
  }
  if (connection_info->connection_type == 1)
  {
    MHD_destroy_post_processor(connection_info->post_result_settings);
  }

  free(connection_info);
  *settings = NULL;
}



/*
-----------------------------------------------------------------------------------------------------------
Name: get_statistics
Description: Gets the statistics from the database
Parameters:
   result - The json data for the statistics
Return: The http status
-----------------------------------------------------------------------------------------------------------
*/

int get_statistics(char* result)
{
  // Variables
  char* data = (char*)calloc(BUFFER_SIZE,sizeof(char));  
  size_t count = 0;
  size_t count2 = 0;
  size_t counter = 0;
  size_t number = 0;
  size_t current_block_height = 0;
  double generated_supply;
  struct database_document_fields database_data;
  struct database_multiple_documents_fields database_multiple_documents_fields;
  int document_count = 0;

  // define macros
  #define DATABASE_COLLECTION "statistics"
  #define DATABASE_FIELDS "username|"
  #define DATA "{\"username\":\"XCASH\"}"
  #define MAXIMUM_DELEGATES_COUNT 100 // The maximum amount of delegates to read from the database

  #define GET_STATISTICS_ERROR \
  memset(result,0,strnlen(result,BUFFER_SIZE)); \
  memcpy(result,"{\"Error\":\"Could not get statistics\"}",36); \
  pointer_reset(data); \
  return 400;

  #define pointer_reset_database_array \
  for (count = 0; count < 11; count++) \
  { \
    pointer_reset(database_data.item[count]); \
    pointer_reset(database_data.value[count]); \
  } \
  for (count = 0; count < document_count; count++) \
  { \
    for (counter = 0; counter < 18; counter++) \
    { \
      pointer_reset(database_multiple_documents_fields.item[count][counter]); \
      pointer_reset(database_multiple_documents_fields.value[count][counter]); \
    } \
  }
  
  // check if the memory needed was allocated on the heap successfully
  if (data == NULL)
  {
    color_print("Could not allocate the memory needed on the heap","red");
    exit(0);
  }

  // check if there is any data in the database that matches the message
  if (count_documents_in_collection(DATABASE_NAME,DATABASE_COLLECTION,DATA,0) <= 0)
  {
    GET_STATISTICS_ERROR;
  }

  // get how many documents are in the database
  document_count = count_all_documents_in_collection(DATABASE_NAME,"delegates",0);
  if (document_count <= 0)
  {
    GET_STATISTICS_ERROR;
  }
  else if (document_count > MAXIMUM_DELEGATES_COUNT)
  {
    document_count = MAXIMUM_DELEGATES_COUNT;
  }

  // initialize the database_document_fields struct 
  for (count = 0; count < 11; count++)
  {
    database_data.item[count] = (char*)calloc(BUFFER_SIZE,sizeof(char));
    database_data.value[count] = (char*)calloc(BUFFER_SIZE,sizeof(char));
  }
  database_data.count = 0;

  // initialize the database_multiple_documents_fields struct 
  for (count = 0; count < document_count; count++)
  {
    for (counter = 0; counter < 18; counter++)
    {
      database_multiple_documents_fields.item[count][counter] = (char*)calloc(BUFFER_SIZE,sizeof(char));
      database_multiple_documents_fields.value[count][counter] = (char*)calloc(BUFFER_SIZE,sizeof(char));
    }
  }
  database_multiple_documents_fields.document_count = 0;
  database_multiple_documents_fields.database_fields_count = 0;

  if (read_multiple_documents_all_fields_from_collection(DATABASE_NAME,"delegates","",&database_multiple_documents_fields,1,document_count,1,"total_vote_count_number",0,0) == 0)
  {
    GET_STATISTICS_ERROR;
  }

  if (read_document_all_fields_from_collection(DATABASE_NAME,DATABASE_COLLECTION,DATA,&database_data,0) == 0)
  {
    pointer_reset_database_array;
    GET_STATISTICS_ERROR;
  }

  // get the current block height
  if (get_current_block_height(data,0) == 0)
  {
    pointer_reset_database_array;
    GET_STATISTICS_ERROR;
  }
  memcpy(database_data.item[database_data.count],"current_block_height",20);
  memcpy(database_data.value[database_data.count],data,strnlen(data,BUFFER_SIZE));
  database_data.count++;

  // get the xcash proof of stake round number
  sscanf(data,"%zu", &current_block_height);
  count = current_block_height - XCASH_PROOF_OF_STAKE_BLOCK_HEIGHT;  
  memset(data,0,strlen(data));
  sprintf(data,"%zu",count);

  memcpy(database_data.item[database_data.count],"xcash_proof_of_stake_round_number",33);
  memcpy(database_data.value[database_data.count],data,strnlen(data,BUFFER_SIZE));
  database_data.count++;

  // get the total votes
  for (count = 0, count2 = 0; count < document_count; count++)
  {
    for (counter = 0; counter < 18; counter++)
    {
      if (strncmp(database_multiple_documents_fields.item[count][counter],"total_vote_count",BUFFER_SIZE) == 0)
      {
        sscanf(database_multiple_documents_fields.value[count][counter],"%zu", &number);
        count2 += number;
      }
    }
  }

  memset(data,0,strlen(data));
  sprintf(data,"%zu",count2);

  memcpy(database_data.item[database_data.count],"total_votes",11);
  memcpy(database_data.value[database_data.count],data,strnlen(data,BUFFER_SIZE));
  database_data.count++;

  // get the xcash_proof_of_stake_circulating_percentage
  generated_supply = 190734.9 + XCASH_PREMINE_TOTAL_SUPPLY;
  for (counter = 2; counter < current_block_height; counter++)
  { 
    generated_supply += (XCASH_TOTAL_SUPPLY - generated_supply) / 524288;
  }  

  number = ((size_t)((((double)count2) / (generated_supply - (XCASH_PREMINE_TOTAL_SUPPLY - XCASH_PREMINE_CIRCULATING_SUPPLY))) * 100) | 0);

  memset(data,0,strlen(data));
  sprintf(data,"%zu",number);

  memcpy(database_data.item[database_data.count],"xcash_proof_of_stake_circulating_percentage",43);
  memcpy(database_data.value[database_data.count],data,strnlen(data,BUFFER_SIZE));
  database_data.count++;
  
  memset(result,0,strnlen(result,BUFFER_SIZE));

  // create a json string out of the database array of item and value
  if (create_json_data_from_database_document_array(&database_data,result,DATABASE_FIELDS) == 0)
  {
    pointer_reset_database_array;
    GET_STATISTICS_ERROR;
  }
 
  pointer_reset_database_array;  
  pointer_reset(data);  
  return 200;

  #undef DATABASE_COLLECTION
  #undef DATABASE_FIELDS
  #undef DATA
  #undef MAXIMUM_DELEGATES_COUNT
  #undef GET_STATISTICS_ERROR
  #undef pointer_reset_database_array
}



/*
-----------------------------------------------------------------------------------------------------------
Name: get_delegates
Description: Gets all of the delegates from the database
Parameters:
   result - The json data for the delegates
Return: The http status
-----------------------------------------------------------------------------------------------------------
*/

int get_delegates(char* result)
{
  // Variables
  char* data;
  size_t count = 0;
  size_t counter = 0;
  struct database_multiple_documents_fields database_data;
  int document_count = 0;

  // define macros
  #define DATABASE_COLLECTION "delegates"
  #define DATABASE_FIELDS "public_address|total_vote_count_number|IP_address|about|website|team|server_settings|block_verifier_online_total_rounds|block_producer_total_rounds|VRF_node_public_and_secret_key_total_rounds|VRF_node_random_data_total_rounds|VRF_node_next_main_nodes_total_rounds|block_producer_block_heights|VRF_node_public_and_private_key_block_heights|VRF_node_random_data_block_heights|VRF_node_next_main_nodes_block_heights|"
  #define MAXIMUM_DELEGATES_COUNT 100 // The maximum amount of delegates to read from the database
 
  #define GET_DELEGATES_ERROR \
  memset(result,0,strnlen(result,BUFFER_SIZE)); \
  memcpy(result,"{\"Error\":\"Could not delegate information\"}",42); \
  pointer_reset_database_array; \
  return 400;

  #define pointer_reset_database_array \
  for (count = 0; count < document_count; count++) \
  { \
    for (counter = 0; counter < 18; counter++) \
    { \
      pointer_reset(database_data.item[count][counter]); \
      pointer_reset(database_data.value[count][counter]); \
    } \
  }

  // get how many documents are in the database
  document_count = count_all_documents_in_collection(DATABASE_NAME,DATABASE_COLLECTION,0);
  if (document_count <= 0)
  {
    GET_DELEGATES_ERROR;
  }
  else if (document_count > MAXIMUM_DELEGATES_COUNT)
  {
    document_count = MAXIMUM_DELEGATES_COUNT;
  }
  
  // initialize the database_multiple_documents_fields struct 
  for (count = 0; count < document_count; count++)
  {
    for (counter = 0; counter < 18; counter++)
    {
      database_data.item[count][counter] = (char*)calloc(BUFFER_SIZE,sizeof(char));
      database_data.value[count][counter] = (char*)calloc(BUFFER_SIZE,sizeof(char));
    }
  }
  database_data.document_count = 0;
  database_data.database_fields_count = 0;
  
  if (read_multiple_documents_all_fields_from_collection(DATABASE_NAME,DATABASE_COLLECTION,"",&database_data,1,document_count,1,"total_vote_count_number",0,0) == 0)
  {
    GET_DELEGATES_ERROR;
  }

  memset(result,0,strnlen(result,BUFFER_SIZE)); 
  if (create_json_data_from_database_multiple_documents_array(&database_data,result,DATABASE_FIELDS) == 0)
  {
    GET_DELEGATES_ERROR;
  }

  pointer_reset_database_array;
  return 200;

  #undef DATABASE_COLLECTION
  #undef DATABASE_FIELDS
  #undef MAXIMUM_DELEGATES_COUNT
  #undef GET_DELEGATES_ERROR
  #undef pointer_reset_database_array
}



/*
-----------------------------------------------------------------------------------------------------------
Name: get_delegates_statistics
Description: Gets all of the delegates from the database
Parameters:
   connection - The server connection
   result - The json data for the delegate statistics. Holds the parameter1 at the start
Return: The http status
-----------------------------------------------------------------------------------------------------------
*/

int get_delegates_statistics(struct MHD_Connection* connection, char* result)
{
  // Variables
  char* data; 
  char* message = (char*)calloc(BUFFER_SIZE,sizeof(char));  
  size_t count = 0;
  size_t counter = 0;
  struct database_document_fields database_data;
  struct database_multiple_documents_fields database_multiple_documents_fields;
  int document_count = 0;

  // define macros
  #define DATABASE_COLLECTION "delegates"
  #define DATABASE_FIELDS ""
  #define MAXIMUM_DELEGATES_COUNT 100 // The maximum amount of delegates to read from the database

  #define GET_DELEGATES_STATISTICS_ERROR \
  memset(result,0,strnlen(result,BUFFER_SIZE)); \
  memcpy(result,"{\"Error\":\"Could not delegate statistics\"}",41); \
  pointer_reset(message); \
  return 400;
  
  #define pointer_reset_database_array \
  for (count = 0; count < 19; count++) \
  { \
    pointer_reset(database_data.item[count]); \
    pointer_reset(database_data.value[count]); \
  } \
  for (count = 0; count < document_count; count++) \
  { \
    for (counter = 0; counter < 18; counter++) \
    { \
      pointer_reset(database_multiple_documents_fields.item[count][counter]); \
      pointer_reset(database_multiple_documents_fields.value[count][counter]); \
    } \
  }

  // check if the memory needed was allocated on the heap successfully
  if (message == NULL)
  {
    color_print("Could not allocate the memory needed on the heap","red");
    exit(0);
  }

  // get the parameter1
  if (MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "parameter1") == NULL)
  {
    GET_DELEGATES_STATISTICS_ERROR;
  } 
  data = (char*)MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "parameter1"); 
  decode_html(data); 

  // error check
  if (strncmp(data,"",BUFFER_SIZE) == 0)
  {
    GET_DELEGATES_STATISTICS_ERROR;
  } 

  const size_t DATA_LENGTH = strnlen(data,BUFFER_SIZE);

  // check if the data is a public address or a delegate name
  memcpy(message,"{\"",2);
  if (memcmp(data,XCASH_WALLET_PREFIX,3) == 0 && DATA_LENGTH == XCASH_WALLET_LENGTH)
  {
    memcpy(message+2,"public_address\":\"",17);
    memcpy(message+19,data,DATA_LENGTH);
    memcpy(message+19+DATA_LENGTH,"\"}",2);
  }
  else
  {
    memcpy(message+2,"delegate_name\":\"",16);
    memcpy(message+18,data,DATA_LENGTH);
    memcpy(message+18+DATA_LENGTH,"\"}",2);
  }

  // check if there is any data in the database that matches the message
  if (count_documents_in_collection(DATABASE_NAME,DATABASE_COLLECTION,message,0) <= 0)
  {
    GET_DELEGATES_STATISTICS_ERROR;
  }
  
  // initialize the database_document_fields struct 
  for (count = 0; count < 19; count++)
  {
    database_data.item[count] = (char*)calloc(BUFFER_SIZE,sizeof(char));
    database_data.value[count] = (char*)calloc(BUFFER_SIZE,sizeof(char));
  }
  database_data.count = 0;

  // get how many documents are in the database
  document_count = count_all_documents_in_collection(DATABASE_NAME,DATABASE_COLLECTION,0);
  if (document_count <= 0)
  {
    GET_DELEGATES_STATISTICS_ERROR;
  }
  else if (document_count > MAXIMUM_DELEGATES_COUNT)
  {
    document_count = MAXIMUM_DELEGATES_COUNT;
  }

  // initialize the database_multiple_documents_fields struct 
  for (count = 0; count < document_count; count++)
  {
    for (counter = 0; counter < 18; counter++)
    {
      database_multiple_documents_fields.item[count][counter] = (char*)calloc(BUFFER_SIZE,sizeof(char));
      database_multiple_documents_fields.value[count][counter] = (char*)calloc(BUFFER_SIZE,sizeof(char));
    }
  }
  database_multiple_documents_fields.document_count = 0;
  database_multiple_documents_fields.database_fields_count = 0;

  if (read_document_all_fields_from_collection(DATABASE_NAME,DATABASE_COLLECTION,message,&database_data,0) == 0)
  {
    pointer_reset_database_array;
    GET_DELEGATES_STATISTICS_ERROR;
  }

  if (read_multiple_documents_all_fields_from_collection(DATABASE_NAME,DATABASE_COLLECTION,"",&database_multiple_documents_fields,1,document_count,1,"total_vote_count_number",0,0) == 0)
  {
    GET_DELEGATES_STATISTICS_ERROR;
  }

  // get the delegates rank
  for (count = 0, counter = 0; count < database_multiple_documents_fields.document_count; count++)
  {
    if (strncmp(database_multiple_documents_fields.item[count][0],data,XCASH_WALLET_LENGTH) == 0 || strncmp(database_multiple_documents_fields.item[count][4],data,BUFFER_SIZE) == 0)
    {
      break;
    }
    counter++;
  }

  memset(data,0,strlen(data));
  sprintf(data, "%zu", counter);
  memcpy(database_data.item[18],"current_delegate_rank",21);
  memcpy(database_data.value[18],data,strnlen(data,BUFFER_SIZE));
  database_data.count++;

  memset(result,0,strnlen(result,BUFFER_SIZE)); 

  // create a json string out of the database array of item and value
  if (create_json_data_from_database_document_array(&database_data,result,DATABASE_FIELDS) == 0)
  {
    pointer_reset_database_array;
    GET_DELEGATES_STATISTICS_ERROR;
  }
  
  pointer_reset_database_array;
  pointer_reset(message);
  return 200;

  #undef DATABASE_COLLECTION
  #undef DATABASE_FIELDS
  #undef MAXIMUM_DELEGATES_COUNT
  #undef GET_DELEGATES_STATISTICS_ERROR
  #undef pointer_reset_database_array
}



/*
-----------------------------------------------------------------------------------------------------------
Name: get_delegates_information
Description: Gets the delegates information from the database
Parameters:
   connection - The server connection
   result - The json data for the delegates information. Holds the parameter1 at the start
Return: The http status
-----------------------------------------------------------------------------------------------------------
*/

int get_delegates_information(struct MHD_Connection* connection, char* result)
{
  // Variables
  char* data; 
  char* message = (char*)calloc(BUFFER_SIZE,sizeof(char));  
  int count = 0;
  struct database_document_fields database_data;

  // define macros
  #define DATABASE_COLLECTION "delegates"
  #define DATABASE_FIELDS "total_vote_count|total_vote_count_number|IP_address|delegate_name|block_verifier_score|online_status|block_verifier_total_rounds|block_verifier_online_total_rounds|block_verifier_online_percentage|block_producer_total_rounds|VRF_node_public_and_secret_key_total_rounds|VRF_node_random_data_total_rounds|VRF_node_next_main_nodes_total_rounds|block_producer_block_heights|VRF_node_public_and_secret_key_block_heights|VRF_node_random_data_block_heights|VRF_node_next_main_nodes_block_heights|"

  #define GET_DELEGATES_INFORMATION_ERROR \
  memset(result,0,strnlen(result,BUFFER_SIZE)); \
  memcpy(result,"{\"Error\":\"Could not delegate information\"}",42); \
  pointer_reset(message); \
  return 400;
  
  #define pointer_reset_database_array \
  for (count = 0; count < 18; count++) \
  { \
    pointer_reset(database_data.item[count]); \
    pointer_reset(database_data.value[count]); \
  }

  // check if the memory needed was allocated on the heap successfully
  if (message == NULL)
  {
    color_print("Could not allocate the memory needed on the heap","red");
    exit(0);
  }

  // get the parameter1
  if (MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "parameter1") == NULL)
  {
    GET_DELEGATES_INFORMATION_ERROR;
  } 
  data = (char*)MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "parameter1"); 
  decode_html(data);  

  // error check
  if (strncmp(data,"",BUFFER_SIZE) == 0)
  {
    GET_DELEGATES_INFORMATION_ERROR;
  } 

  const size_t DATA_LENGTH = strnlen(data,BUFFER_SIZE);

  // check if the data is a public address or a delegate name
  memcpy(message,"{\"",2);
  if (memcmp(data,XCASH_WALLET_PREFIX,3) == 0 && DATA_LENGTH == XCASH_WALLET_LENGTH)
  {
    memcpy(message+2,"public_address\":\"",17);
    memcpy(message+19,data,DATA_LENGTH);
    memcpy(message+19+DATA_LENGTH,"\"}",2);
  }
  else
  {
    memcpy(message+2,"delegate_name\":\"",16);
    memcpy(message+18,data,DATA_LENGTH);
    memcpy(message+18+DATA_LENGTH,"\"}",2);
  }

  // check if there is any data in the database that matches the message
  if (count_documents_in_collection(DATABASE_NAME,DATABASE_COLLECTION,message,0) <= 0)
  {
    GET_DELEGATES_INFORMATION_ERROR;
  }
  
  // initialize the database_document_fields struct 
  for (count = 0; count < 18; count++)
  {
    database_data.item[count] = (char*)calloc(BUFFER_SIZE,sizeof(char));
    database_data.value[count] = (char*)calloc(BUFFER_SIZE,sizeof(char));
  }
  database_data.count = 0;

  if (read_document_all_fields_from_collection(DATABASE_NAME,DATABASE_COLLECTION,message,&database_data,0) == 0)
  {
    pointer_reset_database_array;
    GET_DELEGATES_INFORMATION_ERROR;
  }

  memset(result,0,strnlen(result,BUFFER_SIZE));

  // create a json string out of the database array of item and value
  if (create_json_data_from_database_document_array(&database_data,result,DATABASE_FIELDS) == 0)
  {
    pointer_reset_database_array;
    GET_DELEGATES_INFORMATION_ERROR;
  }
  
  pointer_reset_database_array;
  pointer_reset(message);
  return 200;

  #undef DATABASE_COLLECTION
  #undef DATABASE_FIELDS
  #undef GET_DELEGATES_INFORMATION_ERROR
  #undef pointer_reset_database_array
}



/*
-----------------------------------------------------------------------------------------------------------
Name: get_delegates_voters_list
Description: Gets the delegates voters list from the database
Parameters:
   connection - The server connection
   result - The json data for the delegate voters list. Holds the parameter1 at the start
Return: The http status
-----------------------------------------------------------------------------------------------------------
*/

int get_delegates_voters_list(struct MHD_Connection* connection, char* result)
{
  // Variables
  char* data;
  char* data2 = (char*)calloc(BUFFER_SIZE,sizeof(char));  
  char* data3 = (char*)calloc(BUFFER_SIZE,sizeof(char));  
  char* message = (char*)calloc(BUFFER_SIZE,sizeof(char));  
  int count = 0;
  int counter = 0;
  struct database_multiple_documents_fields database_data;
  int document_count = 0;

  // define macros
  #define DATABASE_COLLECTION "public_addresses_payments"

  // define macros
  #define pointer_reset_all \
  free(data2); \
  data2 = NULL; \
  free(data3); \
  data3 = NULL; \
  free(message); \
  message = NULL;

  #define GET_DELEGATES_VOTERS_LIST_ERROR \
  memset(result,0,strnlen(result,BUFFER_SIZE)); \
  memcpy(result,"{\"Error\":\"Could not get voters list data\"}",42); \
  pointer_reset_all; \
  return 400;
  
  #define pointer_reset_database_array \
  for (count = 0; count < document_count; count++) \
  { \
    for (counter = 0; counter < 4; counter++) \
    { \
      pointer_reset(database_data.item[count][counter]); \
      pointer_reset(database_data.value[count][counter]); \
    } \
  }

  // check if the memory needed was allocated on the heap successfully
  if (data2 == NULL || data3 == NULL || message == NULL)
  {
    if (data2 != NULL)
    {
      pointer_reset(data2);
    }
    if (data3 != NULL)
    {
      pointer_reset(data3);
    }
    if (message != NULL)
    {
      pointer_reset(message);
    }
    color_print("Could not allocate the memory needed on the heap","red");
    exit(0);
  }

  // get the parameter1
  if (MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "parameter1") == NULL)
  {
    GET_DELEGATES_VOTERS_LIST_ERROR;
  } 
  data = (char*)MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "parameter1"); 
  decode_html(data);  

  // error check
  if (strncmp(data,"",BUFFER_SIZE) == 0)
  {
    GET_DELEGATES_VOTERS_LIST_ERROR;
  } 
  const size_t DATA_LENGTH = strnlen(data,BUFFER_SIZE);

  // get the delegates public address
  memcpy(message,"{\"",2);
  if (memcmp(data,XCASH_WALLET_PREFIX,3) != 0 || DATA_LENGTH != XCASH_WALLET_LENGTH)
  {
    memcpy(message+2,"delegate_name\":\"",16);
    memcpy(message+18,data,DATA_LENGTH);
    memcpy(message+18+DATA_LENGTH,"\"}",2);

    if (read_document_field_from_collection(DATABASE_NAME,"delegates",message,"public_address",data2,0) == 0)
    {
      GET_DELEGATES_VOTERS_LIST_ERROR;
    }
  }
  else
  {
    memcpy(data2,data,XCASH_WALLET_LENGTH);
  }  

  // create the message
  memset(message,0,strlen(message));
  memcpy(message,"{\"public_address_voted_for\":\"",29);
  memcpy(message+29,data2,XCASH_WALLET_LENGTH);
  memcpy(message+127,"\"}",2);

  // check how many reserve proofs are for the public address
  for (count = 1; count <= 50; count++)
  { 
    memset(data3,0,strlen(data3));
    memcpy(data3,"reserve_proofs_",15);
    sprintf(data3+15,"%d",count);

    counter = count_documents_in_collection(DATABASE_NAME,data3,message,0);
    if (counter < 0)
    {
      GET_DELEGATES_VOTERS_LIST_ERROR;
    }
    document_count += counter;
  }
  if (document_count <= 0)
  {
    GET_DELEGATES_VOTERS_LIST_ERROR;
  }

  // initialize the database_multiple_documents_fields struct
  for (count = 0; count < document_count; count++)
  {
    for (counter = 0; counter < 4; counter++)
    {
      database_data.item[count][counter] = (char*)calloc(RESERVE_PROOFS_LENGTH,sizeof(char));
      database_data.value[count][counter] = (char*)calloc(RESERVE_PROOFS_LENGTH,sizeof(char));
    }
  }
  database_data.document_count = 0;
  database_data.database_fields_count = 0;

  // get all of the reserve proofs for the public address
  memset(data3,0,strlen(data3));
  for (count = 1; count <= 50; count++)
  { 
    memset(data2,0,strlen(data2));
    memcpy(data2,"reserve_proofs_",15);
    sprintf(data2+15,"%d",count);

    counter = count_documents_in_collection(DATABASE_NAME,data2,message,0);
    if (counter > 0)
    {
      if (read_multiple_documents_all_fields_from_collection(DATABASE_NAME,data2,data3,&database_data,1,counter,0,"",1,0) == 0)
      {
        GET_DELEGATES_VOTERS_LIST_ERROR;
      }
    }
  }
  memset(result,0,strnlen(result,BUFFER_SIZE)); 
  if (create_json_data_from_database_multiple_documents_array(&database_data,result,"") == 0)
  {
    GET_DELEGATES_VOTERS_LIST_ERROR;
  }  
  pointer_reset_database_array;
  pointer_reset_all;
  return 200;

  #undef DATABASE_COLLECTION
  #undef pointer_reset_all
  #undef GET_DELEGATES_VOTERS_LIST_ERROR
  #undef pointer_reset_database_array
}



/*
-----------------------------------------------------------------------------------------------------------
Name: get_round_statistics
Description: Gets the round statistics from the database
Parameters:
   result - The json data for the statistics
Return: The http status
-----------------------------------------------------------------------------------------------------------
*/

int get_round_statistics(struct MHD_Connection* connection, char* result)
{
  // Variables
  char* data;
  char* data2 = (char*)calloc(BUFFER_SIZE,sizeof(char));  
  char* data3 = (char*)calloc(BUFFER_SIZE,sizeof(char));   
  size_t count = 0;
  size_t count2 = 0;
  size_t counter = 0;
  size_t number = 0;
  size_t current_block_height = 0;
  double generated_supply;
  int document_count = 0;

  // define macros
  #define pointer_reset_all \
  free(data2); \
  data2 = NULL; \
  free(data3); \
  data3 = NULL;

  #define GET_ROUND_STATISTICS_ERROR \
  memset(result,0,strnlen(result,BUFFER_SIZE)); \
  memcpy(result,"{\"Error\":\"Could not get round statistics\"}",42); \
  pointer_reset_all; \
  return 400;
  
  // check if the memory needed was allocated on the heap successfully
  if (data2 == NULL || data3 == NULL)
  {
    if (data2 != NULL)
    {
      pointer_reset(data2);
    }
    if (data3 != NULL)
    {
      pointer_reset(data3);
    }
    color_print("Could not allocate the memory needed on the heap","red");
    exit(0);
  }

  // reset the database_document_fields struct 
  for (count = 0; count < 36; count++)
  {
    memset(database_document_fields.item[count],0,strlen(database_document_fields.item[count]));
    memset(database_document_fields.value[count],0,strlen(database_document_fields.value[count]));
  }
  database_document_fields.count = 0;

  // get the parameter1
  if (MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "parameter1") == NULL)
  {
    GET_ROUND_STATISTICS_ERROR;
  } 
  data = (char*)MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "parameter1"); 
  decode_html(data);  

  // error check
  if (strncmp(data,"",BUFFER_SIZE) == 0)
  {
    GET_ROUND_STATISTICS_ERROR;
  } 
  const size_t DATA_LENGTH = strnlen(data,BUFFER_SIZE);

  // calculate what reserve_bytes database the reserve bytes data will be in
  sscanf(data,"%zu", &count);
  if (count < XCASH_PROOF_OF_STAKE_BLOCK_HEIGHT)
  {
    GET_ROUND_STATISTICS_ERROR;
  }

  count = (count - XCASH_PROOF_OF_STAKE_BLOCK_HEIGHT) / BLOCKS_PER_DAY_FIVE_MINUTE_BLOCK_TIME;
  memcpy(data2,"reserve_bytes_",14);
  sprintf(data2+14,"%zu",count);

  // create the message
  memcpy(data3,"{\"block_height\":\"",17);
  memcpy(data3+17,data,strnlen(data,BUFFER_SIZE));
  memcpy(data3+strlen(data3),"\"}",2);

  // check if there is any data in the database that matches the message
  if (count_documents_in_collection(DATABASE_NAME,data2,data3,0) <= 0)
  {fprintf(stderr,"3");
    GET_ROUND_STATISTICS_ERROR;
  }

  if (read_document_all_fields_from_collection(DATABASE_NAME,data2,data3,&database_document_fields,0) == 0)
  {fprintf(stderr,"4");
    GET_ROUND_STATISTICS_ERROR;
  }
  
  memset(result,0,strnlen(result,BUFFER_SIZE));
  memset(data2,0,strlen(data2));
  memset(data3,0,strlen(data3));
  memcpy(data2,data,strnlen(data,BUFFER_SIZE));

  // get the previous block reserve bytes
  sscanf(data,"%zu", &count);
  count--;
  count = (count - XCASH_PROOF_OF_STAKE_BLOCK_HEIGHT) / BLOCKS_PER_DAY_FIVE_MINUTE_BLOCK_TIME;
  memcpy(data2,"reserve_bytes_",14);
  sprintf(data2+14,"%zu",count);

  // create the message
  memcpy(data3,"{\"block_height\":\"",17);
  memcpy(data3+17,data,strnlen(data,BUFFER_SIZE));
  memcpy(data3+strlen(data3),"\"}",2);

  // check if there is any data in the database that matches the message
  if (count_documents_in_collection(DATABASE_NAME,data2,data3,0) <= 0)
  {fprintf(stderr,"5");
    GET_ROUND_STATISTICS_ERROR;
  }

  memset(result,0,strlen(result));
  if (read_document_field_from_collection(DATABASE_NAME,data2,data3,"reserve_bytes",result,0) == 0)
  {fprintf(stderr,"6");
    GET_ROUND_STATISTICS_ERROR;
  }

  memset(data2,0,strlen(data2));
  memset(data3,0,strlen(data3));
  memcpy(data2,data,strnlen(data,BUFFER_SIZE));
  memcpy(data3,result,strnlen(result,BUFFER_SIZE));
  memset(result,0,strlen(result));

  // verify the block
  for (count = 0; count < 36; count++)
  {
    if (strncmp(database_document_fields.item[count],"reserve_bytes",BUFFER_SIZE) == 0)
    {
      if (network_block_string_to_blockchain_data(database_document_fields.value[count],data2) == 0)
      {
        GET_ROUND_STATISTICS_ERROR;
      }
    }
  }
  if (verify_network_block_data(0,1,0,1,data2,data3) == 0)
  {
    GET_ROUND_STATISTICS_ERROR;
  }

  // create a json string out of the database array of item and value
  if (create_json_data_from_database_document_array(&database_document_fields,result,"") == 0)
  {
    GET_ROUND_STATISTICS_ERROR;
  }
  pointer_reset_all; 
  return 200;

  #undef pointer_reset_all
  #undef GET_ROUND_STATISTICS_ERROR
}



/*
-----------------------------------------------------------------------------------------------------------
Name: get_delegates_nodes_list
Description: Gets the delegates nodes list
Parameters:
   result - The json data for the delegates
Return: The http status
-----------------------------------------------------------------------------------------------------------
*/

int get_delegates_nodes_list(char* result)
{
  // Variables
  char* data;
  size_t count = 0;
  size_t counter = 0;
  struct database_multiple_documents_fields database_data;
  int document_count = 0;

  // define macros
  #define DATABASE_COLLECTION "delegates"
  #define MAXIMUM_DELEGATES_COUNT 100 // The maximum amount of delegates to read from the database
 
  #define GET_DELEGATES_NODES_LIST_ERROR \
  memset(result,0,strnlen(result,BUFFER_SIZE)); \
  memcpy(result,"{\"Error\":\"Could not delegate information\"}",42); \
  pointer_reset_database_array; \
  return 400;

  #define pointer_reset_database_array \
  for (count = 0; count < MAXIMUM_DELEGATES_COUNT; count++) \
  { \
    for (counter = 0; counter < 18; counter++) \
    { \
      pointer_reset(database_data.item[count][counter]); \
      pointer_reset(database_data.value[count][counter]); \
    } \
  }

  // initialize the database_multiple_documents_fields struct 
  for (count = 0; count < MAXIMUM_DELEGATES_COUNT; count++)
  {
    for (counter = 0; counter < 18; counter++)
    {
      database_data.item[count][counter] = (char*)calloc(BUFFER_SIZE,sizeof(char));
      database_data.value[count][counter] = (char*)calloc(BUFFER_SIZE,sizeof(char));
    }
  }
  database_data.document_count = 0;
  database_data.database_fields_count = 0;
  
  if (read_multiple_documents_all_fields_from_collection(DATABASE_NAME,DATABASE_COLLECTION,"",&database_data,1,MAXIMUM_DELEGATES_COUNT,1,"total_vote_count_number",0,0) == 0)
  {
    GET_DELEGATES_NODES_LIST_ERROR;
  }

  memset(result,0,strnlen(result,BUFFER_SIZE)); 
  for (count = 0; count < MAXIMUM_DELEGATES_COUNT; count++)
  {
    for (counter = 0; counter < 18; counter++)
    {
      if (memcmp(database_data.item[count][counter],"IP_address",10) == 0)
      {
        memcpy(result+strlen(result),database_data.value[count][counter],strnlen(database_data.value[count][counter],BUFFER_SIZE));
        memcpy(result+strlen(result),"|",1);
      }  
    }    
  }

  pointer_reset_database_array;
  return 200;

  #undef DATABASE_COLLECTION
  #undef MAXIMUM_DELEGATES_COUNT
  #undef GET_DELEGATES_NODES_LIST_ERROR
  #undef pointer_reset_database_array
}



/*
-----------------------------------------------------------------------------------------------------------
Name: create_server
Description: Creates the server
Parameters:
   cls - Callback value
   connection -Server connection
   url - The url
   method - The HTTP method
   version - The HTPP version
   upload_data - The HTTP data
   upload_data_size - The size of the HTTP Data
   settings - Settings
Return: 0 if an error has occured, 1 if successfull
-----------------------------------------------------------------------------------------------------------
*/

int create_server(void* cls,struct MHD_Connection* connection,const char* url,const char* method,const char* version,const char* upload_data,size_t* upload_data_size,void** settings)
{
  // Constants
  const char* errorstr = "<html><body>An internal server error has occured!</body></html>";

  // Variables 
  char* data = (char*)calloc(BUFFER_SIZE,sizeof(char));  
  int server;
  int http_status;
  struct MHD_Response* response;
  size_t count = 0;
  FILE* file;

  #define pointer_reset_all \
  free(data); \
  data = NULL; 
  
  #define HTTP_RESULTS(http_data,content_type,http_status) \
  response = MHD_create_response_from_buffer(strnlen(http_data,BUFFER_SIZE),(void *)http_data,MHD_RESPMEM_MUST_FREE); \
  MHD_add_response_header(response, "Access-Control-Allow-Origin", "*"); \
  MHD_add_response_header(response, "Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept"); \
  MHD_add_response_header(response, "Content-Type", content_type); \
  MHD_queue_response(connection, http_status, response); \
  MHD_destroy_response(response); \
  return MHD_YES;

  if (data == NULL)
  {
    return 0;
  }

  // error check
  if (strncmp(method, MHD_HTTP_METHOD_GET,BUFFER_SIZE) != 0 && strncmp(method, MHD_HTTP_METHOD_POST,BUFFER_SIZE) != 0)
  {
    pointer_reset_all;
    return MHD_NO;
  }

   if (*settings == NULL)
   {
     struct connection_info_struct* connection_info = malloc(sizeof(struct connection_info_struct));
     connection_info->count = 0;

     if (strncmp(method,MHD_HTTP_METHOD_POST,BUFFER_SIZE) == 0)
     {
       connection_info->post_result_settings = MHD_create_post_processor(connection, READ_FILE_BUFFER_SIZE,post_result,(void*)connection_info);
       if (connection_info->post_result_settings == NULL)
       {
         pointer_reset(connection_info);
         pointer_reset_all;
         return MHD_NO;
       }
       else
       {
         connection_info->connection_type = 1;
         *settings = (void *) connection_info;
         pointer_reset_all;
         return MHD_YES;
       }       
      }
   }

  if (strncmp(method,MHD_HTTP_METHOD_POST,BUFFER_SIZE) == 0)
   {
     struct connection_info_struct* connection_info = *settings;     

     if (*upload_data_size != 0)
     {
       MHD_post_process(connection_info->post_result_settings, upload_data,*upload_data_size);
       *upload_data_size = 0;
       pointer_reset_all;
       return MHD_YES;
     }
   }


   if (strncmp(method,MHD_HTTP_METHOD_GET,BUFFER_SIZE) == 0)
   {
     if (strncmp(url,"/getstatistics",BUFFER_SIZE) == 0)
     {
       memset(data,0,strnlen(data,BUFFER_SIZE));
       http_status = get_statistics(data);
       HTTP_RESULTS(data,"application/json",http_status);
     }
     else if (strncmp(url,"/getdelegates",BUFFER_SIZE) == 0)
     {
       memset(data,0,strnlen(data,BUFFER_SIZE));
       http_status = get_delegates(data);
       HTTP_RESULTS(data,"application/json",http_status);
     }
     else if (strncmp(url,"/getdelegatesstatistics",BUFFER_SIZE) == 0)
     { 
       memset(data,0,strnlen(data,BUFFER_SIZE));  
       http_status = get_delegates_statistics(connection,data);
       HTTP_RESULTS(data,"application/json",http_status);
     }  
     else if (strncmp(url,"/getdelegatesinformation",BUFFER_SIZE) == 0)
     { 
       memset(data,0,strnlen(data,BUFFER_SIZE));  
       http_status = get_delegates_information(connection,data);
       HTTP_RESULTS(data,"application/json",http_status);
     }  
     else if (strncmp(url,"/getdelegatesvoterslist",BUFFER_SIZE) == 0)
     { 
       memset(data,0,strnlen(data,BUFFER_SIZE));  
       http_status = get_delegates_voters_list(connection,data);
       HTTP_RESULTS(data,"application/json",http_status);
     }  
     else if (strncmp(url,"/getroundstatistics",BUFFER_SIZE) == 0)
     { 
       memset(data,0,strnlen(data,BUFFER_SIZE));  
       http_status = get_round_statistics(connection,data);
       HTTP_RESULTS(data,"application/json",http_status);
     }  
     else if (strncmp(url,"/getdelegatesnodeslist",BUFFER_SIZE) == 0)
     { 
       memset(data,0,strnlen(data,BUFFER_SIZE));  
       http_status = get_delegates_nodes_list(data);
       HTTP_RESULTS(data,"application/json",http_status);
     }   
     else
     {
       // check if the file exist
       file = fopen(&url[1],"rb");
       if (file == NULL)
       {
         // open the index.html page
         file = fopen("index.html","rb");
       } 
       // get the file size
       fseek(file, 0, SEEK_END);
       const long file_size = ftell(file);
       fseek(file, 0, SEEK_SET); 

       response = MHD_create_response_from_callback(file_size, READ_FILE_BUFFER_SIZE,&read_file_server,file,&callback_reset);
       if (response == NULL)
       {
  	     fclose(file);
         pointer_reset_all;
     	   return MHD_NO;
     	 }
       // set the content type
       if (strstr(&url[1],".html") != NULL)
       {
         MHD_add_response_header(response, "Content-Type", "text/html");
       }
       if (strstr(&url[1],".js") != NULL)
       {
         MHD_add_response_header(response, "Content-Type", "application/javascript");
       }
       if (strstr(&url[1],".css") != NULL)
       {
         MHD_add_response_header(response, "Content-Type", "text/css");
       }
       if (strstr(&url[1],".png") != NULL)
       {
         MHD_add_response_header(response, "Content-Type", "image/png");
       }
       if (strstr(&url[1],".jpg") != NULL || strstr(&url[1],".jpeg") != NULL)
       {
         MHD_add_response_header(response, "Content-Type", "image/jpeg");
       }
       MHD_queue_response(connection, MHD_HTTP_OK, response);
       MHD_destroy_response(response);
       pointer_reset_all;
       return MHD_YES;
     }  
   }

  #undef pointer_reset_all   
  #undef HTTP_RESULTS
}