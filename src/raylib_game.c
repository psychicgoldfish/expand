/*******************************************************************************************
*
*   raylib [core] example - 3d camera first person
*
*   Example originally created with raylib 1.3, last time updated with raylib 1.3
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2015-2024 Ramon Santamaria (@raysan5)
*
********************************************************************************************/
#include <stdio.h>
#include "raylib.h"
#include "rcamera.h"
#include "raymath.h"

#define FLT_MAX     340282346638528859811704183484516925440.0f 
//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------


// TREE FUNCTIONS AND STUFF
//---------------------------------------------------------------------------------------------
typedef struct Tree {
    Model model;
    Vector3 position;
    float scale;
    BoundingBox colBox;
    int arrayPos;
} Tree;

Tree allTrees[100];
int treeNum = 0;

Tree CreateTree(const char* modelPath, Vector3 position, float scale) {
    Tree tree;
    tree.model = LoadModel(modelPath);  // Load the tree model
    tree.position = position;           // Set its position
    tree.scale = scale;                 // Set the scale factor
    tree.colBox.max = Vector3AddValue(tree.position, (scale * 0.7f));
    tree.colBox.max = Vector3Add(tree.colBox.max, (Vector3){ 0.0f, (5.0f * tree.scale), 0.0f});
    tree.colBox.min = Vector3SubtractValue(tree.position, (scale * 0.7f));
    allTrees[treeNum] = tree;
    tree.arrayPos = treeNum;
    treeNum++;
    return tree;                        // Return the tree instance
}

void DrawTree(Tree* tree) {
    // Draw the model at its position with the given scale
    DrawModel(tree->model, tree->position, tree->scale, WHITE);
    DrawBoundingBox(tree->colBox, RED);
}

void UnloadTree(Tree* tree) {
    UnloadModel(tree->model);  // Unload the tree's model from memory
}

void KillTree(Tree* tree) {
    int index = tree->arrayPos;
    if (index < 0 || index >= treeNum) {
        // Invalid index, return
        return;
    }

    // Unload the model to free resources
    UnloadTree(&allTrees[index]);

    // Shift all trees after the removed tree to the left by 1 position
    for (int i = index; i < treeNum - 1; i++) {
        allTrees[i] = allTrees[i + 1];
    }

    // Decrease the tree count
    treeNum--;
}
Tree CreateTree(const char* modelPath, Vector3 position, float scale);
void DrawTree(Tree* tree);
void UnloadTree(Tree* tree);
void KillTree(Tree* tree);
//-----------------------------------------------------------------------------------------

// TOOLS
// -----------------------------------------------------------------------------------------

typedef struct Tool {
    Model model;
    Vector3 position;
    Vector3 rotation;
} Tool;

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera first person");

    // Define the camera to look into our 3d world (position, target, up vector)
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, 4.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    int cameraMode = CAMERA_FIRST_PERSON;
    
    Tree tree1 = CreateTree("src/resources/models/testing/tree/testpsx.glb", (Vector3){ 0.0f, 0.0f, 0.0f }, 1.0f);
    Tree tree2 = CreateTree("src/resources/models/testing/tree/testpsx.glb", (Vector3){ 10.0f, 0.0f, 2.0f }, 1.5f);
    Tree tree3 = CreateTree("src/resources/models/testing/tree/testpsx.glb", (Vector3){ -10.0f, 0.0f, -2.0f }, 1.2f);
    
    DisableCursor();
    Ray ray = { 0 };

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {

        UpdateCamera(&camera, cameraMode);                  // Update camera
        
        ray = GetMouseRay(GetMousePosition(), camera);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            for (int i = 0; i < treeNum; i++) {
                if (GetRayCollisionBox(ray, allTrees[i].colBox).hit) {
                    KillTree(&allTrees[i]);
                    break;
                }
            }
            
        } 


        BeginDrawing();

            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
              /*   DrawTree(&tree1);  // Draw the first tree
                DrawTree(&tree2);  // Draw the second tree
                Draw Tree(&tree3);  // Draw the third tree */
                for (int i = 0; i < treeNum; i++) {
                    DrawTree(&allTrees[i]);
                }

                DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ 32.0f, 32.0f }, LIGHTGRAY); // Draw ground
                DrawCube((Vector3){ -16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, BLUE);     // Draw a blue wall
                DrawCube((Vector3){ 16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, LIME);      // Draw a green wall
                DrawCube((Vector3){ 0.0f, 2.5f, 16.0f }, 32.0f, 5.0f, 1.0f, GOLD);      // Draw a yellow wall

                //DrawCube((Vector3){ 0.0f, 1.0f, 0.0f },  1.0f, 1.0f, 1.0f, RED);

            EndMode3D();

            DrawFPS(10, 10);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    for (int i = 0; i < treeNum; i++) {
        UnloadTree(&allTrees[i]);
    }
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
