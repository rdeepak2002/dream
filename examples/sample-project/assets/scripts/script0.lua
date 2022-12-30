function update(entity, dt)
	local animVel = 0
	local slash = 0
	local xzVel = vec2:new(0, 0)

	if Input.getButtonDown(Key.w) then
		xzVel.y = xzVel.y + 1
		animVel = 1
	end

	if Input.getButtonDown(Key.s) then
		xzVel.y = xzVel.y - 1
		animVel = -1
	end

	if Input.getButtonDown(Key.LeftMouse) then
		slash = 1
	end

	entity:getRigidBody():setLinearVelocity(vec3:new(xzVel.x, 0, xzVel.y))
	entity:getAnimator():setVariable("velocity", animVel)
	entity:getAnimator():setVariable("slash", slash)
end










