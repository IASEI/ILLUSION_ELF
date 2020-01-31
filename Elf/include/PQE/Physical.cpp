#include "Physical.h"
namespace PQE
{
	PhysicalForce::PhysicalForce() {}
	void PhysicalForce::ApplyForce(glm::vec3 force) {}
	void PhysicalForce::Simulate(float dt) {}
	void PhysicalForce::Solve() {}
	PhysicalRule::PhysicalRule(glm::vec3 gravitation, float airFrictionConstant, float groundFrictionConstant,float groundHeight)
		:gravitation(gravitation), airFrictionConstant(airFrictionConstant), groundFrictionConstant(groundFrictionConstant), groundHeight(groundHeight)
	{
		
	}
	PhysicalRule::PhysicalRule() {}
	Physical* PhysicalRule::GetPhysicalObject(int index)
	{
		if (index < 0 || index >= this->PhysicalObject.size())
			return NULL;

		return PhysicalObject[index];
	}
	void PhysicalRule::appendPhysical(Physical*obj)
	{
		this->PhysicalObject.push_back(obj);
	}
	void PhysicalRule::appendPhysicalForce(PhysicalForce *force)
	{
		this->PhysicalForces.push_back(force);
	}
	void PhysicalRule::Release()
	{
		
	}
	void PhysicalRule::Init()
	{
		for (int a = 0; a < this->PhysicalObject.size(); ++a)
			PhysicalObject[a]->Init();
	}
	void PhysicalRule::Solve()
	{
		int coutForce = this->PhysicalForces.size();
		int coutObj = this->PhysicalObject.size();

		for (int a = 0; a < coutForce; ++a)
		{
			this->PhysicalForces[a]->Solve();
		}

		for (int a = 0; a < coutObj; ++a)
		{
			this->PhysicalObject[a]->ApplyForce(gravitation * this->PhysicalObject[a]->quality);

			this->PhysicalObject[a]->ApplyForce(-this->PhysicalObject[a]->velocity * airFrictionConstant);

			if (this->PhysicalObject[a]->position.y < groundHeight)
			{
				glm::vec3 v;

				v = this->PhysicalObject[a]->velocity;
				v.y = 0;
				this->PhysicalObject[a]->ApplyForce(-v * groundFrictionConstant);

				v = this->PhysicalObject[a]->velocity;
				v.x = 0;
				v.z = 0;

				if (v.y < 0)
					this->PhysicalObject[a]->ApplyForce(-v * groundAbsorptionConstant);

				glm::vec3 force = glm::vec3(0, groundRepulsionConstant, 0) *
					(groundHeight - this->PhysicalObject[a]->position.y);

				this->PhysicalObject[a]->ApplyForce(force);
			}

		}
	}
	void  PhysicalRule::setRopeConnectionVel(glm::vec3 ropeConnectionVel) {
		this->ropeConnectionVel = ropeConnectionVel;
	}
	void PhysicalRule::Simulate(float dt)
	{
		for (int a = 0; a < this->PhysicalObject.size(); ++a) {
			PhysicalObject[a]->Simulate(dt);

		}
		/**/
		ropeConnectionPos += ropeConnectionVel * dt;
		if (ropeConnectionPos.y < groundHeight)			//ropeConnectionPos shall not go under the ground
		{
			ropeConnectionPos.y = groundHeight;
			ropeConnectionVel.y = 0;
		}
		this->PhysicalObject[0]->velocity = glm::vec3(0);
		
	}
	void PhysicalRule::Operate(float dt)
	{
		Init();
		Solve();
		Simulate(dt);
	}
	
	Physical::Physical(float quality)
	{
		this->quality = quality;
	}
	void Physical::ApplyForce(glm::vec3 force)
	{
		this->force += force;
	}
	void Physical::Init()
	{
		force.x = 0;
		force.y = 0;
		force.z = 0;
	}
	void Physical::Simulate(float dt)
	{
		velocity += (force / quality) * dt;
		position += velocity * dt;

	}
	
	Elastic::Elastic(Physical* obj1, Physical* obj2,float springConstant, float springLength, float frictionConstant)
	{
		this->springConstant = springConstant;
		this->springLength = springLength;
		this->frictionConstant = frictionConstant;

		this->obj1 = obj1;
		this->obj2 = obj2;
	}
	void  Elastic::Solve()
	{
		glm::vec3 springVector = obj1->position - obj2->position;
		float r = glm::length(springVector);
		glm::vec3 force;
		if (r != 0)
			force += (springVector / r) * (r - springLength) * (-springConstant);
		force += -(obj1->velocity - obj2->velocity) * frictionConstant;
		obj1->ApplyForce(force);
		obj2->ApplyForce(-force);
	}




}