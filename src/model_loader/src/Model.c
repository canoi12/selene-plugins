#include "model_loader.h"

#if !defined(OS_WIN)
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#else
#include <windows.h>
#endif

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"

// #define CGLTF_IMPLEMENTATION
// #include "cgltf.h"

static void CalcNormal(float N[3], float v0[3], float v1[3], float v2[3]) {
  float v10[3];
  float v20[3];
  float len2;

  v10[0] = v1[0] - v0[0];
  v10[1] = v1[1] - v0[1];
  v10[2] = v1[2] - v0[2];

  v20[0] = v2[0] - v0[0];
  v20[1] = v2[1] - v0[1];
  v20[2] = v2[2] - v0[2];

  N[0] = v20[1] * v10[2] - v20[2] * v10[1];
  N[1] = v20[2] * v10[0] - v20[0] * v10[2];
  N[2] = v20[0] * v10[1] - v20[1] * v10[0];

  len2 = N[0] * N[0] + N[1] * N[1] + N[2] * N[2];
  if (len2 > 0.0f) {
    float len = (float)sqrt((double)len2);

    N[0] /= len;
    N[1] /= len;
  }
}

static char* mmap_file(size_t* len, const char* filename) {
#if defined(_WIN64) || defined(_WIN32)
  HANDLE file =
      CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                  FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

  if (file == INVALID_HANDLE_VALUE) { /* E.g. Model may not have materials. */
    return NULL;
  }

  HANDLE fileMapping = CreateFileMapping(file, NULL, PAGE_READONLY, 0, 0, NULL);
  assert(fileMapping != INVALID_HANDLE_VALUE);

  LPVOID fileMapView = MapViewOfFile(fileMapping, FILE_MAP_READ, 0, 0, 0);
  char* fileMapViewChar = (char*)fileMapView;
  assert(fileMapView != NULL);

  DWORD file_size = GetFileSize(file, NULL);
  (*len) = (size_t)file_size;

  return fileMapViewChar;
#else

  struct stat sb;
  char* p;
  int fd;

  fd = open(filename, O_RDONLY);
  if (fd == -1) {
    perror("open");
    return NULL;
  }

  if (fstat(fd, &sb) == -1) {
    perror("fstat");
    return NULL;
  }

  if (!S_ISREG(sb.st_mode)) {
    fprintf(stderr, "%s is not a file\n", filename);
    return NULL;
  }

  p = (char*)mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);

  if (p == MAP_FAILED) {
    perror("mmap");
    return NULL;
  }

  if (close(fd) == -1) {
    perror("close");
    return NULL;
  }

  (*len) = sb.st_size;

  return p;

#endif
}

/* path will be modified */
static char* get_dirname(char* path) {
  char* last_delim = NULL;

  if (path == NULL) {
    return path;
  }

#if defined(_WIN32)
  /* TODO: Unix style path */
  last_delim = strrchr(path, '\\');
#else
  last_delim = strrchr(path, '/');
#endif

  if (last_delim == NULL) {
    /* no delimiter in the string. */
    return path;
  }

  /* remove '/' */
  last_delim[0] = '\0';

  return path;
}

static void get_file_data(void* ctx, const char* filename, const int is_mtl,
                          const char* obj_filename, char** data, size_t* len) {
  // NOTE: If you allocate the buffer with malloc(),
  // You can define your own memory management struct and pass it through `ctx`
  // to store the pointer and free memories at clean up stage(when you quit an
  // app)
  // This example uses mmap(), so no free() required.
  (void)ctx;

  if (!filename) {
    fprintf(stderr, "null filename\n");
    (*data) = NULL;
    (*len) = 0;
    return;
  }

  size_t data_len = 0;

  *data = mmap_file(&data_len, filename);
  (*len) = data_len;
}

MODULE_FUNCTION(Model, load) {
    INIT_ARG();
    CHECK_STRING(filename);
    tinyobj_attrib_t attrib;
    tinyobj_shape_t* shapes = NULL;
    size_t num_shapes;
    tinyobj_material_t* materials = NULL;
    size_t num_materials;

    unsigned int flags = TINYOBJ_FLAG_TRIANGULATE;
    int ret = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &materials,
                          &num_materials, filename, get_file_data, NULL, flags);
    if (ret != TINYOBJ_SUCCESS) {
      return luaL_error(L, "[selene] failed to load 3D Model");
    }

    fprintf(stdout, "obj, vertices(%d) normals(%d) texcoords(%d)\n", attrib.num_vertices, attrib.num_normals, attrib.num_texcoords);
    fprintf(stdout, "obj, faces(%d) face_num_verts(%d)\n", attrib.num_faces, attrib.num_face_num_verts);
    fprintf(stdout, "obj, shapes(%d) shape_len(%d) shape_name(%s) face_offset(%d)\n", num_shapes, shapes[0].length, shapes[0].name, shapes[0].face_offset);
    int total_size = attrib.num_faces * (8 * sizeof(float)) + (attrib.num_faces * 3 * sizeof(int));
    fprintf(stdout, "obj, total(%d)\n", total_size);

    NEW_UDATA_ADD(MeshData, mesh, total_size);
    mesh->num_vertices = attrib.num_face_num_verts;
    mesh->num_triangles = attrib.num_faces;
    mesh->data_size = total_size;
    char* end_data = (char*)(mesh + 1);
    mesh->vertices = (float*)end_data;
    end_data += (3 * sizeof(float)) * shapes[0].length;
    mesh->normals = (float*)end_data;
    end_data += (3 * sizeof(float)) * shapes[0].length;
    mesh->texcoords = (float*)end_data;
    end_data += (2 * sizeof(float)) * shapes[0].length;
    mesh->num_indices = attrib.num_faces * 3;
    mesh->indices = (unsigned int*)end_data;

    for (int i = 0; i < num_shapes; i++) {
        size_t index_offset = 0;
        size_t index_count = 0;
        for (size_t f = 0; f < attrib.num_face_num_verts; f++) {
            size_t fv = (size_t)attrib.face_num_verts[f];
            for (size_t v = 0; v < fv; v++) {
                tinyobj_vertex_index_t idx = attrib.faces[index_offset + v];
                mesh->vertices[index_offset + 0] = attrib.vertices[3 * idx.v_idx + 0];
                mesh->vertices[index_offset + 1] = attrib.vertices[3 * idx.v_idx + 1];
                mesh->vertices[index_offset + 2] = attrib.vertices[3 * idx.v_idx + 2];

                if (idx.vn_idx >= 0) {
                    mesh->normals[index_offset + 0] = attrib.normals[3 * idx.vn_idx + 0];
                    mesh->normals[index_offset + 1] = attrib.normals[3 * idx.vn_idx + 1];
                    mesh->normals[index_offset + 2] = attrib.normals[3 * idx.vn_idx + 2];
                }

                if (idx.vt_idx >= 0) {
                    mesh->texcoords[index_offset + 0] = attrib.texcoords[2 * idx.vt_idx + 0];
                    mesh->texcoords[index_offset + 1] = attrib.texcoords[2 * idx.vt_idx + 1];
                }

                mesh->indices[index_offset + 0] = index_offset + 0;
                mesh->indices[index_offset + 1] = index_offset + 1;
                mesh->indices[index_offset + 2] = index_offset + 2;
                // index_count++;
            }
            // fprintf(stdout, "f(%d) fv(%d)\n", f, fv);
            index_offset += fv;
        }
        fprintf(stdout, "final offset: %d\n", index_offset);
    }

    tinyobj_attrib_free(&attrib);
    tinyobj_shapes_free(shapes, num_shapes);
    tinyobj_materials_free(materials, num_materials);
    #if 0
    for (i = 0; i < attrib.num_face_num_verts; i++) {
        size_t f;
        if (attrib.face_num_verts[i] % 3 != 0)
            return luaL_error(L, "[selene] failed to load model: cannot triangulate");
        for (f = 0; f < (size_t)attrib.face_num_verts[i] / 3; f++) {
            size_t k;
            float v[3][3];
            float n[3][3];
            float c[3];
            float len2;

            tinyobj_vertex_index_t idx0 = attrib.faces[face_offset + 3 * f + 0];
            tinyobj_vertex_index_t idx1 = attrib.faces[face_offset + 3 * f + 1];
            tinyobj_vertex_index_t idx2 = attrib.faces[face_offset + 3 * f + 2];
            
            for (k = 0; k < 3; k++) {
                int f0 = idx0.v_idx;
                int f1 = idx1.v_idx;
                int f2 = idx2.v_idx;
                assert(f0 >= 0);
                assert(f1 >= 0);
                assert(f2 >= 0);

                v[0][k] = attrib.vertices[3 * (size_t)f0 + k];
                v[1][k] = attrib.vertices[3 * (size_t)f1 + k];
                v[2][k] = attrib.vertices[3 * (size_t)f2 + k];
            }

            if (attrib.num_normals > 0) {
                int f0 = idx0.vn_idx;
                int f1 = idx1.vn_idx;
                int f2 = idx2.vn_idx;
                if (f0 >= 0 && f1 >= 0 && f2 >= 0) {
                    assert(f0 < (int)attrib.num_normals);
                    assert(f1 < (int)attrib.num_normals);
                    assert(f2 < (int)attrib.num_normals);
                    for (k = 0; k < 3; k++) {
                    n[0][k] = attrib.normals[3 * (size_t)f0 + k];
                    n[1][k] = attrib.normals[3 * (size_t)f1 + k];
                    n[2][k] = attrib.normals[3 * (size_t)f2 + k];
                    }
                } else { /* normal index is not defined for this face */
                    /* compute geometric normal */
                    CalcNormal(n[0], v[0], v[1], v[2]);
                    n[1][0] = n[0][0];
                    n[1][1] = n[0][1];
                    n[1][2] = n[0][2];
                    n[2][0] = n[0][0];
                    n[2][1] = n[0][1];
                    n[2][2] = n[0][2];
                }
                } else {
                /* compute geometric normal */
                CalcNormal(n[0], v[0], v[1], v[2]);
                n[1][0] = n[0][0];
                n[1][1] = n[0][1];
                n[1][2] = n[0][2];
                n[2][0] = n[0][0];
                n[2][1] = n[0][1];
                n[2][2] = n[0][2];
            }

            for (k = 0; k < 3; k++) {
                vb[(3 * i + k) * stride + 0] = v[k][0];
                vb[(3 * i + k) * stride + 1] = v[k][1];
                vb[(3 * i + k) * stride + 2] = v[k][2];
                vb[(3 * i + k) * stride + 3] = n[k][0];
                vb[(3 * i + k) * stride + 4] = n[k][1];
                vb[(3 * i + k) * stride + 5] = n[k][2];

                /* Set the normal as alternate color */
                c[0] = n[k][0];
                c[1] = n[k][1];
                c[2] = n[k][2];
                len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
                if (len2 > 0.0f) {
                    float len = (float)sqrt((double)len2);

                    c[0] /= len;
                    c[1] /= len;
                    c[2] /= len;
                }

                vb[(3 * i + k) * stride + 6] = (c[0] * 0.5f + 0.5f);
                vb[(3 * i + k) * stride + 7] = (c[1] * 0.5f + 0.5f);
                vb[(3 * i + k) * stride + 8] = (c[2] * 0.5f + 0.5f);

                /* now set the color from the material */
                // if (attrib.material_ids[i] >= 0) {
                //     int matidx = attrib.material_ids[i];
                //     vb[(3 * i + k) * stride + 9] = materials[matidx].diffuse[0];
                //     vb[(3 * i + k) * stride + 10] = materials[matidx].diffuse[1];
                //     vb[(3 * i + k) * stride + 11] = materials[matidx].diffuse[2];
                // } else {
                //     /* Just copy the default value */
                //     vb[(3 * i + k) * stride + 9] = vb[(3 * i + k) * stride + 6];
                //     vb[(3 * i + k) * stride + 10] = vb[(3 * i + k) * stride + 7];
                //     vb[(3 * i + k) * stride + 11] = vb[(3 * i + k) * stride + 8];
                // }

            }
        }
    }
    #endif
    return 1;
}

BEGIN_META(Model) {
    BEGIN_REG(reg)
        REG_FIELD(Model, load),
    END_REG()
    luaL_newmetatable(L, "Model");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, reg, 0);
    return 1;
}