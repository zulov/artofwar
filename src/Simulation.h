#pragma once

#include "Main.h"
#include "Unit.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include "EnviromentStrategy.h"
#include "ForceStrategy.h"
namespace Urho3D {
	class Node;
	class Scene;
}

class Test : public Main {
	URHO3D_OBJECT(Test, Main);

public:
	Test(Context* context);
	virtual void Start();

protected:
	/// Return XML patch instructions for screen joystick layout for a specific sample app, if any.
	virtual String GetScreenJoystickPatchString() const {
		return
			"<patch>"
			"    <remove sel=\"/element/element[./attribute[@name='Name' and @value='Button0']]/attribute[@name='Is Visible']\" />"
			"    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button0']]/element[./attribute[@name='Name' and @value='Label']]/attribute[@name='Text']/@value\">Group</replace>"
			"    <add sel=\"/element/element[./attribute[@name='Name' and @value='Button0']]\">"
			"        <element type=\"Text\">"
			"            <attribute name=\"Name\" value=\"KeyBinding\" />"
			"            <attribute name=\"Text\" value=\"G\" />"
			"        </element>"
			"    </add>"
			"    <remove sel=\"/element/element[./attribute[@name='Name' and @value='Button1']]/attribute[@name='Is Visible']\" />"
			"    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button1']]/element[./attribute[@name='Name' and @value='Label']]/attribute[@name='Text']/@value\">Animation</replace>"
			"    <add sel=\"/element/element[./attribute[@name='Name' and @value='Button1']]\">"
			"        <element type=\"Text\">"
			"            <attribute name=\"Name\" value=\"KeyBinding\" />"
			"            <attribute name=\"Text\" value=\"SPACE\" />"
			"        </element>"
			"    </add>"
			"</patch>";
	}

private:	
	void CreateScene();
	void createCamera();
	void createLight() {
		Node* lightNode = scene->CreateChild("DirectionalLight");
		lightNode->SetDirection(Vector3(-0.6f, -1.0f, -0.8f)); // The direction vector does not need to be normalized
		Light* light = lightNode->CreateComponent<Light>();
		light->SetLightType(LIGHT_DIRECTIONAL);
		light->SetColor(Color(0.7f, 0.35f, 0.0f));
	}
	void createZone();
	void createUnits(int size, double space);
	void CreateInstructions();
	//void SetupViewport();
	void SubscribeToEvents();
	void moveCamera(float timeStep);
	void AnimateObjects(float timeStep);
	void moveUnits(float timeStep) {
		for (unsigned i = 0; i < units.size(); ++i) {
			units.at(i)->applyForce(timeStep);
			units.at(i)->move(timeStep);
		}
	}
	void HandleUpdate(StringHash eventType, VariantMap& eventData);

	bool animate;

	std::vector<Unit*> units;

	//CONST
	const float ROTATE_SPEED = 115.0f;
	const double coef = 10;

	void calculateForces();
	EnviromentStrategy *envStrategy;
	ForceStrategy * forceStrategy;
	void control();
	void resetUnits();
};
