#include "wrp_interface.h"

size_t create_response_to_message(char *data, size_t cnt)
{
    wrp_msg_t *msg;
    size_t response_code = 400;
    
    if (0 < wrp_to_struct(data, cnt, WRP_BYTES, &msg)) {
      if(msg->msg_type == WRP_MSG_TYPE__SVC_REGISTRATION || msg->msg_type == WRP_MSG_TYPE__RETREIVE) {
    //   record_it();
         response_code = 200;
      }
    wrp_free_struct(msg);
    }
    
    return response_code;
}