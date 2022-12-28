function update(entity, dt)
	local vel = 0
	local slash = 0
	if Input.getButtonDown(Key.w) then
		vel = 1
	end
	if Input.getButtonDown(Key.LeftMouse) then
		slash = 1
	end

	entity:getAnimator():setVariable("velocity", vel)
	entity:getAnimator():setVariable("slash", slash)
end




