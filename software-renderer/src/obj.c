#include "obj.h"

struct ObjModel *objRead(const char *filename)
{
	FILE *file = fopen(filename, "rb");
	if (file == NULL) {
		fprintf(stderr, "Error: failed to open: %s\n", filename);
		return NULL;
	}

	struct ObjModel *model = malloc(sizeof(struct ObjModel));
	if (model == NULL) {
		fprintf(stderr, "Error: failed to allocate memory for model: %s\n", filename);
		fclose(file);
		return NULL;
	}

	model->num_verts = 0;
	model->num_faces = 0;
	char line_buffer[OBJ_MAX_LINE_SIZE];

	// count the number lines for faces and verts
	while (fgets(line_buffer, sizeof(line_buffer), file) != NULL) {
		if (!strncmp(line_buffer, "v ", 2)) {
			++model->num_verts;
		} else if (!strncmp(line_buffer, "f ", 2)) {
			++model->num_faces;
		}
	}

	model->verts = malloc(sizeof(Vec3f) * model->num_verts);
	model->faces = malloc(sizeof(Vec3z) * model->num_faces);
	if (model->verts == NULL || model->faces == NULL) {
		fprintf(stderr, "Error: failed to allocate memory for model: %s\n", filename);
		objFree(model);
		fclose(file);
		return NULL;
	}

	rewind(file); //

	Vec3f *vert_p = model->verts;
	Vec3z *face_p = model->faces;
	while (fgets(line_buffer, sizeof(line_buffer), file) != NULL) {

		if (!strncmp(line_buffer, "v ", 2)) {
			//scan from buffer string into ObjModel struct
			sscanf_s(line_buffer + 2, "%f%f%f", *vert_p, *vert_p+1, *vert_p+2);
			++vert_p;

		} else if (!strncmp(line_buffer, "f ", 2)) { 
			char *next_t = NULL;
			size_t *face_vert = *face_p;

			//this code splits apart the vertices into tokens separated by '\0'
			for (char *p = strtok_s(line_buffer + 2, " ", &next_t); p != NULL; p = strtok_s(NULL, " ", &next_t)) {
				sscanf_s(p, "%zu", face_vert); //TODO: triangularise polygons
				*face_vert -= 1; //obj file index begins at 1
				++face_vert;
			}
			++face_p;
		}
	}

	fclose(file);
	return model;
}

void objFree(struct ObjModel *model)
{
	if (model != NULL) {
		free(model->verts);
		free(model->faces);
		free(model);
	}
}