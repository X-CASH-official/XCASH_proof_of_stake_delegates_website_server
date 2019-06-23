#ifndef NETWORK_WALLET_FUNCTIONS_H_   /* Include guard */
#define NETWORK_WALLET_FUNCTIONS_H_

/*
-----------------------------------------------------------------------------------------------------------
Function prototypes
-----------------------------------------------------------------------------------------------------------
*/

int get_public_address(const int HTTP_SETTINGS);
int data_verify(const int MESSAGE_SETTINGS, const char* PUBLIC_ADDRESS, const char* DATA_SIGNATURE, const char* DATA);
int send_payment(const char* PUBLIC_ADDRESS, const char* TOTAL, char *tx_hash, char *tx_key, const int HTTP_SETTINGS);
#endif