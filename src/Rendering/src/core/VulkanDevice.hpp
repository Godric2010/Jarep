//
// Created by sebastian on 3/22/2025.
//

#pragma once

class VulkanDevice {
    public:
      VulkanDevice();
      ~VulkanDevice();

    private:
      void pickPhysicalDevice();
      void createLogicalDevice();
      void findQueueFamilies();



};



