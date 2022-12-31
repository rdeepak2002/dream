function update(entity, dt)
	local animVelForward = 0

	-- get main camera
	local cameraEntity = Scene.getEntityByTag("camera")
	if not cameraEntity:isValid() then
		Logger.error("Knight cannot find camera entity")
		return
	end

	-- get forward and right vectors of camera
	local cameraTranslation = cameraEntity:getTransform().translation
	local knightTranslation = entity:getTransform().translation
	local cameraForwardVector = knightTranslation - cameraTranslation
	cameraForwardVector.y = 0
	cameraForwardVector = MathUtils.normalizeVec3(cameraForwardVector)
	local cameraRightVector = MathUtils.crossProductVec3(vec3:new(0, 1, 0), cameraForwardVector)
	cameraRightVector.y = 0
	cameraRightVector = MathUtils.normalizeVec3(cameraRightVector)

	-- get forward and right vectors
	local knightForwardVector = entity:getTransform():getFront()
	knightForwardVector.y = 0
	knightForwardVector = MathUtils.normalizeVec3(knightForwardVector)
	local knightLeftVector = entity:getTransform():getLeft()
	knightLeftVector.y = 0
	knightLeftVector = MathUtils.normalizeVec3(knightLeftVector)

	local pKnightTranslation = vec3:new(knightTranslation.x, 0, knightTranslation.z)
	local pCameraTranslation = vec3:new(cameraTranslation.x, 0, cameraTranslation.z)

	-- calculate ground linear velocity based off inputs
	local linearVelocity = vec3:new(0, 0, 0)
	local speed = 2.0
	
	if Input.getButtonDown(Key.w) then
		linearVelocity = linearVelocity + cameraForwardVector
	end

	if Input.getButtonDown(Key.s) then
		linearVelocity = linearVelocity - cameraForwardVector
	end

	if Input.getButtonDown(Key.a) then
		linearVelocity = linearVelocity + cameraRightVector
	end

	if Input.getButtonDown(Key.d) then
		linearVelocity = linearVelocity - cameraRightVector
	end

	if Input.getButtonDown(Key.Space) then
		local isGrounded = PhysicsComponentSystem.checkRaycast(knightTranslation - vec3:new(0, -0.1, 0), knightTranslation - vec3:new(0, 1.0, 0))
		if isGrounded then
			entity:getRigidBody():applyCentralImpulse(vec3:new(0, 20.0 * dt, 0))
		else
			-- Logger.debug("in air")
		end
	end

	if MathUtils.magnitudeVec3(linearVelocity) > 0 then
		-- set rigid body velocity
		linearVelocity = MathUtils.normalizeVec3(linearVelocity)
		linearVelocity = linearVelocity * speed
		linearVelocity.y = entity:getRigidBody():getLinearVelocity().y
		entity:getRigidBody():setLinearVelocity(linearVelocity)

		-- rotate rigid body in direction of velocity
		linearVelocity.y = 0
		local targetRotationQuat = MathUtils.safeQuatLookAt(pKnightTranslation, pKnightTranslation - linearVelocity, vec3:new(0, 1, 0) , vec3:new(0, 1, 0))
		local curRot = entity:getRigidBody():getRotation()
		local newQuat = MathUtils.quatSlerp(curRot, targetRotationQuat, math.min(6.0 * dt, 1.0))
		entity:getRigidBody():setRotation(newQuat)

		-- set velocity for animator
		entity:getAnimator():setVariable("velocity", 1)
	else
		-- set velocity for animator
		entity:getAnimator():setVariable("velocity", 0)
	end

	-- set slashing for animator
	local slash = 0
	if Input.getButtonDown(Key.LeftMouse) then
		slash = 1
	end
	entity:getAnimator():setVariable("slash", slash)
end

































