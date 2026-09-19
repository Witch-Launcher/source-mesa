/*
 * kk_gpu_detect.h - Runtime GPU detection for KosmicKrisp on iOS
 *
 * Detects Apple GPU family at runtime using MTLGPUFamily API.
 * Maps GPU capabilities to Vulkan feature support levels.
 *
 * Usage:
 *   KKGPUCaps caps = kk_detect_gpu();
 *   if (caps.can_use_kosmickrisp) { ... }
 */

#ifndef KK_GPU_DETECT_H
#define KK_GPU_DETECT_H

#include <stdint.h>

#ifdef __OBJC__
#import <Metal/Metal.h>
#endif

typedef enum {
    KK_GPU_FAMILY_UNKNOWN = 0,
    KK_GPU_FAMILY_APPLE1 = 1,   // A7
    KK_GPU_FAMILY_APPLE2 = 2,   // A8
    KK_GPU_FAMILY_APPLE3 = 3,   // A9/A10
    KK_GPU_FAMILY_APPLE4 = 4,   // A11
    KK_GPU_FAMILY_APPLE5 = 5,   // A12
    KK_GPU_FAMILY_APPLE6 = 6,   // A13 (Tier 2 AB, image atomics)
    KK_GPU_FAMILY_APPLE7 = 7,   // A14/M1 (Metal 3/4, mesh shaders)
    KK_GPU_FAMILY_APPLE8 = 8,   // A15/A16/M2
    KK_GPU_FAMILY_APPLE9 = 9,   // A17 Pro/M3/M4 (HW RT)
    KK_GPU_FAMILY_APPLE10 = 10, // A18/M5
} KKGPUFamily;

typedef enum {
    KK_VULKAN_LEVEL_NONE = 0,     // No Vulkan support
    KK_VULKAN_LEVEL_1_2 = 12,     // Vulkan 1.2 (A13)
    KK_VULKAN_LEVEL_1_3 = 13,     // Vulkan 1.3 (A14+)
    KK_VULKAN_LEVEL_1_4 = 14,     // Vulkan 1.4 (A14+ with Metal 4)
} KKVulkanLevel;

typedef struct {
    KKGPUFamily family;
    KKVulkanLevel vulkan_level;
    int can_use_kosmickrisp;    // A13+ minimum
    int has_image_atomics;      // A13+ (Apple6)
    int has_vulkan_mem_model;   // A13+ (Apple6)
    int has_arg_buffers_tier2;  // A13+ (Apple6)
    int has_mesh_shaders;       // A14+ (Apple7)
    int has_hw_ray_tracing;     // A17 Pro+ (Apple9)
    int has_metal4;             // A14+ (Apple7)
    int has_64bit_atomics;      // A17 Pro+ (Apple9)
    char device_name[256];
} KKGPUCaps;

#ifdef __OBJC__
/*
 * Runtime GPU detection using Metal API.
 * Call from Objective-C code.
 */
static inline KKGPUCaps kk_detect_gpu(void) {
    KKGPUCaps caps = {0};
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    if (!device) return caps;

    // Copy device name
    const char *name = [device.name UTF8String];
    if (name) {
        strncpy(caps.device_name, name, sizeof(caps.device_name) - 1);
    }

    // Detect GPU family using MTLGPUFamily API
    if ([device respondsToSelector:@selector(supportsFamily:)]) {
        if ([device supportsFamily:MTLGPUFamilyApple10]) caps.family = KK_GPU_FAMILY_APPLE10;
        else if ([device supportsFamily:MTLGPUFamilyApple9]) caps.family = KK_GPU_FAMILY_APPLE9;
        else if ([device supportsFamily:MTLGPUFamilyApple8]) caps.family = KK_GPU_FAMILY_APPLE8;
        else if ([device supportsFamily:MTLGPUFamilyApple7]) caps.family = KK_GPU_FAMILY_APPLE7;
        else if ([device supportsFamily:MTLGPUFamilyApple6]) caps.family = KK_GPU_FAMILY_APPLE6;
        else if ([device supportsFamily:MTLGPUFamilyApple5]) caps.family = KK_GPU_FAMILY_APPLE5;
        else if ([device supportsFamily:MTLGPUFamilyApple4]) caps.family = KK_GPU_FAMILY_APPLE4;
        else if ([device supportsFamily:MTLGPUFamilyApple3]) caps.family = KK_GPU_FAMILY_APPLE3;
        else if ([device supportsFamily:MTLGPUFamilyApple2]) caps.family = KK_GPU_FAMILY_APPLE2;
        else if ([device supportsFamily:MTLGPUFamilyApple1]) caps.family = KK_GPU_FAMILY_APPLE1;
    }

    // Feature detection
    caps.has_image_atomics     = (caps.family >= KK_GPU_FAMILY_APPLE6);  // A13+
    caps.has_vulkan_mem_model  = (caps.family >= KK_GPU_FAMILY_APPLE6);  // A13+
    caps.has_arg_buffers_tier2 = (caps.family >= KK_GPU_FAMILY_APPLE6);  // A13+
    caps.has_mesh_shaders      = (caps.family >= KK_GPU_FAMILY_APPLE7);  // A14+
    caps.has_metal4            = (caps.family >= KK_GPU_FAMILY_APPLE7);  // A14+
    caps.has_hw_ray_tracing   = (caps.family >= KK_GPU_FAMILY_APPLE9);  // A17 Pro+
    caps.has_64bit_atomics    = (caps.family >= KK_GPU_FAMILY_APPLE9);  // A17 Pro+

    // KosmicKrisp minimum: A13 (Apple6) for image atomics + Vulkan Memory Model
    caps.can_use_kosmickrisp = caps.has_image_atomics && caps.has_vulkan_mem_model;

    // Vulkan version based on capabilities
    if (caps.has_metal4 && caps.has_image_atomics) {
        caps.vulkan_level = KK_VULKAN_LEVEL_1_4;  // A14+
    } else if (caps.has_image_atomics && caps.has_vulkan_mem_model) {
        caps.vulkan_level = KK_VULKAN_LEVEL_1_2;  // A13
    } else {
        caps.vulkan_level = KK_VULKAN_LEVEL_NONE;
    }

    return caps;
}
#endif

/*
 * Get GPU family name string.
 */
static inline const char *kk_gpu_family_name(KKGPUFamily family) {
    switch (family) {
        case KK_GPU_FAMILY_APPLE1:  return "Apple1 (A7)";
        case KK_GPU_FAMILY_APPLE2:  return "Apple2 (A8)";
        case KK_GPU_FAMILY_APPLE3:  return "Apple3 (A9/A10)";
        case KK_GPU_FAMILY_APPLE4:  return "Apple4 (A11)";
        case KK_GPU_FAMILY_APPLE5:  return "Apple5 (A12)";
        case KK_GPU_FAMILY_APPLE6:  return "Apple6 (A13)";
        case KK_GPU_FAMILY_APPLE7:  return "Apple7 (A14/M1)";
        case KK_GPU_FAMILY_APPLE8:  return "Apple8 (A15/A16/M2)";
        case KK_GPU_FAMILY_APPLE9:  return "Apple9 (A17 Pro/M3/M4)";
        case KK_GPU_FAMILY_APPLE10: return "Apple10 (A18/M5)";
        default: return "Unknown";
    }
}

/*
 * Get Vulkan level string.
 */
static inline const char *kk_vulkan_level_name(KKVulkanLevel level) {
    switch (level) {
        case KK_VULKAN_LEVEL_1_2: return "Vulkan 1.2";
        case KK_VULKAN_LEVEL_1_3: return "Vulkan 1.3";
        case KK_VULKAN_LEVEL_1_4: return "Vulkan 1.4";
        default: return "None";
    }
}

#endif /* KK_GPU_DETECT_H */
