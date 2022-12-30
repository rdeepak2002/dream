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

	local pKnightTranslation = vec3:new(knightTranslation.x, 0, knightTranslation.z)
	local pCameraTranslation = vec3:new(cameraTranslation.x, 0, cameraTranslation.z)
	local targetRotationQuat = MathUtils.safeQuatLookAt(pKnightTranslation, pCameraTranslation, vec3:new(0, 1, 0) , vec3:new(0, 1, 0))
	-- entity:getTransform().rotation = targetRotationQuat
	-- entity:getRigidBody():setRotation(targetRotationQuat)

	-- calculate ground linear velocity based off inputs
	local linearVelocity = vec3:new(0, 0, 0)
	local speed = 2.0
	
	if Input.getButtonDown(Key.w) then
		linearVelocity = linearVelocity + cameraForwardVector
		animVelForward = animVelForward + 1
	end

	if Input.getButtonDown(Key.s) then
		linearVelocity = linearVelocity - cameraForwardVector
		animVelForward = animVelForward - 1
	end

	if Input.getButtonDown(Key.a) then
		linearVelocity = linearVelocity + cameraRightVector
	end

	if Input.getButtonDown(Key.d) then
		linearVelocity = linearVelocity - cameraRightVector
	end

	if MathUtils.magnitudeVec3(linearVelocity) > 0 then
		linearVelocity = MathUtils.normalizeVec3(linearVelocity)
		linearVelocity = linearVelocity * speed
		-- entity:getRigidBody():setRotation(targetRotationQuat)
		entity:getRigidBody():setLinearVelocity(linearVelocity)
		entity:getRigidBody():setRotation(targetRotationQuat)
	else
		-- entity:getRigidBody():setRotation(vec3:new(0, 0, 0))
		-- entity:getRigidBody():setLinearVelocity(vec3:new(0, 0, 0))
	end

	-- entity:getRigidBody():setRotation(quat:new(1, 0, 0, 0))
	-- entity:getRigidBody():setRotation(targetRotationQuat)

	-- set velocity for animator
	entity:getAnimator():setVariable("velocity", animVelForward)

	-- set slashing for animator
	local slash = 0
	if Input.getButtonDown(Key.LeftMouse) then
		slash = 1
	end
	entity:getAnimator():setVariable("slash", slash)

	-- entity:getRigidBody():setAngularVelocity(vec3:new(0, 0, 0))

	-- local animVel = 0
	-- local slash = 0
	-- local xzVel = vec2:new(0, 0)

	-- if Input.getButtonDown(Key.w) then
	-- 	xzVel.y = xzVel.y + 1
	-- 	animVel = 1
	-- end

	-- if Input.getButtonDown(Key.s) then
	-- 	xzVel.y = xzVel.y - 1
	-- 	animVel = -1
	-- end

	-- if Input.getButtonDown(Key.LeftMouse) then
	-- 	slash = 1
	-- end

	-- entity:getRigidBody():setLinearVelocity(vec3:new(xzVel.x, 0, xzVel.y))
	-- entity:getAnimator():setVariable("velocity", animVel)
	-- entity:getAnimator():setVariable("slash", slash)
end


























