function update(entity, dt)
	-- get main camera
	local cameraEntity = Scene.getEntityByTag("camera")
	if not cameraEntity:isValid() then
		Logger.error("Knight cannot find camera entity")
		return
	end
	
	-- check if user is close to either door
	local entityTranslation = entity:getTransform().translation
	local door1Entity = Scene.getEntityByTag("Door 1")
	if not door1Entity:isValid() then
		Logger.error("Door1 entity cannot be found by character controller")
		return
	end
	local door1Translation = door1Entity:getTransform().translation

	if MathUtils.distance(entityTranslation, door1Translation) < 1 then
		entity:getRigidBody():setTranslation(vec3:new(0.3, 0.03, -5.59))
	end

	local door2Entity = Scene.getEntityByTag("Door 2")
	if not door2Entity:isValid() then
		Logger.error("Door2 entity cannot be found by character controller")
		return
	end
	local door2Translation = door2Entity:getTransform().translation

	if MathUtils.distance(entityTranslation, door2Translation) < 1 then
		entity:getRigidBody():setTranslation(vec3:new(0.3, 0.03, 5.6))
	end

	-- get forward vector of camera and project onto xz plane
	local cameraTranslation = cameraEntity:getTransform().translation
	local knightTranslation = entity:getTransform().translation
	local cameraForwardVector = knightTranslation - cameraTranslation
	cameraForwardVector.y = 0
	cameraForwardVector = MathUtils.normalizeVec3(cameraForwardVector)

	-- get right vector of camera and project onto xz plane
	local cameraRightVector = MathUtils.crossProductVec3(vec3:new(0, 1, 0), cameraForwardVector)
	cameraRightVector.y = 0
	cameraRightVector = MathUtils.normalizeVec3(cameraRightVector)

	-- get direction to move player w.r.t. xz plane based off joystick input 
	local newMovementDirection = vec2:new(0, 0)
	if Input.getButtonDown(Key.w) then
		newMovementDirection = newMovementDirection + vec2:new(cameraForwardVector.x, cameraForwardVector.z)
	end
	if Input.getButtonDown(Key.s) then
		newMovementDirection = newMovementDirection - vec2:new(cameraForwardVector.x, cameraForwardVector.z)
	end
	if Input.getButtonDown(Key.a) then
		newMovementDirection = newMovementDirection + vec2:new(cameraRightVector.x, cameraRightVector.z)
	end
	if Input.getButtonDown(Key.d) then
		newMovementDirection = newMovementDirection - vec2:new(cameraRightVector.x, cameraRightVector.z)
	end
	newMovementDirection = MathUtils.normalizeVec2(newMovementDirection)

	-- prevent movement when slashing
	local slash1Anim = entity:getAnimator():getCurrentStateName() == "Idle Slash 1"
	local slash2Anim = entity:getAnimator():getCurrentStateName() == "Idle Slash 2"
	local isSlashing = slash1Anim or slash2Anim
	if isSlashing then
		newMovementDirection = vec2:new(0, 0)
	end

	if MathUtils.magnitudeVec2(newMovementDirection) > 0 then
		-- update linear velocity
		local newLinearVelocity = entity:getRigidBody():getLinearVelocity()
		local translationalSpeed = 1.0
		if Input.getButtonDown(Key.LeftShift) then
			translationalSpeed = 2.0
		end
		newLinearVelocity.x = newMovementDirection.x * translationalSpeed
		newLinearVelocity.z = newMovementDirection.y * translationalSpeed
		entity:getRigidBody():setLinearVelocity(newLinearVelocity)

		-- rotate entity in direction of movement (around y-axis)
		newLinearVelocity.y = 0
		local xzTranslation = vec3:new(entity:getTransform().translation.x, 0, entity:getTransform().translation.z)
		local targetRotationQuat = MathUtils.safeQuatLookAt(xzTranslation, xzTranslation - newLinearVelocity, vec3:new(0, 1, 0) , vec3:new(0, 1, 0))
		local curRot = entity:getRigidBody():getRotation()
		local newQuat = MathUtils.quatSlerp(curRot, targetRotationQuat, math.min(6.0 * dt, 1.0))
		entity:getRigidBody():setRotation(newQuat)

		-- update animation
		-- entity:getAnimator():setVariable("speed", math.floor(translationalSpeed))
		if Input.getButtonDown(Key.LeftShift) then
			entity:getAnimator():setVariable("speed", 2)
		else
			entity:getAnimator():setVariable("speed", 1)
		end
	else
		-- update animation
		entity:getAnimator():setVariable("speed", 0)
	end

	-- attack when mouse down
	if Input.getButtonDown(Key.LeftMouse) then
		-- entity:getAnimator():setVariable("slashing", 1)
	else
		-- entity:getAnimator():setVariable("slashing", 0)
	end
end

























































