#include "AbstractGeometry3D.hpp"

#include "complex/DataStructure/DataStructure.hpp"

using namespace complex;

AbstractGeometry3D::AbstractGeometry3D(DataStructure* ds, const std::string& name)
: AbstractGeometry(ds, name)
{
}

AbstractGeometry3D::AbstractGeometry3D(const AbstractGeometry3D& other)
: AbstractGeometry(other)
, m_VertexListId(other.m_VertexListId)
, m_EdgeListId(other.m_EdgeListId)
, m_UnsharedEdgeListId(other.m_UnsharedEdgeListId)
, m_FaceListId(other.m_FaceListId)
, m_UnsharedFaceListId(other.m_UnsharedFaceListId)
{
}

AbstractGeometry3D::AbstractGeometry3D(AbstractGeometry3D&& other) noexcept
: AbstractGeometry(std::move(other))
, m_VertexListId(std::move(other.m_VertexListId))
, m_EdgeListId(std::move(other.m_EdgeListId))
, m_UnsharedEdgeListId(std::move(other.m_UnsharedEdgeListId))
, m_FaceListId(std::move(other.m_FaceListId))
, m_UnsharedFaceListId(std::move(other.m_UnsharedFaceListId))
{
}

AbstractGeometry3D::~AbstractGeometry3D() = default;

AbstractGeometry3D::SharedQuadList* AbstractGeometry3D::createSharedQuadList(size_t numQuads)
{
  auto dataStore = new DataStore<MeshIndexType>(4, numQuads);
  SharedQuadList* quads = getDataStructure()->createDataArray<MeshIndexType>("Shared Quad List", dataStore, getId());
  dataStore->fill(0);
  return quads;
}

AbstractGeometry3D::SharedTriList* AbstractGeometry3D::createSharedTriList(size_t numTris)
{
  auto dataStore = new DataStore<MeshIndexType>(3, numTris);
  SharedTriList* triangles = getDataStructure()->createDataArray<MeshIndexType>("Shared Tri List", dataStore, getId());
  triangles->getDataStore()->fill(0);
  return triangles;
}

void AbstractGeometry3D::resizeVertexList(size_t numVertices)
{
  auto vertices = dynamic_cast<SharedVertexList*>(getDataStructure()->getData(m_VertexListId));
  if(!vertices)
  {
    return;
  }
  vertices->getDataStore()->resizeTuples(numVertices);
}

void AbstractGeometry3D::setVertices(const SharedVertexList* vertices)
{
  if(vertices == nullptr)
  {
    m_VertexListId.reset();
    return;
  }

  m_VertexListId = vertices->getId();
}

AbstractGeometry::SharedVertexList* AbstractGeometry3D::getVertices()
{
  return dynamic_cast<SharedVertexList*>(getDataStructure()->getData(m_VertexListId));
}

const AbstractGeometry::SharedVertexList* AbstractGeometry3D::getVertices() const
{
  return dynamic_cast<const SharedVertexList*>(getDataStructure()->getData(m_VertexListId));
}

void AbstractGeometry3D::setCoords(size_t vertId, const complex::Point3D<float>& coords)
{
  auto vertices = dynamic_cast<FloatArray*>(getDataStructure()->getData(m_VertexListId));
  if(!vertices)
  {
    return;
  }

  size_t index = vertId * 3;
  for(size_t i = 0; i < 3; i++)
  {
    (*vertices)[index + i] = coords[i];
  }
}

complex::Point3D<float> AbstractGeometry3D::getCoords(size_t vertId) const
{
  auto vertices = dynamic_cast<const FloatArray*>(getDataStructure()->getData(m_VertexListId));
  if(!vertices)
  {
    return Point3D<float>();
  }

  size_t index = vertId * 3;
  auto x = (*vertices)[index];
  auto y = (*vertices)[index + 1];
  auto z = (*vertices)[index + 2];
  return Point3D<float>(x, y, z);
}

size_t AbstractGeometry3D::getNumberOfVertices() const
{
  auto vertices = dynamic_cast<const FloatArray*>(getDataStructure()->getData(m_VertexListId));
  if(!vertices)
  {
    return 0;
  }
  return vertices->getTupleCount();
}

void AbstractGeometry3D::resizeEdgeList(size_t numEdges)
{
  auto edges = getEdges();
  if(!edges)
  {
    return;
  }
  edges->getDataStore()->resizeTuples(numEdges);
}

AbstractGeometry::SharedEdgeList* AbstractGeometry3D::getEdges()
{
  return dynamic_cast<SharedEdgeList*>(getDataStructure()->getData(m_EdgeListId));
}

const AbstractGeometry::SharedEdgeList* AbstractGeometry3D::getEdges() const
{
  return dynamic_cast<const SharedEdgeList*>(getDataStructure()->getData(m_EdgeListId));
}

void AbstractGeometry3D::setVertsAtEdge(size_t edgeId, const size_t verts[2])
{
  auto edges = dynamic_cast<SharedEdgeList*>(getDataStructure()->getData(m_EdgeListId));
  if(!edges)
  {
    return;
  }
  size_t index = edgeId * 2;
  (*edges)[index] = verts[0];
  (*edges)[index + 1] = verts[1];
}

void AbstractGeometry3D::getVertsAtEdge(size_t edgeId, size_t verts[2]) const
{
  auto edges = getEdges();
  if(!edges)
  {
    return;
  }
  size_t index = edgeId * 2;
  verts[0] = (*edges)[index];
  verts[1] = (*edges)[index + 1];
}

void AbstractGeometry3D::getVertCoordsAtEdge(size_t edgeId, complex::Point3D<float>& vert1, complex::Point3D<float>& vert2) const
{
  auto edges = getEdges();
  if(!edges)
  {
    return;
  }

  auto vertices = getVertices();
  if(!vertices)
  {
    return;
  }

  size_t verts[2];
  getVertsAtEdge(edgeId, verts);

  for(size_t i = 0; i < 3; i++)
  {
    vert1[i] = (*vertices)[verts[0] * 3 + i];
    vert2[i] = (*vertices)[verts[1] * 3 + i];
  }
}

size_t AbstractGeometry3D::getNumberOfEdges() const
{
  auto edges = getEdges();
  if(!edges)
  {
    return 0;
  }

  return edges->getTupleCount();
}

void AbstractGeometry3D::deleteEdges()
{
  getDataStructure()->removeData(m_EdgeListId);
  m_EdgeListId.reset();
}

void AbstractGeometry3D::deleteFaces()
{
  getDataStructure()->removeData(m_FaceListId);
  m_FaceListId.reset();
}

const AbstractGeometry::SharedEdgeList* AbstractGeometry3D::getUnsharedEdges() const
{
  return dynamic_cast<const SharedEdgeList*>(getDataStructure()->getData(m_UnsharedEdgeListId));
}

void AbstractGeometry3D::deleteUnsharedEdges()
{
  getDataStructure()->removeData(m_UnsharedEdgeListId);
}

const AbstractGeometry::SharedEdgeList* AbstractGeometry3D::getUnsharedFaces() const
{
  return dynamic_cast<const SharedEdgeList*>(getDataStructure()->getData(m_UnsharedFaceListId));
}

void AbstractGeometry3D::deleteUnsharedFaces()
{
  getDataStructure()->removeData(m_UnsharedFaceListId);
  m_UnsharedFaceListId.reset();
}

size_t AbstractGeometry3D::getNumberOfFaces() const
{
  auto faces = getFaces();
  if(!faces)
  {
    return 0;
  }
  return faces->getTupleCount();
}

AbstractGeometry3D::SharedFaceList* AbstractGeometry3D::getFaces()
{
  return dynamic_cast<SharedFaceList*>(getDataStructure()->getData(m_FaceListId));
}

const AbstractGeometry3D::SharedFaceList* AbstractGeometry3D::getFaces() const
{
  return dynamic_cast<const SharedFaceList*>(getDataStructure()->getData(m_FaceListId));
}

void AbstractGeometry3D::setEdges(const SharedEdgeList* edges)
{
  if(!edges)
  {
    m_EdgeListId.reset();
    return;
  }
  m_EdgeListId = edges->getId();
}

void AbstractGeometry3D::setFaces(const SharedFaceList* faces)
{
  if(!faces)
  {
    m_FaceListId.reset();
    return;
  }
  m_FaceListId = faces->getId();
}

void AbstractGeometry3D::setUnsharedEdges(const SharedEdgeList* bEdgeList)
{
  if(!bEdgeList)
  {
    m_UnsharedEdgeListId.reset();
    return;
  }
  m_UnsharedEdgeListId = bEdgeList->getId();
}

void AbstractGeometry3D::setUnsharedFaces(const SharedFaceList* bFaceList)
{
  if(!bFaceList)
  {
    m_UnsharedFaceListId.reset();
    return;
  }
  m_UnsharedFaceListId = bFaceList->getId();
}
