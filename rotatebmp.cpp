#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#pragma pack(push, 1) // 防止对齐，BMP文件不对齐
struct BMPHeader { // 文件头 14
    uint16_t bfType;      // 文件类型，必须是0x4D42
    uint32_t bfSize;      // 文件大小
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;   // 像素数据的起始位置
};
struct BMPInfoHeader { // 信息头 40
    uint32_t biSize;        // 信息头大小
    int32_t biWidth;        // 图像宽度
    int32_t biHeight;       // 图像高度
    uint16_t biPlanes;      // 色彩平面数，必须为1
    uint16_t biBitCount;    // 每个像素的位数（24位即每个像素3个字节）
    uint32_t biCompression; // 压缩方式
    uint32_t biSizeImage;   // 图像数据的大小
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
#pragma pack(pop)

void rotateClockwise90(vector<uint8_t> &inputImage, int32_t height, int32_t width, uint16_t byteCount, vector<uint8_t> &outputImage) {
    int newHeight = width, newWidth = height;
    int padding = (4 - width * byteCount % 4) % 4, newPadding = (4 - newWidth * byteCount % 4) % 4;
    outputImage.resize(newHeight * (newWidth * byteCount + newPadding));
    for (int i = 0; i < height; ++i)
        for (int j = 0; j < width; ++j) {
            int newI = j, newJ = newWidth - 1 - i;
            int iter = (height - 1 - i) * (width * byteCount + padding) + j * byteCount, newIter = (newHeight - 1 - newI) * (newWidth * byteCount + newPadding) + newJ * byteCount;
            for (int k = 0; k < byteCount; ++ k)
                outputImage[newIter + k] = inputImage[iter + k];
        }
}

int main(int argc, char* argv[]) {
    ifstream inputFile(argv[1], ios :: in | ios :: binary);
    ofstream outputFile(argv[2], ios :: out | ios :: binary);
    if (!inputFile) {
        cerr << "[Error] Unable to open input file" << endl;
        return 0;
    }
    if (!outputFile) {
        cerr << "[Error] Unable to open output file" << endl;
        return 0;
    }
    BMPHeader header;
    BMPInfoHeader infoHeader;
    inputFile.read((char*)&header, sizeof(BMPHeader));
    inputFile.read((char*)&infoHeader, sizeof(BMPInfoHeader));
    if (header.bfType != 0x4D42) {
        cerr << "[Error] Invalid input type" << endl;
        inputFile.close();
        return 0;
    }
    if (infoHeader.biBitCount != 24 && infoHeader.biBitCount != 32) {
        cerr << "[Error] Not 24-bit or 32-bit BMP" << endl;
        inputFile.close();
        return 0;
    }
    if (!infoHeader.biSizeImage) {
        int byteCount = infoHeader.biBitCount / 8;
        int padding = (4 - infoHeader.biWidth * byteCount % 4) % 4;
        infoHeader.biSizeImage = infoHeader.biHeight * (infoHeader.biWidth * byteCount + padding);
    }
    inputFile.seekg(header.bfOffBits, ios :: beg);
    vector<uint8_t> inputImage(infoHeader.biSizeImage), outputImage;
    inputFile.read((char*)inputImage.data(), infoHeader.biSizeImage);

    rotateClockwise90(inputImage, infoHeader.biHeight, infoHeader.biWidth, infoHeader.biBitCount / 8, outputImage);

    BMPHeader newHeader = header;
    BMPInfoHeader newInfoHeader = infoHeader;
    newInfoHeader.biHeight = infoHeader.biWidth, newInfoHeader.biWidth = infoHeader.biHeight;
    if (infoHeader.biSize == 40){
        newHeader.bfSize = sizeof(BMPHeader) + sizeof(BMPInfoHeader) + newInfoHeader.biSizeImage;
        outputFile.write((char*)&newHeader, sizeof(BMPHeader));
        outputFile.write((char*)&newInfoHeader, sizeof(BMPInfoHeader));
        outputFile.write((char*)outputImage.data(), outputImage.size());
    }
    else if (infoHeader.biSize == 108 || infoHeader.biSize == 124) {
        inputFile.seekg(sizeof(BMPHeader) + sizeof(BMPInfoHeader), ios :: beg);
        vector<uint8_t> temporary_info(infoHeader.biSize - 40);
        inputFile.read((char*)temporary_info.data(), infoHeader.biSize - 40);
        newHeader.bfSize = sizeof(BMPHeader) + sizeof(BMPInfoHeader) + (infoHeader.biSize - 40) + newInfoHeader.biSizeImage;
        outputFile.write((char*)&newHeader, sizeof(BMPHeader));
        outputFile.write((char*)&newInfoHeader, sizeof(BMPInfoHeader));
        outputFile.write((char*)temporary_info.data(), infoHeader.biSize - 40);
        outputFile.write((char*)outputImage.data(), outputImage.size());
    }
    else {
        cerr << "[Error] Invalid infoheader" << endl;
        inputFile.close();
        outputFile.close();
        return 0;
    }
    inputFile.close();
    outputFile.close();
    cout << "[Finished]Rotated successfully" << endl;
    return 0;
}