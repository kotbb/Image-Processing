// File Name : CS112_A3_Part2A_S28_20230315_20230058_20230213.cpp
// Purpose   : Image filters

// Author 1  : Mohamed Ahmed Kotb  - Email: kotb200484@gmail.com
// ID 1      : 20230315 and he makes 3,6,9,12 filters and menu

// Author 2  : Eslam Gabr Abd-El-Azim  - Email: eslamjabr55@gmail.com
// ID 2      : 20230058 and he makes 1,4,7,10 filters

// Author 3  : Abdelrahman Mohamed Ahmed El-Gaml  - Email: abdulrahmanelgaml89@gmail.com
// ID 3      : 20230213 and he makes 2,5,8,11 filters

// The link of the diagram:
// https://drive.google.com/file/d/1hAUGrUxfEbHFz-UI_x5bk9mf_8gMmevk/view?usp=sharing

#include "Image_Class.h"
#include <string>
#include <vector>
#include <cmath>
using namespace std;
// Simple resize function to resize an image
void resizeImage(Image& image, int newWidth, int newHeight) {
    // Create a new image with the desired dimensions
    Image resizedImage(newWidth, newHeight);

    // Iterate over the pixels in the resized image
    for (int i = 0; i < newWidth; ++i) {
        for (int j = 0; j < newHeight; ++j) {
            for (int k = 0; k < image.channels; ++k) {
                // Calculate the corresponding position in the original image
                int originalX = i * image.width / newWidth;
                int originalY = j * image.height / newHeight;

                // Set the pixel value in the resized image to the corresponding value in the original image
                resizedImage(i, j, k) = image(originalX, originalY, k);
            }
        }
    }
    // Update the original image with the resized image
    image = resizedImage;
}
void Edges(Image& image,string& new_image_name,int& check_convert)
{
    Image grayImage(image.width, image.height); // Create a new image to store the grayscale version of the image

    // Convert the image to grayscale
    unsigned int avg = 0;
    for (int i = 0; i < image.width; ++i) {
        for (int j = 0; j < image.height; ++j) {
            for (int k = 0; k < image.channels; ++k) {
                avg += image(i, j, k);
            }
            // Turn the image into grayscale
            avg = avg / 3;
            for (int k = 0; k < image.channels; ++k) {
                grayImage(i, j, k) = avg;
            }
        }
    }

    // Create a new image to store the edges
    Image edges(grayImage.width, grayImage.height);

    // Apply the Sobel edge detection algorithm to the grayscale image
    for (int i = 1; i < grayImage.width - 1; ++i) {
        for (int j = 1; j < grayImage.height - 1; j++) {
            int gx =
                    -1 * grayImage(i - 1, j - 1, 0) + grayImage(i + 1, j - 1, 0)
                    -2 * grayImage(i - 1, j, 0) + 2 * grayImage(i + 1, j, 0)
                    -1 * grayImage(i - 1, j + 1, 0) + grayImage(i + 1, j + 1, 0);

            int gy =
                    -1 * grayImage(i - 1, j - 1, 0) - 2 * grayImage(i, j - 1, 0) - 1 * grayImage(i + 1, j - 1, 0)
                    + 1 * grayImage(i - 1, j + 1, 0) + 2 * grayImage(i, j + 1, 0) + 1 * grayImage(i + 1, j + 1, 0);

            int magnitude = sqrt(gx * gx + gy * gy);

            // Store the edge magnitude in the new image
            edges(i, j, 0) = magnitude;
        }
    }

    // Convert black to white and white to black
    for (int i = 0; i < edges.width; ++i) {
        for (int j = 0; j < edges.height; j++) {
            int threshold = 128; // Threshold value
            edges(i, j, 0) = (edges(i, j, 0) > threshold) ? 0 : 255; // Invert the pixel value
        }
    }

    // Convert the final image to grayscale
    for (int i = 0; i < edges.width; ++i) {
        for (int j = 0; j < edges.height; j++) {
            int grayValue = edges(i, j, 0);
            edges(i, j, 0) = grayValue;
            edges(i, j, 1) = grayValue;
            edges(i, j, 2) = grayValue;
        }
    }

    check_convert = edges.saveImage(new_image_name);

}
void Darken_Lighten (Image& image, string& new_image_name, int& check_convert)
{

    int brightnessLevel;
    do {
        cout << "Enter brightness level (0-100, 0=darker, 100=lighter): ";
        cin >> brightnessLevel;
    } while (brightnessLevel < 0 || brightnessLevel > 100);
    double adjustmentFactor = (double)brightnessLevel / 100.0;

    for (int i = 0; i < image.width; ++i) {
        for (int j = 0; j < image.height; ++j) {
            for (int k = 0; k < image.channels; ++k) {
                int originalValue = image(i, j, k);
                int adjustedValue;

                if (brightnessLevel < 50) {
                    adjustedValue = originalValue * adjustmentFactor;
                } else {
                    adjustedValue = (int)(originalValue * (1.0 + adjustmentFactor));
                }

                adjustedValue = std::min(255, max(0, adjustedValue));

                image(i, j, k) = adjustedValue;
            }
        }
    }
    check_convert = image.saveImage(new_image_name);
}
void Blur(const Image& image, string& new_image_name, int& check_convert) {
    // Create a copy of the input image to store the blurred result
    Image blurredImage = image;
    int blur_radius = 40;
    // Compute the prefix sum 2D array
    vector<vector<pixel>> prefixSumTable(image.height, vector<pixel>(image.width * 3));

    // Compute the first row and column of the prefix sum table
    for (int y = 0; y < image.height; ++y) {
        for (int x = 0; x < image.width; ++x) {
            pixel sumR = image(x, y, 0);
            pixel sumG = image(x, y, 1);
            pixel sumB = image(x, y, 2);

            if (y > 0) {
                sumR += prefixSumTable[y - 1][x * 3];
                sumG += prefixSumTable[y - 1][x * 3 + 1];
                sumB += prefixSumTable[y - 1][x * 3 + 2];
            }

            if (x > 0) {
                sumR += prefixSumTable[y][x * 3 - 3];
                sumG += prefixSumTable[y][x * 3 - 2];
                sumB += prefixSumTable[y][x * 3 - 1];
            }

            if (y > 0 && x > 0) {
                sumR -= prefixSumTable[y - 1][x * 3 - 3];
                sumG -= prefixSumTable[y - 1][x * 3 - 2];
                sumB -= prefixSumTable[y - 1][x * 3 - 1];
            }

            prefixSumTable[y][x * 3] = sumR;
            prefixSumTable[y][x * 3 + 1] = sumG;
            prefixSumTable[y][x * 3 + 2] = sumB;
        }
    }

    // Iterate over each pixel in the image
    for (int y = 0; y < image.height; ++y) {
        for (int x = 0; x < image.width; ++x) {
            // Compute the coordinates of the neighborhood rectangle
            int startX = max(0, x - blur_radius);
            int startY = max(0, y - blur_radius);
            int endX = min(image.width - 1, x + blur_radius);
            int endY = min(image.height - 1, y + blur_radius);

            // Compute the total number of pixels in the neighborhood
            int numPixels = (endX - startX + 1) * (endY - startY + 1);

            // Initialize sums for each color channel
            pixel sumR = prefixSumTable[endY][endX * 3] - prefixSumTable[startY][endX * 3] - prefixSumTable[endY][startX * 3] + prefixSumTable[startY][startX * 3];
            pixel sumG = prefixSumTable[endY][endX * 3 + 1] - prefixSumTable[startY][endX * 3 + 1] - prefixSumTable[endY][startX * 3 + 1] + prefixSumTable[startY][startX * 3 + 1];
            pixel sumB = prefixSumTable[endY][endX * 3 + 2] - prefixSumTable[startY][endX * 3 + 2] - prefixSumTable[endY][startX * 3 + 2] + prefixSumTable[startY][startX * 3 + 2];

            // Compute the average pixel value for each color channel
            pixel averageR = sumR / numPixels;
            pixel averageG = sumG / numPixels;
            pixel averageB = sumB / numPixels;

            // Update the pixel value in the blurred image for each channel
            blurredImage.setPixel(x, y, 0, averageR);
            blurredImage.setPixel(x, y, 1, averageG);
            blurredImage.setPixel(x, y, 2, averageB);
        }
    }

    check_convert = blurredImage.saveImage(new_image_name);
}

void Fancy_Frame(Image& image, const std::string& new_image_name, int& check_convert,unsigned int R,unsigned int G,unsigned int B) {
    int outer_frame_width = 40; // Width of the outer border
    int middle_frame_width = 20; // Width of the middle border
    int inner_frame_width = 10; // Width of the inner border

    // Loop over the outer border of the image
    for (int y = 0; y < image.height; ++y) {
        for (int x = 0; x < image.width; ++x) {
            // Set the outer border color
            if (x < outer_frame_width || x >= image.width - outer_frame_width ||
                y < outer_frame_width || y >= image.height - outer_frame_width) {
                image(x, y, 0) = R; // Red channel
                image(x, y, 1) = G;   // Green channel
                image(x, y, 2) = B;   // Blue channel
            }
                // Set the middle border color
            else if (x < outer_frame_width + middle_frame_width || x >= image.width - (outer_frame_width + middle_frame_width) ||
                     y < outer_frame_width + middle_frame_width || y >= image.height - (outer_frame_width + middle_frame_width)) {
                image(x, y, 0) = 200; // Red channel
                image(x, y, 1) = 200; // Green channel
                image(x, y, 2) = 200; // Blue channel
            }
                // Set the inner border color
            else if (x < outer_frame_width + middle_frame_width + inner_frame_width || x >= image.width - (outer_frame_width + middle_frame_width + inner_frame_width) ||
                     y < outer_frame_width + middle_frame_width + inner_frame_width || y >= image.height - (outer_frame_width + middle_frame_width + inner_frame_width)) {
                image(x, y, 0) = 0;   // Red channel
                image(x, y, 1) = 0; // Green channel
                image(x, y, 2) = 0;   // Blue channel
            }
        }
    }

    // Save the modified image
    check_convert = image.saveImage(new_image_name);
}

void Frame(Image& image,string& new_image_name,int& check_convert,unsigned int R, unsigned int G,unsigned int B)
{
    int frame_width = 40;
    // Loop over the top border of the image
    for (int y = 0; y < frame_width; ++y) {
        for (int x = 0; x < image.width; ++x) {
            // Set the pixel to blue
            image(x, y, 0) = R;    // Red channel
            image(x, y, 1) = G;    // Green channel
            image(x, y, 2) = B;    // Blue channel
        }
    }
    // Loop over the bottom border of the image
    for (int y = image.height - frame_width; y < image.height; ++y) {
        for (int x = 0; x < image.width; ++x) {
            // Set the pixel to blue
            image(x, y, 0) = R;
            image(x, y, 1) = G;
            image(x, y, 2) = B;
        }
    }

    // Loop over the left border of the image (excluding the corners already covered)
    for (int y = frame_width; y < image.height - frame_width; ++y) {
        for (int x = 0; x < frame_width; ++x) {
            // Set the pixel to blue
            image(x, y, 0) = R;
            image(x, y, 1) = G;
            image(x, y, 2) = B;
        }
    }

    // Loop over the right border of the image (excluding the corners already covered)
    for (int y = frame_width; y < image.height - frame_width; ++y) {
        for (int x = image.width - frame_width; x < image.width; ++x) {
            // Set the pixel to blue
            image(x, y, 0) = R;
            image(x, y, 1) = G;
            image(x, y, 2) = B;
        }
    }
    check_convert = image.saveImage( new_image_name);
}

// Function to filter the image to black and white
void Black_White(Image& image, string& new_image, int& check_convert)
{
    // loop over every pixel in the image
    for (int i = 0; i < image.width; ++i) {
        for (int j = 0; j < image.height; ++j) {

            unsigned int avg = 0;
            for (int k = 0; k < image.channels; ++k) {
                avg += image(i, j, k);
            }
            avg /= image.channels;    // turns the image into grey

            unsigned char new_color = avg;
            // check if the color more or less than grey
            if (new_color > 127)
                new_color = 255;     //changes it into white
            else
                new_color = 0;       //changes it into black
            for (int k = 0; k < image.channels; ++k) {
                image(i, j, k) = new_color;
            }
        }
    }
    // Saving the image
    check_convert = image.saveImage(new_image);
}
void Grey(Image& image, string& new_image,int& check_convert)
{
    unsigned int avg = 0;
    for (int i = 0; i <image.width ; ++i) {
        for (int j = 0; j <image.height ; ++j) {
            for (int k = 0; k <image.channels ; ++k) {
                avg+=image(i,j,k);

            }
            // turns the image into grey
            avg = avg/3;
            for (int k = 0; k< image.channels ; ++k) {
                image(i,j,k) = avg;
            }

        }
    }
    // Saving the image
    check_convert = image.saveImage( new_image);
}
// function to detect invalid names of images
bool check_extension(string& name)
{
    int pos =0;
    for (int i = 0; i <name.length() ; ++i)
    {
        if (name[i]== '.')
             pos = i;
    }
    string after_dot = name.substr(pos+1,3);    // to have the letters after dot
    // check if the is invalid to the extensions
    if (after_dot == "jpg" || after_dot == "png" || after_dot == "bmp" || after_dot == "tga")
        return true;
    else
        return false;

}
// function to invert the color of the image
void Invert(Image& image, string& new_image,int& check_convert){
    for (int i = 0; i <image.width ; ++i) {
        for (int j = 0; j <image.height ; ++j) {
            for (int k = 0; k <image.channels ; ++k) {
              image(i,j,k) = 255 - image(i,j,k);
            }
        }
    }
    // Saving the image
    check_convert = image.saveImage( new_image);
}
// function rotate the image by 90 degrees as the number of cycles
void Rotate(Image& image ,string& new_name,int& check_convert,int cycles) {
    if (cycles == 0)
        return;
    // reverting the dimensions
    Image rotated_image (image.height , image.width );
    for (int i = 0; i < image.height; ++i) {
        for (int j = 0; j < image.width; ++j) {
            int new_row = image.height - 1 - i;      // making new width
            int new_col = j;                         // making new height
            for (int c = 0; c < image.channels; ++c) {
                // rotate the image
                rotated_image(new_row, new_col, c)= image(j, i, c);
            }
        }
    }
    // Saving the image
    check_convert = rotated_image.saveImage(new_name);
    // make recursion to rotate the image by a number of cycle as the user wants
    Rotate(rotated_image, new_name , check_convert , cycles-1);
}
void flip_horizontally(Image& image ,string& new_name,int& check_convert)
{


    // Get the dimensions of the original image
    int width = image.width;
    int height = image.height;

    // Create a new image for flipping
    Image filip_horz(width, height);

    // Flip the image horizontally
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < 3; ++c) {
                filip_horz(x, y, c) = image(width - 1 - x, y, c);
            }
        }
    }
    check_convert = filip_horz.saveImage(new_name);
}
void flip_vertically(Image& image ,string& new_name,int& check_convert)
{
    // Get the dimensions of the original image
    int width = image.width;
    int height = image.height;
    // Create a new image for flipping
    Image flip_ver(width, height);
    // Flip the image vertically
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            for (int c = 0; c < 3; ++c) {
                flip_ver(x, y, c) = image(x, height - 1 - y, c);
            }
        }
    }
    check_convert = flip_ver.saveImage(new_name);
}
void crop(Image& image ,string& new_name,int& check_convert,int x ,int y, int w , int h)
{
    // Create a buffer for the cropped image
    Image croppedImage(w, h);

    // Copy the region from the original image to the cropped image buffer
    for (int j = 0; j < h; ++j) {
        for (int i = 0; i < w; ++i) {
            for (int c = 0; c < image.channels; ++c) {
                croppedImage(i, j, c) = image(x + i, y + j, c);
            }
        }
    }

    check_convert = croppedImage.saveImage(new_name);
}
void resize_main_filter(Image& image, int& new_width, int& new_height, string& new_name, int& check_convert)
{
    // Create a new image with the desired dimensions

    Image resized_image(new_width, new_height);

    // Calculate scaling factors
    double scale_X = static_cast<double>(image.width) / new_width;
    double scale_Y = static_cast<double>(image.height) / new_height;

    // Resize the image
    for (int y = 0; y < new_height; ++y) {
        for (int x = 0; x < new_width; ++x) {
            for (int c = 0; c < image.channels; ++c) {
                // Calculate the corresponding pixel in the original image
                int src_X = static_cast<int>(x * scale_X);
                int src_Y = static_cast<int>(y * scale_Y);

                // Get the pixel value from the original image and set it in the resized image
                resized_image(x, y, c) = image(src_X, src_Y, c);
            }
        }
    }

    image = resized_image;
    check_convert = image.saveImage(new_name);
}
int main()
{
    while (true)
    {
        try
        {
            string name_image, new_image_name, choose_filter;
            int check_convert;
            cout << "Please enter the image you want to filter\n"
                    "if you want to exit enter (exit):";
            cin >> name_image;
            // to check if the user enter invalid name
            // to exit the program if the user enters exit
            if (name_image == "exit")
            {
                cout << "Exiting the program....\n"
                        "Hava a nice time.";
                return 0;
            }
            Image image(name_image);
            bool flag = true;
            while (flag)
            {
                cout << "Please enter image name to store new image\n"
                        "and specify the extension of it\n"
                        "(.jpg, .png, .tga, .bmp):";
                cin >> new_image_name;
                // to check if the image extension is invalid
                bool check_ext = check_extension(new_image_name);
                if (!check_ext)
                {
                    cout << "You have entered a wrong extension...\n"
                            "Please try again\n";
                    flag = true;
                }
                else
                    flag = false;
            }
            cout << "Choose the filter you want:\n"
                    "1) Grey\n"
                    "2) Black and White\n"
                    "3) Invert\n"
                    "4) Merge Images\n"
                    "5) Flip\n"
                    "6) Rotate\n"
                    "7) Darken and Lighten\n"
                    "8) Crop\n"
                    "9) Adding Frame\n"
                    "10)Detect Edges\n"
                    "11)Resize\n"
                    "12)Blur\n";
            cin >> choose_filter;

            // Grey
            if (choose_filter == "1")
                Grey(image, new_image_name, check_convert);

            // Black and White
            else if (choose_filter == "2")
                Black_White(image, new_image_name, check_convert);

            // Invert
            else if(choose_filter == "3")
                Invert(image, new_image_name, check_convert);

            // Merge Images
            else if (choose_filter == "4")
            {
                string name_image_2;
                cout << "Enter the second image you want to merge with the first one:";
                cin >> name_image_2;
                Image image2(name_image_2);

                // Resize the larger image to match the dimensions of the smaller one
                int minWidth = min(image.width, image2.width);
                int minHeight = min(image.height, image2.height);
                // Ensure both images have the same dimensions
                if (image.width != image2.width || image.height != image2.height) {

                    if (image.width > minWidth || image.height > minHeight) {
                        resizeImage(image, minWidth, minHeight);
                    } else {
                        resizeImage(image2, minWidth, minHeight);
                    }
                }

                // Merge the two images
                Image mergedImage(minWidth, minHeight);
                for (int i = 0; i < minWidth; ++i) {
                    for (int j = 0; j < minHeight; ++j) {
                        for (int k = 0; k < image.channels; ++k) {
                            // Calculate the average of corresponding pixel values
                            unsigned int avg = (image(i, j, k) + image2(i, j, k)) / 2;
                            mergedImage(i, j, k) = avg;
                        }
                    }
                }
                // Save the merged image
                check_convert= mergedImage.saveImage(new_image_name);
            }

            else if (choose_filter == "5")
            {
                string choice_flip;
                cout << "Enter the the flip that you want\n"
                        "1) Horizontally\n"
                        "2) Vertically\n";
                cin >> choice_flip;
                if (choice_flip == "1")
                    flip_horizontally(image, new_image_name, check_convert);
                else if (choice_flip == "2")
                     flip_vertically(image, new_image_name, check_convert);
            }

            // Rotate
            else if (choose_filter == "6")
            {
                string degree_choice;
                cout << "Choose the rotation degrees:\n"
                        "1) 90\n"
                        "2) 180\n"
                        "3) 270\n";
                // to take the degree from the user
                cin >> degree_choice;
                if (degree_choice == "1"||degree_choice == "2"||degree_choice == "3")
                {
                    // to convert the string to int to calculate number of cycles
                    int rotation_degree = stoi(degree_choice);
                    Rotate(image, new_image_name, check_convert,rotation_degree );
                }

            }
            // Darken_Lighten
            else if (choose_filter == "7")
            {
                Darken_Lighten(image, new_image_name, check_convert);

            }
            else if (choose_filter == "8")
            {
                bool check = true;
                while(check)
                {
                    int x, y, w, h;
                    cout << "Enter the coordinates of the region to cut out\n"
                            "x =";
                    cin >> x;
                    cout << "y =";
                    cin >> y;
                    cout << "w =";
                    cin >> w;
                    cout << "h =";
                    cin >> h;
                    // Check if the region is within the image bounds
                    if (x < 0 || y < 0 || x + w >image.width || y + h >image.height) {
                        cout << "Invalid region coordinates.\n";
                        check = true;
                        continue;
                    }
                    else
                    {
                        crop(image, new_image_name, check_convert,x,y,w,h);
                        check = false;
                    }
                }

            }

            // Adding Frame
            else if (choose_filter == "9")
            {
                string frame_choice;
                cout << "Enter the type of fram you want\n"
                        "1) Simple frame\n"
                        "2) Fancy frame\n";
                cin >> frame_choice;

                unsigned int R,G,B;
                cout << "Enter the colors RGB: (known that the color range between (0,255)\n"
                        "R =";
                cin >> R;
                cout << "G =";
                cin >> G;
                cout << "B =";
                cin >> B;
                if (frame_choice == "1")
                {
                    Frame(image, new_image_name, check_convert,R,G,B);
                }
                else if (frame_choice == "2")
                    Fancy_Frame(image, new_image_name, check_convert,R,G,B);

            }

            // Detect Edges
            else if(choose_filter == "10")
            {
                Edges(image, new_image_name, check_convert);
            }

            // Resize
            else if(choose_filter == "11")
            {
                string choice_resize;
                cout << "Enter what you want\n"
                        "1) Resize by entering new dimensions\n"
                        "2) Resize by a ratio of reduction or increase\n";
                cin >> choice_resize;
                int new_width,new_height;
                if (choice_resize == "1")
                {
                    cout << "Enter new width:";
                    cin >> new_width;
                    cout << "Enter new height:";
                    cin >> new_height;
                    resize_main_filter(image, new_width, new_height, new_image_name, check_convert);
                }

                else if (choice_resize == "2")
                {
                    double ratio;
                    cout << "Enter the ratio of reduction or increase:";
                    cin >> ratio;
                    new_width = image.width * ratio  ;
                    new_height = image.height * ratio ;
                    cout << "ratio " << ratio << endl;
                    cout << "new_width " << new_width << endl;
                    cout << "new_height " << new_height << endl;
                    resize_main_filter(image, new_width, new_height, new_image_name, check_convert);                }
                }

            // Blur
            else if(choose_filter == "12")
            {

                Blur(image,new_image_name, check_convert);
            }
            else
                cout << "Invalid choice...\n"
                        "Please try again.\n";
            if (check_convert == 1)
                cout << "Converted successfully\n";
            else
                cout << "Converted unsuccessfully\n";

        }
        catch (...)
        {
            cout << "Enter a valid file name...\n";
        }
    }
}
