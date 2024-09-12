#include <windows.h>
#include <stdio.h>

unsigned char* LoadBitmapFile(const char* filename, BITMAPINFOHEADER* bitmapInfoHeader)
{
    FILE* filePtr;                     // file pointer
    BITMAPFILEHEADER bitmapFileHeader; // bitmap file header
    unsigned char* bitmapImage;        // image data
    unsigned int imageIdx = 0;         // image index counter
    unsigned char tempRGB;             // swap variable

    // Open filename in read binary mode using fopen_s
    if (fopen_s(&filePtr, filename, "rb") != 0)
        return NULL;

    if (filePtr == NULL)
        return NULL;

    fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr); // read the bitmap file header

    if (bitmapFileHeader.bfType != 0x4D42)   // verify that this is a bmp file
    {
        fclose(filePtr);
        return NULL;
    }

    fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);  // read the bitmap info header
    printf("%i  %i  ", bitmapInfoHeader->biWidth, bitmapInfoHeader->biHeight);

    fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET); // set file pointer to start of bitmap data

    bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage); // allocate memory for bitmap image

    if (!bitmapImage) // verify memory allocation
    {
        free(bitmapImage);
        fclose(filePtr);
        return NULL;
    }

    fread(bitmapImage, bitmapInfoHeader->biSizeImage, 1, filePtr);  // read the bitmap image data

    if (bitmapImage == NULL)  // make sure bitmap image data was read
    {
        fclose(filePtr);
        return NULL;
    }

    // swap the r and b values to get RGB (bitmap is BGR)
    for (imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3)
    {
        tempRGB = bitmapImage[imageIdx];
        bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
        bitmapImage[imageIdx + 2] = tempRGB;
    }

    fclose(filePtr);
    return bitmapImage;
}
