#include <stdio.h>
#include <stdlib.h>

#include "memory_arena.h"

#define VOLK_IMPLEMENTATION
#include <volk.h>

#include <GLFW/glfw3.h>

#define KB(x) (1024 * x)
#define MB(x) (1024 * KB(x))

enum {
	WINDOW_WIDTH = 1280,
	WINDOW_HEIGHT = 720,
};

#ifdef PLAGUE_DEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
    VkDebugUtilsMessageTypeFlagsEXT             type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void*                                       user_data) {
	
	switch (severity) {
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			printf("\x1b[37m[VERBOSE]: %s\n", callback_data->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			printf("\x1b[32m[INFO]: %s\n", callback_data->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			printf("\x1b[33m[WARNING]: %s\n", callback_data->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		default:
			printf("\x1b[31m[VERBOSE]: %s\n", callback_data->pMessage);
			break;
	}
	return VK_FALSE;
}

static void fill_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT* create_info) {
	*create_info = (VkDebugUtilsMessengerCreateInfoEXT) {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.pfnUserCallback = debug_callback,
	};
}

static VkDebugUtilsMessengerEXT create_messenger(VkInstance instance) {
	VkDebugUtilsMessengerCreateInfoEXT create_info = {0};
	fill_debug_messenger_create_info(&create_info);

	VkDebugUtilsMessengerEXT messenger = NULL;
	VkResult result = vkCreateDebugUtilsMessengerEXT(instance, &create_info, NULL, &messenger);
	if (result != VK_SUCCESS) {
		fprintf(stderr, "vkCreateDebugUtilsMessengerEXT failed with return code %d", result);
		return NULL;
	}

	return messenger;
}

#endif // PLAGUE_DEBUG

static VkInstance create_instance(plg_memory_arena_t* arena) {
	uint32_t extension_count = 0;
	char const** required_extensions = glfwGetRequiredInstanceExtensions(&extension_count);
	if (required_extensions == NULL) {
		fprintf(stderr, "failed to get vulkan extensions required by glfw\n");
		return NULL;
	}

	char const** extensions = required_extensions;
#ifdef PLAGUE_DEBUG
	extension_count += 1;
	extensions = plg_arena_push(arena, extension_count * sizeof(char const*));
	for (uint32_t i = 0; i < extension_count - 1; i++) extensions[i] = required_extensions[i];
	extensions[extension_count - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

	char const* layers[] = { "VK_LAYER_KHRONOS_validation" };
	VkDebugUtilsMessengerCreateInfoEXT debug_info = {0};
	fill_debug_messenger_create_info(&debug_info);
#endif // PLAGUE_DEBUG

	VkInstanceCreateInfo create_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.enabledExtensionCount = extension_count,
		.ppEnabledExtensionNames = extensions,
		.pApplicationInfo = &(VkApplicationInfo) {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.apiVersion = VK_API_VERSION_1_2,
		},
#ifdef PLAGUE_DEBUG
		.enabledLayerCount = sizeof(layers) / sizeof(layers[0]),
		.ppEnabledLayerNames = layers,
		.pNext = &debug_info,
#endif // PLAGUE_DEBUG
	};

	VkInstance instance = NULL;
	VkResult result = vkCreateInstance(&create_info, NULL, &instance);
	if (result != VK_SUCCESS) {
		fprintf(stderr, "vkCreateInstance failed with return code %d", result);
		return NULL;
	}
	return instance;
}

int main(void) {
	if (!glfwInit()) {
		fprintf(stderr, "failed to initialize GLFW\n");
		return 1;
	}
	
	if (volkInitialize() != VK_SUCCESS) {
		fprintf(stderr, "failed to initialize volk\n");
		return 1;
	}

	// if this is not set then glfw will attempt to create an opengl context which, for whatever 
	// reason causes either vulkan device or surface creation (I don't remember which one) to fail.
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "plague", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "failed to create window\n");
		return 1;
	}

	// 1 megabyte memory arena used for temporary allocations
	size_t arena_size = MB(1);
	plg_memory_arena_t arena = {
		.data = malloc(arena_size),
		.size = arena_size,
	};

	VkInstance instance = create_instance(&arena);
	if (instance == NULL) {
		fprintf(stderr, "failed to create vulkan instance\n");
		return 1;
	}
	volkLoadInstance(instance);

	VkDebugUtilsMessengerEXT debug_messenger = create_messenger(instance);
	if (debug_messenger == NULL) {
		fprintf(stderr, "failed to create debug messenger\n");
		return 1;
	}

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	free(arena.data);
	vkDestroyDebugUtilsMessengerEXT(instance, debug_messenger, NULL);
	vkDestroyInstance(instance, NULL);
	glfwTerminate();
	return 0;
}
