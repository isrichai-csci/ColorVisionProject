#include <iostream>
#include <cstdint> // For exact integer widths. 
#include <fstream>
#include <vector>
#include <optional> // It's just sensical code paradigm
#include <cmath>

// std::pow is slow and won't be optimized out. Rewriting is ugly. 
#define CUBED(x) x * x * x 

// sRGB definitions in terms of XYZ colorspace. 
#define SRGB_R_x 0.64
#define SRGB_R_y 0.33

#define SRGB_G_x 0.30
#define SRGB_G_y 0.60

#define SRGB_B_x 0.15
#define SRGB_B_y 0.06

// Unused. The white point of sRGB. The derived scaling factors from sRGB are defined as their own constants. 
#define SRGB_W_x 0.3127
#define SRGB_W_y 0.3290

// Derived from the white point of sRGB. 
#define SRGB_R_SF 0.21264
#define SRGB_G_SF 0.71517
#define SRGB_B_SF 0.07219

// using namespace std; <//3  

// To-do: Base class of RGB, XYZ, LMS with common methods like sum(), normalize(), and 
// basic vector manipulation. Not necessary at this point in time but something to consider. 

// class ThreeVector 

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

struct RGBTuple {
    RGBValue left;
    RGBValue right;
};

double yToLightness(double Y);

double deuteranopiaScore(XYZValue lColor, XYZValue rColor);

LMSValue xyzToLMS(XYZValue color);

double linearizeSRGB(double nValue);

XYZValue rgbToXYZ(RGBValue color);

std::optional<unsigned int> hexToInt(std::string hexadecimal);

template <typename T>
T numberToFields(unsigned int num);

std::vector<std::optional<RGBTuple>> parseFile(std::istream& file);

int main() {
    std::string inputFilename;
    std::string outputFilename;
    std::cout << "Specify an input file to read: ";
    std::getline(std::cin, inputFilename);
    std::cout << "Specify an output file to write ";
    std::getline(std::cin, outputFilename);

    std::fstream input, output;
    input.open(inputFilename, std::ios::in);
    output.open(outputFilename, std::ios::out);

    if (!input.is_open()) {
        std::cout << "Error: The input file cannot be read from!";
        return 0; 
    }

    std::vector<std::optional<RGBTuple>> tests = parseFile(input);

    for (std::optional<RGBTuple> tuple : tests) {
        if (tuple == std::nullopt) output << "Invalid test case.\n";
        else { 
            output << deuteranopiaScore(
                rgbToXYZ(tuple -> left),
                rgbToXYZ(tuple -> right)
            ) << "\n";
        };
    }

    std::cout << "Line-by-line scores written to " << outputFilename << ".";
    std::cout << "\nA lower score indicates a greater proximity between the two values" 
        << "\nin terms of long and short cones, and indicates a potentially"
        << "\nbad color combination which may appear similar to deutan-type color-blindness.";

    return 0;
}

// UNUSED:
// Derived from CIELAB L* formula. Measures the lightness of a color (proximity to black/white). 
// Assumes reference white point Yn = 1, which we do transform the RGB in accordance to. 
// L* ranges from 0 - 1 rather than 0 to 100 here. 
// Will be used to assign special weights to luminosity.
double yToLightness(double Y) {
    if (Y > CUBED(6 / 29)) return 1.16 * std::cbrt(Y) - .16;
    else return 9.033 * Y;
}

double deuteranopiaScore(XYZValue lColor, XYZValue rColor) {
    LMSValue lColorLMS = xyzToLMS(lColor);
    LMSValue rColorLMS = xyzToLMS(rColor);
    return (lColorLMS.l - rColorLMS.l) * (lColorLMS.l - rColorLMS.l) 
         + (lColorLMS.s - rColorLMS.s) * (lColorLMS.s - rColorLMS.s);
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
    if (nValue <= 0.04045) return nValue / 12.92;
    else return std::pow((nValue + 0.055) / 1.055, 2.4);
}

// Inverses the sRGB transfer function and then does a linear transformation to convert sRGB to CIE XYZ colorspace.
XYZValue rgbToXYZ(RGBValue color) {     

    // The inverse transfer function takes into account sRGB from 0-1, so we scale our current 0x00-0xFF values like so:
    RGBValue linearRGB {
        linearizeSRGB(color.r / (double)0xFF ),
        linearizeSRGB(color.g / (double)0xFF ),
        linearizeSRGB(color.b / (double)0xFF ),
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
    
    // The XYZ value calculated preserves both the chromacity and luminosity of the RGB value.  
    return XYZValue {
        transformedXYZ.x,
        transformedXYZ.y,
        transformedXYZ.z
    };
}

std::optional<unsigned int> hexToInt(std::string hexadecimal) {
    unsigned int buffer = 0;
    for (int i = 0; i < hexadecimal.length(); i++) { 
        char character = hexadecimal[i];
        if (character <= '9' && character >= '0') { 
            buffer += (character - '0') * std::pow(16, i); 
        } else if (character <= 'F' && character >= 'A') { 
            buffer += (10 + character - 'A') * std::pow(16, i);
        } else {
            return std::nullopt;
        }
    }
    return buffer; 
}

// Todo: support more than basic RGB in the future, so this templated 3 field int-to-struct builder will suffice.
// Unfortunately reinterpret_casting the integer to the struct itself is UB! 
template <typename T> 
T numberToFields(unsigned int num) {
    return T { 
        (num & 0xFF0000) >> 16, 
        (num & 0x00FF00) >> 8, 
        (num & 0x0000FF)
    };
}

std::vector<std::optional<RGBTuple>> parseFile(std::istream& file) {
    std::string line; 
    std::vector<std::optional<RGBTuple>> values;
    while (std::getline(file, line)) {
        std::string lLine = line.substr(0, 6);
        std::string rLine = line.substr(7, 6);
        std::optional<int> lValue = hexToInt(lLine);
        std::optional<int> rValue = hexToInt(rLine);
        
        if (lValue == std::nullopt || rValue == std::nullopt) values.push_back(std::nullopt);
        else values.push_back(RGBTuple {
            numberToFields<RGBValue>(*lValue),
            numberToFields<RGBValue>(*rValue)
        });
    }
    return values;
}

