function update(entity, dt)
	local vel = 0.2
    local transformComponent = entity:getTransform()
	if (transformComponent.translation.x > 1) then
		transformComponent.translation.x = 1
	end
	if (transformComponent.translation.x < -1) then
		transformComponent.translation.x = -1
	end
	transformComponent.translation.x = transformComponent.translation.x + vel * dt
end

