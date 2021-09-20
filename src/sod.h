#ifndef __SOD_H__
#define __SOD_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * G E N E R A L   P U R P O S E
 */
typedef struct {
    float u;
    float v;
} SODVector2;

typedef struct {
    float x;
    float y;
    float z;
} SODVector3;

typedef struct {
    SODVector3 *right;
    SODVector3 *up;
    SODVector3 *front;
    SODVector3 *position;
} SODMatrix34;

typedef struct {
    float red;
    float green;
    float blue;
} SODColor;


/**
 * L I G H T I N G   M A T E R I A L S
 */
enum SODLightingModel {
    LIGHTING_MODEL_CONSTANT = 0,
    LIGHTING_MODEL_LAMBERT  = 1,
    LIGHTING_MODEL_PHONG    = 2
};

typedef struct {
    char     *identifier;
    SODColor *ambient;
    SODColor *diffuse;
    SODColor *specular;
    float     specular_exponent;
    uint8_t   lighting_model; // 0=constant, 1=lambert, 2=phong
} SODLightingMaterial;


/**
 * N O D E S
 */
enum SODNodeType {
    NODE_TYPE_NULL    = 0,
    NODE_TYPE_MESH    = 1,
    NODE_TYPE_SPRITE  = 3,
    NODE_TYPE_LOD     = 11, // LOD control node
    NODE_TYPE_EMITTER = 12  // particle emitter node
};

enum SODNodeMeshCullType {
    CULL_TYPE_NO_CULL       = 0,
    CULL_TYPE_BACKFACE_CULL = 1
};

typedef struct {
    uint16_t index_vertices;
    uint16_t index_texcoords;
} SODFaceVertex;

typedef struct {
    SODFaceVertex *face_vertices[3];
} SODFace;

typedef struct {
    uint16_t   nfaces;
    char      *material_id;
    SODFace  **faces;
} SODVertexLightingGroup;

typedef struct {
    char *identifier;
} SODNodeDataEmitter;

typedef struct {
    char                    *material_id;
    char                    *texture_id;
    uint16_t                 nvertices;
    uint16_t                 ntexcoords;
    uint16_t                 ngroups; // number of vertex lighting groups
    SODVector3             **vertices;
    SODVector2             **texcoords;
    SODVertexLightingGroup **lighting_groups;
    uint8_t                  cull_type; // 0=no cull, 1=backface cull
    uint16_t                 unused;
} SODNodeDataMesh;

typedef struct _SODNode {
    uint16_t          type; // 0=null, 1=mesh, 3=sprite, 11=LOD control node, 12=emitter
    char             *identifier;
    char             *parent;
    struct _SODNode **children;
    SODMatrix34      *local_transform;
    union
    {
        SODNodeDataEmitter *emitter;
        SODNodeDataMesh    *mesh;
    } data;
} SODNode;


/**
 * A N I M A T I O N   C H A N N E L S
 */
typedef struct {
    char          *node_id;
    uint16_t       nkeyframes;
    float          period; // the length of time one loop of this channel lasts
    uint16_t       unused;
    SODMatrix34  **keyframes; // The actual animation transforms, evenly spaced over time 'channel_period'
} SODAnimationChannel;


/**
 * A N I M A T I O N   R E F E R E N C E S
 */
typedef struct {
    uint8_t  type;
    char    *node_id;
    char    *animation_id; // .spr animation to be applied to this node
    float    playback_offset;
} SODAnimationReference;


/**
 * T O P   L E V E L
 */
typedef struct {
    char  id[10];
    float version;
} SODFileHeader;

/* TODO refactor top level arrays to use this type */
typedef struct {
    uint16_t  count;
    void     *data;
    void     (*free)(void *);
} SODArray;

typedef struct {
    uint16_t              count;
    SODLightingMaterial **array;
} SODLightingMaterials;

typedef struct {
    uint16_t  count;
    SODNode **array;
} SODNodes;

typedef struct {
    uint16_t              count;
    SODAnimationChannel **array;
} SODAnimationChannels;

typedef struct {
    uint16_t                count;
    SODAnimationReference **array;
} SODAnimationReferences;

typedef struct {
    SODFileHeader          *header;
    SODLightingMaterials   *lighting_materials;
    SODNodes               *nodes;
    SODAnimationChannels   *animation_channels;
    SODAnimationReferences *animation_references;
} SODFile;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * C O N S T R U C T O R S
 */
static inline void * sod_new(size_t size)
{
    void *p = malloc(size);
    memset(p, 0, size);
    return p;
}
#define sod_new_animation_channel()     (SODAnimationChannel *)   sod_new(sizeof(SODAnimationChannel))
#define sod_new_animation_channels()    (SODAnimationChannels *)  sod_new(sizeof(SODAnimationChannels))
#define sod_new_animation_reference()   (SODAnimationReference *) sod_new(sizeof(SODAnimationReference))
#define sod_new_animation_references()  (SODAnimationReferences *)sod_new(sizeof(SODAnimationReferences))
#define sod_new_color()                 (SODColor *)              sod_new(sizeof(SODColor))
#define sod_new_face()                  (SODFace *)               sod_new(sizeof(SODFace))
#define sod_new_face_vertex()           (SODFaceVertex *)         sod_new(sizeof(SODFaceVertex))
#define sod_new_file()                  (SODFile *)               sod_new(sizeof(SODFile))
#define sod_new_file_header()           (SODFileHeader *)         sod_new(sizeof(SODFileHeader))
#define sod_new_identifier()            (char *)         sod_new(sizeof(SODIdentifier))
#define sod_new_lighting_material()     (SODLightingMaterial *)   sod_new(sizeof(SODLightingMaterial))
#define sod_new_lighting_materials()    (SODLightingMaterials *)  sod_new(sizeof(SODLightingMaterials))
#define sod_new_matrix34()              (SODMatrix34 *)           sod_new(sizeof(SODMatrix34))
#define sod_new_node()                  (SODNode *)               sod_new(sizeof(SODNode))
#define sod_new_nodes()                 (SODNodes *)              sod_new(sizeof(SODNodes))
#define sod_new_node_data_emitter()     (SODNodeDataEmitter *)    sod_new(sizeof(SODNodeDataEmitter))
#define sod_new_node_data_mesh()        (SODNodeDataMesh *)       sod_new(sizeof(SODNodeDataMesh))
#define sod_new_vector2()               (SODVector2 *)            sod_new(sizeof(SODVector2))
#define sod_new_vector3()               (SODVector3 *)            sod_new(sizeof(SODVector3))
#define sod_new_vertex_lighting_group() (SODVertexLightingGroup *)sod_new(sizeof(SODVertexLightingGroup))

/**
 * M A C R O S
 */
#define sod_free_list(LIST, FREE_FUNC)              \
    do {                                            \
        if (LIST && LIST->array) {                  \
            for (int i = 0; i < LIST->count; i++) { \
                FREE_FUNC(LIST->array[i]);          \
            }                                       \
            free(LIST->array);                      \
            free(LIST);                             \
        }                                           \
    } while (0)


/**
 * F U N C T I O N S
 */
void sod_free_animation_channel(SODAnimationChannel *);
void sod_free_animation_channels(SODAnimationChannels *);
void sod_free_animation_reference(SODAnimationReference *);
void sod_free_animation_references(SODAnimationReferences *);
void sod_free_face(SODFace *);
void sod_free_file(SODFile *);
void sod_free_lighting_material(SODLightingMaterial *);
void sod_free_lighting_materials(SODLightingMaterials *);
void sod_free_matrix34(SODMatrix34 *);
void sod_free_node(SODNode *);
void sod_free_node_data_mesh(SODNodeDataMesh *);
void sod_free_nodes(SODNodes *);
void sod_free_vertex_lighting_group(SODVertexLightingGroup *);

void sod_print_animation_channel(SODAnimationChannel *, uint8_t);
void sod_print_animation_channels(SODAnimationChannels *);
void sod_print_animation_reference(SODAnimationReference *, uint8_t);
void sod_print_animation_references(SODAnimationReferences *);
void sod_print_header(SODFileHeader *);
void sod_print_identifier(const char *, const char *, uint8_t);
void sod_print_lighting_material(SODLightingMaterial *);
void sod_print_lighting_materials(SODLightingMaterials *);
void sod_print_matrix34(SODMatrix34 *, uint8_t);
void sod_print_node(SODNode *);
void sod_print_node_data_mesh(SODNodeDataMesh *, uint8_t);
void sod_print_nodes(SODNodes *);
void sod_print_vertex_lighting_group(SODVertexLightingGroup *, uint8_t);

void   sod_read_animation_channel(SODAnimationChannel *, FILE *);
void   sod_read_animation_channels(SODAnimationChannels *, FILE *);
void   sod_read_animation_reference(SODAnimationReference *, FILE *);
void   sod_read_animation_references(SODAnimationReferences *, FILE *);
void   sod_read_color(SODColor *, FILE *);
void   sod_read_face(SODFace *, FILE *);
void   sod_read_face_vertex(SODFaceVertex *, FILE *);
void   sod_read_file(SODFile *, FILE *file);
void   sod_read_file_header(SODFileHeader *, FILE *);
char * sod_read_identifier(FILE *);
void   sod_read_lighting_material(SODLightingMaterial *, FILE *);
void   sod_read_lighting_materials(SODLightingMaterials *, FILE *);
void   sod_read_matrix34(SODMatrix34 *, FILE *);
void   sod_read_node(SODNode *, FILE *);
void   sod_read_node_data_mesh(SODNodeDataMesh *, FILE *);
void   sod_read_nodes(SODNodes *, FILE *);
void   sod_read_vector2(SODVector2 *, FILE *);
void   sod_read_vector3(SODVector3 *, FILE *);
void   sod_read_vertex_lighting_group(SODVertexLightingGroup *, FILE *);

const char * sod_str_lighting_model(enum SODLightingModel);
const char * sod_str_node_mesh_cull_type(enum SODNodeMeshCullType);
const char * sod_str_node_type(enum SODNodeType);

SODNode * sod_find_node(SODNodes *, const char *);

#ifdef __cplusplus
}
#endif


#endif//__SOD_H__
