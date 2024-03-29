#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "complex/Core/FilterHandle.hpp"
#include "complex/Filter/IFilter.hpp"

#include "complex/complex_export.hpp"

namespace complex
{
class AbstractPlugin;
class PluginLoader;

/**
 * @class FilterList
 * @brief The FilterList class serves as a lookup point for finding and
 * creating filters. The FilterList stores and loads plugins, adds
 * FilterHandles for each plugin's available filters, and handles the creation
 * of those filters at a later time.
 */
class COMPLEX_EXPORT FilterList
{
public:
  /**
   * Empty Constructor
   */
  FilterList();

  ~FilterList();

  /**
   * @brief Returns a set of FilterHandles for each of the filters.
   * @return std::unordered_set<FilterHandle>
   */
  const std::unordered_set<FilterHandle>& getFilterHandles() const;

  /**
   * @brief Searches the FilterHandles for the text in the filter and plugin
   * names. Returns a vector of the FilterHandles that contain the specified text.
   * @param text
   * @return std::vector<FilterHandle>
   */
  std::vector<FilterHandle> search(const std::string& text) const;

  /**
   * @brief Attempts to create an IFilter specified by the given
   * FilterHandle. Returns a pointer to the created filter if one was created.
   * Returns nullptr otherwise.
   * @param handle
   * @return IFilter*
   */
  IFilter::UniquePointer createFilter(const FilterHandle& handle) const;

  /**
   * @brief Returns the AbstractPlugin pointer corresponding to the specified
   * FilterHandle. Returns nullptr if the plugin could not be found.
   * @param handle
   * @return AbstractPlugin*
   */
  AbstractPlugin* getPlugin(const FilterHandle& handle) const;

  /**
   * @brief Attempts to add a plugin using the specified PluginLoader. Returns
   * true if the plugin was added. Returns false otherwise.
   * @param loader
   * @return bool
   */
  bool addPlugin(const std::shared_ptr<PluginLoader>& loader);

  /**
   * @brief Attempts to add the plugin at the specified filepath. Returns true
   * if the plugin was added. Returns false otherwise.
   * @param path
   * @return bool
   */
  bool addPlugin(const std::string& path);

  /**
   * @brief Returns a set of pointers to loaded plugins.
   * @return std::unordered_set<AbstractPlugin*>
   */
  std::unordered_set<AbstractPlugin*> getLoadedPlugins() const;

  /**
   * @brief Returns a pointer to the plugin with the specified ID. Returns
   * nullptr if no plugin with the given ID is found.
   * @param id
   * @return AbstractPlugin*
   */
  AbstractPlugin* getPluginById(const FilterHandle::PluginIdType& id) const;

private:
  /**
   * @brief Registers core filters with the FilterList.
   *
   * This method is defined within a CMake generated source file.
   */
  void registerCoreFilters();

  /**
   * @brief Registers a specific core filter with the FilterList.
   *
   * This method should only be called within registerCoreFilters().
   * @param func
   */
  void addCoreFilter(FilterCreationFunc func);

  /**
   * @brief Attempts to create and return a filter with the specified ID
   * from the list of core filters. Returns nullptr if no core filter with
   * the specified ID could be found.
   * @param filterId
   * @return IFilter*
   */
  IFilter::UniquePointer createCoreFilter(const FilterHandle::FilterIdType& filterId) const;

  ////////////
  // Variables
  std::unordered_set<FilterHandle> m_FilterHandles;
  std::unordered_map<FilterHandle::FilterIdType, FilterCreationFunc> m_CoreFiltersMap;
  std::unordered_map<FilterHandle::PluginIdType, std::shared_ptr<PluginLoader>> m_PluginMap;
};
} // namespace complex
