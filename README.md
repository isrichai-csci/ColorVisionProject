# Colorblindness Scorer

## Description

**version 1.0**

This program scores color combinations based on their distinguishability to those with colorblindness. The program does not take into account intensity of the light, and currently only takes into account proximity based on sRGB colorspace mapping

## Developer

Irfan Srichai

## Example

To run the program, give the following commands:

```
g++ --std=c++11 *.cpp -o cvp
./cvp
```

Here is an example of the program running:

```
Enter an sRGB color code in hexadecimal. Format like so: "RR GG BB": 
FF DD CC
Enter a second sRGB color code in hexadecimal. Format like so: "RR GG BB" 
BB AA 99
The measured score for Deuteranopia (red-green color blindness) is: 9.50835e-06

A lower score indicates a greater proximity between the two values
in terms of long and short cones, and indicates a potentially
bad color combination which may appear similar to deutan-type color-blindness.
```