#include "passive_socket.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define INVALID_SOCKET -1

// PASIVNY -> nacuva a caka na info od klienta
// 1. vytvorim 1 objekt typu pasivny socket
// 2. zavolam jeho konstruktor
// 3. zavolam start listening

void passive_socket_init(struct passive_socket* self) {
    self->socket_descriptor = INVALID_SOCKET;
    self->is_listening = false;
    self->is_waiting = false;
    pthread_mutex_init(&self->mutex, NULL);
    pthread_cond_init(&self->waiting_finished, NULL);
}

void passive_socket_destroy(struct passive_socket* self) {
    if (self->socket_descriptor >= 0) {
        close(self->socket_descriptor);
    }
    self->socket_descriptor = INVALID_SOCKET;
    self->is_listening = false;
    self->is_waiting = false;
    pthread_mutex_destroy(&self->mutex);
    pthread_cond_destroy(&self->waiting_finished);
}

// pasivny socket = taky, na ktorom budem pocuvat
_Bool passive_socket_start_listening(struct passive_socket* self, short port) {
    pthread_mutex_lock(&self->mutex); // zamkni mutex
    // otestujem, ci uz v nejakom vlakne nerobim listen - ak uz robim, tak mi nedovoli robit listen v dalsom vlakne
    if (self->is_listening || self->is_waiting) {  // pokial pocuvam/cakam
        pthread_mutex_unlock(&self->mutex);
        fprintf(stderr, "passive_socket is listening. Cannot call start_listening again.\n");
        return false;
    }

    // vytvorim socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket failed.");
        pthread_mutex_unlock(&self->mutex);
        return false;
    }

    //definovanie adresy servera <arpa/inet.h>
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;         //internetove sockety
    serverAddress.sin_addr.s_addr = INADDR_ANY; //prijimame spojenia z celeho internetu
    serverAddress.sin_port = htons(port);       //nastavenie portu

    //bindovanie: prepojenie adresy servera so socketom <sys/socket.h>
    if (bind(server_socket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("bind failed.");
        pthread_mutex_unlock(&self->mutex);
        return false;
    }

    //listen: server bude prijimat nove spojenia cez socket serverSocket <sys/socket.h>
    listen(server_socket, 10);
    self->socket_descriptor = server_socket;

    self->is_listening = true;
    pthread_mutex_unlock(&self->mutex);

    return true;
}

// zastavit nacuvanie
// pozastavenie pasivneho socketu
void passive_socket_stop_listening(struct passive_socket* self) {
    pthread_mutex_lock(&self->mutex);
    if (!self->is_listening) {
        pthread_mutex_unlock(&self->mutex);
        return;
    }
    self->is_listening = false;
    while (self->is_waiting) {
        pthread_cond_wait(&self->waiting_finished, &self->mutex);
    }
    close(self->socket_descriptor);
    self->socket_descriptor = INVALID_SOCKET;
    pthread_mutex_unlock(&self->mutex);
}

// len na otestovanie, ci naozaj je nastaveny ako pasivny socket
_Bool passive_socket_is_listening(struct passive_socket* self) {
    pthread_mutex_lock(&self->mutex);
    bool is_listening = self->is_listening;
    pthread_mutex_unlock(&self->mutex);
    return is_listening;
}

// cakam na klientov
// vstupny param: pasivny socket, ktory caka na klienta
// vystupny param: aktivny socket, ktory vrati info o klientovi, ktory sa pripojil
// accept je BLOKUJUCE systemove volanie (nedokazem ho nasilu ukoncit)
// ked sa niekto pripoji, cakanie skonci
// ak chcem, aby sa pripojili 2 stroje, musim wat_for_client zavolat 2x
    // pre n klientov zavolam v cykle n-krat funkciu wait_for_client
    // az ked sa dostanem na koniec cyklu, to znamena, ze mam pripojenych n klientov
_Bool passive_socket_wait_for_client(struct passive_socket* self, struct active_socket* client_sock) {
    pthread_mutex_lock(&self->mutex);
    // accept mozem volat len v 1 vlakne, nie vo viacerych => skontrolujem, ci uz nebezi na inom vlakne
    if (self->is_waiting) {
        pthread_mutex_unlock(&self->mutex);
        fprintf(stderr, "passive_socket is waiting for client. Cannot call wait_for_client again.\n");
        return false;
    }
    self->is_waiting = true;
    pthread_mutex_unlock(&self->mutex);

    //server caka na pripojenie klienta <sys/socket.h>
    struct sockaddr_in client_address;
    socklen_t client_address_length = sizeof(client_address);

    fd_set sockets;
    FD_ZERO(&sockets);
    struct timeval tv;
    tv.tv_usec = 0;
    while (passive_socket_is_listening(self)) { // pokial pocuvam, vznikaju nove pripojenia klientov
        FD_SET(self->socket_descriptor, &sockets);
        tv.tv_sec = 1;

        // select riadi komunikaciu asynchronne
        // z blokujucich systemovych volani urobim neblokujuce :)
        select(self->socket_descriptor + 1, &sockets, NULL, NULL, &tv);
        if (FD_ISSET(self->socket_descriptor, &sockets)) {
            // !! accept -> vytvara pripojenia klientov; fd: PASIVNY socket
            // do client_socket vrati informacie o klientovi
            int client_socket = accept(self->socket_descriptor, (struct sockaddr *) &client_address,
                                       &client_address_length);

            if (client_socket < 0) {
                perror("accept failed.");
                return false;
            }

            // ulozim info o pripojenom klientovi na vystupny parameter
            client_sock->socket_descriptor = client_socket;

            pthread_mutex_lock(&self->mutex);
            self->is_waiting = false;
            pthread_mutex_unlock(&self->mutex);
            return true;
        }
    }

    pthread_mutex_lock(&self->mutex);
    self->is_waiting = false;
    pthread_cond_signal(&self->waiting_finished);
    pthread_mutex_unlock(&self->mutex);
    return false;
}

#undef INVALID_SOCKET
