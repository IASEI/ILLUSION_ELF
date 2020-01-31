#ifndef __PHYSICAL_H__
#define __PHYSICAL_H__

#include "fantastic.h"

namespace PQE
{
	class PhysicalRule;
	class Physical;
	class Elastic;
	class RopeSimulation;
	class PhysicalForce;

	class PhysicalForce
	{
	public:
		PhysicalForce();
		virtual void ApplyForce(glm::vec3 force);
		virtual void Simulate(float dt);
		virtual void Solve();
	public:

	};

	class PhysicalRule
	{
	public:
		PhysicalRule();
		PhysicalRule(glm::vec3 gravitation, float airFrictionConstant, float groundFrictionConstant, float groundHeight);
		Physical* GetPhysicalObject(int index);
		void appendPhysical(Physical*obj);
		void appendPhysicalForce(PhysicalForce*obj);
		void setRopeConnectionVel(glm::vec3 ropeConnectionVel);
		virtual void Release();
		virtual void Init();
		virtual void Solve();
		virtual void Simulate(float dt);
		virtual void Operate(float dt);
	public:
		glm::vec3 gravitation;
		glm::vec3 ropeConnectionPos;
		glm::vec3 ropeConnectionVel;
		float groundRepulsionConstant;
		float groundFrictionConstant;
		float groundAbsorptionConstant;
		float groundHeight;
		float airFrictionConstant;
		std::vector<Physical*> PhysicalObject;
		std::vector<PhysicalForce*> PhysicalForces;
	};

	class Physical
	{
	public:
		Physical(float m);
		void ApplyForce(glm::vec3 force);
		void Init();
		void Simulate(float dt);
	public:
		float quality;
		glm::vec3 position;
		glm::vec3 velocity;
		glm::vec3 force;
	};

	class Elastic : public PhysicalForce
	{
	public:
		Elastic(Physical* obj1, Physical* obj2, float springConstant, float springLength, float frictionConstant);
		void Solve();
	public:
		Physical* obj1;
		Physical* obj2;
		float springConstant;
		float springLength;
		float frictionConstant;

	};
}
#endif

