function update(entity, dt)
	-- get entity to orbit around and identify it by tag
	if self.targetTag == nil then
		self.targetTag = "knight"
	end

	local targetEntity = Scene.getEntityByTag(self.targetTag)

	if not targetEntity:isValid() then
		Logger.error("Camera cannot find target entity")
		Input.activatePointerLock(false)
		return
	end

	-- activate pointer lock when clicking and escape to cancel pointer lock
	-- if Input.getButtonDown(Key.Escape) then
	-- 	Input.activatePointerLock(false)
	-- end
	if Input.getButtonDown(Key.LeftMouse) then
		Input.activatePointerLock(true)
	end

	-- define spherical coordinate values
	if self.phi == nil then
		self.phi = 0.1
	end
	if self.theta == nil then
		self.minTheta = 1.2
		self.maxTheta = 1.9
		self.theta = 1.8
	end
	if self.radius == nil then
		self.radius = 3.3
	end

	-- set position of camera based off spherical coordinate values
	self.radius = 2.5
	local lookAtOffset = vec3:new(0, 0.6, 0)
	local targetTranslation = targetEntity:getTransform().translation + lookAtOffset

	local xPos = targetTranslation.x - self.radius * math.sin(self.theta) * math.cos(self.phi)
	local yPos = targetTranslation.y - self.radius * math.cos(self.theta)
	local zPos = targetTranslation.z + self.radius * math.sin(self.theta) * math.sin(self.phi)

	entity:getTransform().translation = vec3:new(xPos, yPos, zPos);

	local frontVec = 0.1 * MathUtils.normalizeVec3(entity:getTransform().translation - targetTranslation)

	if PhysicsComponentSystem.checkRaycast(targetTranslation, entity:getTransform().translation + frontVec) then
		local hit = PhysicsComponentSystem.raycastGetFirstHit(targetTranslation, entity:getTransform().translation + frontVec)
		self.radius = MathUtils.distance(hit, targetTranslation) - 0.1

		xPos = targetTranslation.x - self.radius * math.sin(self.theta) * math.cos(self.phi)
		yPos = targetTranslation.y - self.radius * math.cos(self.theta)
		zPos = targetTranslation.z + self.radius * math.sin(self.theta) * math.sin(self.phi)

		entity:getTransform().translation = vec3:new(xPos, yPos, zPos);
	end

	-- rotate around object using mouse input when pointer locked
	if Input.pointerLockActivated() then
		local mouseMovement = Input.getMouseMovement()
		self.theta = self.theta + mouseMovement.y * dt * 0.2
		self.phi = self.phi - mouseMovement.x * dt * 0.2

		if self.phi < 0.0001 then
		    self.phi = 0.0001
		end

        if self.phi > 3.14 then
            self.phi = 3.14
        end

        -- Logger.debug(tostring(self.phi))
	end

	-- bound phi and theta
	self.theta = math.min(self.theta, 1.9)
	self.theta = math.max(self.theta, 1.2)
	while self.phi > 2 * math.pi do
		self.phi = self.phi - 2 * math.pi
	end

	-- make camera look at target (player)
	lookAt = vec3:new(targetTranslation.x, targetTranslation.y, targetTranslation.z)
	-- entity:getCamera().lookAt = lookAt
	entity:getCamera():lookAt(entity, lookAt)
end

















