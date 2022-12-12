function update(entity, dt)
    local transformComponent = entity:getTransform()
	local mouseMovement = Input.getMouseMovement()
	local mouseWheel = Input.getMouseScroll()
	local translatePadding = 0.02
	local zoomPadding = 0.5
	local rotatePadding = 0.02

	-- move scene camera up left down right
	if (Input.getButtonDown(Key.LeftMouse)) then
		transformComponent.translation = transformComponent.translation - transformComponent.left * mouseMovement.x * translatePadding
		transformComponent.translation = transformComponent.translation - transformComponent.up * mouseMovement.y * translatePadding
	end

	-- move scene camera forward and backward
	transformComponent.translation = transformComponent.translation + transformComponent.front * mouseWheel.y * zoomPadding

	-- rotate scene camera
	if (Input.getButtonDown(Key.RightMouse)) then
		local eulerRotation = entity:getTransform():getEulerAngleRotation()
		eulerRotation.x = eulerRotation.x + mouseMovement.x * translatePadding
		eulerRotation.y = eulerRotation.y - mouseMovement.y * rotatePadding
		entity:getTransform():setRotation(eulerRotation)
	end
end







