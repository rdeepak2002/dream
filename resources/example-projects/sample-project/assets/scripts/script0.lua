function update(entity, dt)
	newLookAt = vec3:new(0, 0, 0)
	entity:getCamera().lookAt = newLookAt
end



