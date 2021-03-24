#include "mybio.hpp"
#include <openssl/evp.h>  
#include <openssl/bio.h>  
#include <openssl/buffer.h>

#include <cstring>
#include <iostream>

size_t base64Encode(const char *buf, size_t len, bool newline, char *result) {
    BIO *bmem = nullptr, *b64 = nullptr;

    b64 = BIO_new(BIO_f_base64());
    if (!newline) BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, buf, (int)len);
    BIO_flush(b64);

    char *pp = nullptr;
    size_t data_len = BIO_get_mem_data(bmem, &pp);
    memcpy(result, pp, data_len);

    BIO_free_all(b64);

    return data_len;
}