#include "tga.h"


struct TgaImage *tgaRead(const char *filename)
{
	FILE *file = fopen(filename, "rb");
	if (file == NULL) {
		fprintf(stderr, "Error: failed to open: %s\n", filename);
		return NULL;
	}

	struct TgaHeader header;
	fread(&header, sizeof(struct TgaHeader), 1, file);

	if (!header.width || !header.height) {
		fprintf(stderr, "Error: bad header for: %s\n", filename);
		fclose(file);
		return NULL;
	}

	if (header.bitsPerPixel != 32 && header.bitsPerPixel != 24) { // AARRGGBB or RRGGBB
		fprintf(stderr, "Error: bad bbp for: %s\n", filename);
		fclose(file);
		return NULL;
	}
	
	if (header.IDLength) //skip ID field
		fseek(file, header.IDLength, SEEK_CUR);

	struct TgaImage *image = malloc(sizeof(struct TgaImage));
	if (image == NULL) {
		fprintf(stderr, "Error: failed allocate memory for image: %s\n", filename);
		return NULL;
	}
	
	image->width = (size_t)header.width;
	image->height = (size_t)header.height;

	size_t num_pixels = image->width * image->height;
	if ((image->data = malloc(num_pixels * sizeof(uint32_t))) == NULL) {
		fprintf(stderr, "Error: failed allocate memory\n");
		free(image);
		fclose(file);
		return NULL;
	}

	if (header.imageType == 2 || header.imageType == 3) { //uncompressed
		switch (header.bitsPerPixel) {
		case 32:
			//reads into pixel array where bottom left is the origin
			fread(image->data, num_pixels * sizeof(uint32_t), 1, file);
			break;
		case 24:
			for (uint32_t *pixel = image->data; pixel < image->data + num_pixels; ++pixel) {
				fread(pixel, 3, 1, file);
				*pixel |= 0xFF000000u; //sets alpha bye
			}
			break;
		}
	} //TODO: compressed
	fclose(file);
	return image;
}

bool tgaWrite(const char *filename, const struct TgaImage *image)
{
	FILE *file = fopen(filename, "wb");
	if (file == NULL)
		return false;

	// basic header
	struct TgaHeader header = (struct TgaHeader){ 0, 0, 2, 0, 0, 0, 0, 0, (uint16_t)image->width, (uint16_t)image->height, 32, 0 };
	fwrite(&header, sizeof(struct TgaHeader), 1, file); //writes header

	size_t num_bytes = (size_t)(image->width) * (size_t)(image->height) * sizeof(uint32_t);
	fwrite(image->data, 1, num_bytes, file); //uncompressed, 32bit

	fclose(file);
	return true;
}

struct TgaImage *tgaAlloc(size_t width, size_t height)
{
	if (!width || !height)
		return NULL;

	struct TgaImage *image = malloc(sizeof(struct TgaImage));
	if (image == NULL)
		return NULL;

	image->width = width;
	image->height = height;

	image->data = malloc(sizeof(uint32_t) * width * height);
	if (image->data == NULL) {
		free(image);
		return NULL;
	}
	return image;
}


void tgaFree(struct TgaImage *image)
{
	if (image != NULL) {
		free(image->data);
		free(image);
	}
}
