function update(entity, dt)
	local vel = 0
	if Input.getButtonDown(Key.w) then
		vel = 1
	end
	if Input.getButtonDown(Key.s) then
		vel = 0
	end

	entity:getAnimator():setVariable("velocity", vel)
end



