#include <iostream>
#include <cstdint> // For exact integer widths. 

#include <cmath>



// Table of 'magic numbers' experimentally derived. 
// Monitors have different gamuts and corresponding wavelengths for the color primaries.
// We will choose sRGB, which has standard points according to XYZ colorspace.

// Primaries of sRGB (Unused so far)
#define SRGB_R_x 0.64
#define SRGB_R_y 0.33

#define SRGB_G_x 0.30
#define SRGB_G_y 0.60

#define SRGB_B_x 0.15
#define SRGB_B_y 0.06

// Unused so far.. Will be used in deriving scaling factors later. 
#define SRGB_W_x 0.3127
#define SRGB_W_y 0.3290

// Derived from the white point. Involves some matrix manipulation. 
// This may be derived and defined from the white point later.
#define SRGB_R_SF 0.21264
#define SRGB_G_SF 0.71517
#define SRGB_B_SF 0.07219

// The problem with sRGB is that the primaries themselves are not pure monochromatic colors
// but rather mixtures found in the interior of the color horseshoe. 


// using namespace std; <//3  

struct RGBValue { 
    double r;
    double g;
    double b;
};

struct XYZValue { 
    double x;
    double y; 
    double z;
};

struct LMSValue {
    double l;
    double m;
    double s; 
};

double deuteranopiaScore(LMSValue lColor, LMSValue rColor) {
    return (lColor.l - rColor.l) * (lColor.l - rColor.l) 
         + (lColor.s - rColor.s) * (lColor.s - rColor.s);
}

LMSValue xyzToLMS(XYZValue color) {
    // The matrix transformation involved is taken from Hunt-Pointer-Estevez.
    // Eventually this may be derived in code or another method may be used. 
    XYZValue transformation[3] = {
        XYZValue{
            0.38971,  0.68898, -0.07868
        }, 
        XYZValue{
            -0.22981, 1.18340, 0.04641
        }, 
        XYZValue{ 
            0,        0,       1
        }
    };
    // The transformation matrix is multiplied with XYZ to produce LMS.
    return LMSValue {
        color.x * transformation[0].x + color.y * transformation[1].x + color.z * transformation[2].x,
        color.x * transformation[0].y + color.y * transformation[1].y + color.z * transformation[2].y,
        color.x * transformation[0].z + color.y * transformation[1].z + color.z * transformation[2].z
    };
}


// Taken from the sRGB transfer function; linearizes sRGB into XYZ colorspace. 
double linearizeSRGB(double nValue) {
    if (nValue <= 0.04045) {
        return nValue / 12.92;
    } else {
        return std::pow((nValue + 0.055) / 1.055, 2.4);
    }
}

// Inverses the sRGB transfer function and then does a linear transformation to convert sRGB to CIE XYZ colorspace.
XYZValue rgbToXYZ(RGBValue color) { 
    double rgbaSum = color.r + color.g + color.b;
    RGBValue normalizedRGB{
        color.r / rgbaSum,
        color.g / rgbaSum,
        color.b / rgbaSum, 
    }; 
    RGBValue linearRGB{
        linearizeSRGB(normalizedRGB.r),
        linearizeSRGB(normalizedRGB.g),
        linearizeSRGB(normalizedRGB.b),
    }; 

    // We then apply a matrix transformation to transform the linear RGB to XYZ. 
    // The transformation matrix is defined below.
    XYZValue transformation[3] = {
        XYZValue{
            SRGB_R_x / SRGB_R_y * SRGB_R_SF,
            1 * SRGB_R_SF,
            (1 - SRGB_R_x - SRGB_R_y) / SRGB_R_y * SRGB_R_SF
        }, 
        XYZValue{
            SRGB_G_x / SRGB_G_y * SRGB_G_SF,
            1 * SRGB_G_SF,
            (1 - SRGB_G_x - SRGB_G_y) / SRGB_G_y * SRGB_G_SF
        }, 
        XYZValue{ 
            SRGB_B_x / SRGB_B_y * SRGB_B_SF,
            1 * SRGB_B_SF,
            (1 - SRGB_B_x - SRGB_B_y) / SRGB_B_y * SRGB_B_SF
        }
    };
    // The transformation matrix is multiplied with the linearized RGB to produce XYZ.
    XYZValue transformedXYZ{
        linearRGB.r * transformation[0].x + linearRGB.g * transformation[1].x + linearRGB.b * transformation[2].x,
        linearRGB.r * transformation[0].y + linearRGB.g * transformation[1].y + linearRGB.b * transformation[2].y,
        linearRGB.r * transformation[0].z + linearRGB.g * transformation[1].z + linearRGB.b * transformation[2].z
    };

    double xyzSum = transformedXYZ.x + transformedXYZ.y + transformedXYZ.z;
    return XYZValue {
        transformedXYZ.x / xyzSum,
        transformedXYZ.y / xyzSum,
        transformedXYZ.z / xyzSum
    };
}

int hexToInt(std::string hexadecimal) {
    int buffer = 0;
    for (int i = 0; i < hexadecimal.length(); i++) { 
        char character = hexadecimal[i];
        if (character <= '9' && character >= '0') { 
            buffer += (character - '0') * std::pow(16, i); 
        } else if (character <= 'F' && character >= 'A') { 
            buffer += (10 + character - 'A') * std::pow(16, i);
        } else {
            return -1;
        }
    }
    return buffer; 
}

int main() {
    std::string rHex;
    std::string gHex;
    std::string bHex;

    RGBValue lColor;
    RGBValue rColor;

    double score;

    std::cout << "Enter an sRGB color code in hexadecimal. Format like so: \"RR GG BB\": \n";
    std::cin >> rHex >> gHex >> bHex;
    lColor = RGBValue {
        hexToInt(rHex),
        hexToInt(gHex),
        hexToInt(bHex)
    };
    
    std::cout <<"Enter a second sRGB color code in hexadecimal. Format like so: \"RR GG BB\" \n";
    std::cin >> rHex >> gHex >> bHex;
    rColor = RGBValue {
        hexToInt(rHex),
        hexToInt(gHex),
        hexToInt(bHex)
    };

    score = deuteranopiaScore(
        xyzToLMS(
            rgbToXYZ(lColor)
        ),
        xyzToLMS(
            rgbToXYZ(rColor)
        )
    );



    std::cout << "The measured score for Deuteranopia (red-green color blindness) is: " << score << "\n";

    std::cout << "\nA lower score indicates a greater proximity between the two values" 
        << "\nin terms of long and short cones, and indicates a potentially"
        << "\nbad color combination which may appear similar to deutan-type color-blindness.";

    

    return 0;
}
