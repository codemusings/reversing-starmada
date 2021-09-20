#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>

#include "model.hpp"
#include "sod.h"
#include "shader.hpp"
#include "viewer.hpp"

#define WIDTH 800
#define HEIGHT 600

static float z_offset = -25.0f;
static float rot_degree = 0.0f;

void vwr_process_input(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        z_offset -= 0.1f;
        printf("z: %.1f\n", z_offset);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        z_offset += 0.1f;
        printf("z: %.1f\n", z_offset);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        rot_degree -= 5.0f;
        printf("rotation: %f°\n", rot_degree);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        rot_degree += 5.0f;
        printf("rotation: %f°\n", rot_degree);
    }
}

void vwr_resize_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main(int argc, char *argv[])
{
    //if (argc != 2) {
    //    fprintf(stderr, "Usage: %s <SOD file>\n", argv[0]);
    //    return 1;
    //}

    // const char *mdl_filepath = "CDROM/setup/data/SOD/Fentd.SOD";
    // const char *img_filepath = "CDROM/setup/data/Textures/RGB/Fentd.jpg";
    // const char *node_name = "fentd";
    const char *mdl_filepath = "CDROM/setup/data/SOD/Fente.SOD";
    const char *img_filepath = "CDROM/setup/data/Textures/RGB/FEntE.tga";
    const char *node_name = "fbattle_1";
    // const char *mdl_filepath = "CDROM/setup/data/SOD/Borpod9.SOD";
    // const char *img_filepath = "CDROM/setup/data/Textures/RGB/Bbuildng.tga";
    // const char *node_name = "Borpod9_ultritiumburst";

    //FILE *fp = fopen(argv[1], "r");
    FILE *fp = fopen(mdl_filepath, "r");
    if (!fp) {
        fprintf(stderr, "Unable to read input file %s\n", argv[1]);
        return 1;
    }

    SODFile *file = sod_new_file();
    sod_read_file(file, fp);
    fclose(fp);

    SODNode *node = sod_find_node(file->nodes, node_name);
    if (node == NULL) {
        std::cerr << "Unable to find node" << std::endl;
        return 1;
    }

    Model m(node, img_filepath);
    float *vertices = m.getVertices();

    //SODNodeDataMesh *data = node->data.mesh;
    //int stride = 3 + 3 + 2; // coords + color + tex_coords
    //int total_size = 0;
    //for (int i = 0; i < data->ngroups; i++) {
    //    SODVertexLightingGroup *group = data->lighting_groups[i];
    //    int group_size = 0;
    //    for (int j = 0; j < group->nfaces; j++) {
    //        group_size = group->nfaces * 3 * stride;
    //    }
    //    total_size += group_size;
    //}

    //float vertices[total_size];
    //int lgroup_offset = 0;
    //for (int i = 0; i < data->ngroups; i++) {
    //    SODVertexLightingGroup *group = data->lighting_groups[i];
    //    for (int j = 0; j < group->nfaces; j++) {
    //        // 3 * (3 + 3 + 2) = 24
    //        int face_offset = lgroup_offset + j * 3 * stride;
    //        for (int k = 0; k < 3; k++) {
    //            printf("%d\n", face_offset + k * stride);
    //            SODFaceVertex *vertex = group->faces[j]->face_vertices[k];
    //            uint16_t v = vertex->index_vertices;
    //            uint16_t t = vertex->index_texcoords;
    //            // coords
    //            vertices[face_offset + k * stride + 0] = data->vertices[v]->x;
    //            vertices[face_offset + k * stride + 1] = data->vertices[v]->y;
    //            vertices[face_offset + k * stride + 2] = data->vertices[v]->z;
    //            // color
    //            vertices[face_offset + k * stride + 3] = 1.0f;
    //            vertices[face_offset + k * stride + 4] = 1.0f;
    //            vertices[face_offset + k * stride + 5] = 1.0f;
    //            // texture coords
    //            vertices[face_offset + k * stride + 6] = data->texcoords[t]->u;
    //            vertices[face_offset + k * stride + 7] = data->texcoords[t]->v;
    //        }
    //    }
    //    lgroup_offset += group->nfaces * 3 * stride;
    //}

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "SOD Viewer", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return 1;
    }

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, vwr_resize_callback);

    Shader shader("./src/vs.glsl", "./src/fs.glsl");

    //float vertices[] = {
    //     0.5f,  0.5f, 0.0f,  // top right
    //     0.5f, -0.5f, 0.0f,  // bottom right
    //    -0.5f, -0.5f, 0.0f,  // bottom left
    //    -0.5f,  0.5f, 0.0f   // top left
    //};
    //unsigned int indices[] = {
    //    0, 1, 3,
    //    1, 2, 3
    //};
    // float vertices[] = {
    //     -0.5f, -0.5f, 0.0f,
    //      0.0f,  0.5f, 0.0f,
    //      0.5f, -0.5f, 0.0f,
    // };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    printf("%ld\n", m.getNumberOfVertices() * sizeof(float));

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, m.getNumberOfVertices() * sizeof(float), vertices, GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //stbi_set_flip_vertically_on_load(true);
    //int width, height, num_channels;
    //unsigned char *tex_data;
    //tex_data = stbi_load(img_filepath, &width, &height, &num_channels, 0);
    int width = m.getTextureWidth();
    int height = m.getTextureHeight();
    unsigned char *tex_data = m.getTextureData();
    if (tex_data) {
        std::cout << "WIDTH: " << width << ", HEIGHT: " << height << std::endl;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Unable to load texture: " << img_filepath << std::endl;
        return 1;
    }
    //stbi_image_free(tex_data);

    /* unbind the currently bound buffer & array */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {

        vwr_process_input(window);

        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
        model = glm::rotate(model, glm::radians(rot_degree), glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", model);

        glm::mat4 view = glm::mat4(1.0f);
        // right
        SODVector3 *vec3 = node->local_transform->right;
        view = glm::translate(view, glm::vec3(vec3->x, vec3->y, vec3->z));
        // up
        vec3 = node->local_transform->up;
        view = glm::translate(view, glm::vec3(vec3->x, vec3->y, vec3->z));
        // front
        vec3 = node->local_transform->front;
        view = glm::translate(view, glm::vec3(vec3->x, vec3->y, vec3->z));
        // position
        vec3 = node->local_transform->position;
        view = glm::translate(view, glm::vec3(vec3->x, vec3->y, vec3->z));
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, z_offset));
        shader.setMat4("view", view);

        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);
        shader.setMat4("proj", proj);

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices));
        glDrawArrays(GL_TRIANGLES, 0, m.getNumberOfVertices() * sizeof(float));
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();

    sod_free_file(file);

    return 0;
}
