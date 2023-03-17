#pragma once


struct FrameInfo;
class ObjectsInfo;

class SimInfo {
public:
	SimInfo();
	~SimInfo();
	
	const ObjectsInfo* getObjectsInfo() const { return objectsInfo; }
	const FrameInfo* getFrameInfo() const { return frameInfo; }
	void setObjectsInfo(ObjectsInfo* objectsInfo) { this->objectsInfo = objectsInfo; }
	void setIsRealFrame(bool realFrame) const;
	void setFrame(unsigned char currentFrame, unsigned secondsElapsed) const;
private:
	ObjectsInfo* objectsInfo;
	FrameInfo* frameInfo;
};
