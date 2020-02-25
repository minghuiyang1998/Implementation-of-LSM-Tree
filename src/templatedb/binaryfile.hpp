#ifndef _TEMPLATEDB_BINARY_FILE_H_
#define _TEMPLATEDB_BINARY_FILE_H_

/*
 *
 * Heavily inspired by nkansal96 on GitHub
 * https://github.com/nkansal96/disk-multi-map
 * 
 */

#include <iostream>
#include <fstream>
#include <string>

namespace templatedb
{

class BinaryFile
{
public:
    BinaryFile() {};
    ~BinaryFile() {stream.close();};

    void open(const std::string & fname)
    {
        stream.open(fname, std::ios::in | std::ios::out | std::ios::binary);
    }

    void close() 
    {
        if (stream.is_open())
            stream.close();
    };

    template<typename T>
    bool read(T & data, std::streamoff offset)
    {
        return read(reinterpret_cast<char *> (&data), sizeof(data), offset);
    }

    template<typename T>
    bool write(const T & data, std::streamoff offset)
    {
        return write(reinterpret_cast<const char *> (&data), sizeof(data), offset);
    }

    bool write(const char * data, size_t size, std::streamoff offset)
    {
        return stream.seekp(offset, std::ios::beg) && stream.write(data, size);
    }

private:
    std::fstream stream;
};

}


#endif /* _TEMPLATEDB_BINARY_FILE_H_ */