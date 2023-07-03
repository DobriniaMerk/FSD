# FSD: an image compression algorithm

This is a new image storing format, which decreases the number of colors in the image to achieve an extremely low file size. With different quality settings, it gets 3-5 times smaller image files than jpg images. It was written entirely in C++, which gives reasonable processing speeds.
This repository contains source for FSD file reading program and a program to convert regular images to FSD format along with an installer for them.

## Realization Details

### Image processing

This program uses [Floyd–Steinberg's](https://en.wikipedia.org/wiki/Floyd%E2%80%93Steinberg_dithering) dithering algorithm to distribute colors on the picture without losing its details. The colors themselves are chosen with the [k-means clustering](https://en.wikipedia.org/wiki/K-means_clustering) method, which searches for big clusters of similar colors in color space and chooses their centers as new image colors.

### File saving
Image is saved to disk with almost the most basic and simple algorithm possible — [Run-Length encoding](https://en.wikipedia.org/wiki/Run-length_encoding) (surely must be changed in future). To make the resulting file smaller, it is then compressed with the [zpaq](https://github.com/zpaq/zpaq) file compression library.

## Comparison with other formats
JPEG / PNG | FSD
---|---
![passage](https://github.com/DobriniaMerk/FSD/assets/93145779/0703ea3f-9b6f-4b5a-b502-fb14955ded40) | ![fsdpssg](https://github.com/DobriniaMerk/FSD/assets/93145779/4f88d57b-faad-459a-9673-ec47acacad42)
Regular JPEG image with default quality setting of 90, wheighting 216 KB. <br> In PNG format, it weights 1013 KB. | FSD image compressed to 8 colors, weights only 71 KB.
