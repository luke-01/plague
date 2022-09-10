#include <stdio.h>

#define VOLK_IMPLEMENTATION
#include <volk.h>

#include <GLFW/glfw3.h>

enum {
	WINDOW_WIDTH = 1280,
	WINDOW_HEIGHT = 720,
};

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

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
