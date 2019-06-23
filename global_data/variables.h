#ifndef VARIABLES_H_   /* Include guard */
#define VARIABLES_H_

/*
-----------------------------------------------------------------------------------------------------------
Global Variables
-----------------------------------------------------------------------------------------------------------
*/

// database
mongoc_client_t* database_client;
mongoc_client_pool_t* database_client_thread_pool;

char* xcash_wallet_public_address; // Holds your wallets public address
struct network_data_nodes_list network_data_nodes_list; // The network data nodes
struct blockchain_data blockchain_data; // The data for a new block to be added to the network.
struct database_document_fields database_document_fields;
#endif