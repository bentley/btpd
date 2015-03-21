struct url {
    char *host;
    char *uri;
    uint16_t port;
    enum {
        HTTP, UDP
    } pro;
};

struct url *parse_url(const char *url);
