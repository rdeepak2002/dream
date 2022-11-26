function update(entity, dt)
    local transformComponent = entity:getTransform()
    transformComponent.translation.x = transformComponent.translation.x + -0.1 * dt
end