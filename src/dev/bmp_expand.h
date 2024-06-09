/**
 * @file bmp_expand.h
 * @author your name (you@domain.com)
 * @brief Provide a way to convert some BMP files to a bitmap image
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __bmp_expand_h__
#define __bmp_expand_h__

extern const short BMP_OK;					// BMP file was processed without error
extern const short BMP_BAD_FILETYPE;		// File does not have a BMP signature
extern const short BMP_BAD_FORMAT;			// BMP file is not in a supported format (color format or compression)

/**
 * @brief Render a BMP file into a destination bitmap image and a destination color palette
 * 
 * @param bmp_data pointer to the raw BMP data
 * @param dest_img pointer to the area in memory to load the expanded bitmap data
 * @param clut the number of the graphics CLUT to use for the image
 * @return 0 if image could be processed, any other number is an error
 */
extern short bm_bmp_render(char * bmp_data, char * dest_img, short clut);

#endif