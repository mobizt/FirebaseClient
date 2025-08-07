
/*
  WiFiClientBearSSL- SSL client/server for esp8266 using BearSSL libraries
  - Mostly compatible with Arduino WiFi shield library and standard
    WiFiClient/ServerSecure (except for certificate handling).

  Copyright (c) 2018 Earle F. Philhower, III

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef BSSL_HELPER_H
#define BSSL_HELPER_H

#if defined(USE_EMBED_SSL_ENGINE)

#if defined(ESP8266)

#ifdef __GNUC__
#if __GNUC__ > 4 || __GNUC__ == 10
#if defined(ARDUINO_ESP8266_GIT_VER)
#if ARDUINO_ESP8266_GIT_VER > 0
#define ESP8266_CORE_SDK_V3_X_X
#endif
#endif
#endif
#endif

#include <Arduino.h>

#include <bearssl/bearssl.h>
#include <vector>
#include <StackThunk.h>
#include <sys/time.h>
#include <IPAddress.h>
#include <Client.h>
#include <FS.h>
#include <time.h>
#include <ctype.h>
#include <vector>
#include <algorithm>

#else

#include <Arduino.h>
#include <bearssl/bearssl.h>
#include <Updater.h>
#include <StackThunk.h>

#endif

#elif defined(USE_LIB_SSL_ENGINE)

namespace key_bssl
{
    class private_key
    {
    public:
        int key_type; /* BR_KEYTYPE_RSA or BR_KEYTYPE_EC */
        union
        {
            br_rsa_private_key rsa;
            br_ec_private_key ec;
        } key;
    };

    class public_key
    {
    public:
        int key_type; /* BR_KEYTYPE_RSA or BR_KEYTYPE_EC */
        union
        {
            br_rsa_public_key rsa;
            br_ec_public_key ec;
        } key;
    };

    class pem_object
    {
    public:
        char *name;
        unsigned char *data;
        size_t data_len;
    };

    // Forward definitions
    void free_ta_contents(br_x509_trust_anchor *ta);
    void free_public_key(public_key *pk);
    void free_private_key(private_key *sk);
    bool looks_like_DER(const unsigned char *buf, size_t len);
    pem_object *decode_pem(const void *src, size_t len, size_t *num);
    void free_pem_object_contents(pem_object *po);
    char *strdupImpl(const char *s);

    // Used as callback multiple places to append a string to a vector
    static void byte_vector_append(void *ctx, const void *buff, size_t len)
    {
        std::vector<uint8_t> *vec = static_cast<std::vector<uint8_t> *>(ctx);
        vec->reserve(vec->size() + len); // Allocate extra space all at once
        for (size_t i = 0; i < len; i++)
        {
            vec->push_back((reinterpret_cast<const uint8_t *>(buff))[i]);
        }
    }

    static bool certificate_to_trust_anchor_inner(br_x509_trust_anchor *ta, const br_x509_certificate *xc)
    {
        std::unique_ptr<br_x509_decoder_context> dc(new br_x509_decoder_context); // auto-delete on exit
        std::vector<uint8_t> vdn;
        br_x509_pkey *pk;

        // Clear everything in the Trust Anchor
        memset(ta, 0, sizeof(*ta));

        br_x509_decoder_init(dc.get(), byte_vector_append, reinterpret_cast<void *>(&vdn));
        br_x509_decoder_push(dc.get(), xc->data, xc->data_len);
        pk = br_x509_decoder_get_pkey(dc.get());
        if (pk == nullptr)
            return false; // No key present, something broken in the cert!

        // Copy the raw certificate data
        ta->dn.data = reinterpret_cast<uint8_t *>(esp_sslclient_malloc(vdn.size()));
        if (!ta->dn.data)
            return false; // OOM, but nothing yet allocated

        memcpy(ta->dn.data, &vdn[0], vdn.size());
        ta->dn.len = vdn.size();
        ta->flags = 0;
        if (br_x509_decoder_isCA(dc.get()))
            ta->flags |= BR_X509_TA_CA;

        // Extract the public key
        switch (pk->key_type)
        {
        case BR_KEYTYPE_RSA:
            ta->pkey.key_type = BR_KEYTYPE_RSA;
            ta->pkey.key.rsa.n = reinterpret_cast<uint8_t *>(esp_sslclient_malloc(pk->key.rsa.nlen));
            ta->pkey.key.rsa.e = reinterpret_cast<uint8_t *>(esp_sslclient_malloc(pk->key.rsa.elen));
            if ((ta->pkey.key.rsa.n == nullptr) || (ta->pkey.key.rsa.e == nullptr))
            {
                free_ta_contents(ta); // OOM, so clean up
                return false;
            }
            memcpy(ta->pkey.key.rsa.n, pk->key.rsa.n, pk->key.rsa.nlen);
            ta->pkey.key.rsa.nlen = pk->key.rsa.nlen;
            memcpy(ta->pkey.key.rsa.e, pk->key.rsa.e, pk->key.rsa.elen);
            ta->pkey.key.rsa.elen = pk->key.rsa.elen;
            return true;

        case BR_KEYTYPE_EC:
            ta->pkey.key_type = BR_KEYTYPE_EC;
            ta->pkey.key.ec.curve = pk->key.ec.curve;
            ta->pkey.key.ec.q = reinterpret_cast<uint8_t *>(esp_sslclient_malloc(pk->key.ec.qlen));
            if (ta->pkey.key.ec.q == nullptr)
            {
                free_ta_contents(ta); // OOM, so clean up
                return false;
            }
            memcpy(ta->pkey.key.ec.q, pk->key.ec.q, pk->key.ec.qlen);
            ta->pkey.key.ec.qlen = pk->key.ec.qlen;
            return true;
        default:
            free_ta_contents(ta); // Unknown key type
            return false;
        }

        // Should never get here, if so there was an unknown error
        return false;
    }

    br_x509_trust_anchor *certificate_to_trust_anchor(const br_x509_certificate *xc)
    {
        br_x509_trust_anchor *ta = reinterpret_cast<br_x509_trust_anchor *>(esp_sslclient_malloc(sizeof(br_x509_trust_anchor)));
        if (!ta)
            return nullptr;

        if (!certificate_to_trust_anchor_inner(ta, xc))
        {
            esp_sslclient_free(&ta);
            return nullptr;
        }
        return ta;
    }

    void free_ta_contents(br_x509_trust_anchor *ta)
    {
        if (ta)
        {
            esp_sslclient_free(&ta->dn.data);
            if (ta->pkey.key_type == BR_KEYTYPE_RSA)
            {
                esp_sslclient_free(&ta->pkey.key.rsa.n);
                esp_sslclient_free(&ta->pkey.key.rsa.e);
            }
            else if (ta->pkey.key_type == BR_KEYTYPE_EC)
                esp_sslclient_free(&ta->pkey.key.ec.q);

            memset(ta, 0, sizeof(*ta));
        }
    }

    // Checks if a bitstream looks like a valid DER(binary) encoding.
    // Basically tries to verify the length of all included segments
    // matches the length of the input buffer.  Does not actually
    // validate any contents.
    bool looks_like_DER(const unsigned char *buff, size_t len)
    {
        if (len < 2)
            return false;

        if (pgm_read_byte(buff++) != 0x30)
            return false;

        int fb = pgm_read_byte(buff++);
        len -= 2;
        if (fb < 0x80)
            return (size_t)fb == len;
        else if (fb == 0x80)
            return false;
        else
        {
            fb -= 0x80;
            if (len < (size_t)fb + 2)
                return false;

            len -= (size_t)fb;
            size_t dlen = 0;
            while (fb-- > 0)
            {
                if (dlen > (len >> 8))
                    return false;
                dlen = (dlen << 8) + (size_t)pgm_read_byte(buff++);
            }
            return dlen == len;
        }
    }

    void free_pem_object_contents(pem_object *po)
    {
        if (po)
        {
            esp_sslclient_free(&po->name);
            esp_sslclient_free(&po->data);
        }
    }

    char *strdupImpl(const char *s)
    {
        size_t slen = strlen(s);
        char *result = reinterpret_cast<char *>(esp_sslclient_malloc(slen + 1));
        if (!result)
            return NULL;
        memcpy(result, s, slen + 1);
        return result;
    }

    // Converts a PEM (~=base64) source into a set of DER-encoded binary blobs.
    // Each blob is named by the ---- BEGIN xxx ---- field, and multiple
    // blobs may be returned.
    pem_object *decode_pem(const void *src, size_t len, size_t *num)
    {
        std::vector<pem_object> pem_list;
        std::unique_ptr<br_pem_decoder_context> pc(new br_pem_decoder_context); // auto-delete on exit
        if (!pc.get())
            return nullptr;

        pem_object po, *pos = nullptr;
        const unsigned char *buff = nullptr;
        std::vector<uint8_t> bv;

        *num = 0;
        br_pem_decoder_init(pc.get());
        buff = reinterpret_cast<const unsigned char *>(src);
        po.name = nullptr;
        po.data = nullptr;
        po.data_len = 0;
        bool inobj = false;
        bool extra_nl = true;

        while (len > 0)
        {
            size_t tlen;

            tlen = br_pem_decoder_push(pc.get(), buff, len);
            buff += tlen;
            len -= tlen;
            switch (br_pem_decoder_event(pc.get()))
            {
            case BR_PEM_BEGIN_OBJ:
                po.name = strdupImpl(br_pem_decoder_name(pc.get()));
                br_pem_decoder_setdest(pc.get(), byte_vector_append, &bv);
                inobj = true;
                break;

            case BR_PEM_END_OBJ:
                if (inobj)
                {
                    // Stick data into the vector
                    po.data = reinterpret_cast<uint8_t *>(esp_sslclient_malloc(bv.size()));
                    if (po.data)
                    {
                        memcpy(po.data, &bv[0], bv.size());
                        po.data_len = bv.size();
                        pem_list.push_back(po);
                    }
                    // Clean up state for next blob processing
                    bv.clear();
                    po.name = nullptr;
                    po.data = nullptr;
                    po.data_len = 0;
                    inobj = false;
                }
                break;

            case BR_PEM_ERROR:
                esp_sslclient_free(&po.name);
                for (size_t i = 0; i < pem_list.size(); i++)
                {
                    free_pem_object_contents(&pem_list[i]);
                }
                return nullptr;

            default:
                // Do nothing here, the parser is still working on things
                break;
            }

            if (len == 0 && extra_nl)
            {
                extra_nl = false;
                buff = reinterpret_cast<const unsigned char *>("\n");
                len = 1;
            }
        }

        if (inobj)
        {
            esp_sslclient_free(&po.name);
            for (size_t i = 0; i < pem_list.size(); i++)
            {
                free_pem_object_contents(&pem_list[i]);
            }
            return nullptr;
        }

        pos = reinterpret_cast<pem_object *>(esp_sslclient_malloc((1 + pem_list.size()) * sizeof(*pos)));
        if (pos)
        {
            *num = pem_list.size();
            pem_list.push_back(po); // Null-terminate list
            memcpy(pos, &pem_list[0], pem_list.size() * sizeof(*pos));
        }
        return pos;
    }

    // Parse out DER or PEM encoded certificates from a binary buffer,
    // potentially stored in PROGMEM.
    br_x509_certificate *read_certificates(const char *buff, size_t len, size_t *num)
    {
        std::vector<br_x509_certificate> cert_list;
        pem_object *pos = nullptr;
        size_t u = 0, num_pos = 0;
        br_x509_certificate *xcs = nullptr;
        br_x509_certificate dummy;

        *num = 0;

        if (looks_like_DER(reinterpret_cast<const unsigned char *>(buff), len))
        {
            xcs = reinterpret_cast<br_x509_certificate *>(esp_sslclient_malloc(2 * sizeof(*xcs)));
            if (!xcs)
                return nullptr;

            xcs[0].data = reinterpret_cast<uint8_t *>(esp_sslclient_malloc(len));
            if (!xcs[0].data)
            {
                esp_sslclient_free(&xcs);
                return nullptr;
            }
            memcpy_P(xcs[0].data, buff, len);
            xcs[0].data_len = len;
            xcs[1].data = nullptr;
            xcs[1].data_len = 0;
            *num = 1;
            return xcs;
        }

        pos = decode_pem(buff, len, &num_pos);
        if (!pos)
            return nullptr;

        for (u = 0; u < num_pos; u++)
        {
            if (!strcmp_P(pos[u].name, PSTR("CERTIFICATE")) || !strcmp_P(pos[u].name, PSTR("X509 CERTIFICATE")))
            {
                br_x509_certificate xc;
                xc.data = pos[u].data;
                xc.data_len = pos[u].data_len;
                pos[u].data = nullptr; // Don't free the data we moved to the xc vector!
                cert_list.push_back(xc);
            }
        }
        for (u = 0; u < num_pos; u++)
        {
            free_pem_object_contents(&pos[u]);
        }
        esp_sslclient_free(&pos);

        if (cert_list.size() == 0)
            return nullptr;

        *num = cert_list.size();
        dummy.data = nullptr;
        dummy.data_len = 0;
        cert_list.push_back(dummy);
        xcs = reinterpret_cast<br_x509_certificate *>(esp_sslclient_malloc(cert_list.size() * sizeof(*xcs)));
        if (!xcs)
        {
            for (size_t i = 0; i < cert_list.size(); i++)
            {
                free(cert_list[i].data); // Clean up any captured data blobs
            }
            return nullptr;
        }
        memcpy(xcs, &cert_list[0], cert_list.size() * sizeof(br_x509_certificate));
        // XCS now has [].data pointing to the previously allocated blobs, so don't
        // want to free anything in cert_list[].
        return xcs;
    }

    void free_certificates(br_x509_certificate *certs, size_t num)
    {
        if (certs)
        {
            for (size_t u = 0; u < num; u++)
            {
                free(certs[u].data);
            }
            esp_sslclient_free(&certs);
        }
    }

    static public_key *decode_public_key(const unsigned char *buff, size_t len)
    {
        std::unique_ptr<br_pkey_decoder_context> dc(new br_pkey_decoder_context); // auto-delete on exit
        if (!dc.get())
            return nullptr;

        public_key *pk = nullptr;
        // https://github.com/yglukhov/bearssl_pkey_decoder
        br_pkey_decoder_init(dc.get());
        br_pkey_decoder_push(dc.get(), buff, len);
        int err = br_pkey_decoder_last_error(dc.get());
        if (err != 0)
            return nullptr;

        const br_rsa_public_key *rk = nullptr;
        const br_ec_public_key *ek = nullptr;
        switch (br_pkey_decoder_key_type(dc.get()))
        {
        case BR_KEYTYPE_RSA:
            rk = br_pkey_decoder_get_rsa(dc.get());
            pk = reinterpret_cast<public_key *>(esp_sslclient_malloc(sizeof *pk));
            if (!pk)
                return nullptr;
            pk->key_type = BR_KEYTYPE_RSA;
            pk->key.rsa.n = reinterpret_cast<uint8_t *>(esp_sslclient_malloc(rk->nlen));
            pk->key.rsa.e = reinterpret_cast<uint8_t *>(esp_sslclient_malloc(rk->elen));
            if (!pk->key.rsa.n || !pk->key.rsa.e)
            {
                esp_sslclient_free(&pk->key.rsa.n);
                esp_sslclient_free(&pk->key.rsa.e);
                esp_sslclient_free(&pk);
                return nullptr;
            }
            memcpy(pk->key.rsa.n, rk->n, rk->nlen);
            pk->key.rsa.nlen = rk->nlen;
            memcpy(pk->key.rsa.e, rk->e, rk->elen);
            pk->key.rsa.elen = rk->elen;
            return pk;

        case BR_KEYTYPE_EC:
            ek = br_pkey_decoder_get_ec(dc.get());
            pk = reinterpret_cast<public_key *>(esp_sslclient_malloc(sizeof *pk));
            if (!pk)
                return nullptr;

            pk->key_type = BR_KEYTYPE_EC;
            pk->key.ec.q = reinterpret_cast<uint8_t *>(esp_sslclient_malloc(ek->qlen));
            if (!pk->key.ec.q)
            {
                esp_sslclient_free(&pk);
                return nullptr;
            }
            memcpy(pk->key.ec.q, ek->q, ek->qlen);
            pk->key.ec.qlen = ek->qlen;
            pk->key.ec.curve = ek->curve;
            return pk;

        default:
            return nullptr;
        }
    }

    void free_public_key(public_key *pk)
    {
        if (pk)
        {
            if (pk->key_type == BR_KEYTYPE_RSA)
            {
                esp_sslclient_free(&pk->key.rsa.n);
                esp_sslclient_free(&pk->key.rsa.e);
            }
            else if (pk->key_type == BR_KEYTYPE_EC)
                esp_sslclient_free(&pk->key.ec.q);
            esp_sslclient_free(&pk);
        }
    }

    static private_key *decode_private_key(const unsigned char *buff, size_t len)
    {
        std::unique_ptr<br_skey_decoder_context> dc(new br_skey_decoder_context); // auto-delete on exit
        if (!dc.get())
            return nullptr;

        private_key *sk = nullptr;

        br_skey_decoder_init(dc.get());
        br_skey_decoder_push(dc.get(), buff, len);
        int err = br_skey_decoder_last_error(dc.get());
        if (err != 0)
            return nullptr;

        const br_rsa_private_key *rk = nullptr;
        const br_ec_private_key *ek = nullptr;
        switch (br_skey_decoder_key_type(dc.get()))
        {
        case BR_KEYTYPE_RSA:
            rk = br_skey_decoder_get_rsa(dc.get());
            sk = reinterpret_cast<private_key *>(esp_sslclient_malloc(sizeof *sk));
            if (!sk)
                return nullptr;
            sk->key_type = BR_KEYTYPE_RSA;
            sk->key.rsa.p = reinterpret_cast<uint8_t *>(esp_sslclient_malloc(rk->plen));
            sk->key.rsa.q = reinterpret_cast<uint8_t *>(esp_sslclient_malloc(rk->qlen));
            sk->key.rsa.dp = reinterpret_cast<uint8_t *>(esp_sslclient_malloc(rk->dplen));
            sk->key.rsa.dq = reinterpret_cast<uint8_t *>(esp_sslclient_malloc(rk->dqlen));
            sk->key.rsa.iq = reinterpret_cast<uint8_t *>(esp_sslclient_malloc(rk->iqlen));
            if (!sk->key.rsa.p || !sk->key.rsa.q || !sk->key.rsa.dp || !sk->key.rsa.dq || !sk->key.rsa.iq)
            {
                free_private_key(sk);
                return nullptr;
            }
            sk->key.rsa.n_bitlen = rk->n_bitlen;
            memcpy(sk->key.rsa.p, rk->p, rk->plen);
            sk->key.rsa.plen = rk->plen;
            memcpy(sk->key.rsa.q, rk->q, rk->qlen);
            sk->key.rsa.qlen = rk->qlen;
            memcpy(sk->key.rsa.dp, rk->dp, rk->dplen);
            sk->key.rsa.dplen = rk->dplen;
            memcpy(sk->key.rsa.dq, rk->dq, rk->dqlen);
            sk->key.rsa.dqlen = rk->dqlen;
            memcpy(sk->key.rsa.iq, rk->iq, rk->iqlen);
            sk->key.rsa.iqlen = rk->iqlen;
            return sk;

        case BR_KEYTYPE_EC:
            ek = br_skey_decoder_get_ec(dc.get());
            sk = reinterpret_cast<private_key *>(esp_sslclient_malloc(sizeof *sk));
            if (!sk)
                return nullptr;
            sk->key_type = BR_KEYTYPE_EC;
            sk->key.ec.curve = ek->curve;
            sk->key.ec.x = reinterpret_cast<uint8_t *>(esp_sslclient_malloc(ek->xlen));
            if (!sk->key.ec.x)
            {
                free_private_key(sk);
                return nullptr;
            }
            memcpy(sk->key.ec.x, ek->x, ek->xlen);
            sk->key.ec.xlen = ek->xlen;
            return sk;

        default:
            return nullptr;
        }
    }

    void free_private_key(private_key *sk)
    {
        if (sk)
        {
            switch (sk->key_type)
            {
            case BR_KEYTYPE_RSA:
                esp_sslclient_free(&sk->key.rsa.p);
                esp_sslclient_free(&sk->key.rsa.q);
                esp_sslclient_free(&sk->key.rsa.dp);
                esp_sslclient_free(&sk->key.rsa.dq);
                esp_sslclient_free(&sk->key.rsa.iq);
                break;
            case BR_KEYTYPE_EC:
                esp_sslclient_free(&sk->key.ec.x);
                break;
            default:
                // Could be an uninitted key, no sub elements to free
                break;
            }
            esp_sslclient_free(&sk);
        }
    }

    void free_pem_object(pem_object *pos)
    {
        if (pos != nullptr)
        {
            for (size_t u = 0; pos[u].name; u++)
            {
                free_pem_object_contents(&pos[u]);
            }
            esp_sslclient_free(&pos);
        }
    }

    private_key *read_private_key(const char *buff, size_t len)
    {
        private_key *sk = nullptr;
        pem_object *pos = nullptr;

        if (looks_like_DER(reinterpret_cast<const unsigned char *>(buff), len))
        {
            sk = decode_private_key(reinterpret_cast<const unsigned char *>(buff), len);
            return sk;
        }

        size_t num;
        pos = decode_pem(buff, len, &num);
        if (pos == nullptr)
            return nullptr; // PEM decode error

        for (size_t u = 0; pos[u].name; u++)
        {
            const char *name = pos[u].name;
            if (!strcmp_P(name, PSTR("RSA PRIVATE KEY")) || !strcmp_P(name, PSTR("EC PRIVATE KEY")) || !strcmp_P(name, PSTR("PRIVATE KEY")))
            {
                sk = decode_private_key(pos[u].data, pos[u].data_len);
                free_pem_object(pos);
                return sk;
            }
        }
        // If we hit here, no match
        free_pem_object(pos);
        return nullptr;
    }

    public_key *read_public_key(const char *buff, size_t len)
    {
        public_key *pk = nullptr;
        pem_object *pos = nullptr;

        if (looks_like_DER(reinterpret_cast<const unsigned char *>(buff), len))
        {
            pk = decode_public_key(reinterpret_cast<const unsigned char *>(buff), len);
            return pk;
        }
        size_t num;
        pos = decode_pem(buff, len, &num);
        if (pos == nullptr)
            return nullptr; // PEM decode error

        for (size_t u = 0; pos[u].name; u++)
        {
            const char *name = pos[u].name;
            if (!strcmp_P(name, PSTR("RSA PUBLIC KEY")) || !strcmp_P(name, PSTR("EC PUBLIC KEY")) || !strcmp_P(name, PSTR("PUBLIC KEY")))
            {
                pk = decode_public_key(pos[u].data, pos[u].data_len);
                free_pem_object(pos);
                return pk;
            }
        }

        // We hit here == no key found
        free_pem_object(pos);
        return pk;
    }

    static uint8_t *loadStream(Stream &stream, size_t size)
    {
        uint8_t *dest = reinterpret_cast<uint8_t *>(esp_sslclient_malloc(size));
        if (!dest)
            return nullptr; // OOM error

        if (size != stream.readBytes(dest, size))
        {
            esp_sslclient_free(&dest); // Error during read
            return nullptr;
        }
        return dest;
    }
}
#endif

#if defined(USE_LIB_SSL_ENGINE) || defined(USE_EMBED_SSL_ENGINE)
// Cache for a TLS session with a server
// Use with BearSSL::WiFiClientSecure::setSession
// to accelerate the TLS handshake
class BearSSL_Session
{
    friend class BSSL_SSLClient;

public:
    BearSSL_Session() { memset(&_session, 0, sizeof(_session)); }

    br_ssl_session_parameters *getSession() { return &_session; }

private:
    // The actual BearSSL session information
    br_ssl_session_parameters _session;
};

static const uint16_t suites_P[] PROGMEM = {
#ifndef BEARSSL_SSL_BASIC
    BR_TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256,
    BR_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256,
    BR_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
    BR_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
    BR_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,
    BR_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,
    BR_TLS_ECDHE_ECDSA_WITH_AES_128_CCM,
    BR_TLS_ECDHE_ECDSA_WITH_AES_256_CCM,
    BR_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8,
    BR_TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8,
    BR_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256,
    BR_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256,
    BR_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384,
    BR_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384,
    BR_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA,
    BR_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA,
    BR_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA,
    BR_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA,
    BR_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256,
    BR_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256,
    BR_TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384,
    BR_TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384,
    BR_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256,
    BR_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256,
    BR_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384,
    BR_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384,
    BR_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA,
    BR_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA,
    BR_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA,
    BR_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA,
    BR_TLS_RSA_WITH_AES_128_GCM_SHA256,
    BR_TLS_RSA_WITH_AES_256_GCM_SHA384,
    BR_TLS_RSA_WITH_AES_128_CCM,
    BR_TLS_RSA_WITH_AES_256_CCM,
    BR_TLS_RSA_WITH_AES_128_CCM_8,
    BR_TLS_RSA_WITH_AES_256_CCM_8,
#endif
    BR_TLS_RSA_WITH_AES_128_CBC_SHA256,
    BR_TLS_RSA_WITH_AES_256_CBC_SHA256,
    BR_TLS_RSA_WITH_AES_128_CBC_SHA,
    BR_TLS_RSA_WITH_AES_256_CBC_SHA,
#ifndef BEARSSL_SSL_BASIC
    BR_TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA,
    BR_TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA,
    BR_TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA,
    BR_TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA,
    BR_TLS_RSA_WITH_3DES_EDE_CBC_SHA
#endif
};

// For apps which want to use less secure but faster ciphers, only
static const uint16_t faster_suites_P[] PROGMEM = {
    BR_TLS_RSA_WITH_AES_256_CBC_SHA256,
    BR_TLS_RSA_WITH_AES_128_CBC_SHA256,
    BR_TLS_RSA_WITH_AES_256_CBC_SHA,
    BR_TLS_RSA_WITH_AES_128_CBC_SHA};

// Internal opaque structures, not needed by user applications
namespace key_bssl
{
    class public_key;
    class private_key;
};

namespace bssl
{
#if defined(USE_LIB_SSL_ENGINE)

    // Holds either a single public RSA or EC key for use when BearSSL wants a pubkey.
    // Copies all associated data so no need to keep input PEM/DER keys.
    // All inputs can be either in RAM or PROGMEM.
    class PublicKey
    {
    public:
        PublicKey() { _key = nullptr; }
        explicit PublicKey(const char *pemKey)
        {
            _key = nullptr;
            parse(pemKey);
        }

        explicit PublicKey(const uint8_t *derKey, size_t derLen)
        {
            _key = nullptr;
            parse(derKey, derLen);
        }

        explicit PublicKey(Stream &stream, size_t size)
        {
            _key = nullptr;
            auto buff = key_bssl::loadStream(stream, size);
            if (buff)
            {
                parse(buff, size);
                esp_sslclient_free(&buff);
            }
        }
        explicit PublicKey(Stream &stream) : PublicKey(stream, stream.available()) {};

        ~PublicKey()
        {
            if (_key)
                key_bssl::free_public_key(_key);
        }

        bool parse(const char *pemKey) { return parse(reinterpret_cast<const uint8_t *>(pemKey), strlen_P(pemKey)); }

        bool parse(const uint8_t *derKey, size_t derLen)
        {
            if (_key)
            {
                key_bssl::free_public_key(_key);
                _key = nullptr;
            }
            _key = key_bssl::read_public_key(reinterpret_cast<const char *>(derKey), derLen);
            return _key ? true : false;
        }

        // Accessors for internal use, not needed by apps
        bool isRSA() const
        {
            if (!_key || _key->key_type != BR_KEYTYPE_RSA)
                return false;
            return true;
        }

        bool isEC() const
        {
            if (!_key || _key->key_type != BR_KEYTYPE_EC)
                return false;
            return true;
        }

        const br_rsa_public_key *getRSA() const
        {
            if (!_key || _key->key_type != BR_KEYTYPE_RSA)
                return nullptr;
            return &_key->key.rsa;
        }

        const br_ec_public_key *getEC() const
        {
            if (!_key || _key->key_type != BR_KEYTYPE_EC)
                return nullptr;
            return &_key->key.ec;
        }

        // Disable the copy constructor, we're pointer based
        PublicKey(const PublicKey &that) = delete;
        PublicKey &operator=(const PublicKey &that) = delete;

    private:
        key_bssl::public_key *_key;
    };

    // Holds either a single private RSA or EC key for use when BearSSL wants a secretkey.
    // Copies all associated data so no need to keep input PEM/DER keys.
    // All inputs can be either in RAM or PROGMEM.
    class PrivateKey
    {
    public:
        PrivateKey() { _key = nullptr; }

        explicit PrivateKey(const char *pemKey)
        {
            _key = nullptr;
            parse(pemKey);
        }

        explicit PrivateKey(const uint8_t *derKey, size_t derLen)
        {
            _key = nullptr;
            parse(derKey, derLen);
        }

        explicit PrivateKey(Stream &stream, size_t size)
        {
            _key = nullptr;
            auto buff = key_bssl::loadStream(stream, size);
            if (buff)
            {
                parse(buff, size);
                esp_sslclient_free(&buff);
            }
        }

        explicit PrivateKey(Stream &stream) : PrivateKey(stream, stream.available()) {};

        ~PrivateKey()
        {
            if (_key)
                key_bssl::free_private_key(_key);
        }

        bool parse(const char *pemKey) { return parse(reinterpret_cast<const uint8_t *>(pemKey), strlen_P(pemKey)); }

        bool parse(const uint8_t *derKey, size_t derLen)
        {
            if (_key)
            {
                key_bssl::free_private_key(_key);
                _key = nullptr;
            }
            _key = key_bssl::read_private_key(reinterpret_cast<const char *>(derKey), derLen);
            return _key ? true : false;
        }

        // Accessors for internal use, not needed by apps
        bool isRSA() const
        {
            if (!_key || _key->key_type != BR_KEYTYPE_RSA)
                return false;
            return true;
        }

        bool isEC() const
        {
            if (!_key || _key->key_type != BR_KEYTYPE_EC)
                return false;
            return true;
        }

        const br_rsa_private_key *getRSA() const
        {
            if (!_key || _key->key_type != BR_KEYTYPE_RSA)
                return nullptr;
            return &_key->key.rsa;
        }

        const br_ec_private_key *getEC() const
        {
            if (!_key || _key->key_type != BR_KEYTYPE_EC)
                return nullptr;
            return &_key->key.ec;
        }

        // Disable the copy constructor, we're pointer based
        PrivateKey(const PrivateKey &that) = delete;
        PrivateKey &operator=(const PrivateKey &that) = delete;

    private:
        key_bssl::private_key *_key;
    };

    // Holds one or more X.509 certificates and associated trust anchors for
    // use whenever BearSSL needs a cert or TA.  May want to have multiple
    // certs for things like a series of trusted CAs (but check the CertStore class
    // for a more memory efficient way).
    // Copies all associated data so no need to keep input PEM/DER certs.
    // All inputs can be either in RAM or PROGMEM.
    class X509List
    {
    public:
        X509List()
        {
            _count = 0;
            _cert = nullptr;
            _ta = nullptr;
        }
        explicit X509List(const char *pemCert)
        {
            _count = 0;
            _cert = nullptr;
            _ta = nullptr;
            append(pemCert);
        }
        explicit X509List(const uint8_t *derCert, size_t derLen)
        {
            _count = 0;
            _cert = nullptr;
            _ta = nullptr;
            append(derCert, derLen);
        }
        explicit X509List(Stream &stream, size_t size)
        {
            _count = 0;
            _cert = nullptr;
            _ta = nullptr;
            auto buff = key_bssl::loadStream(stream, size);
            if (buff)
            {
                append(buff, size);
                esp_sslclient_free(&buff);
            }
        }
        explicit X509List(Stream &stream) : X509List(stream, stream.available()) {};
        ~X509List()
        {
            key_bssl::free_certificates(_cert, _count); // also frees cert
            for (size_t i = 0; i < _count; i++)
            {
                key_bssl::free_ta_contents(&_ta[i]);
            }
            esp_sslclient_free(&_ta);
        }

        bool append(const char *pemCert) { return append(reinterpret_cast<const uint8_t *>(pemCert), strlen_P(pemCert)); }
        bool append(const uint8_t *derCert, size_t derLen)
        {
            size_t numCerts;
            br_x509_certificate *newCerts = key_bssl::read_certificates(reinterpret_cast<const char *>(derCert), derLen, &numCerts);
            if (!newCerts)
                return false;

            // Add in the certificates
            br_x509_certificate *saveCert = _cert;
            _cert = reinterpret_cast<br_x509_certificate *>(esp_sslclient_realloc(_cert, (numCerts + _count) * sizeof(br_x509_certificate)));
            if (!_cert)
            {
                esp_sslclient_free(&newCerts);
                _cert = saveCert;
                return false;
            }

            memcpy(&_cert[_count], newCerts, numCerts * sizeof(br_x509_certificate));
            esp_sslclient_free(&newCerts);

            // Build TAs for each certificate
            br_x509_trust_anchor *saveTa = _ta;
            _ta = reinterpret_cast<br_x509_trust_anchor *>(esp_sslclient_realloc(_ta, (numCerts + _count) * sizeof(br_x509_trust_anchor)));
            if (!_ta)
            {
                _ta = saveTa;
                return false;
            }
            for (size_t i = 0; i < numCerts; i++)
            {
                br_x509_trust_anchor *newTa = key_bssl::certificate_to_trust_anchor(&_cert[_count + i]);
                if (newTa)
                {
                    _ta[_count + i] = *newTa;
                    esp_sslclient_free(&newTa);
                }
                else
                    return false; // OOM
            }
            _count += numCerts;
            return true;
        }

        // Accessors
        size_t getCount() const { return _count; }

        const br_x509_certificate *getX509Certs() const { return _cert; }

        const br_x509_trust_anchor *getTrustAnchors() const { return _ta; }

        // Disable the copy constructor, we're pointer based
        explicit X509List(const X509List &that) = delete;
        X509List &operator=(const X509List &that) = delete;

    private:
        size_t _count;
        br_x509_certificate *_cert;
        br_x509_trust_anchor *_ta;
    };

#endif

    extern "C"
    {
        // Install hashes into the SSL engine
        static void br_ssl_client_install_hashes(br_ssl_engine_context *eng)
        {
            br_ssl_engine_set_hash(eng, br_md5_ID, &br_md5_vtable);
            br_ssl_engine_set_hash(eng, br_sha1_ID, &br_sha1_vtable);
            br_ssl_engine_set_hash(eng, br_sha224_ID, &br_sha224_vtable);
            br_ssl_engine_set_hash(eng, br_sha256_ID, &br_sha256_vtable);
            br_ssl_engine_set_hash(eng, br_sha384_ID, &br_sha384_vtable);
            br_ssl_engine_set_hash(eng, br_sha512_ID, &br_sha512_vtable);
        }

        static void br_x509_minimal_install_hashes(br_x509_minimal_context *x509)
        {
            br_x509_minimal_set_hash(x509, br_md5_ID, &br_md5_vtable);
            br_x509_minimal_set_hash(x509, br_sha1_ID, &br_sha1_vtable);
            br_x509_minimal_set_hash(x509, br_sha224_ID, &br_sha224_vtable);
            br_x509_minimal_set_hash(x509, br_sha256_ID, &br_sha256_vtable);
            br_x509_minimal_set_hash(x509, br_sha384_ID, &br_sha384_vtable);
            br_x509_minimal_set_hash(x509, br_sha512_ID, &br_sha512_vtable);
        }

        // Default initializion for our SSL clients
        static void br_ssl_client_base_init(br_ssl_client_context *cc, const uint16_t *cipher_list, int cipher_cnt)
        {
            uint16_t suites[cipher_cnt];
            memcpy_P(suites, cipher_list, cipher_cnt * sizeof(cipher_list[0]));
            br_ssl_client_zero(cc);
            br_ssl_engine_add_flags(&cc->eng, BR_OPT_NO_RENEGOTIATION); // forbid SSL renegotiation, as we free the Private Key after handshake
            br_ssl_engine_set_versions(&cc->eng, BR_TLS10, BR_TLS12);
            br_ssl_engine_set_suites(&cc->eng, suites, (sizeof suites) / (sizeof suites[0]));
            br_ssl_client_set_default_rsapub(cc);
            br_ssl_engine_set_default_rsavrfy(&cc->eng);
#ifndef BEARSSL_SSL_BASIC
            br_ssl_engine_set_default_ecdsa(&cc->eng);
#endif
            br_ssl_client_install_hashes(&cc->eng);
            br_ssl_engine_set_prf10(&cc->eng, &br_tls10_prf);
            br_ssl_engine_set_prf_sha256(&cc->eng, &br_tls12_sha256_prf);
            br_ssl_engine_set_prf_sha384(&cc->eng, &br_tls12_sha384_prf);
            br_ssl_engine_set_default_aes_cbc(&cc->eng);
#ifndef BEARSSL_SSL_BASIC
            br_ssl_engine_set_default_aes_gcm(&cc->eng);
            br_ssl_engine_set_default_aes_ccm(&cc->eng);
            br_ssl_engine_set_default_des_cbc(&cc->eng);
            br_ssl_engine_set_default_chapol(&cc->eng);
#endif
        }

        // BearSSL doesn't define a true insecure decoder, so we make one ourselves
        // from the simple parser.  It generates the issuer and subject hashes and
        // the SHA1 fingerprint, only one (or none!) of which will be used to
        // "verify" the certificate.

        // Private x509 decoder state
        struct br_x509_insecure_context
        {
            const br_x509_class *vtable;
            bool done_cert;
            const uint8_t *match_fingerprint;
            br_sha1_context sha1_cert;
            bool allow_self_signed;
            br_sha256_context sha256_subject;
            br_sha256_context sha256_issuer;
            br_x509_decoder_context ctx;
        };

        // Callback for the x509_minimal subject DN
        static void insecure_subject_dn_append(void *ctx, const void *buf, size_t len)
        {
            br_x509_insecure_context *xc = reinterpret_cast<br_x509_insecure_context *>(ctx);
            br_sha256_update(&xc->sha256_subject, buf, len);
        }

        // Callback for the x509_minimal issuer DN
        static void insecure_issuer_dn_append(void *ctx, const void *buf, size_t len)
        {
            br_x509_insecure_context *xc = reinterpret_cast<br_x509_insecure_context *>(ctx);
            br_sha256_update(&xc->sha256_issuer, buf, len);
        }

        // Callback for each certificate present in the chain (but only operates
        // on the first one by design).
        static void insecure_start_cert(const br_x509_class **ctx, uint32_t length)
        {
            (void)ctx;
            (void)length;
        }

        // Callback for each byte stream in the chain.  Only process first cert.
        static void insecure_append(const br_x509_class **ctx, const unsigned char *buf, size_t len)
        {
            br_x509_insecure_context *xc = reinterpret_cast<br_x509_insecure_context *>(ctx);
            // Don't process anything but the first certificate in the chain
            if (!xc->done_cert)
            {
                br_sha1_update(&xc->sha1_cert, buf, len);
                br_x509_decoder_push(&xc->ctx, reinterpret_cast<const void *>(buf), len);
            }
        }
        // Callback on the first byte of any certificate
        static void insecure_start_chain(const br_x509_class **ctx, const char *server_name)
        {
            br_x509_insecure_context *xc = reinterpret_cast<br_x509_insecure_context *>(ctx);
#if defined(USE_EMBED_SSL_ENGINE)
            br_x509_decoder_init(&xc->ctx, insecure_subject_dn_append, xc, insecure_issuer_dn_append, xc);
#elif defined(ESP32) || defined(USE_LIB_SSL_ENGINE)
            br_x509_decoder_init(&xc->ctx, insecure_subject_dn_append, xc);
#endif
            xc->done_cert = false;
            br_sha1_init(&xc->sha1_cert);
            br_sha256_init(&xc->sha256_subject);
            br_sha256_init(&xc->sha256_issuer);
            (void)server_name;
        }

        // Callback on individual cert end.
        static void insecure_end_cert(const br_x509_class **ctx)
        {
            br_x509_insecure_context *xc = reinterpret_cast<br_x509_insecure_context *>(ctx);
            xc->done_cert = true;
        }

        // Callback when complete chain has been parsed.
        // Return 0 on validation success, !0 on validation error
        static unsigned insecure_end_chain(const br_x509_class **ctx)
        {
            const br_x509_insecure_context *xc = reinterpret_cast<const br_x509_insecure_context *>(ctx);
            if (!xc->done_cert)
            {
                // BSSL_BSSL_SSL_Client_DEBUG_PRINTF("insecure_end_chain: No cert seen\n");
                return 1; // error
            }

            // Handle SHA1 fingerprint matching
            char res[20];
            br_sha1_out(&xc->sha1_cert, res);
            if (xc->match_fingerprint && memcmp(res, xc->match_fingerprint, sizeof(res)))
                return BR_ERR_X509_NOT_TRUSTED;

            // Handle self-signer certificate acceptance
            char res_issuer[32];
            char res_subject[32];
            br_sha256_out(&xc->sha256_issuer, res_issuer);
            br_sha256_out(&xc->sha256_subject, res_subject);
            if (xc->allow_self_signed && memcmp(res_subject, res_issuer, sizeof(res_issuer)))
            {
                // BSSL_BSSL_SSL_Client_DEBUG_PRINTF("insecure_end_chain: Didn't get self-signed cert\n");
                return BR_ERR_X509_NOT_TRUSTED;
            }

            // Default (no validation at all) or no errors in prior checks = success.
            return 0;
        }

        // Return the public key from the validator (set by x509_minimal)
        static const br_x509_pkey *insecure_get_pkey(const br_x509_class *const *ctx, unsigned *usages)
        {
            const br_x509_insecure_context *xc = reinterpret_cast<const br_x509_insecure_context *>(ctx);
            if (usages != nullptr)
                *usages = BR_KEYTYPE_KEYX | BR_KEYTYPE_SIGN; // I said we were insecure!
            return &xc->ctx.pkey;
        }
    }

};
#endif

#endif