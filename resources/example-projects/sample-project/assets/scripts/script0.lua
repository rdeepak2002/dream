function update(entity, dt)
	-- activate pointer lock when clicking and escape to cancel pointer lock
	if Input.getButtonDown(Key.Escape) then
		Input.activatePointerLock(false)
	end
	if Input.getButtonDown(Key.LeftMouse) then
		Input.activatePointerLock(true)
	end

	-- define spherical coordinate values
	if self.phi == nil then
		self.phi = 0.1
	end
	if self.theta == nil then
		self.minTheta = 1.59
		self.maxTheta = 2.6
		self.theta = 1.9
	end
	if self.radius == nil then
		self.radius = 3.3
	end

	-- set position of camera based off spherical coordinate values
	local xPos = self.radius * math.sin(self.theta) * math.cos(self.phi)
	local yPos = self.radius * math.cos(self.theta)
	local zPos = self.radius * math.sin(self.theta) * math.sin(self.phi)
	entity:getTransform().translation = vec3:new(xPos, yPos, zPos);

	-- rotate around object using mouse input when pointer locked
	if Input.pointerLockActivated() then
		local mouseMovement = Input.getMouseMovement()
		self.theta = self.theta + mouseMovement.y * dt * 0.2
		self.phi = self.phi - mouseMovement.x * dt * 0.2
	end

	-- bound phi and theta
	self.theta = math.min(self.theta, 2.41)
	self.theta = math.max(self.theta, 1.59)
	while self.phi > 2 * math.pi do
		self.phi = self.phi - 2 * math.pi
	end

	Logger.debug("phi: " .. tostring(self.phi))
	Logger.debug("theta: " .. tostring(self.theta))

	-- make camera look at target (player)
	lookAt = vec3:new(0, -1, 0)
	entity:getCamera().lookAt = lookAt
end






















