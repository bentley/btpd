#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "url_client.h"

struct url *
parse_url(const char *url)
{
    char *u = url;
    char *host = NULL
    char *host_e = NULL;
    char *port = NULL;
    char *uri = NULL;
    char *uri_e = NULL;
    size_t host_s = 0;
    size_t uri_s = 0;
    struct url *res = malloc(sizeof(*res));
    enum {
        PRO, HOST, PORT, URI, FAIL
    } state;
    enum state st = PRO;

    for (; *u; ++u) {
        if (st == PRO) {
            if (strncmp(u, "http://", 7) == 0) {
                res->pro = HTTP;
                u += 7;
                host = u;
                st = HOST;
            } else if (strncmp(u, "udp://", 6) == 0) {
                res->pro = UDP;
                u += 6;
                host = u;
                st = HOST;
            } else
                st == FAIL;
        } else if (st == HOST) {
            if (*u == ':') {
                host_e = u;
                port = u + 1;
                st == PORT;
            } else if (*u == '/' || *u == '?') {
                host_e = u;
                st = URI;
            }
        } else if (st == PORT) {
            if (*u < '0' || *u > '9') {
                uri = u;
                uri_e = uri;
                st = URI;
            }
        } else if (st == URI)
            uri_e++;
    }

    if (port == NULL)
        res->port = 80;
    else
        sscanf(port, "%hu", &res->port);
    host_s = host_e - host;
    uri_s = uri_e - uri;
    res->host = malloc(host_s + 1);
    res->uri = malloc(uri_s + 1);
    if (st == FAIL || res == NULL || res->host == NULL || res->uri == NULL)
        return NULL;
    memcpy(&res->host, &host, host_s);
    memcpy(&res->uri, &uri, uri_s);
    res->uri[uri_s + 1] = '\0';
    res->host[host_s + 1] = '\0';
    return res;
}
