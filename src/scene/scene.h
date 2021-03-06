////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012, 2013
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// Scene Node heirachy
//

#ifndef OCTET_SCENE_INCLUDED
#define OCTET_SCENE_INCLUDED

#include "../scene/scene_node.h"
#include "../scene/skin.h"
#include "../scene/skeleton.h"
#include "../scene/animation.h"
#include "../scene/mesh.h"
#include "../scene/image.h"
#include "../scene/sampler.h"
#include "../scene/param.h"
#include "../scene/material.h"
#include "../scene/camera_instance.h"
#include "../scene/light_instance.h"
#include "../scene/mesh_instance.h"
#include "../scene/animation_instance.h"
#include "../scene/visual_scene.h"
#include "../scene/displacement_map.h"
#include "../scene/indexer.h"
#include "../scene/smooth.h"
#include "../scene/mesh_text.h"
#include "../scene/mesh_box.h"
#ifdef OCTET_VOXEL_TEST
  #include "../scene/mesh_voxel_subcube.h"
  #include "../scene/mesh_voxels.h"
#endif
#include "../scene/mesh_points.h"
#include "../scene/wireframe.h"

#include "../../src/examples/layer2/Sound.h"
#define CX 16
#define CY 16
#define CZ 16
#define SCX 16
#define SCY 8
#define SCZ 16
#include "../scene/chunk.h"
#include "../scene/super_chunk.h"
#include "../scene/sky.h"

#include "../scene/ui_element.h"
#include "../scene/ui_builder.h"

namespace octet {
  using namespace scene;
}

#endif
