// Labellum Engine 🌺 (Formerly Azalea Engine... Formerly formerly Abisko Engine❄️) - Vilhelm Hansson / Vespera Chromatic -- Started 18-11-2023

// cont https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/01_Presentation/00_Window_surface.html

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <string>
#include <chrono>
#include <map>
#include <optional>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// CONSTANT VARIABLES
const uint16_t LUM_WINDOW_WIDTH = 800;
const uint16_t LUM_WINDOW_HEIGHT = 600;
const int BUILD_NR = 200124;

//TERMINAL STUFF https://en.wikipedia.org/wiki/ANSI_escape_code#8-bit
const std::string TC_BELL = "\a🔔 ";
const std::string TC_RED = "\x1B[38;5;1m";
const std::string TC_GREEN = "\x1B[38;5;2m";
const std::string TC_YELLOW = "\x1B[38;5;3m";
const std::string TC_BLUE = "\x1B[38;5;4m";
const std::string TC_PURPLE = "\x1B[38;5;5m";
const std::string TC_CYAN = "\x1B[38;5;6m";
const std::string TC_LUM = "\x1B[38;5;210m";
const std::string TC_LUM2 = "\x1B[38;5;182m";
const std::string TC_WARN = "\x1B[38;5;15m\x1B[48;5;1m";
const std::string TC_INFO = "\x1B[38;5;15m\x1B[48;5;4m";
const std::string TC_RESET = "\x1B[0m";

// DEBUG STUFF ----------------------------------------------------------------
#ifdef NDEBUG // If Not Debug Mode
const std::string WINDOW_NAME = "Labellum Engine 🌺";
bool enableValidationLayers = false;
bool lumDebugMode = false;

#else
const std::string WINDOW_NAME = "Labellum Engine 🌺 - Debug Mode";
bool enableValidationLayers = true;
bool lumDebugMode = true;
#endif
// ----------------------------------------------------------------------------

VkResult CreateDebugUtilsMessenerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

std::string intToBool(int nr);

/// @brief Creates a triangle, lmao
class HelloTriangleApplication
{
public:
    void run()
    {
        initVulkan();
        checkExtensions();
        mainLoop();
        cleanup();
    }

    void initWindow()
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        //Create Main Window
        window = glfwCreateWindow(LUM_WINDOW_WIDTH, LUM_WINDOW_HEIGHT, WINDOW_NAME.c_str(), nullptr, nullptr);

        //Load window icon and set it
        windowIcon.pixels = stbi_load("LabellumIcon.png", &windowIcon.width, &windowIcon.height, 0, 4);
        glfwSetWindowIcon(window, 1, &windowIcon);
    }

    GLFWwindow *getWindow()
    {
        return this->window;
    }

private:
    GLFWwindow *window = nullptr;
    GLFWimage windowIcon;
    VkInstance instance = NULL;
    VkDebugUtilsMessengerEXT debugMessenger = nullptr;
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = nullptr;
    VkPhysicalDevice physicalDevice = nullptr;
    VkDevice device = nullptr;
    VkQueue graphicsQueue = nullptr;

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value();
        }
    };

    std::vector<const char *> getRequiredExtensions()
    {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        if (enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    bool checkValidationLayerSupport()
    {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char *layerName : validationLayers)
        {
            bool layerFound = false;

            for (const auto &layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }

    bool checkExtensions()
    {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        std::cout << "Available Extensions:\n";
        for (const auto &extension : extensions)
        {
            std::cout << "\t" << extension.extensionName << "\n";
        }

        return false;
    }

    void createInstance()
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.pEngineName = "Labellum Engine";
        appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0;
        auto extensions = getRequiredExtensions();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
        }
        else
        {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create instance.");
        }

        if (enableValidationLayers && !checkValidationLayerSupport())
        {
            throw std::runtime_error("Validation layers requested but none available.");
        }
    }

    void initVulkan()
    {
        createInstance();
        setupDebugMessenger();
        pickPhysicalDevice();
        createLogicalDevice();
    }

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    void setupDebugMessenger()
    {
        if (!enableValidationLayers)
            return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;

        if (CreateDebugUtilsMessenerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to set up debug messenger.");
        }
    }

    void pickPhysicalDevice()
    {
        // Listing available graphics cards
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        // If no graphics cards with vulkan support is found, throw error
        if (deviceCount == 0)
        {
            throw std::runtime_error("No GPU with Vulkan support was found.");
        }
        
        // Allocate an array that holds our VkPhysicalDevice(s)
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        // Check if the found devices meet the requirements for Labellum Engine
        for (const auto &device : devices)
        {
            if (isDeviceSuitable(device) != 0)
            {
                physicalDevice = device;
                break;
            }   
        }
        if (physicalDevice == nullptr)
        {
            throw std::runtime_error("The found GPU(s) are not suitable to run Labellum Engine.");
        }
        
    }

    int rateDeviceSuitability(VkPhysicalDevice device)
    {
        int score = 0;

        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            score += 1000;
        }
        score += deviceProperties.limits.maxImageDimension2D;
        if (!deviceFeatures.geometryShader)
        {
            return 0;
        }
        
        std::cout << "Graphics Card Name: " << deviceProperties.deviceName << std::endl;
        std::cout << "Max Image Dimension 2D: " << deviceProperties.limits.maxImageDimension2D << std::endl;
        std::cout << "Support for Tessellation Shader: " << intToBool(deviceFeatures.tessellationShader) << std::endl;

        return score;
    }

    bool isDeviceSuitable(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices = findQueueFamilies(device);

        return indices.isComplete();
    }

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;

        // Logic to find graphics queue family
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }
            if (indices.isComplete())
            {
                break;
            }
            i++;
        }
        return indices;
    }

    void createLogicalDevice()
    {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        float queuePriority = 1.0f;

        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pEnabledFeatures = &deviceFeatures;
        // Ignored by modern Vulkan applications. They are only set to ensure compatibillity with older vulkan --------------------------------------
        createInfo.enabledExtensionCount = 0;
        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }
        // ------------------------------------------------------------------------------------------------------------------------------------------
        
        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create logical device.");
        }
        vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);

    }

    void mainLoop()
    {
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
        }
    }

    void cleanup()
    {
        if (enableValidationLayers)
        {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }
        vkDestroyDevice(device, nullptr);
        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
        void *pUserData)
    {
        std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }
};

// FUNCTION PROTOTYPES
void checkArgs(char *argv[], int argNr);

// ############################################
// ############# START OF MAIN ################
// ############################################
int main(int argc, char *argv[])
{
    HelloTriangleApplication app;
    app.initWindow();

    // Check Command Line Arguments ------------------------------------
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            checkArgs(argv, i);
        }
        if (lumDebugMode)
        {
            glfwSetWindowTitle(app.getWindow(), "Labellum Engine 🌺 - Debug Mode");
        }
    }
    // -----------------------------------------------------------------

    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// FUNCTION DEFINITIONS
void checkArgs(char *argv[], int argNr)
{
    std::string argument = argv[argNr];

    //Help Arg
    if (argument == "--help" || argument == "-h" || argument == "help"  || argument == "-help")
    {
        std::cout << TC_BELL + TC_CYAN + "Startup Arguments: \n \t -debug / -d : Debug Mode \n" + TC_RESET << std::endl;
    }
    // Debug Mode Arg
    else if (argument == "-debug" || argument == "-d")
    {
        std::cout << TC_LUM + "Labellum Engine 🌺 " + TC_INFO + "Debug Mode Enabled\n" + TC_RESET + "\t Build: " + std::to_string(BUILD_NR) + "\n" << std::endl;
        lumDebugMode = true;
        enableValidationLayers = true;
    }
}

std::string intToBool(int nr)
{
    switch (nr)
    {
        case 0: return "False"; break;
        case 1: return "True"; break;
    }
}
