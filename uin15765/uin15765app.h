#include <stdint.h>
#include <stdio.h>
#include "uin15765.h"

void i15765app_process(i15765_t *msg);							//Processes a received 15765 messages.
void i15765app_tx_completed_process(i15765_t *msg,uint8_t *status);				//Processes tp completed.

