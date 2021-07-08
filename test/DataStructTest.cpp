#include <memory>
#include <vector>

#include <catch2/catch.hpp>

#include "DataStructObserver.hpp"

#include "complex/DataStructure/DataGroup.hpp"
#include "complex/DataStructure/DataStructure.hpp"
#include "complex/DataStructure/DataStore.hpp"

/**
 * @brief Test creation and removal of items in a tree-style structure. No node has more than one parent.
 */
TEST_CASE("DataGroupTree")
{
  // Create structuure
  DataStructure dataStr;
  auto group = dataStr.createGroup("Foo");
  auto child = dataStr.createGroup("bar", group->getId());
  auto grandchild = dataStr.createGroup("bazz", child->getId());

  auto groupId = group->getId();
  auto childId = child->getId();
  auto grandchildId = grandchild->getId();

  SECTION("find data")
  {
    // Can all data be found by ID?
    REQUIRE(nullptr != dataStr.getData(groupId));
    REQUIRE(nullptr != dataStr.getData(childId));
    REQUIRE(nullptr != dataStr.getData(grandchildId));
  }
  SECTION("remove mid-level group")
  {
    group->remove(child->getName());
    REQUIRE(nullptr == dataStr.getData(childId));
    REQUIRE(nullptr == dataStr.getData(grandchildId));
  }
  SECTION("remove top-level group")
  {
    dataStr.removeData(group->getId());
    REQUIRE(nullptr == dataStr.getData(groupId));
  }
}

/**
 * @brief Test creation and removal of DataObjects in a graph-style DataStructure where multiple parents are allowed.
 */
TEST_CASE("DataGroupGraph")
{
  // Create DataStructure
  DataStructure dataStr;
  auto group = dataStr.createGroup("Foo");
  auto child1 = dataStr.createGroup("Bar1", group->getId());
  auto child2 = dataStr.createGroup("Bar2", group->getId());
  auto grandchild = dataStr.createGroup("Bazz", child1->getId());

  // Get IDs
  auto groupId = group->getId();
  auto child1Id = child1->getId();
  auto child2Id = child2->getId();
  auto grandchildId = grandchild->getId();

  // Multi-parent connections
  dataStr.setAdditionalParent(grandchildId, child2Id);

  SECTION("find data")
  {
    REQUIRE(nullptr != dataStr.getData(groupId));
    REQUIRE(nullptr != dataStr.getData(child1Id));
    REQUIRE(nullptr != dataStr.getData(child2Id));
    REQUIRE(nullptr != dataStr.getData(grandchildId));
  }
  SECTION("remove mid-level group")
  {
    group->remove(child1->getName());
    REQUIRE(nullptr == dataStr.getData(child1Id));
    REQUIRE(nullptr != dataStr.getData(grandchildId));
  }
  SECTION("remove top-level group")
  {
    dataStr.removeData(group->getId());
    REQUIRE(nullptr == dataStr.getData(groupId));
    REQUIRE(nullptr == dataStr.getData(grandchildId));
  }
}

/**
 * @brief Test DataPath usage
 */
TEST_CASE("DataPathTest")
{
  // Create DataStructure
  DataStructure dataStr;
  auto group = dataStr.createGroup("Foo");
  auto child1 = dataStr.createGroup("Bar1", group->getId());
  auto child2 = dataStr.createGroup("Bar2", group->getId());
  auto grandchild = dataStr.createGroup("Bazz", child1->getId());

  auto child2Id = child2->getId();
  auto grandchildId = grandchild->getId();
  SECTION("add additional parent")
  {
    REQUIRE(dataStr.setAdditionalParent(grandchildId, child2Id));
  }

  const DataPath gcPath1({"Foo", "Bar1", "Bazz"});
  const DataPath gcPath2({"Foo", "Bar2", "Bazz"});

  REQUIRE(nullptr != dataStr.getData(gcPath1));
  REQUIRE(nullptr != dataStr.getData(gcPath2));

  const DataPath c1Path({"Foo", "Bar1"});
  REQUIRE(dataStr.removeData(c1Path));
  REQUIRE(nullptr == dataStr.getData(gcPath1));
  REQUIRE(nullptr != dataStr.getData(DataPath({"Foo", "Bar2"})));
  auto gc2 = dataStr.getData(gcPath2);
  REQUIRE(nullptr != dataStr.getData(gcPath2));

  dataStr.removeData(child2Id);
  REQUIRE(nullptr == dataStr.getData(gcPath2));
}

TEST_CASE("LinkedPathTest")
{
  DataStructure dataStr;
  auto group = dataStr.createGroup("Foo");
  auto child1 = dataStr.createGroup("Bar1", group->getId());
  auto child2 = dataStr.createGroup("Bar2", group->getId());
  auto grandchild = dataStr.createGroup("Bazz", child1->getId());

  auto child2Id = child2->getId();
  auto grandchildId = grandchild->getId();

  REQUIRE(dataStr.setAdditionalParent(grandchildId, child2Id));

  // DataPath
  const DataPath grandPath({"Foo", "Bar1", "Bazz"});
  const LinkedPath linkedPath = dataStr.getLinkedPath(grandPath);

  REQUIRE(linkedPath.isValid());
  REQUIRE(grandchild == linkedPath.getData());

  REQUIRE(child1->rename("Bar1.3"));
  REQUIRE(nullptr == dataStr.getData(grandPath));
  REQUIRE(nullptr != linkedPath.getData());
  REQUIRE(linkedPath.isValid());

  REQUIRE(dataStr.removeData(linkedPath.getIdAt(1)));
  REQUIRE(!linkedPath.isValid());
}

/**
 * @brief Tests IDataStructureListener usage
 */
TEST_CASE("DataStructureListenerTest")
{
  DataStructure dataStr;
  DataStructObserver dsListener(dataStr);

  // Adds items to the DataStructure. Each addition should trigger dsListener.
  auto group = dataStr.createGroup("Foo");
  auto child1 = dataStr.createGroup("Bar1", group->getId());
  auto child2 = dataStr.createGroup("Bar2", group->getId());
  auto grandchild = dataStr.createGroup("Bazz", child1->getId());

  auto groupId = group->getId();
  auto child2Id = child2->getId();
  auto grandchildId = grandchild->getId();

  REQUIRE(dsListener.getDataAddedCount() == 4);

  REQUIRE(dataStr.setAdditionalParent(grandchildId, child2Id));
  REQUIRE(dsListener.getDataReparentedCount() == 1);

  dataStr.removeData(child2Id);
  REQUIRE(dsListener.getDataRemovedCount() == 1);
  dataStr.removeData(groupId);
  REQUIRE(dsListener.getDataRemovedCount() == 4);
}

TEST_CASE("DataStructureCopyTest")
{
  DataStructure dataStr;
  auto group = dataStr.createGroup("Foo");
  auto child1 = dataStr.createGroup("Bar1", group->getId());
  auto child2 = dataStr.createGroup("Bar2", group->getId());
  auto grandchild = dataStr.createGroup("Bazz", child1->getId());

  auto groupId = group->getId();
  auto child1Id = child1->getId();
  auto child2Id = child2->getId();
  auto grandchildId = grandchild->getId();

  REQUIRE(dataStr.setAdditionalParent(grandchildId, child2Id));

  // Copy DataStructure
  DataStructure dataStrCopy(dataStr);

  REQUIRE(dataStrCopy.getData(groupId));
  REQUIRE(dataStrCopy.getData(child1Id));
  REQUIRE(dataStrCopy.getData(child2Id));
  REQUIRE(dataStrCopy.getData(grandchildId));

  DataObject* data = dataStr.getData(groupId);
  DataObject* dataCopy = dataStrCopy.getData(groupId);
  REQUIRE(dataStrCopy.getData(groupId) != dataStr.getData(groupId));
  REQUIRE(dataStrCopy.getData(child1Id) != dataStr.getData(child1Id));
  REQUIRE(dataStrCopy.getData(child2Id) != dataStr.getData(child2Id));
  REQUIRE(dataStrCopy.getData(grandchildId) != dataStr.getData(grandchildId));

  // Create new group
  auto newGroup = dataStr.createGroup("New Group", child2Id);
  auto newId = newGroup->getId();
  REQUIRE(dataStr.getData(newId));
  REQUIRE(!dataStrCopy.getData(newId));

  auto newGroup2 = dataStrCopy.createGroup("New Group (2)", child2Id);
  auto newId2 = newGroup2->getId();
  REQUIRE(!dataStr.getData(newId2));
  REQUIRE(dataStrCopy.getData(newId2));
}

TEST_CASE("DataStoreTest")
{
  const size_t tupleSize = 3;
  const size_t tupleCount = 10;
  DataStore<int32_t> store(tupleSize, tupleCount);

  REQUIRE(store.getTupleCount() == tupleCount);
  REQUIRE(store.getTupleSize() == tupleSize);
  REQUIRE(store.getSize() == (tupleSize * tupleCount));

  // subscript operator
  {
    for(size_t i = 0; i < store.getSize(); i++)
    {
      store[i] = i + 1;
    }
    int32_t x = 1;
    for(size_t i = 0; i < store.getSize(); i++)
    {
      REQUIRE(store[i] == x++);
    }
  }
  // get / set values
  {
    const size_t index = 5;
    const size_t value = 25;
    store.setValue(index, value);
    REQUIRE(store.getValue(index) == value);
  }
  // iterators
  {
    const int32_t initVal = -30;
    int32_t x = initVal;
    for(auto& value : store)
    {
      value = x++;
    }
    x = initVal;
    for(const auto& value : store)
    {
      REQUIRE(value == x++);
    }
  }
}

TEST_CASE("DataArrayTest")
{
  DataStructure dataStr;

  auto store = new DataStore<int32_t>(2, 2);
  auto dataArr = dataStr.createDataArray<int32_t>("array", store);

  SECTION("test size")
  {
    REQUIRE(dataArr->getSize() == store->getSize());
    REQUIRE(dataArr->getTupleCount() == store->getTupleCount());
    REQUIRE(dataArr->getTupleSize() == store->getTupleSize());
  }

  SECTION("test reading / writing to memory")
  {
    SECTION("test subscript operators")
    {
      for(size_t i = 0; i < dataArr->getSize(); i++)
      {
        (*dataArr)[i] = i + 1;
      }
      int32_t x = 1;
      for(size_t i = 0; i < dataArr->getSize(); i++)
      {
        REQUIRE((*dataArr)[i] == x++);
      }
    }
    SECTION("test iterators")
    {
      const int32_t initVal = -30;
      int32_t x = initVal;
      for(auto& value : *dataArr)
      {
        value = x++;
      }
      x = initVal;
      for(const auto& value : *dataArr)
      {
        REQUIRE(value == x++);
      }
    }
  }
}

TEST_CASE("ScalarDataTest")
{
  DataStructure dataStr;
  const int32_t value = 6;
  auto scalar = dataStr.createScalar<int32_t>("scalar", value);

  // get value
  REQUIRE(value == scalar->getValue());
  REQUIRE((*scalar) == value);
  REQUIRE((*scalar) != (value + 1));

  // edit values
  const int32_t newValue = 11;
  scalar->setValue(newValue);
  REQUIRE(newValue == scalar->getValue());

  const int32_t newValue2 = 14;
  (*scalar) = newValue2;
  REQUIRE(scalar->getValue() == newValue2);
}