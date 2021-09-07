# mainCraft

A C version of the 'Minecraft in 500 lines of python'.

## Current state

![plot](./assets/screenshots/maincraft-01-26-2021.png)

## Requirements

> * C11 compatible compiler([Clang](https://releases.llvm.org/download.html) / [GCC](https://repology.org/project/gcc/packages))
> * [FTGL](https://repology.org/project/ftgl/packages) version 2.1.3rc5-9
> * [glfw](https://repology.org/project/glfw/packages) version 3.1.2-1 or higher
> * OpenGL 2.0 or higher
> * Vulkan 1.0 or higher
> * [cglm](https://repology.org/project/cglm/packages)
> * [stb](https://repology.org/project/stb/packages)
> * [FastNoise](https://github.com/Auburn/FastNoise) version 1.0.2 (Already included)

## Intructions

### Build:
~~~~
make all
~~~~

### Run:
~~~~
make run
~~~~

### Clean:
~~~~
make clean
~~~~

### Docker

Check the [README.md](docker/README.md) at docker folder.

### Credits

The vulkan part was highly based on the following works

https://github.com/Overv/VulkanTutorial <br>
https://github.com/LunarG/VulkanSamples <br>
https://vulkan.lunarg.com/doc/view/latest/linux/tutorial/html/index.html <br>
https://software.intel.com/content/www/us/en/develop/articles/api-without-secrets-introduction-to-vulkan-preface.html
