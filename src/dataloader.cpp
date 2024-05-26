#include "dataLoader.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <random>

DataLoader::DataLoader(std::string path, int x, int y, int z, int datatype, bool hashing=false) {
    this->size = x;
    // get the file type
    std::string type = path.substr(path.length()-3, 3);
    std::string AbsPath;
    if (type != "raw" && type != "RAW") {
        std::cout<<"Invalid data type."<<std::endl;
        exit(-1);
    } else {
        AbsPath = getPath(path);
        std::cout<<"Detect raw file : read from " << AbsPath << std::endl;
    }
    
    FILE *fp;
    int size = x * y * z;
    fp = fopen(path.c_str(), "rb");
    // uint8_t
    // an attempt for perfect hash encoding
    if (hashing == true) {
        std::cout << "Uint8 datatype loaded and use PSH." << std::endl;
        GLubyte *rawdata = new GLubyte[size];
        fread(rawdata, sizeof(char), size, fp);
        fclose(fp);

        // PSH
        PSH(rawdata);
    }
    else if (datatype == 1) {
        std::cout << "Uint8 datatype loaded." << std::endl;
        GLubyte *rawdata = new GLubyte[size];
        fread(rawdata, sizeof(char), size, fp);
        fclose(fp);
        GLuint RawTex;
        glGenTextures(1, &RawTex);
        glBindTexture(GL_TEXTURE_3D, RawTex);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_INTENSITY, x, y, z, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, rawdata);
        delete[] rawdata;
        texture = RawTex;
    } 

    // uint16_t
    else if (datatype == 2) {
        GLushort *rawdata16 = new GLushort[size];
        fread(rawdata16, sizeof(short), size, fp);
        fclose(fp);

        // find the max and min value
        uint16_t maxVal = 0;
        uint16_t minVal = UINT16_MAX;
        for (size_t i = 0; i < size; ++i) {
            if (rawdata16[i] > maxVal) maxVal = rawdata16[i];
            if (rawdata16[i] < minVal) minVal = rawdata16[i];
        }
        // transform to uint8
        GLubyte *rawdata = new GLubyte[size];
        for (size_t i = 0; i < size; ++i) {
            // shift right 8 bits to transform
            rawdata[i] = static_cast<GLubyte>(255.0 * (rawdata16[i] - minVal) / (maxVal - minVal));
        }
        std::cout << "convert done" << std::endl;
        delete[] rawdata16; 

        GLuint RawTex;
        glGenTextures(1, &RawTex);
        glBindTexture(GL_TEXTURE_3D, RawTex);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_INTENSITY, x, y, z, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, rawdata);
        // in case of memory leak
        delete[] rawdata;
        texture = RawTex;
    }

    // float32
    else if (datatype == 3) {
        GLfloat *rawdata32 = new GLfloat[size];
        fread(rawdata32, sizeof(float), size, fp);
        fclose(fp);
        // Find the maximum value for normalization
        uint16_t maxVal = 0;
        uint16_t minVal = FLT_MAX;
        for (size_t i = 0; i < size; ++i) {
            if (rawdata32[i] > maxVal) maxVal = rawdata32[i];
            if (rawdata32[i] < minVal) minVal = rawdata32[i];
        }
      
        // Allocate memory for float32 data
        GLubyte *rawdata = new GLubyte[size];

        // Normalize and convert the data to uint8
        for (int i = 0; i < size; i++) {
            rawdata[i] = static_cast<GLubyte>(255.0 * (rawdata32[i] - minVal) / (maxVal - minVal));
        }
        std::cout << "convert done" << std::endl;
        delete[] rawdata32;
        GLuint RawTex;
        glGenTextures(1, &RawTex);
        glBindTexture(GL_TEXTURE_3D, RawTex);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_INTENSITY, x, y, z, 0, GL_LUMINANCE,  GL_UNSIGNED_BYTE, rawdata);
        // in case of memory leak
        delete[] rawdata;
        texture = RawTex;
    }
    else {
        GLshort *rawdata = new GLshort[size];
        fread(rawdata, sizeof(int), size, fp);
        fclose(fp);
        GLuint RawTex;
        glGenTextures(1, &RawTex);
        glBindTexture(GL_TEXTURE_3D, RawTex);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_INTENSITY, x, y, z, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, rawdata);
        // in case of memory leak
        delete[] rawdata;
        texture = RawTex;
    }
}

int h(int p, int size, int prime) {
    return (p * prime) % size;
}

bool contains(const std::vector<int>& vec, int value) {
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}


glm::vec3 genRandomOffset(int x, int y, int z, GLubyte* hashTable, int m) {
    glm::vec3 offset = glm::vec3(0, 0, 0);
    glm::vec3 index = glm::vec3(x, y, z);

    const std::vector<glm::vec3> directions = {
        glm::vec3(1, 0, 0), glm::vec3(-1, 0, 0),
        glm::vec3(0, 1, 0), glm::vec3(0, -1, 0),
        glm::vec3(0, 0, 1), glm::vec3(0, 0, -1)
    };

    for (auto dir: directions) {
        // index += dir;
        // if (hashTable[int(index.x + m * (index.y + m * index.z))] == 0) {
        //     offset = dir;
        //     return offset;
        // }
        return offset;
    }

    return offset;
}

void DataLoader::PSH(GLubyte* rawdata) {
    // filter the data
    int voxelnum = 0;
    const GLubyte thresholdValue = static_cast<GLubyte>(0.1 * 255);
    // count the number of voxels
    for (int i = 0; i < size * size * size; ++i) {
        if (rawdata[i] > thresholdValue) {
            voxelnum++;
        }
    }

    // calculate hash table size m and offset table size n
    int m = std::ceil(std::cbrt(voxelnum * 1.2));
    std::cout << "Set hash table size to be " << m << std::endl;
    int n = std::ceil(std::cbrt(voxelnum / 6));
    std::cout << "Set offset table size to be " << n << std::endl;

    // init hash table and offset table
    GLubyte* hashTable = new GLubyte[m * m * m];
    std::memset(hashTable, 0, m * m * m * sizeof(GLubyte));
    GLubyte* offsetTable = new GLubyte[n * n * n];
    std::memset(offsetTable, 0, n * n * n * sizeof(GLubyte));

    int prime1 = 37;
    int prime2 = 47;

    for (int i = 0; i < pow(size, 3); ++i) {
        if (rawdata[i] > thresholdValue) {
            // printf("%d\n", rawdata[i]);
            int hashIndex = std::abs(h(i, m * m * m, prime1));
            int offsetIndex = std::abs(h(i, n * n * n, prime2));
            // printf("hashIndex: %d, offsetIndex: %d\n", hashIndex, offsetIndex);

            // Calculate three-dimensional coordinates
            int hashX = hashIndex % m;
            int hashY = (hashIndex / m) % m;
            int hashZ = hashIndex / (m * m);

            // check collision
            // if (hashTable[hashX + m * (hashY + m * hashZ)] != 0) {
            //     printf("Collision detected!\n");
            glm::vec3 offset = genRandomOffset(hashX, hashY, hashZ, hashTable, m);
            hashX += offset.x;
            hashY += offset.y;
            hashZ += offset.z;

            int offsetX = offsetIndex % n;
            int offsetY = (offsetIndex / n) % n;
            int offsetZ = offsetIndex / (n * n);

            hashTable[hashX + m * (hashY + m * hashZ)] = rawdata[i];
            offsetTable[offsetX + n * (offsetY + n * offsetZ)] = offset.x * 4 + offset.y * 2 + offset.z;

            // // Set values in hash table and offset table
            // hashTable[hashX + m * (hashY + m * hashZ)] = rawdata[i];
            // offsetTable[offsetX + n * (offsetY + n * offsetZ)] = rawdata[i];
        }
    }

    GLuint RawTex;
    glGenTextures(1, &RawTex);
    glBindTexture(GL_TEXTURE_3D, RawTex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_INTENSITY, size, size, size, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, rawdata);
    delete[] rawdata;
    this->texture = RawTex;
    std::cout << "Raw data texture build done." << std::endl;

    GLuint HashTex;
    glGenTextures(1, &HashTex);
    glBindTexture(GL_TEXTURE_3D, HashTex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_INTENSITY, m, m, m, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, hashTable);
    delete[] hashTable;
    this->hash_texture = HashTex;
    std::cout << "Hash table build done." << std::endl;

    GLuint OffsetTex;
    glGenTextures(1, &OffsetTex);
    glBindTexture(GL_TEXTURE_3D, OffsetTex);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_INTENSITY, n, n, n, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, offsetTable);
    delete[] offsetTable;
    this->offset_texture = OffsetTex;
    std::cout << "Offset table build done." << std::endl;
}


