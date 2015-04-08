enum proto { HTTP, UDP };

struct url {
    char *host;
    char *uri;
    uint16_t port;
    enum proto pro;
};

struct request;

struct response {
    enum {
        ERR, CODE, HEADER, DATA, DONE
    } type;
    union {
        int error;
        int code;
        struct {
            char *n;
            char *v;
        } header;
        struct {
            size_t l;
            char *p;
        } data;
    } v;
};

typedef void (*cb_t)(struct request *, struct response *, void *);

struct url *url_parse(const char *url);
void url_free(struct url *u);
int get(struct request **out, const char *url, const char *hdrs, cb_t cb, void *arg);
void cancel(struct request *req);
struct url *url_get(struct request *req);
int want_read(struct request *req);
int want_write(struct request *req);
int rread(struct request *req, int sd);
int rwrite(struct request *req, int sd);
