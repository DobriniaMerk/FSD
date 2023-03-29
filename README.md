# FSD: an image compression algorithm
This is a new image storing format, wich decreases number of color in image to achieve extremely low file size.
With different quality settings, it gets 3-5 time smaller image files than jpg images.
It was written entirely in c++, which gives reasonable processing speeds.

This repository contains source for FSD file reading program and program to convert regular images to FSD format along with installer for them.


## Realization Details
### Image processing
This program uses [Floyd–Steinberg](https://en.wikipedia.org/wiki/Floyd%E2%80%93Steinberg_dithering) dithering algorithm to distribute colors on the picture without losing its details.
The colors themselves are chosen with [k-means clustering](https://en.wikipedia.org/wiki/K-means_clustering) method wich searches for big clusters of similar colors in color space and choses their centers as new image colors.

### File saving
Image is saved to disk with almost most basic and simple algorithm possible [Run-Length encoding](https://en.wikipedia.org/wiki/Run-length_encoding) (surely must be changed in future).
To make the resulting file smaller, it is then compressed with [zpaq](https://github.com/zpaq/zpaq) file compression library.

## Comparison with other formats
// TODO
