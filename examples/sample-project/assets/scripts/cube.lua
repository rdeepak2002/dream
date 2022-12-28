function update(entity, dt)
	local xVel = 0.0
	local yVel = 0.0

	if (Input.getButtonDown(Key.w)) then
		yVel = yVel - 0.2
	end
	if (Input.getButtonDown(Key.a)) then
		xVel = xVel + 0.2
	end
	if (Input.getButtonDown(Key.s)) then
		yVel = yVel + 0.2
	end
	if (Input.getButtonDown(Key.d)) then
		xVel = xVel - 0.2
	end

    local transformComponent = entity:getTransform()
	transformComponent.translation.x = transformComponent.translation.x + xVel * dt
	transformComponent.translation.y = transformComponent.translation.y + yVel * dt
end






