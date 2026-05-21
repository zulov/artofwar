#pragma once
#include <span>
#include <vector>
#include <Eigen/Core>

class Layer {
public:
	Layer(std::vector<float>& w, std::vector<float>& bias);
	Layer(const Layer& rhs) = delete;
	~Layer() = default;
	bool setInput(std::span<const float> data);
	void setValues(const Eigen::VectorXf& mult);

	const Eigen::VectorXf& getValues() const { return values; }
	int getInputSize() const { return weights.cols(); }
	int getOutputSize() const { return weights.rows(); }

private:
	bool sameInput(std::span<const float> data);

	Eigen::MatrixXf weights;
	Eigen::VectorXf values;
	Eigen::VectorXf bias;
};
