#include "scene/mesh_teapot.hpp"

#include <algorithm>

namespace cg
{

namespace
{

std::array<Vector3, 306> teapot_vertex_list = {Vector3(1.4f, 0.0f, 2.4f),
                                               Vector3(1.4f, -0.784f, 2.4f),
                                               Vector3(0.784f, -1.4f, 2.4f),
                                               Vector3(0.0f, -1.4f, 2.4f),
                                               Vector3(1.3375f, 0.0f, 2.53125f),
                                               Vector3(1.3375f, -0.749f, 2.53125f),
                                               Vector3(0.749f, -1.3375f, 2.53125f),
                                               Vector3(0.0f, -1.3375f, 2.53125f),
                                               Vector3(1.4375f, 0.0f, 2.53125f),
                                               Vector3(1.4375f, -0.805f, 2.53125f),
                                               Vector3(0.805f, -1.4375f, 2.53125f),
                                               Vector3(0.0f, -1.4375f, 2.53125f),
                                               Vector3(1.5f, 0.0f, 2.4f),
                                               Vector3(1.5f, -0.84f, 2.4f),
                                               Vector3(0.84f, -1.5f, 2.4f),
                                               Vector3(0.0f, -1.5f, 2.4f),
                                               Vector3(-0.784f, -1.4f, 2.4f),
                                               Vector3(-1.4f, -0.784f, 2.4f),
                                               Vector3(-1.4f, 0.0f, 2.4f),
                                               Vector3(-0.749f, -1.3375f, 2.53125f),
                                               Vector3(-1.3375f, -0.749f, 2.53125f),
                                               Vector3(-1.3375f, 0.0f, 2.53125f),
                                               Vector3(-0.805f, -1.4375f, 2.53125f),
                                               Vector3(-1.4375f, -0.805f, 2.53125f),
                                               Vector3(-1.4375f, 0.0f, 2.53125f),
                                               Vector3(-0.84f, -1.5f, 2.4f),
                                               Vector3(-1.5f, -0.84f, 2.4f),
                                               Vector3(-1.5f, 0.0f, 2.4f),
                                               Vector3(-1.4f, 0.784f, 2.4f),
                                               Vector3(-0.784f, 1.4f, 2.4f),
                                               Vector3(0.0f, 1.4f, 2.4f),
                                               Vector3(-1.3375f, 0.749f, 2.53125f),
                                               Vector3(-0.749f, 1.3375f, 2.53125f),
                                               Vector3(0.0f, 1.3375f, 2.53125f),
                                               Vector3(-1.4375f, 0.805f, 2.53125f),
                                               Vector3(-0.805f, 1.4375f, 2.53125f),
                                               Vector3(0.0f, 1.4375f, 2.53125f),
                                               Vector3(-1.5f, 0.84f, 2.4f),
                                               Vector3(-0.84f, 1.5f, 2.4f),
                                               Vector3(0.0f, 1.5f, 2.4f),
                                               Vector3(0.784f, 1.4f, 2.4f),
                                               Vector3(1.4f, 0.784f, 2.4f),
                                               Vector3(0.749f, 1.3375f, 2.53125f),
                                               Vector3(1.3375f, 0.749f, 2.53125f),
                                               Vector3(0.805f, 1.4375f, 2.53125f),
                                               Vector3(1.4375f, 0.805f, 2.53125f),
                                               Vector3(0.84f, 1.5f, 2.4f),
                                               Vector3(1.5f, 0.84f, 2.4f),
                                               Vector3(1.75f, 0.0f, 1.875),
                                               Vector3(1.75f, -0.98f, 1.875),
                                               Vector3(0.98f, -1.75f, 1.875),
                                               Vector3(0.0f, -1.75f, 1.875),
                                               Vector3(2.0f, 0.0f, 1.35f),
                                               Vector3(2.0f, -1.12f, 1.35f),
                                               Vector3(1.12f, -2.0f, 1.35f),
                                               Vector3(0.0f, -2.0f, 1.35f),
                                               Vector3(2.0f, 0.0f, 0.9f),
                                               Vector3(2.0f, -1.12f, 0.9f),
                                               Vector3(1.12f, -2.0f, 0.9f),
                                               Vector3(0.0f, -2.0f, 0.9f),
                                               Vector3(-0.98f, -1.75f, 1.875f),
                                               Vector3(-1.75f, -0.98f, 1.875f),
                                               Vector3(-1.75f, 0.0f, 1.875f),
                                               Vector3(-1.12f, -2.0f, 1.35f),
                                               Vector3(-2.0f, -1.12f, 1.35f),
                                               Vector3(-2.0f, 0.0f, 1.35f),
                                               Vector3(-1.12f, -2.0f, 0.9f),
                                               Vector3(-2.0f, -1.12f, 0.9f),
                                               Vector3(-2.0f, 0.0f, 0.9f),
                                               Vector3(-1.75f, 0.98f, 1.875f),
                                               Vector3(-0.98f, 1.75f, 1.875f),
                                               Vector3(0.0f, 1.75f, 1.875f),
                                               Vector3(-2.0f, 1.12f, 1.35f),
                                               Vector3(-1.12f, 2.0f, 1.35f),
                                               Vector3(0.0f, 2.0f, 1.35f),
                                               Vector3(-2.0f, 1.12f, 0.9f),
                                               Vector3(-1.12f, 2.0f, 0.9f),
                                               Vector3(0.0f, 2.0f, 0.9f),
                                               Vector3(0.98f, 1.75f, 1.875f),
                                               Vector3(1.75f, 0.98f, 1.875f),
                                               Vector3(1.12f, 2.0f, 1.35f),
                                               Vector3(2.0f, 1.12f, 1.35f),
                                               Vector3(1.12f, 2.0f, 0.9f),
                                               Vector3(2.0f, 1.12f, 0.9f),
                                               Vector3(2.0f, 0.0f, 0.45f),
                                               Vector3(2.0f, -1.12f, 0.45f),
                                               Vector3(1.12f, -2.0f, 0.45f),
                                               Vector3(0.0f, -2.0f, 0.45f),
                                               Vector3(1.5f, 0.0f, 0.225f),
                                               Vector3(1.5f, -0.84f, 0.225f),
                                               Vector3(0.84f, -1.5f, 0.225f),
                                               Vector3(0.0f, -1.5f, 0.225f),
                                               Vector3(1.5f, 0.0f, 0.15f),
                                               Vector3(1.5f, -0.84f, 0.15f),
                                               Vector3(0.84f, -1.5f, 0.15f),
                                               Vector3(0.0f, -1.5f, 0.15f),
                                               Vector3(-1.12f, -2.0f, 0.45f),
                                               Vector3(-2.0f, -1.12f, 0.45f),
                                               Vector3(-2.0f, 0.0f, 0.45f),
                                               Vector3(-0.84f, -1.5f, 0.225f),
                                               Vector3(-1.5f, -0.84f, 0.225f),
                                               Vector3(-1.5f, 0.0f, 0.225f),
                                               Vector3(-0.84f, -1.5f, 0.15f),
                                               Vector3(-1.5f, -0.84f, 0.15f),
                                               Vector3(-1.5f, 0.0f, 0.15f),
                                               Vector3(-2.0f, 1.12f, 0.45f),
                                               Vector3(-1.12f, 2.0f, 0.45f),
                                               Vector3(0.0f, 2.0f, 0.45f),
                                               Vector3(-1.5f, 0.84f, 0.225f),
                                               Vector3(-0.84f, 1.5f, 0.225f),
                                               Vector3(0.0f, 1.5f, 0.225f),
                                               Vector3(-1.5f, 0.84f, 0.15f),
                                               Vector3(-0.84f, 1.5f, 0.15f),
                                               Vector3(0.0f, 1.5f, 0.15f),
                                               Vector3(1.12f, 2.0f, 0.45f),
                                               Vector3(2.0f, 1.12f, 0.45f),
                                               Vector3(0.84f, 1.5f, 0.225f),
                                               Vector3(1.5f, 0.84f, 0.225f),
                                               Vector3(0.84f, 1.5f, 0.15f),
                                               Vector3(1.5f, 0.84f, 0.15f),
                                               Vector3(-1.6f, 0.0f, 2.025f),
                                               Vector3(-1.6f, -0.3f, 2.025f),
                                               Vector3(-1.5f, -0.3f, 2.25f),
                                               Vector3(-1.5f, 0.0f, 2.25f),
                                               Vector3(-2.3f, 0.0f, 2.025f),
                                               Vector3(-2.3f, -0.3f, 2.025f),
                                               Vector3(-2.5f, -0.3f, 2.25f),
                                               Vector3(-2.5f, 0.0f, 2.25f),
                                               Vector3(-2.7f, 0.0f, 2.025f),
                                               Vector3(-2.7f, -0.3f, 2.025f),
                                               Vector3(-3.0f, -0.3f, 2.25f),
                                               Vector3(-3.0f, 0.0f, 2.25f),
                                               Vector3(-2.7f, 0.0f, 1.8f),
                                               Vector3(-2.7f, -0.3f, 1.8f),
                                               Vector3(-3.0f, -0.3f, 1.8f),
                                               Vector3(-3.0f, 0.0f, 1.8f),
                                               Vector3(-1.5f, 0.3f, 2.25f),
                                               Vector3(-1.6f, 0.3f, 2.025f),
                                               Vector3(-2.5f, 0.3f, 2.25f),
                                               Vector3(-2.3f, 0.3f, 2.025f),
                                               Vector3(-3.0f, 0.3f, 2.25f),
                                               Vector3(-2.7f, 0.3f, 2.025f),
                                               Vector3(-3.0f, 0.3f, 1.8f),
                                               Vector3(-2.7f, 0.3f, 1.8f),
                                               Vector3(-2.7f, 0.0f, 1.575f),
                                               Vector3(-2.7f, -0.3f, 1.575f),
                                               Vector3(-3.0f, -0.3f, 1.35f),
                                               Vector3(-3.0f, 0.0f, 1.35f),
                                               Vector3(-2.5f, 0.0f, 1.125f),
                                               Vector3(-2.5f, -0.3f, 1.125f),
                                               Vector3(-2.65f, -0.3f, 0.9375f),
                                               Vector3(-2.65f, 0.0f, 0.9375f),
                                               Vector3(-2.0f, -0.3f, 0.9f),
                                               Vector3(-1.9f, -0.3f, 0.6f),
                                               Vector3(-1.9f, 0.0f, 0.6f),
                                               Vector3(-3.0f, 0.3f, 1.35f),
                                               Vector3(-2.7f, 0.3f, 1.575f),
                                               Vector3(-2.65f, 0.3f, 0.9375),
                                               Vector3(-2.5f, 0.3f, 1.125),
                                               Vector3(-1.9f, 0.3f, 0.6f),
                                               Vector3(-2.0f, 0.3f, 0.9f),
                                               Vector3(1.7f, 0.0f, 1.425f),
                                               Vector3(1.7f, -0.66f, 1.425f),
                                               Vector3(1.7f, -0.66f, 0.6f),
                                               Vector3(1.7f, 0.0f, 0.6f),
                                               Vector3(2.6f, 0.0f, 1.425f),
                                               Vector3(2.6f, -0.66f, 1.425f),
                                               Vector3(3.1f, -0.66f, 0.825f),
                                               Vector3(3.1f, 0.0f, 0.825f),
                                               Vector3(2.3f, 0.0f, 2.1f),
                                               Vector3(2.3f, -0.25f, 2.1f),
                                               Vector3(2.4f, -0.25f, 2.025f),
                                               Vector3(2.4f, 0.0f, 2.025f),
                                               Vector3(2.7f, 0.0f, 2.4f),
                                               Vector3(2.7f, -0.25f, 2.4f),
                                               Vector3(3.3f, -0.25f, 2.4f),
                                               Vector3(3.3f, 0.0f, 2.4f),
                                               Vector3(1.7f, 0.66f, 0.6f),
                                               Vector3(1.7f, 0.66f, 1.425f),
                                               Vector3(3.1f, 0.66f, 0.825f),
                                               Vector3(2.6f, 0.66f, 1.425f),
                                               Vector3(2.4f, 0.25f, 2.025f),
                                               Vector3(2.3f, 0.25f, 2.1f),
                                               Vector3(3.3f, 0.25f, 2.4f),
                                               Vector3(2.7f, 0.25f, 2.4f),
                                               Vector3(2.8f, 0.0f, 2.475f),
                                               Vector3(2.8f, -0.25f, 2.475f),
                                               Vector3(3.525f, -0.25f, 2.49375f),
                                               Vector3(3.525f, 0.0f, 2.49375f),
                                               Vector3(2.9f, 0.0f, 2.475f),
                                               Vector3(2.9f, -0.15f, 2.475f),
                                               Vector3(3.45f, -0.15f, 2.5125f),
                                               Vector3(3.45f, 0.0f, 2.5125f),
                                               Vector3(2.8f, 0.0f, 2.4f),
                                               Vector3(2.8f, -0.15f, 2.4f),
                                               Vector3(3.2f, -0.15f, 2.4f),
                                               Vector3(3.2f, 0.0f, 2.4f),
                                               Vector3(3.525f, 0.25f, 2.49375f),
                                               Vector3(2.8f, 0.25f, 2.475f),
                                               Vector3(3.45f, 0.15f, 2.5125f),
                                               Vector3(2.9f, 0.15f, 2.475f),
                                               Vector3(3.2f, 0.15f, 2.4f),
                                               Vector3(2.8f, 0.15f, 2.4f),
                                               Vector3(0.0f, 0.0f, 3.15f),
                                               Vector3(0.0f, -0.002f, 3.15f),
                                               Vector3(0.002f, 0.0f, 3.15f),
                                               Vector3(0.8f, 0.0f, 3.15f),
                                               Vector3(0.8f, -0.45f, 3.15f),
                                               Vector3(0.45f, -0.8f, 3.15f),
                                               Vector3(0.0f, -0.8f, 3.15f),
                                               Vector3(0.0f, 0.0f, 2.85f),
                                               Vector3(0.2f, 0.0f, 2.7f),
                                               Vector3(0.2f, -0.112f, 2.7f),
                                               Vector3(0.112f, -0.2f, 2.7f),
                                               Vector3(0.0f, -0.2f, 2.7f),
                                               Vector3(-0.002f, 0.0f, 3.15f),
                                               Vector3(-0.45f, -0.8f, 3.15f),
                                               Vector3(-0.8f, -0.45f, 3.15f),
                                               Vector3(-0.8f, 0.0f, 3.15f),
                                               Vector3(-0.112f, -0.2f, 2.7f),
                                               Vector3(-0.2f, -0.112f, 2.7f),
                                               Vector3(-0.2f, 0.0f, 2.7f),
                                               Vector3(0.0f, 0.002f, 3.15f),
                                               Vector3(-0.8f, 0.45f, 3.15f),
                                               Vector3(-0.45f, 0.8f, 3.15f),
                                               Vector3(0.0f, 0.8f, 3.15f),
                                               Vector3(-0.2f, 0.112f, 2.7f),
                                               Vector3(-0.112f, 0.2f, 2.7f),
                                               Vector3(0.0f, 0.2f, 2.7f),
                                               Vector3(0.45f, 0.8f, 3.15f),
                                               Vector3(0.8f, 0.45f, 3.15f),
                                               Vector3(0.112f, 0.2f, 2.7f),
                                               Vector3(0.2f, 0.112f, 2.7f),
                                               Vector3(0.4f, 0.0f, 2.55f),
                                               Vector3(0.4f, -0.224f, 2.55f),
                                               Vector3(0.224f, -0.4f, 2.55f),
                                               Vector3(0.0f, -0.4f, 2.55f),
                                               Vector3(1.3f, 0.0f, 2.55f),
                                               Vector3(1.3f, -0.728f, 2.55f),
                                               Vector3(0.728f, -1.3f, 2.55f),
                                               Vector3(0.0f, -1.3f, 2.55f),
                                               Vector3(1.3f, 0.0f, 2.4f),
                                               Vector3(1.3f, -0.728f, 2.4f),
                                               Vector3(0.728f, -1.3f, 2.4f),
                                               Vector3(0.0f, -1.3f, 2.4f),
                                               Vector3(-0.224f, -0.4f, 2.55f),
                                               Vector3(-0.4f, -0.224f, 2.55f),
                                               Vector3(-0.4f, 0.0f, 2.55f),
                                               Vector3(-0.728f, -1.3f, 2.55f),
                                               Vector3(-1.3f, -0.728f, 2.55f),
                                               Vector3(-1.3f, 0.0f, 2.55f),
                                               Vector3(-0.728f, -1.3f, 2.4f),
                                               Vector3(-1.3f, -0.728f, 2.4f),
                                               Vector3(-1.3f, 0.0f, 2.4f),
                                               Vector3(-0.4f, 0.224f, 2.55f),
                                               Vector3(-0.224f, 0.4f, 2.55f),
                                               Vector3(0.0f, 0.4f, 2.55f),
                                               Vector3(-1.3f, 0.728f, 2.55f),
                                               Vector3(-0.728f, 1.3f, 2.55f),
                                               Vector3(0.0f, 1.3f, 2.55f),
                                               Vector3(-1.3f, 0.728f, 2.4f),
                                               Vector3(-0.728f, 1.3f, 2.4f),
                                               Vector3(0.0f, 1.3f, 2.4f),
                                               Vector3(0.224f, 0.4f, 2.55f),
                                               Vector3(0.4f, 0.224f, 2.55f),
                                               Vector3(0.728f, 1.3f, 2.55f),
                                               Vector3(1.3f, 0.728f, 2.55f),
                                               Vector3(0.728f, 1.3f, 2.4f),
                                               Vector3(1.3f, 0.728f, 2.4f),
                                               Vector3(0.0f, 0.0f, 0.0f),
                                               Vector3(1.5f, 0.0f, 0.15f),
                                               Vector3(1.5f, 0.84f, 0.15f),
                                               Vector3(0.84f, 1.5f, 0.15f),
                                               Vector3(0.0f, 1.5f, 0.15f),
                                               Vector3(1.5f, 0.0f, 0.075f),
                                               Vector3(1.5f, 0.84f, 0.075f),
                                               Vector3(0.84f, 1.5f, 0.075f),
                                               Vector3(0.0f, 1.5f, 0.075f),
                                               Vector3(1.425f, 0.0f, 0.0f),
                                               Vector3(1.425f, 0.798f, 0.0f),
                                               Vector3(0.798f, 1.425f, 0.0f),
                                               Vector3(0.0f, 1.425f, 0.0f),
                                               Vector3(-0.84f, 1.5f, 0.15f),
                                               Vector3(-1.5f, 0.84f, 0.15f),
                                               Vector3(-1.5f, 0.0f, 0.15f),
                                               Vector3(-0.84f, 1.5f, 0.075f),
                                               Vector3(-1.5f, 0.84f, 0.075f),
                                               Vector3(-1.5f, 0.0f, 0.075f),
                                               Vector3(-0.798f, 1.425f, 0.0f),
                                               Vector3(-1.425f, 0.798f, 0.0f),
                                               Vector3(-1.425f, 0.0f, 0.0f),
                                               Vector3(-1.5f, -0.84f, 0.15f),
                                               Vector3(-0.84f, -1.5f, 0.15f),
                                               Vector3(0.0f, -1.5f, 0.15f),
                                               Vector3(-1.5f, -0.84f, 0.075f),
                                               Vector3(-0.84f, -1.5f, 0.075f),
                                               Vector3(0.0f, -1.5f, 0.075f),
                                               Vector3(-1.425f, -0.798f, 0.0f),
                                               Vector3(-0.798f, -1.425f, 0.0f),
                                               Vector3(0.0f, -1.425f, 0.0f),
                                               Vector3(0.84f, -1.5f, 0.15f),
                                               Vector3(1.5f, -0.84f, 0.15f),
                                               Vector3(0.84f, -1.5f, 0.075f),
                                               Vector3(1.5f, -0.84f, 0.075f),
                                               Vector3(0.798f, -1.425f, 0.0f),
                                               Vector3(1.425f, -0.798f, 0.0f)};

// 32 patches are each defined by 16 vertices, arranged in a 4 x 4 array
// Numbering scheme for teapot has vertices labeled from 1 to 306
static uint32_t PatchIndices[32][4][4] = {
    {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}},
    {{4, 17, 18, 19}, {8, 20, 21, 22}, {12, 23, 24, 25}, {16, 26, 27, 28}},
    {{19, 29, 30, 31}, {22, 32, 33, 34}, {25, 35, 36, 37}, {28, 38, 39, 40}},
    {{31, 41, 42, 1}, {34, 43, 44, 5}, {37, 45, 46, 9}, {40, 47, 48, 13}},
    {{13, 14, 15, 16}, {49, 50, 51, 52}, {53, 54, 55, 56}, {57, 58, 59, 60}},
    {{16, 26, 27, 28}, {52, 61, 62, 63}, {56, 64, 65, 66}, {60, 67, 68, 69}},
    {{28, 38, 39, 40}, {63, 70, 71, 72}, {66, 73, 74, 75}, {69, 76, 77, 78}},
    {{40, 47, 48, 13}, {72, 79, 80, 49}, {75, 81, 82, 53}, {78, 83, 84, 57}},
    {{57, 58, 59, 60}, {85, 86, 87, 88}, {89, 90, 91, 92}, {93, 94, 95, 96}},
    {{60, 67, 68, 69}, {88, 97, 98, 99}, {92, 100, 101, 102}, {96, 103, 104, 105}},
    {{69, 76, 77, 78}, {99, 106, 107, 108}, {102, 109, 110, 111}, {105, 112, 113, 114}},
    {{78, 83, 84, 57}, {108, 115, 116, 85}, {111, 117, 118, 89}, {114, 119, 120, 93}},
    {{121, 122, 123, 124}, {125, 126, 127, 128}, {129, 130, 131, 132}, {133, 134, 135, 136}},
    {{124, 137, 138, 121}, {128, 139, 140, 125}, {132, 141, 142, 129}, {136, 143, 144, 133}},
    {{133, 134, 135, 136}, {145, 146, 147, 148}, {149, 150, 151, 152}, {69, 153, 154, 155}},
    {{136, 143, 144, 133}, {148, 156, 157, 145}, {152, 158, 159, 149}, {155, 160, 161, 69}},
    {{162, 163, 164, 165}, {166, 167, 168, 169}, {170, 171, 172, 173}, {174, 175, 176, 177}},
    {{165, 178, 179, 162}, {169, 180, 181, 166}, {173, 182, 183, 170}, {177, 184, 185, 174}},
    {{174, 175, 176, 177}, {186, 187, 188, 189}, {190, 191, 192, 193}, {194, 195, 196, 197}},
    {{177, 184, 185, 174}, {189, 198, 199, 186}, {193, 200, 201, 190}, {197, 202, 203, 194}},
    {{204, 204, 204, 204}, {207, 208, 209, 210}, {211, 211, 211, 211}, {212, 213, 214, 215}},
    {{204, 204, 204, 204}, {210, 217, 218, 219}, {211, 211, 211, 211}, {215, 220, 221, 222}},
    {{204, 204, 204, 204}, {219, 224, 225, 226}, {211, 211, 211, 211}, {222, 227, 228, 229}},
    {{204, 204, 204, 204}, {226, 230, 231, 207}, {211, 211, 211, 211}, {229, 232, 233, 212}},
    {{212, 213, 214, 215}, {234, 235, 236, 237}, {238, 239, 240, 241}, {242, 243, 244, 245}},
    {{215, 220, 221, 222}, {237, 246, 247, 248}, {241, 249, 250, 251}, {245, 252, 253, 254}},
    {{222, 227, 228, 229}, {248, 255, 256, 257}, {251, 258, 259, 260}, {254, 261, 262, 263}},
    {{229, 232, 233, 212}, {257, 264, 265, 234}, {260, 266, 267, 238}, {263, 268, 269, 242}},
    {{270, 270, 270, 270}, {279, 280, 281, 282}, {275, 276, 277, 278}, {271, 272, 273, 274}},
    {{270, 270, 270, 270}, {282, 289, 290, 291}, {278, 286, 287, 288}, {274, 283, 284, 285}},
    {{270, 270, 270, 270}, {291, 298, 299, 300}, {288, 295, 296, 297}, {285, 292, 293, 294}},
    {{270, 270, 270, 270}, {300, 305, 306, 279}, {297, 303, 304, 275}, {294, 301, 302, 271}}};
} // namespace

MeshTeapot::MeshTeapot(uint16_t level, int32_t position_loc, int32_t normal_loc)
{
    // Data is 32 patches, each with a 4x4 array of point[3].
    // Convert array data into a triple-array of Vector3.
    using PatchType = std::array<std::array<Vector3, 4>, 4>;
    std::array<PatchType, 32> data;

    for(size_t patch = 0; patch < 32; patch++)
    {
        for(uint32_t j = 0; j < 4; j++)
        {
            for(uint32_t k = 0; k < 4; k++)
            {
                // Put teapot data into single array for subdivision
                data[patch][j][k] = teapot_vertex_list[PatchIndices[patch][j][k] - 1];
            }
        }
    }

    // Level 6 or higher would produce more than 65,536 vertices
    level = std::max<uint16_t>(level, 5);

    // Subdivide all 32 patches
    for(size_t patch = 0; patch < 32; patch++) { divide_patch(data[patch], level); }

    // End the mesh - construct vertex normals by averaging
    end(position_loc, normal_loc);
}

void MeshTeapot::divide_patch(std::array<std::array<Vector3, 4>, 4> patch, uint16_t level)
{
    // Helper function to determine the number of points on each individual curve.
    // Note: for integers, this is MUCH faster than std::pow
    auto powi = [](uint32_t val, uint32_t exp) -> uint32_t
    {
        uint32_t res = 1;
        for(uint32_t i = 0; i < exp; ++i) res *= val;
        return res;
    };

    // Each curve will finally have [2^(level) + 1] points.
    const auto sub_patch_dim = powi(2, level) + 1;

    std::vector<std::vector<Vector3>> row_ctrl_points(4);
    std::vector<std::vector<Vector3>> patch_vertices(sub_patch_dim);

    // Compute the curves for each patch row
    for(size_t r = 0; r < 4; ++r)
    {
        row_ctrl_points[r] =
            divide_curve(patch[0][r], patch[1][r], patch[2][r], patch[3][r], level);
    }

    // Use the patch rows as the control points for each column
    for(size_t c = 0; c < sub_patch_dim; ++c)
    {
        patch_vertices[c] = divide_curve(row_ctrl_points[0][c],
                                         row_ctrl_points[1][c],
                                         row_ctrl_points[2][c],
                                         row_ctrl_points[3][c],
                                         level);
    }

    // Add the subdividied patch to the mesh
    add_patch(patch_vertices);
}

std::vector<Vector3> MeshTeapot::divide_curve(const Vector3 &ctrl_0,
                                              const Vector3 &ctrl_1,
                                              const Vector3 &ctrl_2,
                                              const Vector3 &ctrl_3,
                                              uint16_t       level)
{
    // Level 0: add the start and end points only.
    // Note: ctrl_1 and ctrl_2 are not on the curve
    if(level == 0) return {ctrl_0, ctrl_3};

    Vector3 lt_0, lt_1, lt_2, lt_3;
    Vector3 rt_0, rt_1, rt_2, rt_3;

    Vector3 t = (ctrl_1 + ctrl_2) * 0.5f;

    // end points
    lt_0 = ctrl_0;
    rt_3 = ctrl_3;

    // side points
    lt_1 = (ctrl_0 + ctrl_1) * 0.5f;
    rt_2 = (ctrl_2 + ctrl_3) * 0.5f;

    // mid points
    lt_2 = (lt_1 + t) * 0.5f;
    rt_1 = (rt_2 + t) * 0.5f;

    // middle (on curve)
    lt_3 = (lt_2 + rt_1) * 0.5f;
    rt_0 = lt_3;

    // Recursively compute the left and right sides of the curve
    auto lt_sample = divide_curve(lt_0, lt_1, lt_2, lt_3, level - 1);
    auto rt_sample = divide_curve(rt_0, rt_1, rt_2, rt_3, level - 1);

    std::vector<Vector3> result;
    result.reserve(lt_sample.size() + rt_sample.size() - 1);

    // Insert all of the left curve
    result.insert(result.end(), lt_sample.begin(), lt_sample.end());
    // Insert all of the right curve, except for the first vertex (which is the
    // same as the last vertex in the left curve)
    result.insert(result.end(), std::next(rt_sample.begin()), rt_sample.end());

    return result;
}

uint16_t MeshTeapot::add_vertex(const Vector3 &v, bool find_existing)
{
    if(find_existing) return TriSurface::add_vertex(Point3(v.x, v.y, v.z));

    // Don't search for an equivalent vertex, just append to the vertex list
    vertices_.push_back(VertexAndNormal(Point3(v.x, v.y, v.z)));
    return static_cast<uint16_t>(vertices_.size() - 1);
}

void MeshTeapot::add_patch(const std::vector<std::vector<Vector3>> &patch)
{
    size_t patch_size = patch.size();
    size_t max_idx = patch_size - 1;

    std::vector<std::vector<uint16_t>> patch_idxs(patch_size);

    // Initialize index arrays
    for(size_t i = 0; i < patch_size; ++i) patch_idxs[i].resize(patch_size);

    // Add left column edge vertices
    for(size_t r = 0; r < patch_size; ++r) patch_idxs[0][r] = add_vertex(patch[0][r], true);

    // Add right column edge vertices
    for(size_t r = 0; r < patch_size; ++r)
        patch_idxs[max_idx][r] = add_vertex(patch[max_idx][r], true);

    // Add bottom row edge vertices
    for(size_t c = 1; c < max_idx; ++c) patch_idxs[c][0] = add_vertex(patch[c][0], true);

    // Add top row edge vertices
    for(size_t c = 1; c < max_idx; ++c)
        patch_idxs[c][max_idx] = add_vertex(patch[c][max_idx], true);

    // Add middle vertices
    for(size_t r = 1; r < max_idx; ++r)
        for(size_t c = 1; c < max_idx; ++c) patch_idxs[c][r] = add_vertex(patch[c][r], false);

    // Add the faces using the indices returned by 'add_vertex'
    for(size_t r = 0; r < max_idx; ++r)
    {
        for(size_t c = 0; c < max_idx; ++c)
        {
            faces_.push_back(patch_idxs[c][r]);
            faces_.push_back(patch_idxs[c + 1][r + 1]);
            faces_.push_back(patch_idxs[c + 1][r]);

            faces_.push_back(patch_idxs[c][r]);
            faces_.push_back(patch_idxs[c][r + 1]);
            faces_.push_back(patch_idxs[c + 1][r + 1]);
        }
    }
}

} // namespace cg
