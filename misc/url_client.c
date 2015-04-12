#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iobuf.h"
#include "url_client.h"

struct request {
    enum {
        HEAD, CHUNK_SIZE, CHUNK_DATA, CHUNK_CRLF, ID_DATA
    } pstate;
    int parsing;
    int canceled;
    int chunked;
    long length;
    cb_t cb;
    void *arg;
    struct url *url;
    struct iobuf rbuf;
    struct iobuf wbuf;
};

static void
req_free(struct request *req)
{
    if (req != NULL) {
        url_free(req->url);
        iobuf_free(&req->rbuf);
        iobuf_free(&req->wbuf);
        free(req);
    }
}

static void
req_error(struct request *req)
{
    struct response res;
    res.type = ERR;
    res.v.error = 1;
    req->cb(req, &res, req->arg);
    req_free(req);
}

struct url *
url_parse(const char *url)
{
    const char *u = url;
    const char *host = NULL;
    const char *host_e = NULL;
    const char *port = NULL;
    const char *uri = NULL;
    size_t host_s = 0;
    size_t uri_s = 0;
    struct url *res = malloc(sizeof(*res));
    enum state {
        PRO, HOST, PORT, URI, FAIL
    };
    enum state st = PRO;

    /* mark start and end points of url components */
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
                st = FAIL;
        } else if (st == HOST) {
            if (*u == ':') {
                host_e = u;
                port = u + 1;
                st = PORT;
            } else if (*u == '/' || *u == '?') {
                host_e = u;
                uri = host_e;
                st = URI;
            }
        } else if (st == PORT) {
            if (*u < '0' || *u > '9') {
                uri = u;
                st = URI;
            }
        }
    }

    if (port == NULL)
        res->port = 80;
    else
        sscanf(port, "%hu", &res->port);
    host_s = host_e - host;
    uri_s = strlen(uri);
    res->host = malloc(host_s + 1);
    res->uri = malloc(uri_s + 1);
    if (st == FAIL || res == NULL || res->host == NULL || res->uri == NULL) {
        url_free(res);
        return NULL;
    }
    memcpy(&res->host, &host, host_s);
    memcpy(&res->uri, &uri, uri_s);
    res->uri[uri_s] = '\0';
    res->host[host_s] = '\0';
    return res;
}

void
url_free(struct url *u)
{
    if (u != NULL) {
        u->host ? free(u->host) : 1;
        u->uri ? free(u->uri) : 1;
        free(u);
    }
}
