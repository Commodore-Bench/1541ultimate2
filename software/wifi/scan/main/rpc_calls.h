/*
 * rpc_calls.h
 *
 *  Created on: Nov 7, 2021
 *      Author: gideon
 */

#ifndef SOFTWARE_WIFI_SCAN_MAIN_RPC_CALLS_H_
#define SOFTWARE_WIFI_SCAN_MAIN_RPC_CALLS_H_

#include <stdint.h>
#include "lwip/sockets.h"
#include "lwip/netdb.h"

#define DEFAULT_SCAN_LIST_SIZE 24

typedef struct {
    uint8_t command;
    uint8_t thread;
    uint16_t sequence;
} rpc_header_t;

typedef struct {
    rpc_header_t hdr;
    int esp_err; // to be casted to esp_err_t
} rpc_espcmd_resp;

typedef struct {
    rpc_header_t hdr;
} rpc_identify_req;

typedef struct {
    rpc_header_t hdr;
    uint16_t major;
    uint16_t minor;
    char string;
} rpc_identify_resp;

typedef struct {
    rpc_header_t hdr;
    int esp_err; // to be casted to esp_err_t
    uint8_t mac[6];
} rpc_getmac_resp;

typedef struct {
    rpc_header_t hdr;
    int baudrate;
    uint8_t flowctrl;
    uint8_t inversions;
} rpc_setbaud_req;

typedef struct {
    rpc_header_t hdr;
    uint8_t auth_mode; // to be casted to wifi_auth_mode_t
    char ssid[32];
    char password[64];
} rpc_wifi_connect_req;

/* 42 byte AP record */
typedef struct {
    uint8_t bssid[6];            // MAC address of AP
    uint8_t ssid[33];            // SSID of AP
    uint8_t channel;             // channel of AP
    int8_t  rssi;                // signal strength of AP
    uint8_t authmode;            // authmode of AP
} ultimate_ap_record_t;

typedef struct {
    uint8_t num_records;
    uint8_t reserved;
    ultimate_ap_record_t aps[DEFAULT_SCAN_LIST_SIZE];
} ultimate_ap_records_t;

typedef struct {
    rpc_header_t hdr;
    int esp_err;
    ultimate_ap_records_t rec;
} rpc_scan_resp;

typedef struct {
    rpc_header_t hdr;
    u16_t len;
    char data;
} rpc_rx_pkt;

//----------------------------------
// send raw packet
typedef struct {
    rpc_header_t hdr;
    uint32_t length;
    char data;
} rpc_send_eth_req;

typedef struct { // this response might be deprecated soon
    rpc_header_t hdr;
} rpc_send_eth_resp;

//----------------------------------
// socket
typedef struct {
    rpc_header_t hdr;
    int domain;
    int type;
    int protocol;
} rpc_socket_req;

typedef struct {
    rpc_header_t hdr;
    int retval;
    int xerrno;
} rpc_socket_resp;

//----------------------------------
// connect
typedef struct {
    rpc_header_t hdr;
    int socket;
    uint32_t namelen;
    struct sockaddr name;
} rpc_connect_req;

typedef struct {
    rpc_header_t hdr;
    int retval;
    int xerrno;
} rpc_connect_resp;

//----------------------------------
// accept
typedef struct {
    rpc_header_t hdr;
    int socket;
    uint32_t namelen;
} rpc_accept_req;

typedef struct {
    rpc_header_t hdr;
    int retval;
    int xerrno;
    uint32_t namelen;
    struct sockaddr name;
} rpc_accept_resp;

//----------------------------------
// bind
typedef struct {
    rpc_header_t hdr;
    int socket;
    uint32_t namelen;
    struct sockaddr_in name;
} rpc_bind_req;

typedef struct {
    rpc_header_t hdr;
    int retval;
    int xerrno;
} rpc_bind_resp;

//----------------------------------
// shutdown
typedef struct {
    rpc_header_t hdr;
    int socket;
    int how;
} rpc_shutdown_req;

typedef struct {
    rpc_header_t hdr;
    int retval;
    int xerrno;
} rpc_shutdown_resp;

//----------------------------------
// close
typedef struct {
    rpc_header_t hdr;
    int socket;
} rpc_close_req;

typedef struct {
    rpc_header_t hdr;
    int retval;
    int xerrno;
} rpc_close_resp;

//----------------------------------
// listen
typedef struct {
    rpc_header_t hdr;
    int socket;
    int backlog;
} rpc_listen_req;

typedef struct {
    rpc_header_t hdr;
    int retval;
    int xerrno;
} rpc_listen_resp;

//----------------------------------
// read
typedef struct {
    rpc_header_t hdr;
    int socket;
    uint32_t length;
} rpc_read_req;

typedef struct {
    rpc_header_t hdr;
    int retval;
    int xerrno;
    char data;
} rpc_read_resp;

//----------------------------------
// recv / recvfrom
typedef struct {
    rpc_header_t hdr;
    int socket;
    uint32_t length;
    int flags;
} rpc_recvfrom_req;

typedef struct {
    rpc_header_t hdr;
    int retval;
    int xerrno;
    struct sockaddr from;
    socklen_t fromlen;
    char data;
} rpc_recvfrom_resp;

//----------------------------------
// write
typedef struct {
    rpc_header_t hdr;
    int socket;
    uint32_t length;
    char data;
} rpc_write_req;

typedef struct {
    rpc_header_t hdr;
    int retval;
    int xerrno;
} rpc_write_resp;

//----------------------------------
// send
typedef struct {
    rpc_header_t hdr;
    int socket;
    uint32_t length;
    int flags;
    char data;
} rpc_send_req;

typedef struct {
    rpc_header_t hdr;
    int retval;
    int xerrno;
} rpc_send_resp;

//----------------------------------
// sendto
typedef struct {
    rpc_header_t hdr;
    int socket;
    uint32_t length;
    int flags;
    struct sockaddr to;
    socklen_t tolen;
    char data;
} rpc_sendto_req;

typedef struct {
    rpc_header_t hdr;
    int retval;
    int xerrno;
} rpc_sendto_resp;

//----------------------------------
// gethostbyname
typedef struct {
    rpc_header_t hdr;
    char name; // <null terminated string>
} rpc_gethostbyname_req;

typedef struct {
    rpc_header_t hdr;
    int retval;
    int xerrno;
    struct in_addr addr;
} rpc_gethostbyname_resp;

//----------------------------------
// getsockname

typedef struct {
    rpc_header_t hdr;
    int socket;
    int namelen;
} rpc_getsockname_req;

typedef struct {
    rpc_header_t hdr;
    int retval;
    int xerrno;
    int namelen;
    struct sockaddr name;
} rpc_getsockname_resp;

//----------------------------------
// setsockopt

typedef struct {
    rpc_header_t hdr;
    int socket;
    int level;
    int optname;
    int optlen;
    char optval;
} rpc_setsockopt_req;

typedef struct {
    rpc_header_t hdr;
    int retval;
    int xerrno;
} rpc_setsockopt_resp;

//----------------------------------
// getsockopt

typedef struct {
    rpc_header_t hdr;
    int socket;
    int level;
    int optname;
    int optlen;
} rpc_getsockopt_req;

typedef struct {
    rpc_header_t hdr;
    int retval;
    int xerrno;
    int optlen;
    char optval;
} rpc_getsockopt_resp;

// ioctl
// fcntl
// getpeername

// select


// Events
typedef struct {
    rpc_header_t hdr;
    uint32_t ip;       // to be casted to [esp_]ip4_addr_t
    uint32_t netmask;  // to be casted to [esp_]ip4_addr_t
    uint32_t gw;       // to be casted to [esp_]ip4_addr_t
    uint8_t changed;
} event_pkt_got_ip;

#define CMD_ECHO            0x01
#define CMD_IDENTIFY        0x02
#define CMD_SET_BAUD        0x03
#define CMD_WIFI_SCAN       0x04
#define CMD_WIFI_CONNECT    0x05
#define CMD_WIFI_DISCONNECT 0x06
#define CMD_WIFI_GETMAC     0x07
#define CMD_SEND_PACKET     0x08
#define CMD_SOCKET          0x11
#define CMD_CONNECT         0x12
#define CMD_ACCEPT          0x13
#define CMD_BIND            0x14
#define CMD_SHUTDOWN        0x15
#define CMD_CLOSE           0x16
#define CMD_LISTEN          0x17
#define CMD_READ            0x18
#define CMD_RECV            0x19
#define CMD_RECVFROM        0x1A
#define CMD_WRITE           0x1B
#define CMD_SEND            0x1C
#define CMD_SENDTO          0x1D
#define CMD_GETHOSTBYNAME   0x21
#define CMD_GETPEERNAME     0x22
#define CMD_GETSOCKNAME     0x23
#define CMD_GETSOCKOPT      0x24
#define CMD_SETSOCKOPT      0x25
#define CMD_IOCTL           0x28
#define CMD_FCNTL           0x29

#define EVENT_CONNECTED     0x40
#define EVENT_GOTIP         0x41
#define EVENT_DISCONNECTED  0x42
#define EVENT_RECV_PACKET   0x43
#define EVENT_RESCAN        0x44

// int socket(int domain, int type, int protocol);
// int connect(int s, const struct sockaddr *name, socklen_t namelen);
// int accept(int s, struct sockaddr *addr, socklen_t *addrlen);
// int bind(int s, const struct sockaddr *name, socklen_t namelen);
// int shutdown(int s, int how);
// int close(int s);
// int listen(int s, int backlog);

// int read(int s, void *mem, size_t len);
// int recv(int s, void *mem, size_t len, int flags);
// int recvfrom(int s, void *mem, size_t len, int flags,
//       struct sockaddr *from, socklen_t *fromlen);

// int write(int s, const void *dataptr, size_t size);
// int send(int s, const void *dataptr, size_t size, int flags);
// int sendto(int s, const void *dataptr, size_t size, int flags,
//     const struct sockaddr *to, socklen_t tolen);

// int getpeername (int s, struct sockaddr *name, socklen_t *namelen);
// int getsockname (int s, struct sockaddr *name, socklen_t *namelen);
// int getsockopt (int s, int level, int optname, void *optval, socklen_t *optlen);
// int setsockopt (int s, int level, int optname, const void *optval, socklen_t optlen);

// int ioctl(int s, long cmd, void *argp);
// int fcntl(int s, int cmd, int val);

// int select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
//                 struct timeval *timeout);



#endif /* SOFTWARE_WIFI_SCAN_MAIN_RPC_CALLS_H_ */
