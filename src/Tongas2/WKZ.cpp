// .wkz files and the streams they are read from and written to.
#include <new>
#include <stdio.h>
#include <string.h>
#include <io.h>

#include "lib/zlib/zlib.h"
#include "Tongas2/WKZ.h"

#define CHUNK 0x4000

int WKZSTREAM::Read(void *buffer, int size)
{
    if (m_fp) {
        int n = fread(buffer, 1, size, m_fp);
        m_pos += n;
        return n;
    }
    if (size > m_size - m_pos)
        size = m_size - m_pos;
    memcpy(buffer, m_data + m_pos, size);
    m_pos += size;
    return size;
}

int WKZSTREAM::Write(const void *buffer, int size)
{
    if (m_fp) {
        int n = fwrite(buffer, 1, size, m_fp);
        m_pos += n;
        return n;
    }
    if (size > m_size - m_pos)
        size = m_size - m_pos;
    memcpy(m_data + m_pos, buffer, size);
    m_pos += size;
    return size;
}

// zlib's zpipe.c example, on WKZSTREAMs.
int WKZ::Compress(WKZSTREAM *source, WKZSTREAM *dest, int level)
{
    int ret, flush;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    dest->Write("WKZ", 3);
    int size = source->m_size;
    dest->Reserve((int)(size * 1.05f));
    dest->Write(&size, 4);

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, level);
    if (ret != Z_OK)
        return ret;

    do {
        strm.avail_in = source->Read(in, CHUNK);
        // The original keeps the error in flush (it shows in the stack).
        flush = source->Error();
        if (flush) {
            deflateEnd(&strm);
            return Z_ERRNO;
        }
        flush = source->Eof() ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            deflate(&strm, flush);
            have = CHUNK - strm.avail_out;
            if (dest->Write(out, have) != have || dest->Error()) {
                deflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);
    } while (flush != Z_FINISH);

    deflateEnd(&strm);
    return Z_OK;
}

int WKZ::Decompress(WKZSTREAM *source, WKZSTREAM *dest)
{
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    source->Read(in, 3);
    if (memcmp(in, "WKZ", 3))
        return WKZ_NO_SIGNATURE;
    int size;
    source->Read(&size, 4);
    dest->Reserve(size);

    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;

    do {
        strm.avail_in = source->Read(in, CHUNK);
        if (source->Error()) {
            inflateEnd(&strm);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                inflateEnd(&strm);
                return ret;
            }
            have = CHUNK - strm.avail_out;
            if (dest->Write(out, have) != have || dest->Error()) {
                inflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);
    } while (ret != Z_STREAM_END);

    inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

const char *WKZ::ErrorString(int error)
{
    switch (error) {
    case Z_ERRNO:
        return "error in stream";
    case Z_STREAM_ERROR:
        return "invalid compression level";
    case Z_DATA_ERROR:
        return "invalid or incomplete deflate data";
    case Z_MEM_ERROR:
        return "out of memory\n";
    case Z_VERSION_ERROR:
        return "zlib version mismatch!";
    case WKZ_NOT_FOUND:
        return "file not found!";
    case WKZ_NO_SIGNATURE:
        return "no signature found!";
    default:
        return "unknown error";
    }
}

int WKZ::Load(char *name, char **data, int *size, int level, bool compress)
{
    WKZSTREAM *source = new WKZSTREAM;
    WKZSTREAM *dest = new WKZSTREAM;
    int ret;
    source->m_fp = fopen(name, "rb");
    if (!source->m_fp) {
        ret = WKZ_NOT_FOUND;
    }
    else {
        source->m_size = _filelength(_fileno(source->m_fp));
        if (compress)
            ret = Compress(source, dest, level);
        else
            ret = Decompress(source, dest);
    }
    *data = dest->m_data;
    *size = dest->m_pos;
    if (source->m_fp)
        fclose(source->m_fp);
    delete source;
    if (dest->m_fp)
        fclose(dest->m_fp);
    delete dest;
    return ret;
}
