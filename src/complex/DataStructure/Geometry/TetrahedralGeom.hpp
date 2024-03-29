#pragma once

#include "complex/DataStructure/Geometry/AbstractGeometry3D.hpp"

#include "complex/complex_export.hpp"

namespace complex
{
/**
 * @class TetrahedralGeom
 * @brief
 */
class COMPLEX_EXPORT TetrahedralGeom : public AbstractGeometry3D
{
public:
  friend class DataStructure;

  /**
   * @brief
   * @param other
   */
  TetrahedralGeom(const TetrahedralGeom& other);

  /**
   * @brief
   * @param other
   */
  TetrahedralGeom(TetrahedralGeom&& other) noexcept;

  virtual ~TetrahedralGeom();

  /**
   * @brief
   * @return DataObject*
   */
  DataObject* shallowCopy() override;

  /**
   * @brief
   * @return DataObject*
   */
  DataObject* deepCopy() override;

  /**
   * @brief
   * @return std::string
   */
  std::string getGeometryTypeAsString() const override;

  /**
   * @brief
   * @param numTris
   */
  void resizeTriList(size_t numTris);

  /**
   * @brief
   * @param triangles
   */
  void setTriangles(const SharedTriList* triangles);

  /**
   * @brief
   * @return SharedTriList*
   */
  SharedTriList* getTriangles();

  /**
   * @brief
   * @return const SharedTriList*
   */
  const SharedTriList* getTriangles() const;

  /**
   * @brief
   * @param triId
   * @param verts
   */
  void setVertsAtTri(size_t triId, size_t verts[3]);

  /**
   * @brief
   * @param triId
   * @param verts
   */
  void getVertsAtTri(size_t triId, size_t verts[3]) const;

  /**
   * @brief
   * @return size_t
   */
  size_t getNumberOfTris() const;

  /**
   * @brief
   * @param numTets
   */
  void resizeTetList(size_t numTets);

  /**
   * @brief
   * @param tets
   */
  void setTetrahedra(const SharedTetList* tets);

  /**
   * @brief
   * @return SharedTetList*
   */
  SharedTetList* getTetrahedra();

  /**
   * @brief
   * @return const SharedTetList*
   */
  const SharedTetList* getTetrahedra() const;

  /**
   * @brief
   * @param tetId
   * @param verts
   */
  void setVertsAtTet(size_t tetId, size_t verts[4]);

  /**
   * @brief
   * @param tetId
   * @param verts
   */
  void getVertsAtTet(size_t tetId, size_t verts[4]) const;

  /**
   * @brief
   * @param tetId
   * @param vert1
   * @param vert2
   * @param vert3
   * @param vert4
   */
  void getVertCoordsAtTet(size_t tetId, complex::Point3D<float>& vert1, complex::Point3D<float>& vert2, complex::Point3D<float>& vert3, complex::Point3D<float>& vert4) const;

  /**
   * @brief
   * @return size_t
   */
  size_t getNumberOfTets() const;

  /**
   * @brief
   */
  void initializeWithZeros() override;

  /**
   * @brief
   * @return size_t
   */
  size_t getNumberOfElements() const override;

  /**
   * @brief
   * @return StatusCode
   */
  StatusCode findElementSizes() override;

  /**
   * @brief
   * @return const FloatArray*
   */
  const FloatArray* getElementSizes() const override;

  /**
   * @brief
   */
  void deleteElementSizes() override;

  /**
   * @brief
   * @return StatusCode
   */
  StatusCode findElementsContainingVert() override;

  /**
   * @brief
   * @return const ElementDynamicList*
   */
  const ElementDynamicList* getElementsContainingVert() const override;

  /**
   * @brief
   */
  void deleteElementsContainingVert() override;

  /**
   * @brief
   * @return StatusCode
   */
  StatusCode findElementNeighbors() override;

  /**
   * @brief
   * @return const ElementDynamicList*
   */
  const ElementDynamicList* getElementNeighbors() const override;

  /**
   * @brief
   */
  void deleteElementNeighbors() override;

  /**
   * @brief
   * @return StatusCode
   */
  StatusCode findElementCentroids() override;

  /**
   * @brief
   * @return const FloatArray*
   */
  const FloatArray* getElementCentroids() const override;

  /**
   * @brief
   */
  void deleteElementCentroids() override;

  /**
   * @brief
   * @param pCoords
   */
  complex::Point3D<double> getParametricCenter() const override;

  /**
   * @brief
   * @param pCoords
   * @param shape
   */
  void getShapeFunctions(const complex::Point3D<double>& pCoords, double* shape) const override;

  /**
   * @brief
   * @param field
   * @param derivatives
   * @param observable
   */
  void findDerivatives(DoubleArray* field, DoubleArray* derivatives, Observable* observable) const override;

  /**
   * @brief
   * @return complex::TooltipGenerator
   */
  complex::TooltipGenerator getTooltipGenerator() const override;

  /**
   * @brief
   * @return StatusCode
   */
  StatusCode findEdges() override;

  /**
   * @brief
   * @return StatusCode
   */
  StatusCode findFaces() override;

  /**
   * @brief
   * @return StatusCode
   */
  StatusCode findUnsharedEdges() override;

  /**
   * @brief
   * @return StatusCode
   */
  StatusCode findUnsharedFaces() override;

  /**
   * @brief
   * @param elementSizes
   */
  void setElementSizes(const FloatArray* elementSizes) override;

  /**
   * @brief
   * @return uint32_t
   */
  uint32_t getXdmfGridType() const override;

protected:
  /**
   * @brief
   * @param ds
   * @param name
   */
  TetrahedralGeom(DataStructure* ds, const std::string& name);

  /**
   * @brief
   * @param ds
   * @param name
   * @param numTets
   * @param vertices
   * @param allocate = true
   */
  TetrahedralGeom(DataStructure* ds, const std::string& name, size_t numTets, const std::shared_ptr<SharedVertexList>& vertices, bool allocate = true);

  /**
   * @brief
   * @param ds
   * @param name
   * @param tets
   * @param vertices
   */
  TetrahedralGeom(DataStructure* ds, const std::string& name, const std::shared_ptr<SharedTetList>& tets, const std::shared_ptr<SharedVertexList>& vertices);

  /**
   * @brief
   * @param elementsContainingVert
   */
  void setElementsContainingVert(const ElementDynamicList* elementsContainingVert) override;

  /**
   * @brief
   * @param elementNeighbors
   */
  void setElementNeighbors(const ElementDynamicList* elementNeighbors) override;

  /**
   * @brief
   * @param elementCentroids
   */
  void setElementCentroids(const FloatArray* elementCentroids) override;

private:
  std::optional<DataObject::IdType> m_TriListId;
  std::optional<DataObject::IdType> m_UnsharedTriListId;
  std::optional<DataObject::IdType> m_TetListId;
  std::optional<DataObject::IdType> m_TetsContainingVertId;
  std::optional<DataObject::IdType> m_TetNeighborsId;
  std::optional<DataObject::IdType> m_TetCentroidsId;
  std::optional<DataObject::IdType> m_TetSizesId;
};
} // namespace complex
