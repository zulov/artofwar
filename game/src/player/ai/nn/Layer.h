#pragma once
#include <span>
#include <vector>
#include <Eigen/Core>

class Layer {
public:
	Layer(std::vector<float>& w, std::vector<float>& bias);
	Layer(const Layer& rhs) = delete;
	~Layer() = default;
	void setValues(std::span<float> data);
	void setValues(const Eigen::VectorXf& mult);

	const Eigen::VectorXf& getValues() const { return values; }

private:
	Eigen::MatrixXf weights;
	Eigen::VectorXf values;
	Eigen::VectorXf bias;
};
