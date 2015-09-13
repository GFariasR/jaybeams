#include <jb/opencl/device_selector.hpp>
#include <jb/opencl/config.hpp>

#include <iostream>

namespace {

void update_selection(
    cl::Device const& d, int device_type, cl::Device& dev, cl_uint& count) {
  if (d.getInfo<CL_DEVICE_TYPE>() == device_type
      and d.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() > count) {
    count = d.getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>();
    dev = d;
  }
}

} // anonymous namespace

cl::Device jb::opencl::device_selector(config const& cfg) {
  std::vector<cl::Platform> platforms;
  (void) cl::Platform::get(&platforms);
  if (platforms.empty()) {
    throw std::runtime_error("Cannot find any OpenCL platform");
  }

  for (auto const& p : platforms) {
    std::vector<cl::Device> devices;
    p.getDevices(CL_DEVICE_TYPE_ALL, &devices);

    for (auto const& d : devices) {
      if (d.getInfo<CL_DEVICE_NAME>() == cfg.device_name()) {
        return d;
      }
    }
  }

  // Not searching by name or name not found, find the GPU with the
  // largest number of compute units ...
  cl::Device best_gpu;
  cl_uint best_gpu_count = 0;
  cl::Device best_cpu;
  cl_uint best_cpu_count = 0;
  for (auto const& p : platforms) {
    std::vector<cl::Device> devices;
    p.getDevices(CL_DEVICE_TYPE_ALL, &devices);

    for (auto const& d : devices) {
      update_selection(d, CL_DEVICE_TYPE_GPU, best_gpu, best_gpu_count);
      update_selection(d, CL_DEVICE_TYPE_CPU, best_cpu, best_cpu_count);
    }
  }

  if (best_gpu_count != 0 and cfg.device_name() != "BESTCPU") {
    return best_gpu;
  }
    
  return best_cpu;
}

cl::Device jb::opencl::device_selector() {
  return jb::opencl::device_selector(jb::opencl::config());
}