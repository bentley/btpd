enum proto { HTTP, UDP };

struct url {
    char *host;
    char *uri;
    uint16_t port;
    enum proto pro;
};

struct url *parse_url(const char *url);
void free_url(struct url *u);
