//
// Created by Deepak Ramalingam on 11/13/22.
//

#include <gtest/gtest.h>
#include "dream/scene/Entity.h"
#include "dream/scene/Scene.h"
#include "dream/scene/component/Component.h"
#include "dream/project/Project.h"

/**
 * Test HierarchyComponent addChild()
 */
TEST(HierarchyComponentTest, AddChild) {
    auto entityA = Dream::Project::getInstance().getScene().createEntity("A");
    auto entityB = Dream::Project::getInstance().getScene().createEntity("B");
    auto entityC = Dream::Project::getInstance().getScene().createEntity("C");
    // check root entity
    Dream::Entity rootEntity = entityA.scene->getRootEntity();
    EXPECT_EQ(rootEntity.numChildren(), 3);
    EXPECT_EQ(rootEntity.getComponent<Dream::Component::HierarchyComponent>().first, entityC);
    EXPECT_EQ(rootEntity.getComponent<Dream::Component::HierarchyComponent>().first.getComponent<Dream::Component::HierarchyComponent>().next, entityB);
    EXPECT_EQ(rootEntity.getComponent<Dream::Component::HierarchyComponent>().first.getComponent<Dream::Component::HierarchyComponent>().next.getComponent<Dream::Component::HierarchyComponent>().next, entityA);
    // check entity A
    EXPECT_EQ(entityA.numChildren(), 0);
    EXPECT_EQ(rootEntity, entityA.getComponent<Dream::Component::HierarchyComponent>().parent);
    EXPECT_TRUE(!entityA.getComponent<Dream::Component::HierarchyComponent>().first);
    EXPECT_TRUE(!entityA.getComponent<Dream::Component::HierarchyComponent>().next);
    EXPECT_EQ(entityA.getComponent<Dream::Component::HierarchyComponent>().prev, entityB);
    // check entity B
    EXPECT_TRUE(entityB.numChildren() == 0);
    EXPECT_TRUE(rootEntity == entityB.getComponent<Dream::Component::HierarchyComponent>().parent);
    EXPECT_TRUE(!entityB.getComponent<Dream::Component::HierarchyComponent>().first);
    EXPECT_EQ(entityB.getComponent<Dream::Component::HierarchyComponent>().next, entityA);
    EXPECT_EQ(entityB.getComponent<Dream::Component::HierarchyComponent>().prev, entityC);
    // check entity C
    EXPECT_TRUE(entityC.numChildren() == 0);
    EXPECT_TRUE(rootEntity == entityC.getComponent<Dream::Component::HierarchyComponent>().parent);
    EXPECT_TRUE(!entityC.getComponent<Dream::Component::HierarchyComponent>().first);
    EXPECT_TRUE(entityC.getComponent<Dream::Component::HierarchyComponent>().next == entityB);
    EXPECT_TRUE(!entityC.getComponent<Dream::Component::HierarchyComponent>().prev);
    // make entity C child of entity A
    entityA.addChild(entityC);
    // check sizes
    EXPECT_EQ(rootEntity.numChildren(), 2);
    EXPECT_EQ(entityA.numChildren(), 1);
    EXPECT_EQ(entityB.numChildren(), 0);
    // check entity C is only a child of entity A
    EXPECT_EQ(entityA.getComponent<Dream::Component::HierarchyComponent>().first, entityC);
    // make entity C child of entity B
    entityB.addChild(entityC);
    // check sizes
    EXPECT_EQ(rootEntity.numChildren(), 2);
    EXPECT_EQ(entityB.numChildren(), 1);
    EXPECT_EQ(entityA.numChildren(), 0);
    // check entity C is only a child of entity B
    EXPECT_EQ(entityB.getComponent<Dream::Component::HierarchyComponent>().first, entityC);
    // make B a child of A
    entityA.addChild(entityB);
    EXPECT_EQ(rootEntity.numChildren(), 1);
    EXPECT_EQ(entityA.numChildren(), 1);
    EXPECT_EQ(entityB.numChildren(), 1);
    EXPECT_EQ(entityC.numChildren(), 0);
}

// TODO: test HierarchyComponent removeChild()

// TODO: test Scene removeEntity()