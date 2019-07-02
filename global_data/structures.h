#ifndef STRUCTURES_H_   /* Include guard */
#define STRUCTURES_H_

/*
-----------------------------------------------------------------------------------------------------------
Global Structures
-----------------------------------------------------------------------------------------------------------
*/

// server
struct MHD_Daemon* server; 

// post request data
struct connection_info_struct {
    int connection_type;
    size_t count;
    char* item[100];
    char* value[100];
    struct MHD_PostProcessor *post_result_settings;
};

// network_data_nodes_list
  struct network_data_nodes_list {
    char* network_data_nodes_public_address[NETWORK_DATA_NODES_AMOUNT]; // The block verifiers public address
    char* network_data_nodes_IP_address[NETWORK_DATA_NODES_AMOUNT]; // The block verifiers IP address
};

struct blockchain_reserve_bytes {
    char* block_producer_delegates_name_data; // The block_producer_delegates_name
    char* block_producer_delegates_name; // The block_producer_delegates_name_data text
    size_t block_producer_delegates_name_data_length; // The amount of characters of the block_producer_delegates_name_data
    char* block_producer_public_address_data; // The block_producer_public_address
    char* block_producer_public_address; // The block_producer_public_address_data text
    size_t block_producer_public_address_data_length; // The amount of characters of the block_producer_public_address_data
    char* block_producer_node_backup_count_data; // The block_producer_node_backup_count
    char* block_producer_node_backup_count; // The block_producer_node_backup_count_data text
    size_t block_producer_node_backup_count_data_length; // The amount of characters of the block_producer_node_backup_count_data
    char* block_producer_backup_nodes_names_data; // The block_producer_backup_nodes_names
    char* block_producer_backup_nodes_names; // The block_producer_backup_nodes_names_data text
    size_t block_producer_backup_nodes_names_data_length; // The amount of characters of the block_producer_backup_nodes_names_data 
    char* vrf_secret_key_data_round_part_4; // The VRF secret key text for round part 3
    unsigned char* vrf_secret_key_round_part_4; // The VRF secret key for round part 3
    size_t vrf_secret_key_data_length_round_part_4; // The length of the VRF public key for round part 3   
    char* vrf_public_key_data_round_part_4; // The VRF public key text for round part 3
    unsigned char* vrf_public_key_round_part_4; // The VRF public key for round part 3
    size_t vrf_public_key_data_length_round_part_4; // The length of the VRF public key for round part 3
    char* vrf_alpha_string_data_round_part_4; // The VRF alpha string (input string) text for round part 3
    unsigned char* vrf_alpha_string_round_part_4; // The VRF alpha string (input string) for round part 3
    size_t vrf_alpha_string_data_length_round_part_4; // The length of the VRF alpha string (input string) for round part 3
    char* vrf_proof_data_round_part_4; // The VRF proof text for round part 3
    unsigned char* vrf_proof_round_part_4; // The VRF proof for round part 3
    size_t vrf_proof_data_length_round_part_4; // The length of the VRF proof for round part 3
    char* vrf_beta_string_data_round_part_4; // The VRF beta string (output string) text for round part 3
    unsigned char* vrf_beta_string_round_part_4; // The VRF beta string (output string) for round part 3
    size_t vrf_beta_string_data_length_round_part_4; // The length of the VRF beta string (output string) for round part 3
    char* vrf_data_round_part_4; // The VRF data for round part 3  
    char* vrf_data; // The VRF data for all of the round parts
    size_t vrf_data_length; // The length of the VRF data for all of the round parts
    char* block_verifiers_vrf_secret_key_data[BLOCK_VERIFIERS_AMOUNT]; // The VRF secret key text that all of the block verifiers create
    unsigned char* block_verifiers_vrf_secret_key[BLOCK_VERIFIERS_AMOUNT]; // The VRF secret key that all of the block verifiers create
    size_t block_verifiers_vrf_secret_key_data_length; // The amount of characters of the block_verifiers_vrf_secret_key_data
    char* block_verifiers_vrf_public_key_data[BLOCK_VERIFIERS_AMOUNT]; // The VRF public key text that all of the block verifiers create
    unsigned char* block_verifiers_vrf_public_key[BLOCK_VERIFIERS_AMOUNT]; // The VRF public key that all of the block verifiers create
    size_t block_verifiers_vrf_public_key_data_length; // The amount of characters of the block_verifiers_vrf_public_key_data
    char* block_verifiers_random_data[BLOCK_VERIFIERS_AMOUNT]; // The random data that all of the block verifiers create
    char* block_verifiers_random_data_text[BLOCK_VERIFIERS_AMOUNT]; // The random data text that all of the block verifiers create
    size_t block_verifiers_random_data_length; // The amount of characters of the block_verifiers_random_data
    char* next_block_verifiers_public_address_data[BLOCK_VERIFIERS_AMOUNT]; // The next_block_verifiers_public_address
    char* next_block_verifiers_public_address[BLOCK_VERIFIERS_AMOUNT]; // The next_block_verifiers_public_address_data text
    size_t next_block_verifiers_public_address_data_length; // The amount of characters of the next_block_verifiers_public_address_data
    char* previous_block_hash_data; // The previous_block_hash
    size_t previous_block_hash_data_length; // The amount of characters of the previous_block_hash_data
    char* block_validation_node_signature_data[BLOCK_VERIFIERS_AMOUNT]; // The block_validation_node_signature
    char* block_validation_node_signature[BLOCK_VERIFIERS_AMOUNT]; // The block_validation_node_signature_data text
    size_t block_validation_node_signature_data_length; // The amount of characters of the block_validation_node_signature_data
};

struct blockchain_data {
    char* network_version_data; // The network_version
    size_t network_version_data_length; // The amount of characters of the network_version_data
    char* timestamp_data; // The timestamp
    size_t timestamp; // Variant decoded timestamp
    size_t timestamp_data_length; // The amount of characters of the timestamp_data
    char* previous_block_hash_data; // The previous_block_hash
    size_t previous_block_hash_data_length; // The amount of characters of the previous_block_hash_data
    char* nonce_data; // The nonce
    size_t nonce_data_length; // The amount of characters of the nonce_data
    char* block_reward_transaction_version_data; // The block_reward_transaction_version
    size_t block_reward_transaction_version_data_length; // The amount of characters of the block_reward_transaction_version_data
    char* unlock_block_data; // The unlock_block
    size_t unlock_block; // Variant decoded unlock_block
    size_t unlock_block_data_length; // The amount of characters of the unlock_block_data
    char* block_reward_input_data; // The block_reward_input
    size_t block_reward_input_data_length; // The amount of characters of the block_reward_input_data
    char* vin_type_data; // The vin_type
    size_t vin_type_data_length; // The amount of characters of the vin_type_data
    char* block_height_data; // The block_height
    size_t block_height; // Variant decoded block_height
    size_t block_height_data_length; // The amount of characters of the block_height_data
    char* block_reward_output_data; // The block_reward_output
    size_t block_reward_output_data_length; // The amount of characters of the block_reward_output_data
    char* block_reward_data; // The block_reward
    size_t block_reward; // Variant decoded block_reward
    size_t block_reward_data_length; // The amount of characters of the block_reward_data
    char* stealth_address_output_tag_data; // The stealth_address_output_tag
    size_t stealth_address_output_tag_data_length; // The amount of characters of the stealth_address_output_tag_data
    char* stealth_address_output_data; // The stealth_address_output
    size_t stealth_address_output_data_length; // The amount of characters of the stealth_address_output_data
    char* extra_bytes_size_data; // The extra_bytes_size
    size_t extra_bytes_size; // Variant decoded extra_bytes_size
    size_t extra_bytes_size_data_length; // The amount of characters of the extra_bytes_size
    char* transaction_public_key_tag_data; // The transaction_public_key_tag
    size_t transaction_public_key_tag_data_length; // The amount of characters of the transaction_public_key_tag
    char* transaction_public_key_data; // The transaction_public_key
    size_t transaction_public_key_data_length; // The amount of characters of the transaction_public_key
    char* extra_nonce_tag_data; // The extra_nonce_tag
    size_t extra_nonce_tag_data_length; // The amount of characters of the extra_nonce_tag
    char* reserve_bytes_size_data; // The reserve_bytes_size
    size_t reserve_bytes_size; // Variant decoded reserve_bytes_size
    size_t reserve_bytes_size_data_length; // The amount of characters of the reserve_bytes_size
    struct blockchain_reserve_bytes blockchain_reserve_bytes; // A blockchain_reserve_bytes struct that holds all of the reserve bytes
    char* ringct_version_data; // The ringct_version
    size_t ringct_version_data_length; // The amount of characters of the ringct_version
    char* transaction_amount_data; // The transaction_amount
    size_t transaction_amount; // Variant decoded transaction_amount
    size_t transaction_amount_data_length; // The amount of characters of the transaction_amount
    char* transactions[MAXIMUM_TRANSACATIONS_PER_BLOCK]; // All of the transactions in the block.
};

// database struct
struct database_document_fields {
    size_t count;
    char* item[DATABASE_ARRAY_COUNT];
    char* value[DATABASE_ARRAY_COUNT];
};

struct database_multiple_documents_fields {
    size_t document_count;
    size_t database_fields_count;
    char* item[DATABASE_ARRAY_COUNT][DATABASE_ARRAY_COUNT];
    char* value[DATABASE_ARRAY_COUNT][DATABASE_ARRAY_COUNT];
};
#endif