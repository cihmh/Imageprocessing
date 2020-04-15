#include <iostream>
#include <string.h>
using namespace std;
void RGB2YUV420(unsigned char *pBuffer, unsigned char *yuv420frame, int width, int height)
{
    const int rgbBufferSize = 3 * height * width;
    unsigned char *RGBBuffer = new unsigned char[rgbBufferSize];
    const int YBufferSize = height * width;
    const int UVBufferSize = height * width / 4;
    unsigned char *YBuffer = new unsigned char[YBufferSize];
    unsigned char *UBuffer = new unsigned char[UVBufferSize];
    unsigned char *VBuffer = new unsigned char[UVBufferSize];
    unsigned char *temp_U = (new unsigned char[YBufferSize]);
    unsigned char *temp_V = (new unsigned char[YBufferSize]);
    unsigned char R, G, B;
    unsigned char Y, U, V;
    memcpy(RGBBuffer, pBuffer, rgbBufferSize);

    for (int line = 0; line < height; ++line)
    {
        int RGBIndex = 3 * width * line;
        int YIndex = width * line;
        for (int pixel = 0; pixel < width; ++pixel)
        {
            R = RGBBuffer[RGBIndex++];
            G = RGBBuffer[RGBIndex++];
            B = RGBBuffer[RGBIndex++];
            Y = 0.299 * R + 0.587 * G + 0.114 * B + 0.5;
            U = (-0.147 * R - 0.289 * G + 0.436 * B) / 0.872 + 128;
            V = (0.615 * R - 0.515 * G - 0.100 * B) / 1.230 + 128;
            YBuffer[YIndex] = static_cast<unsigned char>((Y < 0) ? 0 : ((Y > 255) ? 255 : Y));
            temp_U[YIndex] = static_cast<unsigned char>((U < 0) ? 0 : ((U > 255) ? 255 : U));
            temp_V[YIndex++] = static_cast<unsigned char>((V < 0) ? 0 : ((V > 255) ? 255 : V));
        }
    }

    for (int line = 0; line < height; line += 2)
    {
        int UVIndex = line * width / 4;
        for (int pixel = 0; pixel < width; pixel += 2)
        {
            U = (temp_U[line * width + pixel]) ;
            V = (temp_V[line * width + pixel]);
            UBuffer[UVIndex] = U;
            VBuffer[UVIndex++] = V;
        }
    }
/*
*****************************************************************************************************
It was calculated as the average value of adjacent pixels. However, the results are not so different.
*****************************************************************************************************
*/
//  for (int line = 0; line < height; line += 2)
//     {
//         int UVIndex = line * width / 4;
        
//         for (int pixel = 0; pixel < width; pixel += 2)
//         {
//             int idx1=line * width+pixel;
//             int idx2=idx1+1;
//             int idx3=idx1+width;
//             int idx4=idx3+1;
//             U = (temp_U[idx1]+temp_U[idx2]+temp_U[idx3]+temp_U[idx4])>>2;
//             V = (temp_V[idx1]+ temp_V[idx2]+temp_V[idx3]+ temp_V[idx4])>>2;
//             UBuffer[UVIndex] = static_cast<unsigned char>((U < 0) ? 0 : ((U > 255) ? 255 : U));
//             VBuffer[UVIndex++] = static_cast<unsigned char>((V < 0) ? 0 : ((V > 255) ? 255 : V));
//         }
//     }
    memcpy(yuv420frame, YBuffer, YBufferSize);
    memcpy(yuv420frame + YBufferSize, UBuffer, UVBufferSize);
    memcpy(yuv420frame + YBufferSize + UVBufferSize, VBuffer, UVBufferSize);

    delete[] temp_V;
    delete[] temp_U;
    delete[] VBuffer;
    delete[] UBuffer;
    delete[] YBuffer;
    delete[] RGBBuffer;
}
int main()
{
    int image_width = 698;
    int image_height = 400;
    FILE *fp = fopen("./RGB_Image.rgb", "rb");
    fseek(fp, 0, SEEK_END);
    int len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    int yuv_frame_size=image_height * image_width * 3 / 2;//like (image_width * image_height + (image_width * image_height) / 2)
    unsigned char *buf = new unsigned char[len];                                                          
    unsigned char *yuvbuf = new unsigned char[yuv_frame_size];

    fread(buf, len, 1, fp);
    fclose(fp);
    RGB2YUV420(buf, yuvbuf, image_width, image_height);
    FILE *outputfile;
    outputfile = fopen("./yuv420.yuv", "wb");
    fwrite(yuvbuf, yuv_frame_size, 1, outputfile);
    fclose(outputfile);
    delete[] buf;
    delete[] yuvbuf;
    return 0;
}
