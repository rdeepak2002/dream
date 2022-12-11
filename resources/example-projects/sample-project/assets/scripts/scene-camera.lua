function update(entity, dt)
    local transformComponent = entity:getTransform()
	local mouseMovement = Input.getMouseMovement()
	local mouseWheel = Input.getMouseScroll()
	local mouseTranslatePadding = 0.02
	local mouseScrollPadding = 0.5

	-- move scene camera up left down right
	if (Input.getButtonDown(Key.LeftMouse)) then
		transformComponent.translation = transformComponent.translation + transformComponent.left * mouseMovement.x * mouseTranslatePadding
		transformComponent.translation = transformComponent.translation + transformComponent.up * mouseMovement.y * mouseTranslatePadding
	end

	-- move scene camera forward and backward
	transformComponent.translation = transformComponent.translation + transformComponent.front * mouseWheel.y * mouseScrollPadding

	-- TODO: rotate scene camera

end








