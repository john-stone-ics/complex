#include "dlfcn.h"
#include <iostream>

#include "Redesign/Plugin/AbstractPlugin.h"
#include "src/Plugin/PluginLoader.h"

PluginLoader::PluginLoader(const std::string& path)
: m_Path(path)
, m_Plugin(nullptr)
{
  loadPlugin();
}
PluginLoader::~PluginLoader()
{
  unloadPlugin();
}

bool PluginLoader::isLoaded() const
{
  return getPlugin() != nullptr;
}

void PluginLoader::loadPlugin()
{
  m_Handle = dlopen(m_Path.c_str(), RTLD_NOW);
  if(!m_Handle)
  {
    printf("could not dlopen: %s\n", dlerror());
    m_Handle = nullptr;
    return;
  }

  typedef AbstractPlugin* (*initPluginFunc)();
  initPluginFunc func = (initPluginFunc)dlsym(m_Handle, "initPlugin");
  const char* err = dlerror();
  if(err)
  {
    printf("could not dlsym: %s\n", err);
    return;
  }
  auto plugin = func();
  m_Plugin = std::shared_ptr<AbstractPlugin>(plugin);
}

void PluginLoader::unloadPlugin()
{
  if(!m_Handle)
  {
    return;
  }

  dlclose(m_Handle);
  m_Handle = nullptr;
}

AbstractPlugin* PluginLoader::getPlugin() const
{
  return m_Plugin.get();
}