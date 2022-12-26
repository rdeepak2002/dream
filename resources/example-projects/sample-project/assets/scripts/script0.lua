function update(entity, dt)
	local vel = 0
	if Input.getButtonDown(Key.w) then
		Logger.debug("a")
		vel = 1
	end
	if Input.getButtonDown(Key.s) then
		Logger.debug("b")
		vel = 0
	end

	entity:getAnimator():setVariable("velocity", vel)
end



