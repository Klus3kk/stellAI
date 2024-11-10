#include "resources/resource_loader.h"
#include "resources/textures.h"
#include "resources/materials.h"
#include <stdio.h>

void load_material() {
    printf("Loading material...\n");
    loadPBRTextures(); 
}

void load_texture() {
    printf("Loading texture...\n");
    loadAllTextures();  
}