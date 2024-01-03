#ifndef SOCKETS_SERVER_ACTIVE_SOCKET_H
#define SOCKETS_SERVER_ACTIVE_SOCKET_H

#include <pthread.h>
#include "char_buffer.h"
#include "linked_list.h"

GENERATE_LINKED_LIST(struct char_buffer, char_buffer);

typedef struct active_socket {
    int socket_descriptor;
    const char* end_message;
    _Bool is_reading;
    pthread_mutex_t mutex_reading;
    pthread_mutex_t mutex_writing;
    LINKED_LIST_char_buffer received_data;
    pthread_mutex_t mutex_received_data;
} ACTIVE_SOCKET;

void active_socket_init(struct active_socket* self);
void active_socket_destroy(struct active_socket* self);

// read = blokujuca operacia = program stoji dovtedy, kym nepridu nejake data
// aktivny socket nacitava data do svojej pamati (pamat tu realizovana cez linked list)
    // do linked listu sa pocas citania postupne nacitavaju data
    // naplna strukturu vsetkymi datami (ked uz prisli kompletne), ktore prisli a su odclenene (na zaklade ukoncovacich znakov)
void active_socket_start_reading(struct active_socket* self);

// na zastavenie cakania na read
void active_socket_stop_reading(struct active_socket* self);

// na otestovanie, ci momentalne som v stave citania dat
_Bool active_socket_is_reading(struct active_socket* self);

// na precitanie dat, ktore som obdrzala
// neblokujuca = ak su nejake data k dispozicii, vrati mi ich ako char buffer output
// napr v jednom vlakne prebieha citanie a v inom vlakne spracujem precitane data
_Bool active_socket_try_get_read_data(struct active_socket* self, struct char_buffer* output);
_Bool active_socket_is_end_message(struct active_socket* self, struct char_buffer* message);

void active_socket_write_data(struct active_socket* self, struct char_buffer* message);
void active_socket_write_end_message(struct active_socket* self);

#endif //SOCKETS_SERVER_ACTIVE_SOCKET_H
