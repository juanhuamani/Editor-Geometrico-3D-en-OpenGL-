#pragma once

#include "types.h"

Camera* getActiveCamera();
Camera createDefaultCamera();
Camera createFrontCamera();
Camera createTopCamera();
Camera createSideCamera();
void setCamera();