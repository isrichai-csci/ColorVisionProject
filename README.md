# Colorblindness Scorer

## Description

**version 1.0**

This program scores color combinations based on their distinguishability to those with colorblindness. The program takes in input via a file containing a line-broken list of color tuples, and assesses each color tuple in separate tests. A score is given which indicates the color's distinguishability. The program makes no assessment to the objective pass/fail of a particular color; such threshold is to be determined qualitatively by the user. 

Input file format:
```
RRGGBB RRGGBB // Test case 1
RRGGBB RRGGBB // Test case 2 .... 
```

Output file format:
```
score // Test case 1
score // Test case 2 ....
```


## Developer

Irfan Srichai

## Example

To run the program, give the following commands:

```
g++ --std=c++11 *.cpp -o cvp
./cvp
```

Here is an example of the program running:

Command Line:
```
Specify an input file to read: testcases.txt
Specify an output file to write output.txt
Line-by-line scores written to output.txt.
A lower score indicates a greater proximity between the two values
in terms of long and short cones, and indicates a potentially
bad color combination which may appear similar to deutan-type color-blindness.
```

testcases.txt:
```
00FF00 FF0000
0000FF FF0000
000000 000000
FF00FF FF00FF
```

output.txt: 
```
0.671079
0.892436
0
0
```